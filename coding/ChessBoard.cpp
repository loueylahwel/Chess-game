#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include "GameLogic.h"
#include "ChessBoard.h"

using namespace std;
using namespace sf;

ChessBoard::ChessBoard() : window(VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "ChessGame"),
                           board(BOARD_SIZE, vector<int>(BOARD_SIZE, 0)),
                           gameOver(false),
                           whiteWon(false)
{
    // Initialize SQUARE_SIZE based on initial window dimensions
    SQUARE_SIZE = WINDOW_WIDTH / static_cast<float>(BOARD_SIZE);
}

vector<vector<int>> &ChessBoard::getMatrix()
{
    return board;
}

void ChessBoard::setPiece(int x, int y, int value)
{
    if (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE)
    {
        board[x][y] = value;
    }
}

int ChessBoard::getPiece(int x, int y) const
{
    if (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE)
    {
        return board[x][y];
    }
    return -1;
}

bool ChessBoard::loadTexture()
{
    string pieceNames[6] = {"Pawn", "Rook", "Knight", "Bishop", "Queen", "King"};

    for (int i = 0; i < 6; i++)
    {
        if (!blackpieces[i].loadFromFile("coding/images/black" + pieceNames[i] + ".png"))
        {
            cout << "Failed to load black " << pieceNames[i] << " texture" << endl;
            return false;
        }
        if (!whitepieces[i].loadFromFile("coding/images/white" + pieceNames[i] + ".png"))
        {
            cout << "Failed to load white " << pieceNames[i] << " texture" << endl;
            return false;
        }

        blackSprites[i].setTexture(blackpieces[i]);
        // float scale = (SQUARE_SIZE * PIECE_SCALE) / blackSprites[i].getTexture()->getSize().x; // Scaling moved
        // blackSprites[i].setScale(scale * SCALE_FACTOR, scale * SCALE_FACTOR); // Scaling moved

        whiteSprites[i].setTexture(whitepieces[i]);
        // scale = (SQUARE_SIZE * PIECE_SCALE) / whiteSprites[i].getTexture()->getSize().x; // Scaling moved
        // whiteSprites[i].setScale(scale * SCALE_FACTOR, scale * SCALE_FACTOR); // Scaling moved
    }
    return true;
}

void ChessBoard::initBoard()
{
    board = {
        {-6, -10, 0, 0, 0, 0, 10, 6},
        {-8, -10, 0, 0, 0, 0, 10, 8},
        {-7, -10, 0, 0, 0, 0, 10, 7},
        {-11, -10, 0, 0, 0, 0, 10, 11},
        {-9, -10, 0, 0, 0, 0, 10, 9},
        {-7, -10, 0, 0, 0, 0, 10, 7},
        {-8, -10, 0, 0, 0, 0, 10, 8},
        {-6, -10, 0, 0, 0, 0, 10, 6}};
}

void ChessBoard::initializePieces()
{
    // Initialize the 2D array (8x8) with empty sprites first
    pieceSprites.resize(BOARD_SIZE, vector<Sprite>(BOARD_SIZE));

    // Set up black pawns
    for (int i = 0; i < 8; ++i)
    {
        pieceSprites[i][1] = blackSprites[0]; // Pawn
    }

    // Set up white pawns
    for (int i = 0; i < 8; ++i)
    {
        pieceSprites[i][6] = whiteSprites[0]; // Pawn
    }

    // Set up other pieces
    int pieceIndices[] = {1, 2, 3, 4, 5};                    // Rook, Knight, Bishop, Queen, King
    int pieceCols[][2] = {{0, 7}, {1, 6}, {2, 5}, {3}, {4}}; // Positions

    for (int i = 0; i < 5; ++i)
    {
        int spriteIndex = pieceIndices[i];
        for (int j = 0; j < (i < 3 ? 2 : 1); ++j)
        {
            int col = pieceCols[i][j];
            // Black pieces (back row)
            pieceSprites[col][0] = blackSprites[spriteIndex];
            // White pieces (front row)
            pieceSprites[col][7] = whiteSprites[spriteIndex];
        }
    }
}

