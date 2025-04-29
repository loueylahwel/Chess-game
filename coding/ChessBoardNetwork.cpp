// Prevent std::byte conflicts with Windows headers
#define _HAS_STD_BYTE 0

#include "ChessBoard.h"
#include "GameLogic.h"
#include <iostream>
#include <sstream>

using namespace std;
using namespace sf;

bool ChessBoard::showNetworkOptions(bool isHost)
{
    // Get the same scaling factors as used in the main menu
    float scaleX = static_cast<float>(WINDOW_WIDTH) / menuTexture.getSize().x;
    float scaleY = static_cast<float>(WINDOW_HEIGHT) / menuTexture.getSize().y;
    menuSprite.setScale(scaleX, scaleY);

    // Calculate scalable positions based on window dimensions
    float centerX = WINDOW_WIDTH / 2.0f;
    float centerY = WINDOW_HEIGHT / 2.0f;
    float buttonWidth = WINDOW_WIDTH * 0.3f;
    float buttonHeight = WINDOW_HEIGHT * 0.08f;
    float verticalSpacing = WINDOW_HEIGHT * 0.05f;
    float inputWidth = WINDOW_WIDTH * 0.4f;
    float inputHeight = buttonHeight;

    // Title text
    Text titleText;
    titleText.setFont(font);
    titleText.setString(isHost ? "Host a LAN Game" : "Join a LAN Game");
    titleText.setCharacterSize(WINDOW_HEIGHT * 0.05f);
    titleText.setPosition(centerX - titleText.getLocalBounds().width / 2, centerY - 3 * verticalSpacing);
    titleText.setFillColor(Color::White);

    // IP Address input (for client only)
    RectangleShape ipAddressBox(Vector2f(inputWidth, inputHeight));
    ipAddressBox.setPosition(centerX - inputWidth / 2, centerY - verticalSpacing);
    ipAddressBox.setFillColor(Color(70, 70, 70, 200));
    ipAddressBox.setOutlineColor(Color::White);
    ipAddressBox.setOutlineThickness(1);

    // Port input
    RectangleShape portBox(Vector2f(inputWidth / 3, inputHeight));
    portBox.setPosition(centerX - inputWidth / 6, centerY + verticalSpacing / 2);
    portBox.setFillColor(Color(70, 70, 70, 200));
    portBox.setOutlineColor(Color::White);
    portBox.setOutlineThickness(1);

    // Start/Connect button
    RectangleShape startButton(Vector2f(buttonWidth, buttonHeight));
    startButton.setPosition(centerX - buttonWidth / 2, centerY + 2 * verticalSpacing);
    startButton.setFillColor(Color(50, 50, 50, 200));

    // Back button
    RectangleShape backButton(Vector2f(buttonWidth, buttonHeight));
    backButton.setPosition(centerX - buttonWidth / 2, centerY + 3 * verticalSpacing);
    backButton.setFillColor(Color(50, 50, 50, 200));

    // Labels
    Text ipAddressLabel;
    ipAddressLabel.setFont(font);
    ipAddressLabel.setString("Server IP Address:");
    ipAddressLabel.setCharacterSize(WINDOW_HEIGHT * 0.03f);
    ipAddressLabel.setPosition(centerX - inputWidth / 2, centerY - verticalSpacing - ipAddressLabel.getLocalBounds().height - 5);
    ipAddressLabel.setFillColor(Color::White);

    Text portLabel;
    portLabel.setFont(font);
    portLabel.setString("Port:");
    portLabel.setCharacterSize(WINDOW_HEIGHT * 0.03f);
    portLabel.setPosition(centerX - inputWidth / 6, centerY + verticalSpacing / 2 - portLabel.getLocalBounds().height - 5);
    portLabel.setFillColor(Color::White);

    // Input text
    string ipAddressInput = isHost ? IpAddress::getLocalAddress().toString() : "127.0.0.1";
    string portInput = "50000";
    bool ipAddressFocused = false;
    bool portFocused = false;

    Text ipAddressText;
    ipAddressText.setFont(font);
    ipAddressText.setString(ipAddressInput);
    ipAddressText.setCharacterSize(WINDOW_HEIGHT * 0.03f);
    ipAddressText.setPosition(ipAddressBox.getPosition().x + 10, ipAddressBox.getPosition().y + (inputHeight - ipAddressText.getLocalBounds().height) / 2);
    ipAddressText.setFillColor(Color::White);

    Text portText;
    portText.setFont(font);
    portText.setString(portInput);
    portText.setCharacterSize(WINDOW_HEIGHT * 0.03f);
    portText.setPosition(portBox.getPosition().x + 10, portBox.getPosition().y + (inputHeight - portText.getLocalBounds().height) / 2);
    portText.setFillColor(Color::White);

    // Button text
    Text startButtonText;
    startButtonText.setFont(font);
    startButtonText.setString(isHost ? "Start Server" : "Connect to Server");
    startButtonText.setCharacterSize(WINDOW_HEIGHT * 0.03f);
    startButtonText.setPosition(startButton.getPosition().x + (buttonWidth - startButtonText.getLocalBounds().width) / 2,
                                startButton.getPosition().y + (buttonHeight - startButtonText.getLocalBounds().height) / 2);
    startButtonText.setFillColor(Color::White);

    Text backButtonText;
    backButtonText.setFont(font);
    backButtonText.setString("Back");
    backButtonText.setCharacterSize(WINDOW_HEIGHT * 0.03f);
    backButtonText.setPosition(backButton.getPosition().x + (buttonWidth - backButtonText.getLocalBounds().width) / 2,
                               backButton.getPosition().y + (buttonHeight - backButtonText.getLocalBounds().height) / 2);
    backButtonText.setFillColor(Color::White);

    // Status text
    Text statusText;
    statusText.setFont(font);
    statusText.setString("");
    statusText.setCharacterSize(WINDOW_HEIGHT * 0.025f);
    statusText.setPosition(centerX - statusText.getLocalBounds().width / 2, centerY + 4 * verticalSpacing);
    statusText.setFillColor(Color::Yellow);

    // Cursor for text input
    RectangleShape cursor(Vector2f(2, ipAddressText.getLocalBounds().height * 1.2f));
    cursor.setFillColor(Color::White);
    float cursorX = ipAddressText.getPosition().x + ipAddressText.getLocalBounds().width + 2;
    float cursorY = ipAddressText.getPosition().y;
    cursor.setPosition(cursorX, cursorY);

    // Cursor blink timer
    Clock cursorBlinkClock;
    bool cursorVisible = true;

    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
            {
                window.close();
                return false;
            }

            // Handle text input
            if (event.type == Event::TextEntered)
            {
                if (ipAddressFocused && event.text.unicode < 128)
                {
                    // Handle backspace
                    if (event.text.unicode == 8 && !ipAddressInput.empty())
                    {
                        ipAddressInput.pop_back();
                    }
                    // Handle printable characters
                    else if (event.text.unicode >= 32 && event.text.unicode < 127)
                    {
                        ipAddressInput += static_cast<char>(event.text.unicode);
                    }
                    ipAddressText.setString(ipAddressInput);
                }
                else if (portFocused && event.text.unicode < 128)
                {
                    // Handle backspace
                    if (event.text.unicode == 8 && !portInput.empty())
                    {
                        portInput.pop_back();
                    }
                    // Handle digits only for port
                    else if (event.text.unicode >= '0' && event.text.unicode <= '9' && portInput.length() < 5)
                    {
                        portInput += static_cast<char>(event.text.unicode);
                    }
                    portText.setString(portInput);
                }
            }

            if (event.type == Event::MouseButtonPressed)
            {
                Vector2i mousePos = Mouse::getPosition(window);

                // Check if IP address box is clicked
                if (ipAddressBox.getGlobalBounds().contains(mousePos.x, mousePos.y))
                {
                    ipAddressFocused = true;
                    portFocused = false;
                    cursorBlinkClock.restart();
                    cursorVisible = true;
                }
                // Check if port box is clicked
                else if (portBox.getGlobalBounds().contains(mousePos.x, mousePos.y))
                {
                    ipAddressFocused = false;
                    portFocused = true;
                    cursorBlinkClock.restart();
                    cursorVisible = true;
                }
                // Check if start button is clicked
                else if (startButton.getGlobalBounds().contains(mousePos.x, mousePos.y))
                {
                    // Try to parse port
                    try
                    {
                        serverPort = stoi(portInput);
                        if (serverPort < 1024 || serverPort > 65535)
                        {
                            statusText.setString("Port must be between 1024 and 65535");
                            statusText.setFillColor(Color::Red);
                            statusText.setPosition(centerX - statusText.getLocalBounds().width / 2, statusText.getPosition().y);
                            continue;
                        }
                    }
                    catch (const exception &e)
                    {
                        statusText.setString("Invalid port number");
                        statusText.setFillColor(Color::Red);
                        statusText.setPosition(centerX - statusText.getLocalBounds().width / 2, statusText.getPosition().y);
                        continue;
                    }

                    // Store IP address (for client)
                    if (!isHost)
                    {
                        serverAddress = ipAddressInput;
                    }

                    // Initialize network
                    if (isHost)
                    {
                        statusText.setString("Starting server...");
                        statusText.setFillColor(Color::Yellow);
                        statusText.setPosition(centerX - statusText.getLocalBounds().width / 2, statusText.getPosition().y);
                        window.draw(statusText);
                        window.display();

                        if (startNetworkHost())
                        {
                            return true;
                        }
                        else
                        {
                            statusText.setString("Failed to start server");
                            statusText.setFillColor(Color::Red);
                            statusText.setPosition(centerX - statusText.getLocalBounds().width / 2, statusText.getPosition().y);
                        }
                    }
                    else
                    {
                        statusText.setString("Connecting to server...");
                        statusText.setFillColor(Color::Yellow);
                        statusText.setPosition(centerX - statusText.getLocalBounds().width / 2, statusText.getPosition().y);
                        window.draw(statusText);
                        window.display();

                        if (joinNetworkGame(serverAddress, serverPort))
                        {
                            return true;
                        }
                        else
                        {
                            statusText.setString("Failed to connect to server");
                            statusText.setFillColor(Color::Red);
                            statusText.setPosition(centerX - statusText.getLocalBounds().width / 2, statusText.getPosition().y);
                        }
                    }
                }
                // Check if back button is clicked
                else if (backButton.getGlobalBounds().contains(mousePos.x, mousePos.y))
                {
                    return false;
                }
                else
                {
                    ipAddressFocused = false;
                    portFocused = false;
                }
            }
        }

        // Update cursor position
        if (ipAddressFocused)
        {
            cursorX = ipAddressText.getPosition().x + ipAddressText.getLocalBounds().width + 2;
            cursorY = ipAddressText.getPosition().y;
        }
        else if (portFocused)
        {
            cursorX = portText.getPosition().x + portText.getLocalBounds().width + 2;
            cursorY = portText.getPosition().y;
        }
        cursor.setPosition(cursorX, cursorY);

        // Blink cursor
        if (cursorBlinkClock.getElapsedTime().asSeconds() > 0.5f)
        {
            cursorVisible = !cursorVisible;
            cursorBlinkClock.restart();
        }

        // Highlight buttons on hover
        Vector2i mousePos = Mouse::getPosition(window);
        startButton.setFillColor(startButton.getGlobalBounds().contains(mousePos.x, mousePos.y) ? Color(70, 70, 70, 220) : Color(50, 50, 50, 200));
        backButton.setFillColor(backButton.getGlobalBounds().contains(mousePos.x, mousePos.y) ? Color(70, 70, 70, 220) : Color(50, 50, 50, 200));

        // Highlight focused input box
        ipAddressBox.setOutlineColor(ipAddressFocused ? Color::Yellow : Color::White);
        portBox.setOutlineColor(portFocused ? Color::Yellow : Color::White);

        window.clear();
        window.draw(menuSprite);
        window.draw(titleText);

        // Only draw IP address input for client
        if (!isHost)
        {
            window.draw(ipAddressLabel);
            window.draw(ipAddressBox);
            window.draw(ipAddressText);
        }

        window.draw(portLabel);
        window.draw(portBox);
        window.draw(portText);
        window.draw(startButton);
        window.draw(startButtonText);
        window.draw(backButton);
        window.draw(backButtonText);
        window.draw(statusText);

        // Draw cursor if focused and visible
        if ((ipAddressFocused || portFocused) && cursorVisible)
        {
            window.draw(cursor);
        }

        window.display();
    }

    return false;
}

