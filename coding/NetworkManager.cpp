// Prevent std::byte conflicts with Windows headers
#define _HAS_STD_BYTE 0

#include "NetworkManager.h"
#include <iostream>
#include <sstream>

using namespace std;
using namespace sf;

NetworkManager::NetworkManager() : isHost(false),
                                   isConnected(false),
                                   running(false),
                                   serverPort(50000)
{
}

NetworkManager::~NetworkManager()
{
    // Stop any running network operations
    if (running.load())
    {
        running.store(false);

        if (isHost)
        {
            stopServer();
        }
        else
        {
            disconnectFromServer();
        }

        // Wait for network thread to finish
        if (networkThread.joinable())
        {
            networkThread.join();
        }
    }

    // Clean up client sockets
    for (auto client : clients)
    {
        client->disconnect();
        delete client;
    }
    clients.clear();
}

// Server methods
bool NetworkManager::startServer(unsigned short port)
{
    // Already running as a client
    if (isConnected && !isHost)
    {
        return false;
    }

    // Already running as a server
    if (isHost && running.load())
    {
        return true;
    }

    // Set up the server
    serverPort = port;

    // Start listening for connections
    if (listener.listen(serverPort) != Socket::Done)
    {
        cout << "Failed to bind server to port " << serverPort << endl;
        return false;
    }

    cout << "Server started on port " << serverPort << endl;

    // Set non-blocking mode
    listener.setBlocking(false);

    // Set flags
    isHost = true;
    running.store(true);

    // Start server thread
    networkThread = thread(&NetworkManager::serverLoop, this);

    return true;
}

void NetworkManager::stopServer()
{
    if (!isHost || !running.load())
    {
        return;
    }

    // Signal thread to stop
    running.store(false);

    // Wait for thread to finish
    if (networkThread.joinable())
    {
        networkThread.join();
    }

    // Clean up client connections
    for (auto client : clients)
    {
        client->disconnect();
        delete client;
    }
    clients.clear();

    // Stop listening
    listener.close();

    cout << "Server stopped" << endl;
}

bool NetworkManager::sendToAllClients(const NetworkMessage &message)
{
    if (!isHost || clients.empty())
    {
        return false;
    }

    // Prepare packet
    Packet packet;
    packet << static_cast<int>(message.type) << message.data;

    // Send to all clients
    bool allSuccessful = true;
    for (auto client : clients)
    {
        if (client->send(packet) != Socket::Done)
        {
            cout << "Failed to send message to a client" << endl;
            allSuccessful = false;
        }
    }

    return allSuccessful;
}

// Client methods
bool NetworkManager::connectToServer(const string &address, unsigned short port)
{
    // Already connected
    if (isConnected)
    {
        return true;
    }

    // Already running as a server
    if (isHost && running.load())
    {
        return false;
    }

    // Set up client
    serverAddress = IpAddress(address);
    serverPort = port;

    // Connect to server
    Socket::Status status = socket.connect(serverAddress, serverPort, seconds(5));
    if (status != Socket::Done)
    {
        cout << "Failed to connect to server at " << address << ":" << port << endl;
        return false;
    }

    cout << "Connected to server at " << address << ":" << port << endl;

    // Set non-blocking mode
    socket.setBlocking(false);

    // Set flags
    isHost = false;
    isConnected = true;
    running.store(true);

    // Start client thread
    networkThread = thread(&NetworkManager::clientLoop, this);

    return true;
}

void NetworkManager::disconnectFromServer()
{
    if (isHost || !isConnected || !running.load())
    {
        return;
    }

    // Signal thread to stop
    running.store(false);

    // Wait for thread to finish
    if (networkThread.joinable())
    {
        networkThread.join();
    }

    // Disconnect socket
    socket.disconnect();
    isConnected = false;

    cout << "Disconnected from server" << endl;
}

