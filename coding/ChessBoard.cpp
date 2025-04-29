#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include "GameLogic.h"
#include "ChessBoard.h"

using namespace std;
using namespace sf;

// Define the global variables here (declared as extern in ChessBoard.h)
int WINDOW_WIDTH = 700;  // Reduced from 773
int WINDOW_HEIGHT = 700; // Reduced from 773
float SCALE_FACTOR = 0.75;
int BOARD_SIZE = 8;
float PIECE_SCALE = 0.9f;

ChessBoard::ChessBoard() : window(VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "ChessGame"),
                           board(BOARD_SIZE, vector<int>(BOARD_SIZE, 0)),
                           gameOver(false),
                           whiteWon(false),
                           whiteTurn(true),
                           currentMode(GameMode::TwoPlayer),
                           playerIsWhite(true),
                           computerDifficulty(ComputerDifficulty::Medium),
                           engine(nullptr),
                           currentPosition(""),
                           network(nullptr),
                           serverPort(50000),
                           waitingForOpponent(false),
                           opponentConnected(false),
                           waitingForMove(false)
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
    // Chess pieces are represented with numbers:
    // Positive for white, negative for black
    // 6: Rook, 7: Bishop, 8: Knight, 9: King, 10: Pawn, 11: Queen

    // Clear the board first
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            board[i][j] = 0;
        }
    }

    // Set up proper chess board
    // First row (black pieces)
    board[0][0] = -6;  // Black rook
    board[1][0] = -8;  // Black knight
    board[2][0] = -7;  // Black bishop
    board[3][0] = -11; // Black queen
    board[4][0] = -9;  // Black king
    board[5][0] = -7;  // Black bishop
    board[6][0] = -8;  // Black knight
    board[7][0] = -6;  // Black rook

    // Second row (black pawns)
    for (int i = 0; i < 8; i++)
    {
        board[i][1] = -10; // Black pawns
    }

    // Seventh row (white pawns)
    for (int i = 0; i < 8; i++)
    {
        board[i][6] = 10; // White pawns
    }

    // Eighth row (white pieces)
    board[0][7] = 6;  // White rook
    board[1][7] = 8;  // White knight
    board[2][7] = 7;  // White bishop
    board[3][7] = 11; // White queen
    board[4][7] = 9;  // White king
    board[5][7] = 7;  // White bishop
    board[6][7] = 8;  // White knight
    board[7][7] = 6;  // White rook
}