void ChessBoard::drawPiece(Sprite sprite, int x, int y)
{
    sprite.setPosition(x * SQUARE_SIZE + (SQUARE_SIZE - sprite.getGlobalBounds().width) / 2,
                       y * SQUARE_SIZE + (SQUARE_SIZE - sprite.getGlobalBounds().height) / 2);
    window.draw(sprite);
}

void ChessBoard::drawPieces()
{
    for (int x = 0; x < BOARD_SIZE; ++x)
    {
        for (int y = 0; y < BOARD_SIZE; ++y)
        {
            // Check if there's a piece at this position
            if (pieceSprites[x][y].getTexture() != nullptr)
            {
                drawPiece(pieceSprites[x][y], x, y);
            }
        }
    }
}

void ChessBoard::run()
{
    // Load menu background
    if (!menuTexture.loadFromFile("coding/images/mainscreen.jpg"))
    {
        cout << "Failed to load menu background!" << endl;
        return;
    }
    menuSprite.setTexture(menuTexture);

    // Scale the menu background to fit the window
    float scaleX = static_cast<float>(WINDOW_WIDTH) / menuTexture.getSize().x;
    float scaleY = static_cast<float>(WINDOW_HEIGHT) / menuTexture.getSize().y;
    menuSprite.setScale(scaleX, scaleY);

    // Load font for buttons
    if (!font.loadFromFile("coding/font/Arial.ttf"))
    {
        cout << "Failed to load font!" << endl;
        return;
    }

    // Show menu first
    if (!showMenu())
    {
        window.close();
        return;
    }

    // If user selected start, run the game
    runGame();
}

bool ChessBoard::showMenu()
{
    // Set window icon
    Image icon;
    if (!icon.loadFromFile("coding/images/logo.png"))
    {
        cout << "Failed to load window icon!" << endl;
    }
    else
    {
        window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
    }
    // Create start and exit buttons
    RectangleShape startButton(Vector2f(200, 60));
    startButton.setPosition(WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2 - 40);
    startButton.setFillColor(Color(50, 50, 50, 200));

    RectangleShape exitButton(Vector2f(200, 60));
    exitButton.setPosition(WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2 + 60);
    exitButton.setFillColor(Color(50, 50, 50, 200));

    Text startText("Start Game", font, 30);
    startText.setPosition(WINDOW_WIDTH / 2 - 80, WINDOW_HEIGHT / 2 - 30);
    startText.setFillColor(Color::White);

    Text exitText("Exit Game", font, 30);
    exitText.setPosition(WINDOW_WIDTH / 2 - 75, WINDOW_HEIGHT / 2 + 70);
    exitText.setFillColor(Color::White);

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

            if (event.type == Event::MouseButtonPressed)
            {
                Vector2i mousePos = Mouse::getPosition(window);
                if (startButton.getGlobalBounds().contains(mousePos.x, mousePos.y))
                {
                    return true; // Start game
                }
                else if (exitButton.getGlobalBounds().contains(mousePos.x, mousePos.y))
                {
                    return false; // Exit game
                }
            }
        }

        // Highlight buttons on hover
        Vector2i mousePos = Mouse::getPosition(window);
        if (startButton.getGlobalBounds().contains(mousePos.x, mousePos.y))
        {
            startButton.setFillColor(Color(70, 70, 70, 220));
        }
        else
        {
            startButton.setFillColor(Color(50, 50, 50, 200));
        }

        if (exitButton.getGlobalBounds().contains(mousePos.x, mousePos.y))
        {
            exitButton.setFillColor(Color(70, 70, 70, 220));
        }
        else
        {
            exitButton.setFillColor(Color(50, 50, 50, 200));
        }

        window.clear();
        window.draw(menuSprite);
        window.draw(startButton);
        window.draw(exitButton);
        window.draw(startText);
        window.draw(exitText);
        window.display();
    }

    return false;
}