bool NetworkManager::sendToServer(const NetworkMessage &message)
{
    if (isHost || !isConnected)
    {
        return false;
    }

    // Prepare packet
    Packet packet;
    packet << static_cast<int>(message.type) << message.data;

    // Send to server
    if (socket.send(packet) != Socket::Done)
    {
        cout << "Failed to send message to server" << endl;
        return false;
    }

    return true;
}

// Common methods
bool NetworkManager::isServerRunning() const
{
    return isHost && running.load();
}

bool NetworkManager::isClientConnected() const
{
    return isConnected && !isHost && running.load();
}

bool NetworkManager::hasMessages() const
{
    // Use const_cast to work around the const qualifier issue
    // This is safe because we're only reading from the mutex-protected resource
    lock_guard<mutex> lock(const_cast<mutex &>(messageMutex));
    return !incomingMessages.empty();
}

NetworkMessage NetworkManager::getNextMessage()
{
    lock_guard<mutex> lock(messageMutex);

    if (incomingMessages.empty())
    {
        return NetworkMessage(MessageType::Error, "No messages available");
    }

    NetworkMessage message = incomingMessages.front();
    incomingMessages.pop();
    return message;
}

void NetworkManager::setMessageCallback(function<void(const NetworkMessage &)> callback)
{
    messageCallback = callback;
}

// Private methods
void NetworkManager::serverLoop()
{
    cout << "Server loop started" << endl;

    try
    {
        // Create a socket selector to monitor multiple sockets
        SocketSelector selector;

        // Add the listener to the selector
        selector.add(listener);

        while (running.load())
        {
            try
            {
                // Wait for activity on any socket (with a timeout)
                if (selector.wait(milliseconds(100)))
                {
                    // Check if the listener is ready (new connection)
                    if (selector.isReady(listener))
                    {
                        try
                        {
                            // Accept the new connection
                            TcpSocket *newClient = new TcpSocket();
                            if (listener.accept(*newClient) == Socket::Done)
                            {
                                // Set non-blocking mode
                                newClient->setBlocking(false);

                                // Add to client list
                                clients.push_back(newClient);

                                // Add the new client to the selector
                                selector.add(*newClient);

                                cout << "New client connected: " << newClient->getRemoteAddress() << endl;

                                // Send connection confirmation
                                Packet packet;
                                packet << static_cast<int>(MessageType::Connect) << "Connected to server";
                                newClient->send(packet);

                                // Add a ready message to the queue
                                NetworkMessage readyMsg(MessageType::Ready, "");
                                lock_guard<mutex> lock(messageMutex);
                                incomingMessages.push(readyMsg);
                            }
                            else
                            {
                                // Failed to accept, clean up
                                delete newClient;
                            }
                        }
                        catch (const exception &e)
                        {
                            cout << "Exception accepting new connection: " << e.what() << endl;
                        }
                    }

                    // Check for messages from existing clients
                    for (auto it = clients.begin(); it != clients.end();)
                    {
                        TcpSocket *client = *it;

                        try
                        {
                            if (selector.isReady(*client))
                            {
                                // Process incoming data
                                processIncomingData(client);
                            }

                            // Check if client is still connected
                            if (client->getRemoteAddress() == IpAddress::None)
                            {
                                cout << "Client disconnected" << endl;

                                // Remove from selector
                                selector.remove(*client);

                                // Remove from list
                                it = clients.erase(it);
                                delete client;
                            }
                            else
                            {
                                ++it;
                            }
                        }
                        catch (const exception &e)
                        {
                            cout << "Exception processing client: " << e.what() << endl;

                            try
                            {
                                // Remove from selector
                                selector.remove(*client);

                                // Remove from list
                                it = clients.erase(it);
                                delete client;
                            }
                            catch (...)
                            {
                                // If we can't remove it properly, just increment and continue
                                ++it;
                            }
                        }
                    }
                }
            }
            catch (const exception &e)
            {
                cout << "Exception in server loop: " << e.what() << endl;
                // Continue running the server loop
            }

            // Add a small delay to prevent high CPU usage
            this_thread::sleep_for(chrono::milliseconds(10));
        }
    }
    catch (const exception &e)
    {
        cout << "Fatal exception in server loop: " << e.what() << endl;
    }

    cout << "Server loop ended" << endl;
}