void ChessBoard::initializePieces()
{
    // Initialize the 2D array (8x8) with empty sprites first
    pieceSprites.resize(BOARD_SIZE, vector<Sprite>(BOARD_SIZE));

    // Clear all sprites
    for (int x = 0; x < BOARD_SIZE; x++)
    {
        for (int y = 0; y < BOARD_SIZE; y++)
        {
            pieceSprites[x][y] = Sprite(); // Empty sprite
        }
    }

    // Initialize pieces based on board values
    for (int x = 0; x < BOARD_SIZE; x++)
    {
        for (int y = 0; y < BOARD_SIZE; y++)
        {
            int pieceValue = board[x][y];
            if (pieceValue != 0)
            {
                bool isWhite = pieceValue > 0;
                int absValue = abs(pieceValue);
                int spriteIndex = -1;

                // Map piece value to sprite index
                switch (absValue)
                {
                case 10:
                    spriteIndex = 0;
                    break; // Pawn
                case 6:
                    spriteIndex = 1;
                    break; // Rook
                case 8:
                    spriteIndex = 2;
                    break; // Knight
                case 7:
                    spriteIndex = 3;
                    break; // Bishop
                case 11:
                    spriteIndex = 4;
                    break; // Queen
                case 9:
                    spriteIndex = 5;
                    break; // King
                }

                if (spriteIndex != -1)
                {
                    pieceSprites[x][y] = isWhite ? whiteSprites[spriteIndex] : blackSprites[spriteIndex];
                }
            }
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

    // Initialize Stockfish if needed
    if (currentMode == GameMode::VsComputer)
    {
        engine = make_unique<StockfishEngine>();
        if (!engine->initialize())
        {
            cout << "Failed to initialize Stockfish engine. Reverting to two-player mode." << endl;
            currentMode = GameMode::TwoPlayer;
        }
        else
        {
            engine->setDifficulty(static_cast<int>(computerDifficulty));
            moveHistory.clear();
            currentPosition = "";
        }
    }
    // For LAN games, network initialization is handled in showNetworkOptions

    // Initialize the game
    resetGame();

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

    // Create buttons
    float buttonHeight = 60;
    float buttonSpacing = 20;
    float totalHeight = 4 * buttonHeight + 3 * buttonSpacing;
    float startY = WINDOW_HEIGHT / 2 - totalHeight / 2;

    RectangleShape twoPlayerButton(Vector2f(300, buttonHeight));
    twoPlayerButton.setPosition(WINDOW_WIDTH / 2 - 150, startY);
    twoPlayerButton.setFillColor(Color(50, 50, 50, 200));

    RectangleShape vsComputerButton(Vector2f(300, buttonHeight));
    vsComputerButton.setPosition(WINDOW_WIDTH / 2 - 150, startY + buttonHeight + buttonSpacing);
    vsComputerButton.setFillColor(Color(50, 50, 50, 200));

    RectangleShape hostLANButton(Vector2f(300, buttonHeight));
    hostLANButton.setPosition(WINDOW_WIDTH / 2 - 150, startY + 2 * (buttonHeight + buttonSpacing));
    hostLANButton.setFillColor(Color(50, 50, 50, 200));

    RectangleShape joinLANButton(Vector2f(300, buttonHeight));
    joinLANButton.setPosition(WINDOW_WIDTH / 2 - 150, startY + 3 * (buttonHeight + buttonSpacing));
    joinLANButton.setFillColor(Color(50, 50, 50, 200));

    RectangleShape exitButton(Vector2f(300, buttonHeight));
    exitButton.setPosition(WINDOW_WIDTH / 2 - 150, startY + 4 * (buttonHeight + buttonSpacing));
    exitButton.setFillColor(Color(50, 50, 50, 200));

    float fontSize = 30;
    Text twoPlayerText("Play Two-Player", font, fontSize);
    twoPlayerText.setPosition(WINDOW_WIDTH / 2 - twoPlayerText.getLocalBounds().width / 2,
                              twoPlayerButton.getPosition().y + (buttonHeight - twoPlayerText.getLocalBounds().height) / 2);
    twoPlayerText.setFillColor(Color::White);

    Text vsComputerText("Play vs Computer", font, fontSize);
    vsComputerText.setPosition(WINDOW_WIDTH / 2 - vsComputerText.getLocalBounds().width / 2,
                               vsComputerButton.getPosition().y + (buttonHeight - vsComputerText.getLocalBounds().height) / 2);
    vsComputerText.setFillColor(Color::White);

    Text hostLANText("Host LAN Game", font, fontSize);
    hostLANText.setPosition(WINDOW_WIDTH / 2 - hostLANText.getLocalBounds().width / 2,
                            hostLANButton.getPosition().y + (buttonHeight - hostLANText.getLocalBounds().height) / 2);
    hostLANText.setFillColor(Color::White);

    Text joinLANText("Join LAN Game", font, fontSize);
    joinLANText.setPosition(WINDOW_WIDTH / 2 - joinLANText.getLocalBounds().width / 2,
                            joinLANButton.getPosition().y + (buttonHeight - joinLANText.getLocalBounds().height) / 2);
    joinLANText.setFillColor(Color::White);

    Text exitText("Exit Game", font, fontSize);
    exitText.setPosition(WINDOW_WIDTH / 2 - exitText.getLocalBounds().width / 2,
                         exitButton.getPosition().y + (buttonHeight - exitText.getLocalBounds().height) / 2);
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
                if (twoPlayerButton.getGlobalBounds().contains(mousePos.x, mousePos.y))
                {
                    currentMode = GameMode::TwoPlayer;
                    return true; // Start game
                }
                else if (vsComputerButton.getGlobalBounds().contains(mousePos.x, mousePos.y))
                {
                    currentMode = GameMode::VsComputer;
                    if (!showComputerOptions())
                    {
                        return false; // User cancelled
                    }
                    return true; // Start game with computer
                }
                else if (hostLANButton.getGlobalBounds().contains(mousePos.x, mousePos.y))
                {
                    currentMode = GameMode::LANHost;
                    playerIsWhite = true; // Host always plays as white
                    if (!showNetworkOptions(true))
                    {
                        return false; // User cancelled
                    }
                    return true; // Start LAN game as host
                }
                else if (joinLANButton.getGlobalBounds().contains(mousePos.x, mousePos.y))
                {
                    currentMode = GameMode::LANClient;
                    playerIsWhite = false; // Client always plays as black
                    if (!showNetworkOptions(false))
                    {
                        return false; // User cancelled
                    }
                    return true; // Start LAN game as client
                }
                else if (exitButton.getGlobalBounds().contains(mousePos.x, mousePos.y))
                {
                    return false; // Exit game
                }
            }
        }

        // Highlight buttons on hover
        Vector2i mousePos = Mouse::getPosition(window);
        twoPlayerButton.setFillColor(twoPlayerButton.getGlobalBounds().contains(mousePos.x, mousePos.y) ? Color(70, 70, 70, 220) : Color(50, 50, 50, 200));
        vsComputerButton.setFillColor(vsComputerButton.getGlobalBounds().contains(mousePos.x, mousePos.y) ? Color(70, 70, 70, 220) : Color(50, 50, 50, 200));
        hostLANButton.setFillColor(hostLANButton.getGlobalBounds().contains(mousePos.x, mousePos.y) ? Color(70, 70, 70, 220) : Color(50, 50, 50, 200));
        joinLANButton.setFillColor(joinLANButton.getGlobalBounds().contains(mousePos.x, mousePos.y) ? Color(70, 70, 70, 220) : Color(50, 50, 50, 200));
        exitButton.setFillColor(exitButton.getGlobalBounds().contains(mousePos.x, mousePos.y) ? Color(70, 70, 70, 220) : Color(50, 50, 50, 200));

        window.clear();
        window.draw(menuSprite);
        window.draw(twoPlayerButton);
        window.draw(vsComputerButton);
        window.draw(hostLANButton);
        window.draw(joinLANButton);
        window.draw(exitButton);
        window.draw(twoPlayerText);
        window.draw(vsComputerText);
        window.draw(hostLANText);
        window.draw(joinLANText);
        window.draw(exitText);
        window.display();
    }

    return false;
}