bool ChessBoard::startNetworkHost()
{
    // Create network manager if it doesn't exist
    if (!network)
    {
        network = make_unique<NetworkManager>();
    }

    // Start server
    if (!network->startServer(serverPort))
    {
        cout << "Failed to start server on port " << serverPort << endl;
        return false;
    }

    cout << "Server started on port " << serverPort << endl;
    cout << "Local IP address: " << IpAddress::getLocalAddress().toString() << endl;

    // Set flags
    waitingForOpponent = true;
    opponentConnected = false;
    waitingForMove = false;

    // Set callback for network messages
    network->setMessageCallback([this](const NetworkMessage &message)
                                { onNetworkMessage(message); });

    return true;
}

bool ChessBoard::joinNetworkGame(const string &address, unsigned short port)
{
    // Create network manager if it doesn't exist
    if (!network)
    {
        network = make_unique<NetworkManager>();
    }

    // Connect to server
    if (!network->connectToServer(address, port))
    {
        cout << "Failed to connect to server at " << address << ":" << port << endl;
        return false;
    }

    cout << "Connected to server at " << address << ":" << port << endl;

    // Set flags
    waitingForOpponent = false;
    opponentConnected = true;
    waitingForMove = true; // Client (black) waits for host (white) to move first

    // Set callback for network messages
    network->setMessageCallback([this](const NetworkMessage &message)
                                { onNetworkMessage(message); });

    // Send ready message
    NetworkMessage readyMessage(MessageType::Ready, "");
    network->sendToServer(readyMessage);

    return true;
}