bool ChessBoard::showGameOverWindow(bool whiteWinner)
{
    // Get current view to match scaling
    View currentView = window.getView();

    // Semi-transparent overlay using current view dimensions
    RectangleShape overlay(Vector2f(currentView.getSize().x, currentView.getSize().y));
    overlay.setPosition(currentView.getCenter() - currentView.getSize() / 2.f);
    overlay.setFillColor(Color(0, 0, 0, 180)); // Semi-transparent black

    // Game over panel - scaled to match viewport
    float panelWidth = currentView.getSize().x * 0.5f;
    float panelHeight = currentView.getSize().y * 0.4f;
    RectangleShape panel(Vector2f(panelWidth, panelHeight));
    panel.setPosition(
        currentView.getCenter().x - panelWidth / 2,
        currentView.getCenter().y - panelHeight / 2);
    panel.setFillColor(Color(50, 50, 50, 250));

    // Scale fonts based on view size
    float fontScaleFactor = std::min(currentView.getSize().x, currentView.getSize().y) / 773.0f;

    // Winner text
    Text winnerText;
    winnerText.setFont(font);
    winnerText.setString(whiteWinner ? "White Wins!" : "Black Wins!");
    winnerText.setCharacterSize(static_cast<unsigned int>(40 * fontScaleFactor));
    winnerText.setFillColor(Color::White);
    winnerText.setPosition(
        currentView.getCenter().x - winnerText.getLocalBounds().width / 2,
        panel.getPosition().y + panelHeight * 0.1f);

    // Analysis button
    float buttonWidth = panelWidth * 0.75f;
    float buttonHeight = panelHeight * 0.2f;
    RectangleShape analysisButton(Vector2f(buttonWidth, buttonHeight));
    analysisButton.setPosition(
        currentView.getCenter().x - buttonWidth / 2,
        panel.getPosition().y + panelHeight * 0.4f);
    analysisButton.setFillColor(Color(70, 70, 70, 220));

    Text analysisText("Proceed to Analysis", font, static_cast<unsigned int>(25 * fontScaleFactor));
    analysisText.setPosition(
        currentView.getCenter().x - analysisText.getLocalBounds().width / 2,
        analysisButton.getPosition().y + (buttonHeight - analysisText.getLocalBounds().height) / 2 - 5);
    analysisText.setFillColor(Color::White);

    // New game button
    RectangleShape newGameButton(Vector2f(buttonWidth, buttonHeight));
    newGameButton.setPosition(
        currentView.getCenter().x - buttonWidth / 2,
        panel.getPosition().y + panelHeight * 0.7f);
    newGameButton.setFillColor(Color(70, 70, 70, 220));

    Text newGameText("Play Another Game", font, static_cast<unsigned int>(25 * fontScaleFactor));
    newGameText.setPosition(
        currentView.getCenter().x - newGameText.getLocalBounds().width / 2,
        newGameButton.getPosition().y + (buttonHeight - newGameText.getLocalBounds().height) / 2 - 5);
    newGameText.setFillColor(Color::White);

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

            if (event.type == Event::MouseButtonPressed)
            {
                // Convert screen coordinates to world coordinates
                Vector2f mouseWorldPos = window.mapPixelToCoords(Vector2i(event.mouseButton.x, event.mouseButton.y));

                // Check if analysis button was clicked
                if (analysisButton.getGlobalBounds().contains(mouseWorldPos))
                {
                    // TODO: Implement analysis functionality
                    return false; // For now, just close the window
                }

                // Check if new game button was clicked
                if (newGameButton.getGlobalBounds().contains(mouseWorldPos))
                {
                    // Reset the game
                    initBoard();
                    initializePieces();
                    gameOver = false;
                    return true;
                }
            }

            // Handle window resize
            if (event.type == Event::Resized)
            {
                // Update the view
                updateBoardAndPieceSizes();
                currentView = window.getView();

                // Update all UI elements based on new view
                overlay.setSize(Vector2f(currentView.getSize().x, currentView.getSize().y));
                overlay.setPosition(currentView.getCenter() - currentView.getSize() / 2.f);

                panelWidth = currentView.getSize().x * 0.5f;
                panelHeight = currentView.getSize().y * 0.4f;
                panel.setSize(Vector2f(panelWidth, panelHeight));
                panel.setPosition(
                    currentView.getCenter().x - panelWidth / 2,
                    currentView.getCenter().y - panelHeight / 2);

                fontScaleFactor = std::min(currentView.getSize().x, currentView.getSize().y) / 773.0f;

                winnerText.setCharacterSize(static_cast<unsigned int>(40 * fontScaleFactor));
                winnerText.setPosition(
                    currentView.getCenter().x - winnerText.getLocalBounds().width / 2,
                    panel.getPosition().y + panelHeight * 0.1f);

                buttonWidth = panelWidth * 0.75f;
                buttonHeight = panelHeight * 0.2f;

                analysisButton.setSize(Vector2f(buttonWidth, buttonHeight));
                analysisButton.setPosition(
                    currentView.getCenter().x - buttonWidth / 2,
                    panel.getPosition().y + panelHeight * 0.4f);

                analysisText.setCharacterSize(static_cast<unsigned int>(25 * fontScaleFactor));
                analysisText.setPosition(
                    currentView.getCenter().x - analysisText.getLocalBounds().width / 2,
                    analysisButton.getPosition().y + (buttonHeight - analysisText.getLocalBounds().height) / 2 - 5);

                newGameButton.setSize(Vector2f(buttonWidth, buttonHeight));
                newGameButton.setPosition(
                    currentView.getCenter().x - buttonWidth / 2,
                    panel.getPosition().y + panelHeight * 0.7f);

                newGameText.setCharacterSize(static_cast<unsigned int>(25 * fontScaleFactor));
                newGameText.setPosition(
                    currentView.getCenter().x - newGameText.getLocalBounds().width / 2,
                    newGameButton.getPosition().y + (buttonHeight - newGameText.getLocalBounds().height) / 2 - 5);
            }
        }

        // Highlight buttons on hover
        Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));

        if (analysisButton.getGlobalBounds().contains(mousePos))
        {
            analysisButton.setFillColor(Color(100, 100, 100, 220));
        }
        else
        {
            analysisButton.setFillColor(Color(70, 70, 70, 220));
        }

        if (newGameButton.getGlobalBounds().contains(mousePos))
        {
            newGameButton.setFillColor(Color(100, 100, 100, 220));
        }
        else
        {
            newGameButton.setFillColor(Color(70, 70, 70, 220));
        }

        // Keep rendering the game in the background
        // Draw the game board (copied from runGame)
        for (int x = 0; x < BOARD_SIZE; x++)
        {
            for (int y = 0; y < BOARD_SIZE; y++)
            {
                RectangleShape square(Vector2f(SQUARE_SIZE, SQUARE_SIZE));
                square.setPosition(x * SQUARE_SIZE, y * SQUARE_SIZE);
                square.setFillColor((x + y) % 2 == 0 ? Color(244, 244, 244) : Color(105, 146, 62));
                window.draw(square);
            }
        }
        drawPieces();

        // Draw overlay and game over elements
        window.draw(overlay);
        window.draw(panel);
        window.draw(winnerText);
        window.draw(analysisButton);
        window.draw(analysisText);
        window.draw(newGameButton);
        window.draw(newGameText);

        window.display();
    }

    return false;
}