bool ChessBoard::showComputerOptions()
{
    // Get the same scaling factors as used in the main menu
    float scaleX = static_cast<float>(WINDOW_WIDTH) / menuTexture.getSize().x;
    float scaleY = static_cast<float>(WINDOW_HEIGHT) / menuTexture.getSize().y;
    menuSprite.setScale(scaleX, scaleY);

    // Calculate scalable positions based on window dimensions
    centerX = WINDOW_WIDTH / 2.0f;
    centerY = WINDOW_HEIGHT / 2.0f;
    buttonWidth = WINDOW_WIDTH * 0.2f;
    buttonHeight = WINDOW_HEIGHT * 0.08f;
    verticalOffset = -buttonHeight;
    verticalSpacing = WINDOW_HEIGHT * 0.03f;
    difficultyY = centerY + verticalOffset + buttonHeight + verticalSpacing * 3;
    backButtonY = difficultyY + buttonHeight + verticalSpacing * 3;

    // Create buttons and text for computer options
    RectangleShape colorWhiteButton(Vector2f(buttonWidth, buttonHeight));
    colorWhiteButton.setPosition(centerX - buttonWidth - (WINDOW_WIDTH * 0.02f), centerY + verticalOffset);
    colorWhiteButton.setFillColor(Color(50, 50, 50, 200));

    RectangleShape colorBlackButton(Vector2f(buttonWidth, buttonHeight));
    colorBlackButton.setPosition(centerX + (WINDOW_WIDTH * 0.02f), centerY + verticalOffset);
    colorBlackButton.setFillColor(Color(50, 50, 50, 200));

    // Difficulty Buttons (5 buttons with adjusted width)
    float difficultyButtonWidth = WINDOW_WIDTH * 0.1f; // Smaller buttons to fit 5
    float horizontalSpacing = WINDOW_WIDTH * 0.01f;    // Spacing between buttons
    float totalDifficultyWidth = 5 * difficultyButtonWidth + 4 * horizontalSpacing;
    float firstDifficultyX = centerX - totalDifficultyWidth / 2.0f;

    RectangleShape easyButton(Vector2f(difficultyButtonWidth, buttonHeight));
    easyButton.setPosition(firstDifficultyX, difficultyY);
    easyButton.setFillColor(Color(50, 50, 50, 200));

    RectangleShape kindaEasyButton(Vector2f(difficultyButtonWidth, buttonHeight));
    kindaEasyButton.setPosition(firstDifficultyX + difficultyButtonWidth + horizontalSpacing, difficultyY);
    kindaEasyButton.setFillColor(Color(50, 50, 50, 200));

    RectangleShape mediumButton(Vector2f(difficultyButtonWidth, buttonHeight));
    mediumButton.setPosition(firstDifficultyX + 2 * (difficultyButtonWidth + horizontalSpacing), difficultyY);
    mediumButton.setFillColor(Color(50, 50, 50, 200));

    RectangleShape kindaMediumButton(Vector2f(difficultyButtonWidth, buttonHeight));
    kindaMediumButton.setPosition(firstDifficultyX + 3 * (difficultyButtonWidth + horizontalSpacing), difficultyY);
    kindaMediumButton.setFillColor(Color(50, 50, 50, 200));

    RectangleShape hardButton(Vector2f(difficultyButtonWidth, buttonHeight));
    hardButton.setPosition(firstDifficultyX + 4 * (difficultyButtonWidth + horizontalSpacing), difficultyY);
    hardButton.setFillColor(Color(50, 50, 50, 200));

    RectangleShape backButton(Vector2f(buttonWidth, buttonHeight));
    backButton.setPosition(centerX - buttonWidth / 2, backButtonY);
    backButton.setFillColor(Color(50, 50, 50, 200));

    // Prepare font sizes
    float largeFontSize = WINDOW_HEIGHT * 0.04f;
    float mediumFontSize = WINDOW_HEIGHT * 0.035f;
    float smallFontSize = WINDOW_HEIGHT * 0.025f; // Smaller for 5 buttons

    // Button texts
    chooseColorText.setFont(font);
    chooseColorText.setString("Choose your color:");
    chooseColorText.setCharacterSize(largeFontSize);
    chooseColorText.setPosition(centerX - chooseColorText.getLocalBounds().width / 2,
                                centerY + verticalOffset - buttonHeight);
    chooseColorText.setFillColor(Color::White);

    whiteText.setFont(font);
    whiteText.setString("White");
    whiteText.setCharacterSize(mediumFontSize);
    whiteText.setPosition(colorWhiteButton.getPosition().x + (buttonWidth - whiteText.getLocalBounds().width) / 2,
                          colorWhiteButton.getPosition().y + (buttonHeight - whiteText.getLocalBounds().height) / 2);
    whiteText.setFillColor(Color::White);

    blackText.setFont(font);
    blackText.setString("Black");
    blackText.setCharacterSize(mediumFontSize);
    blackText.setPosition(colorBlackButton.getPosition().x + (buttonWidth - blackText.getLocalBounds().width) / 2,
                          colorBlackButton.getPosition().y + (buttonHeight - blackText.getLocalBounds().height) / 2);
    blackText.setFillColor(Color::White);

    difficultyText.setFont(font);
    difficultyText.setString("Choose difficulty:");
    difficultyText.setCharacterSize(largeFontSize);
    difficultyText.setPosition(centerX - difficultyText.getLocalBounds().width / 2,
                               difficultyY - buttonHeight);
    difficultyText.setFillColor(Color::White);

    // Button texts for difficulty levels
    Text easyText("Easy", font, smallFontSize);
    easyText.setPosition(easyButton.getPosition().x + (difficultyButtonWidth - easyText.getLocalBounds().width) / 2,
                         easyButton.getPosition().y + (buttonHeight - easyText.getLocalBounds().height) / 2);
    easyText.setFillColor(Color::White);

    Text kindaEasyText("K-Easy", font, smallFontSize);
    kindaEasyText.setPosition(kindaEasyButton.getPosition().x + (difficultyButtonWidth - kindaEasyText.getLocalBounds().width) / 2,
                              kindaEasyButton.getPosition().y + (buttonHeight - kindaEasyText.getLocalBounds().height) / 2);
    kindaEasyText.setFillColor(Color::White);

    Text mediumText("Medium", font, smallFontSize);
    mediumText.setPosition(mediumButton.getPosition().x + (difficultyButtonWidth - mediumText.getLocalBounds().width) / 2,
                           mediumButton.getPosition().y + (buttonHeight - mediumText.getLocalBounds().height) / 2);
    mediumText.setFillColor(Color::White);

    Text kindaMediumText("K-Medium", font, smallFontSize);
    kindaMediumText.setPosition(kindaMediumButton.getPosition().x + (difficultyButtonWidth - kindaMediumText.getLocalBounds().width) / 2,
                                kindaMediumButton.getPosition().y + (buttonHeight - kindaMediumText.getLocalBounds().height) / 2);
    kindaMediumText.setFillColor(Color::White);

    Text hardText("Hard", font, smallFontSize);
    hardText.setPosition(hardButton.getPosition().x + (difficultyButtonWidth - hardText.getLocalBounds().width) / 2,
                         hardButton.getPosition().y + (buttonHeight - hardText.getLocalBounds().height) / 2);
    hardText.setFillColor(Color::White);

    backText.setFont(font);
    backText.setString("Back");
    backText.setCharacterSize(mediumFontSize);
    backText.setPosition(backButton.getPosition().x + (buttonWidth - backText.getLocalBounds().width) / 2,
                         backButton.getPosition().y + (buttonHeight - backText.getLocalBounds().height) / 2);
    backText.setFillColor(Color::White);

    // Function to reset difficulty button colors
    auto resetDifficultyColors = [&]()
    {
        easyButton.setFillColor(Color(50, 50, 50, 200));
        kindaEasyButton.setFillColor(Color(50, 50, 50, 200));
        mediumButton.setFillColor(Color(50, 50, 50, 200));
        kindaMediumButton.setFillColor(Color(50, 50, 50, 200));
        hardButton.setFillColor(Color(50, 50, 50, 200));
    };

    // Highlight current selections
    if (playerIsWhite)
    {
        colorWhiteButton.setFillColor(Color(100, 200, 100, 220));
    }
    else
    {
        colorBlackButton.setFillColor(Color(100, 200, 100, 220));
    }

    resetDifficultyColors(); // Start with default colors
    switch (computerDifficulty)
    {
    case ComputerDifficulty::Easy:
        easyButton.setFillColor(Color(100, 200, 100, 220));
        break;
    case ComputerDifficulty::kindaEasy:
        kindaEasyButton.setFillColor(Color(100, 200, 100, 220));
        break;
    case ComputerDifficulty::Medium:
        mediumButton.setFillColor(Color(100, 200, 100, 220));
        break;
    case ComputerDifficulty::kindaMedium:
        kindaMediumButton.setFillColor(Color(100, 200, 100, 220));
        break;
    case ComputerDifficulty::Hard:
        hardButton.setFillColor(Color(100, 200, 100, 220));
        break;
    }

    bool needsRedraw = false;

    while (window.isOpen())
    {
        if (needsRedraw)
        {
            // Rebuild UI with new dimensions
            return showComputerOptions();
        }

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
                // Get mouse position relative to the window
                Vector2f mousePosF = window.mapPixelToCoords(Mouse::getPosition(window));

                // Color selection
                if (colorWhiteButton.getGlobalBounds().contains(mousePosF))
                {
                    playerIsWhite = true;
                    colorWhiteButton.setFillColor(Color(100, 200, 100, 220));
                    colorBlackButton.setFillColor(Color(50, 50, 50, 200));
                }
                else if (colorBlackButton.getGlobalBounds().contains(mousePosF))
                {
                    playerIsWhite = false;
                    colorBlackButton.setFillColor(Color(100, 200, 100, 220));
                    colorWhiteButton.setFillColor(Color(50, 50, 50, 200));
                }
                // Difficulty selection
                else if (easyButton.getGlobalBounds().contains(mousePosF))
                {
                    computerDifficulty = ComputerDifficulty::Easy;
                    resetDifficultyColors();
                    easyButton.setFillColor(Color(100, 200, 100, 220));
                }
                else if (kindaEasyButton.getGlobalBounds().contains(mousePosF))
                {
                    computerDifficulty = ComputerDifficulty::kindaEasy;
                    resetDifficultyColors();
                    kindaEasyButton.setFillColor(Color(100, 200, 100, 220));
                }
                else if (mediumButton.getGlobalBounds().contains(mousePosF))
                {
                    computerDifficulty = ComputerDifficulty::Medium;
                    resetDifficultyColors();
                    mediumButton.setFillColor(Color(100, 200, 100, 220));
                }
                else if (kindaMediumButton.getGlobalBounds().contains(mousePosF))
                {
                    computerDifficulty = ComputerDifficulty::kindaMedium;
                    resetDifficultyColors();
                    kindaMediumButton.setFillColor(Color(100, 200, 100, 220));
                }
                else if (hardButton.getGlobalBounds().contains(mousePosF))
                {
                    computerDifficulty = ComputerDifficulty::Hard;
                    resetDifficultyColors();
                    hardButton.setFillColor(Color(100, 200, 100, 220));
                }
                // Back button
                else if (backButton.getGlobalBounds().contains(mousePosF))
                {
                    return true; // Return to main menu with options selected
                }
            }

            // Handle window resize events
            if (event.type == Event::Resized)
            {
                // Update view to match new window size
                FloatRect visibleArea(0, 0, event.size.width, event.size.height);
                window.setView(View(visibleArea));

                // Update window dimensions
                WINDOW_WIDTH = event.size.width;
                WINDOW_HEIGHT = event.size.height;

                // Scale menu background
                scaleX = static_cast<float>(WINDOW_WIDTH) / menuTexture.getSize().x;
                scaleY = static_cast<float>(WINDOW_HEIGHT) / menuTexture.getSize().y;
                menuSprite.setScale(scaleX, scaleY);

                // Set flag to redraw UI
                needsRedraw = true;
            }
        }

        // Get mouse position in world coordinates for hover effects
        Vector2f mousePosF = window.mapPixelToCoords(Mouse::getPosition(window));

        // Highlight back button on hover
        if (backButton.getGlobalBounds().contains(mousePosF))
        {
            backButton.setFillColor(Color(70, 70, 70, 220));
        }
        else
        {
            backButton.setFillColor(Color(50, 50, 50, 200));
        }

        window.clear();
        window.draw(menuSprite);
        window.draw(chooseColorText);
        window.draw(colorWhiteButton);
        window.draw(colorBlackButton);
        window.draw(whiteText);
        window.draw(blackText);
        window.draw(difficultyText);
        window.draw(easyButton);
        window.draw(kindaEasyButton);
        window.draw(mediumButton);
        window.draw(kindaMediumButton);
        window.draw(hardButton);
        window.draw(easyText);
        window.draw(kindaEasyText);
        window.draw(mediumText);
        window.draw(kindaMediumText);
        window.draw(hardText);
        window.draw(backButton);
        window.draw(backText);
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
                    // Reset the game completely
                    initBoard();                // Reset board array to starting position
                    initializePieces();         // Reset all piece sprites
                    updateBoardAndPieceSizes(); // Ensure pieces are properly scaled
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

    // Note: whiteTurn is already set in resetGame(), so we don't need to set it here

    // Draw the initial board state to ensure the window is rendered
    window.clear(Color::Black);
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
    window.display();

    // Add a small delay to ensure the window is fully rendered
    sf::sleep(sf::milliseconds(500));

    // If playing as black against computer, make the first move for the computer (white)
    if (currentMode == GameMode::VsComputer && !playerIsWhite)
    {
        makeComputerMove();
        whiteTurn = false; // Now it's player's (black) turn

        // Redraw the board after the computer's move
        window.clear(Color::Black);
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
        window.display();
    }

    // For LAN games, show waiting message if needed
    Text waitingText;
    if ((currentMode == GameMode::LANHost || currentMode == GameMode::LANClient) && waitingForOpponent)
    {
        waitingText.setFont(font);
        waitingText.setString("Waiting for opponent to connect...");
        waitingText.setCharacterSize(WINDOW_HEIGHT * 0.03f);
        waitingText.setPosition(WINDOW_WIDTH / 2 - waitingText.getLocalBounds().width / 2,
                                WINDOW_HEIGHT * 0.1f);
        waitingText.setFillColor(Color::Yellow);

        window.draw(waitingText);
        window.display();
    }

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
        // Handle network messages for LAN games
        if ((currentMode == GameMode::LANHost || currentMode == GameMode::LANClient) && network)
        {
            handleNetworkMessages();

            // Update waiting text if needed
            if (waitingForOpponent)
            {
                // Process events to keep the window responsive
                Event event;
                while (window.pollEvent(event))
                {
                    if (event.type == Event::Closed)
                    {
                        window.close();
                        return;
                    }
                }

                // Clear the window and draw the board first
                window.clear(Color::Black);
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

                // Draw waiting message
                waitingText.setString("Waiting for opponent to connect...");
                waitingText.setPosition(WINDOW_WIDTH / 2 - waitingText.getLocalBounds().width / 2,
                                        WINDOW_HEIGHT * 0.1f);
                window.draw(waitingText);
                window.display();

                // Add a small delay to prevent high CPU usage
                sf::sleep(sf::milliseconds(50));

                // Skip the rest of the loop if waiting for opponent
                continue;
            }

            // Show waiting for move message if it's not our turn
            if (waitingForMove && !gameOver)
            {
                // Process events to keep the window responsive
                Event event;
                while (window.pollEvent(event))
                {
                    if (event.type == Event::Closed)
                    {
                        window.close();
                        return;
                    }
                }

                waitingText.setString("Waiting for opponent's move...");
                waitingText.setPosition(WINDOW_WIDTH / 2 - waitingText.getLocalBounds().width / 2,
                                        WINDOW_HEIGHT * 0.1f);
            }
        }

        // Check for checkmate at start of each loop
        if (!gameOver)
        {
            if (logic.checkMate(whiteTurn))
            {
                gameOver = true;
                whiteWon = !whiteTurn; // If it's white's turn and they're in checkmate, black won

                // Send game over message for network games
                if ((currentMode == GameMode::LANHost || currentMode == GameMode::LANClient) && network && opponentConnected)
                {
                    NetworkMessage gameOverMsg(MessageType::GameOver, whiteWon ? "white" : "black");
                    if (currentMode == GameMode::LANHost)
                    {
                        network->sendToAllClients(gameOverMsg);
                    }
                    else
                    {
                        network->sendToServer(gameOverMsg);
                    }
                }

                // Show game over screen
                bool continueGame = showGameOverWindow(whiteWon);
                if (continueGame)
                {
                    // Reset the game completely
                    resetGame();

                    // Reset local game state variables
                    pieceSelected = false;
                    selectedX = -1;
                    selectedY = -1;
                    validMoves.clear();

                    // Reset game logic
                    logic.reset();

                    // If playing as black, we need to redraw after the computer's first move
                    if (currentMode == GameMode::VsComputer && !playerIsWhite)
                    {
                        // Redraw the board after the computer's move
                        window.clear(Color::Black);
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
                        window.display();
                    }
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
                        // For network games, only allow moves when it's the player's turn
                        if ((currentMode == GameMode::LANHost || currentMode == GameMode::LANClient) && waitingForMove)
                        {
                            continue; // Skip if waiting for opponent's move
                        }

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
                            // Make the player move
                            logic.movePiece(selectedX, selectedY, boardX, boardY);

                            // Record move in UCI format for Stockfish
                            if (currentMode == GameMode::VsComputer)
                            {
                                string uciMove = moveToUci(selectedX, selectedY, boardX, boardY);
                                if (!currentPosition.empty())
                                {
                                    currentPosition += " ";
                                }
                                currentPosition += uciMove;
                                moveHistory.push_back(uciMove);
                            }
                            // Send move to opponent for network games
                            else if ((currentMode == GameMode::LANHost || currentMode == GameMode::LANClient) && network && opponentConnected)
                            {
                                sendMoveToOpponent(selectedX, selectedY, boardX, boardY);
                                waitingForMove = true; // Now wait for opponent's move
                            }

                            whiteTurn = !whiteTurn; // Switch turns
                            pieceSelected = false;
                            validMoves.clear();

                            // Check for checkmate after player move
                            if (logic.checkMate(!whiteTurn))
                            {
                                gameOver = true;
                                whiteWon = whiteTurn; // Current player won

                                // Show game over screen
                                bool continueGame = showGameOverWindow(whiteWon);
                                if (continueGame)
                                {
                                    // Reset the game completely
                                    resetGame();

                                    // Reset local game state variables
                                    pieceSelected = false;
                                    selectedX = -1;
                                    selectedY = -1;
                                    validMoves.clear();

                                    // Reset game logic
                                    logic.reset();

                                    // If playing as black, we need to redraw after the computer's first move
                                    if (currentMode == GameMode::VsComputer && !playerIsWhite)
                                    {
                                        // Redraw the board after the computer's move
                                        window.clear(Color::Black);
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
                                        window.display();
                                    }
                                    continue;
                                }
                                else
                                {
                                    window.close();
                                    return;
                                }
                            }

                            // Make computer move if it's its turn and the game isn't over
                            if (!gameOver && currentMode == GameMode::VsComputer &&
                                ((whiteTurn && !playerIsWhite) || (!whiteTurn && playerIsWhite)))
                            {
                                makeComputerMove();
                                whiteTurn = !whiteTurn; // Switch turns

                                // Check for checkmate after computer move
                                if (logic.checkMate(!whiteTurn))
                                {
                                    gameOver = true;
                                    whiteWon = whiteTurn; // Computer won

                                    // Show game over screen
                                    bool continueGame = showGameOverWindow(whiteWon);
                                    if (continueGame)
                                    {
                                        // Reset the game completely
                                        resetGame();

                                        // Reset local game state variables
                                        pieceSelected = false;
                                        selectedX = -1;
                                        selectedY = -1;
                                        validMoves.clear();

                                        // Reset game logic
                                        logic.reset();

                                        // If playing as black, we need to redraw after the computer's first move
                                        if (currentMode == GameMode::VsComputer && !playerIsWhite)
                                        {
                                            // Redraw the board after the computer's move
                                            window.clear(Color::Black);
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
                                            window.display();
                                        }
                                        continue;
                                    }
                                    else
                                    {
                                        window.close();
                                        return;
                                    }
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

        // Draw waiting message for network games if needed
        if ((currentMode == GameMode::LANHost || currentMode == GameMode::LANClient) &&
            (waitingForOpponent || waitingForMove) && !gameOver)
        {
            window.draw(waitingText);
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