void ChessBoard::handleNetworkMessages()
{
    if (!network)
    {
        return;
    }

    try
    {
        // Process all pending messages
        while (network->hasMessages())
        {
            try
            {
                NetworkMessage message = network->getNextMessage();
                onNetworkMessage(message);
            }
            catch (const exception &e)
            {
                cout << "Exception handling network message: " << e.what() << endl;
                // Continue processing other messages
            }
        }
    }
    catch (const exception &e)
    {
        cout << "Exception in handleNetworkMessages: " << e.what() << endl;
    }
}

void ChessBoard::sendMoveToOpponent(int fromX, int fromY, int toX, int toY)
{
    if (!network || !opponentConnected)
    {
        return;
    }

    // Create move message
    string moveData = NetworkManager::moveToString(fromX, fromY, toX, toY);
    NetworkMessage moveMessage(MessageType::Move, moveData);

    // Send message
    if (currentMode == GameMode::LANHost)
    {
        network->sendToAllClients(moveMessage);
    }
    else
    {
        network->sendToServer(moveMessage);
    }
}

void ChessBoard::processNetworkMove(const string &moveData)
{
    try
    {
        // Parse move data
        int fromX, fromY, toX, toY;
        NetworkManager::stringToMove(moveData, fromX, fromY, toX, toY);

        // Validate coordinates
        if (fromX < 0 || fromX >= BOARD_SIZE || fromY < 0 || fromY >= BOARD_SIZE ||
            toX < 0 || toX >= BOARD_SIZE || toY < 0 || toY >= BOARD_SIZE)
        {
            cout << "Invalid move coordinates received: " << moveData << endl;
            return;
        }

        // Apply move
        GameLogic logic(getMatrix(), *this);

        // Check if the move is valid
        if (!logic.isValidMove(fromX, fromY, toX, toY))
        {
            cout << "Invalid move received: " << moveData << endl;
            return;
        }

        logic.movePiece(fromX, fromY, toX, toY);

        // Update turn
        whiteTurn = !whiteTurn;
        waitingForMove = false;
    }
    catch (const exception &e)
    {
        cout << "Exception in processNetworkMove: " << e.what() << endl;
        throw; // Rethrow to be caught by caller
    }
}