void ChessBoard::runGame()
{
    // Set window icon
    Image icon;
    if (!icon.loadFromFile("coding/images/logo.png"))
    {
        cout << "Failed to load window icon!" << endl;
    }
    else
    {
        window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
    }

    initBoard();
    if (!loadTexture())
    {
        cout << "Failed to load textures!" << endl;
        return;
    }
    initializePieces();

    // Initial calculation of sizes and scales
    updateBoardAndPieceSizes();

    GameLogic logic(getMatrix(), *this);
    bool pieceSelected = false;
    int selectedX = -1, selectedY = -1;
    bool whiteTurn = true; // White starts first
    vector<pair<int, int>> validMoves;
    // Create circle shapes for valid move indicators
    CircleShape moveIndicator(SQUARE_SIZE / 4);
    moveIndicator.setOrigin(moveIndicator.getRadius(), moveIndicator.getRadius());
    // Red circle for capture moves
    CircleShape captureIndicator(SQUARE_SIZE / 3);
    captureIndicator.setFillColor(Color(255, 0, 0, 150)); // Red semi-transparent
    captureIndicator.setOrigin(captureIndicator.getRadius(), captureIndicator.getRadius());

    while (window.isOpen())
    {
        // Check for checkmate at start of each loop
        if (!gameOver)
        {
            if (logic.checkMate(whiteTurn))
            {
                gameOver = true;
                whiteWon = !whiteTurn; // If it's white's turn and they're in checkmate, black won

                // Show game over screen
                bool continueGame = showGameOverWindow(whiteWon);
                if (continueGame)
                {
                    // Reset the game state
                    pieceSelected = false;
                    selectedX = -1;
                    selectedY = -1;
                    whiteTurn = true;
                    validMoves.clear();
                    gameOver = false;
                    continue;
                }
                else
                {
                    window.close();
                    return;
                }
            }
        }

        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
            {
                window.close();
            }
            // Handle window resizing
            else if (event.type == Event::Resized)
            {
                // Recalculate sizes and scales
                updateBoardAndPieceSizes();

                // Set the view to the board size
                float boardPixelSize = BOARD_SIZE * SQUARE_SIZE;
                View view(FloatRect(0, 0, boardPixelSize, boardPixelSize));

                // Calculate viewport to center the board and maintain aspect ratio
                float windowWidth = static_cast<float>(event.size.width);
                float windowHeight = static_cast<float>(event.size.height);
                float viewportX = 0, viewportY = 0, viewportWidth = 1.0, viewportHeight = 1.0;

                if (windowWidth > boardPixelSize)
                { // Letterbox (wider than tall)
                    viewportWidth = boardPixelSize / windowWidth;
                    viewportX = (1.0f - viewportWidth) / 2.0f;
                }
                else if (windowHeight > boardPixelSize)
                { // Pillarbox (taller than wide)
                    viewportHeight = boardPixelSize / windowHeight;
                    viewportY = (1.0f - viewportHeight) / 2.0f;
                }
                // If window is smaller than board in one dimension, the view still scales it down,
                // but we adjust the viewport if the *other* dimension is larger.
                else if (windowWidth < boardPixelSize && windowHeight > windowWidth)
                {                                                                                      // Taller than wide aspect, but board wider than window
                    viewportHeight = (boardPixelSize / boardPixelSize) * (windowWidth / windowHeight); // Aspect ratio correction
                    viewportY = (1.0f - viewportHeight) / 2.0f;
                }
                else if (windowHeight < boardPixelSize && windowWidth > windowHeight)
                {                                                                                     // Wider than tall aspect, but board taller than window
                    viewportWidth = (boardPixelSize / boardPixelSize) * (windowHeight / windowWidth); // Aspect ratio correction
                    viewportX = (1.0f - viewportWidth) / 2.0f;
                }

                view.setViewport(FloatRect(viewportX, viewportY, viewportWidth, viewportHeight));
                window.setView(view);
            }
            else if (event.type == Event::MouseButtonPressed)
            {
                // Map pixel coordinates to world coordinates (using the current view)
                Vector2f worldPos = window.mapPixelToCoords(Vector2i(event.mouseButton.x, event.mouseButton.y));

                // Check if click is within the board boundaries (using the view's coordinate system)
                float boardPixelSize = BOARD_SIZE * SQUARE_SIZE;
                if (worldPos.x >= 0 && worldPos.x < boardPixelSize && worldPos.y >= 0 && worldPos.y < boardPixelSize)
                {
                    // Calculate board coordinates using world coordinates and the current SQUARE_SIZE
                    int boardX = static_cast<int>(worldPos.x / SQUARE_SIZE);
                    int boardY = static_cast<int>(worldPos.y / SQUARE_SIZE);

                    // Ensure calculated indices are within bounds (can be slightly off due to float precision)
                    boardX = std::max(0, std::min(BOARD_SIZE - 1, boardX));
                    boardY = std::max(0, std::min(BOARD_SIZE - 1, boardY));

                    cout << "Clicked Board: [" << boardX << "][" << boardY
                         << "] Mapped Coords: (" << worldPos.x << "," << worldPos.y
                         << ") Value: " << getPiece(boardX, boardY) << endl;

                    if (!pieceSelected)
                    {
                        // Only allow selecting pieces of the current turn's color
                        if ((whiteTurn && getPiece(boardX, boardY) > 0) ||
                            (!whiteTurn && getPiece(boardX, boardY) < 0))
                        {
                            selectedX = boardX;
                            selectedY = boardY;
                            pieceSelected = true;
                            validMoves.clear();
                            for (int x = 0; x < BOARD_SIZE; x++)
                            {
                                for (int y = 0; y < BOARD_SIZE; y++)
                                {
                                    if (logic.isValidMove(selectedX, selectedY, x, y))
                                    {
                                        // Only add moves that are either empty or opponent pieces
                                        if (getPiece(x, y) == 0 ||
                                            (whiteTurn && getPiece(x, y) < 0) ||
                                            (!whiteTurn && getPiece(x, y) > 0))
                                        {
                                            // Check if this move would leave the king in check
                                            if (!logic.wouldBeInCheck(selectedX, selectedY, x, y, whiteTurn))
                                            {
                                                validMoves.emplace_back(x, y);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        // Check if the move is valid
                        bool isValid = false;
                        for (const auto &move : validMoves)
                        {
                            if (move.first == boardX && move.second == boardY)
                            {
                                isValid = true;
                                break;
                            }
                        }
                        if (isValid)
                        {
                            logic.movePiece(selectedX, selectedY, boardX, boardY);
                            whiteTurn = !whiteTurn; // Switch turns
                            pieceSelected = false;
                            validMoves.clear();

                            // Check for checkmate after move
                            if (logic.checkMate(!whiteTurn))
                            {
                                gameOver = true;
                                whiteWon = whiteTurn; // Current player won

                                // Show game over screen
                                bool continueGame = showGameOverWindow(whiteWon);
                                if (continueGame)
                                {
                                    // Reset the game state
                                    pieceSelected = false;
                                    selectedX = -1;
                                    selectedY = -1;
                                    whiteTurn = true;
                                    validMoves.clear();
                                    gameOver = false;
                                }
                                else
                                {
                                    window.close();
                                    return;
                                }
                            }
                        }
                        else if ((whiteTurn && getPiece(boardX, boardY) > 0) ||
                                 (!whiteTurn && getPiece(boardX, boardY) < 0))
                        {
                            // Allow selecting a different piece of the same color
                            selectedX = boardX;
                            selectedY = boardY;
                            validMoves.clear();
                            for (int x = 0; x < BOARD_SIZE; x++)
                            {
                                for (int y = 0; y < BOARD_SIZE; y++)
                                {
                                    if (logic.isValidMove(selectedX, selectedY, x, y))
                                    {
                                        if (getPiece(x, y) == 0 ||
                                            (whiteTurn && getPiece(x, y) < 0) ||
                                            (!whiteTurn && getPiece(x, y) > 0))
                                        {
                                            // Check if this move would leave the king in check
                                            if (!logic.wouldBeInCheck(selectedX, selectedY, x, y, whiteTurn))
                                            {
                                                validMoves.emplace_back(x, y);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else
                        {
                            // Clicked on invalid square - deselect
                            pieceSelected = false;
                            validMoves.clear();
                        }
                    }
                } // End check if click is within board boundaries
            }
        }

        window.clear(Color::Black); // Clear with black for letter/pillarboxing
        // Draw board
        for (int x = 0; x < BOARD_SIZE; x++)
        {
            for (int y = 0; y < BOARD_SIZE; y++)
            {
                RectangleShape square(Vector2f(SQUARE_SIZE, SQUARE_SIZE));
                square.setPosition(x * SQUARE_SIZE, y * SQUARE_SIZE);

                // Highlight selected piece
                if (pieceSelected && x == selectedX && y == selectedY)
                {
                    square.setFillColor(Color(247, 247, 105)); // Yellow highlight
                }
                else
                {
                    square.setFillColor((x + y) % 2 == 0 ? Color(244, 244, 244) : Color(105, 146, 62)); // green + white board
                }
                window.draw(square);
            }
        }

        // Draw valid move indicators
        for (const auto &move : validMoves)
        {
            int x = move.first;
            int y = move.second;
            float centerX = x * SQUARE_SIZE + SQUARE_SIZE / 2;
            float centerY = y * SQUARE_SIZE + SQUARE_SIZE / 2;

            if (getPiece(x, y) == 0)
            {
                // Empty square - gray circle
                moveIndicator.setFillColor(Color(50, 50, 50, 180));
                moveIndicator.setPosition(centerX, centerY);
                window.draw(moveIndicator);
            }
            else
            {
                // Opponent's piece - red circle
                captureIndicator.setPosition(centerX, centerY);
                window.draw(captureIndicator);
            }
        }
        drawPieces();
        window.display();
    }
}

// New method to calculate sizes and scales
void ChessBoard::updateBoardAndPieceSizes()
{
    Vector2u windowSize = window.getSize();
    float minDimension = std::min(windowSize.x, windowSize.y);
    SQUARE_SIZE = minDimension / static_cast<float>(BOARD_SIZE);

    // Update scales for the template sprites
    for (int i = 0; i < 6; ++i)
    {
        if (blackSprites[i].getTexture())
        { // Check if texture exists
            float scale = (SQUARE_SIZE * PIECE_SCALE) / blackSprites[i].getTexture()->getSize().x;
            blackSprites[i].setScale(scale * SCALE_FACTOR, scale * SCALE_FACTOR);
        }
        if (whiteSprites[i].getTexture())
        { // Check if texture exists
            float scale = (SQUARE_SIZE * PIECE_SCALE) / whiteSprites[i].getTexture()->getSize().x;
            whiteSprites[i].setScale(scale * SCALE_FACTOR, scale * SCALE_FACTOR);
        }
    }

    // Update scales for the actual sprites on the board
    for (int x = 0; x < BOARD_SIZE; ++x)
    {
        for (int y = 0; y < BOARD_SIZE; ++y)
        {
            int pieceValue = getPiece(x, y);
            if (pieceValue != 0)
            {
                int index = -1;
                bool isWhite = pieceValue > 0;
                int absValue = abs(pieceValue);

                // Map piece value to sprite index (adjust based on your piece values)
                switch (absValue)
                {
                case 10:
                    index = 0;
                    break; // Pawn
                case 6:
                    index = 1;
                    break; // Rook
                case 8:
                    index = 2;
                    break; // Knight (Assuming 8)
                case 7:
                    index = 3;
                    break; // Bishop
                case 11:
                    index = 4;
                    break; // Queen
                case 9:
                    index = 5;
                    break; // King
                }

                if (index != -1)
                {
                    Sprite &targetSprite = isWhite ? whiteSprites[index] : blackSprites[index];
                    pieceSprites[x][y].setScale(targetSprite.getScale()); // Apply scale from template
                }
            }
        }
    }
}