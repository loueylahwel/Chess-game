#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

// Prevent std::byte conflicts with Windows headers
#define _HAS_STD_BYTE 0

#include <SFML/Network.hpp>
#include <string>
#include <vector>
#include <queue>
#include <mutex>
#include <thread>
#include <atomic>
#include <functional>
#include <chrono>

using namespace std;
using namespace sf;

// Network message types
enum class MessageType
{
    Connect,    // Initial connection message
    Disconnect, // Disconnection message
    Move,       // Chess move message
    GameState,  // Full game state update
    Chat,       // Chat message
    Ready,      // Player ready to start
    GameOver,   // Game over notification
    Error       // Error message
};

// Structure for network messages
struct NetworkMessage
{
    MessageType type;
    string data;

    NetworkMessage() : type(MessageType::Error), data("") {}
    NetworkMessage(MessageType t, const string &d) : type(t), data(d) {}
};

// Class for managing network communication
class NetworkManager
{
private:
    // Network variables
    bool isHost;
    bool isConnected;
    atomic<bool> running;

    // Server variables
    TcpListener listener;
    vector<TcpSocket *> clients;

    // Client variables
    TcpSocket socket;
    IpAddress serverAddress;
    unsigned short serverPort;

    // Message queue
    queue<NetworkMessage> incomingMessages;
    mutex messageMutex;

    // Network thread
    thread networkThread;

    // Callback for message handling
    function<void(const NetworkMessage &)> messageCallback;

    // Private methods
    void serverLoop();
    void clientLoop();
    void processIncomingData(TcpSocket *socket);

public:
    NetworkManager();
    ~NetworkManager();

    // Server methods
    bool startServer(unsigned short port = 50000);
    void stopServer();
    bool sendToAllClients(const NetworkMessage &message);

    // Client methods
    bool connectToServer(const string &address, unsigned short port = 50000);
    void disconnectFromServer();
    bool sendToServer(const NetworkMessage &message);

    // Common methods
    bool isServerRunning() const;
    bool isClientConnected() const;
    bool hasMessages() const;
    NetworkMessage getNextMessage();
    void setMessageCallback(function<void(const NetworkMessage &)> callback);

    // Utility methods
    static string moveToString(int fromX, int fromY, int toX, int toY);
    static void stringToMove(const string &moveStr, int &fromX, int &fromY, int &toX, int &toY);
};

#endif // NETWORKMANAGER_H