void ChessBoard::onNetworkMessage(const NetworkMessage &message)
{
    try
    {
        switch (message.type)
        {
        case MessageType::Connect:
            cout << "Player connected: " << message.data << endl;
            opponentConnected = true;
            waitingForOpponent = false;

            // Set waiting state based on player color
            if (currentMode == GameMode::LANHost)
            {
                // Host (white) moves first
                waitingForMove = false;
            }
            else
            {
                // Client (black) waits for host (white) to move first
                waitingForMove = true;
            }
            break;

        case MessageType::Disconnect:
            cout << "Player disconnected: " << message.data << endl;
            opponentConnected = false;
            waitingForOpponent = true;
            break;

        case MessageType::Move:
            cout << "Received move: " << message.data << endl;
            try
            {
                processNetworkMove(message.data);
            }
            catch (const exception &e)
            {
                cout << "Error processing move: " << e.what() << endl;
            }
            break;

        case MessageType::Ready:
            cout << "Player is ready" << endl;
            opponentConnected = true;
            waitingForOpponent = false;

            // If we're the host, send a connect confirmation
            if (currentMode == GameMode::LANHost)
            {
                try
                {
                    NetworkMessage connectMessage(MessageType::Connect, "Game started");
                    network->sendToAllClients(connectMessage);
                }
                catch (const exception &e)
                {
                    cout << "Error sending connect message: " << e.what() << endl;
                }

                // Host (white) moves first, so we're not waiting for a move
                waitingForMove = false;
            }
            else
            {
                // Client (black) waits for host (white) to move first
                waitingForMove = true;
            }
            break;

        case MessageType::GameOver:
            cout << "Game over: " << message.data << endl;
            gameOver = true;
            whiteWon = message.data == "white";
            break;

        case MessageType::Error:
            cout << "Network error: " << message.data << endl;
            break;

        default:
            cout << "Unknown message type" << endl;
            break;
        }
    }
    catch (const exception &e)
    {
        cout << "Exception in onNetworkMessage: " << e.what() << endl;
    }
}