void NetworkManager::clientLoop()
{
    cout << "Client loop started" << endl;

    try
    {
        // Create a socket selector to monitor the socket
        SocketSelector selector;

        // Add the socket to the selector
        selector.add(socket);

        while (running.load() && isConnected)
        {
            try
            {
                // Wait for activity on the socket (with a timeout)
                if (selector.wait(milliseconds(100)))
                {
                    // Check if the socket is ready
                    if (selector.isReady(socket))
                    {
                        // Process incoming data
                        processIncomingData(&socket);
                    }
                }

                // Check if still connected
                if (socket.getRemoteAddress() == IpAddress::None)
                {
                    cout << "Disconnected from server" << endl;
                    isConnected = false;
                    break;
                }
            }
            catch (const exception &e)
            {
                cout << "Exception in client loop: " << e.what() << endl;
                // Continue running the client loop
            }

            // Add a small delay to prevent high CPU usage
            this_thread::sleep_for(chrono::milliseconds(10));
        }
    }
    catch (const exception &e)
    {
        cout << "Fatal exception in client loop: " << e.what() << endl;
        isConnected = false;
    }

    cout << "Client loop ended" << endl;
}

void NetworkManager::processIncomingData(TcpSocket *socket)
{
    if (!socket)
    {
        cout << "Error: Null socket pointer in processIncomingData" << endl;
        return;
    }

    try
    {
        Packet packet;
        Socket::Status status = socket->receive(packet);

        if (status == Socket::Done)
        {
            // Extract message type and data
            int typeInt;
            string data;

            if (packet >> typeInt >> data)
            {
                // Validate message type
                if (typeInt < 0 || typeInt > static_cast<int>(MessageType::Error))
                {
                    cout << "Invalid message type received: " << typeInt << endl;
                    return;
                }

                MessageType type = static_cast<MessageType>(typeInt);
                NetworkMessage message(type, data);

                // Add to queue
                {
                    lock_guard<mutex> lock(messageMutex);
                    incomingMessages.push(message);
                }

                // Call callback if set
                if (messageCallback)
                {
                    try
                    {
                        messageCallback(message);
                    }
                    catch (const exception &e)
                    {
                        cout << "Exception in message callback: " << e.what() << endl;
                    }
                }
            }
        }
        else if (status == Socket::Disconnected)
        {
            cout << "Socket disconnected" << endl;

            // Add a disconnect message to the queue
            NetworkMessage disconnectMsg(MessageType::Disconnect, "Connection lost");
            lock_guard<mutex> lock(messageMutex);
            incomingMessages.push(disconnectMsg);

            // Call callback if set
            if (messageCallback)
            {
                try
                {
                    messageCallback(disconnectMsg);
                }
                catch (const exception &e)
                {
                    cout << "Exception in disconnect callback: " << e.what() << endl;
                }
            }
        }
        else if (status == Socket::Error)
        {
            cout << "Socket error occurred" << endl;
        }
    }
    catch (const exception &e)
    {
        cout << "Exception in processIncomingData: " << e.what() << endl;
    }
}

// Utility methods
string NetworkManager::moveToString(int fromX, int fromY, int toX, int toY)
{
    stringstream ss;
    ss << fromX << "," << fromY << "," << toX << "," << toY;
    return ss.str();
}

void NetworkManager::stringToMove(const string &moveStr, int &fromX, int &fromY, int &toX, int &toY)
{
    stringstream ss(moveStr);
    char comma;
    ss >> fromX >> comma >> fromY >> comma >> toX >> comma >> toY;
}
