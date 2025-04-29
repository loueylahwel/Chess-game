#include "ChessBoard.h"
#include <iostream>

using namespace std;
using namespace sf;

#ifndef CHESSBOARD_CPP_INCLUDED
ChessBoard::ChessBoard() : window(VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "ChessGame"),
                           board(BOARD_SIZE, vector<int>(BOARD_SIZE, 0)),
                           gameOver(false),
                           whiteWon(false),
                           currentMode(GameMode::TwoPlayer),
                           playerIsWhite(true),
                           computerDifficulty(ComputerDifficulty::Medium),
                           engine(nullptr),
                           currentPosition("")
{
    // Initialize SQUARE_SIZE based on initial window dimensions
    SQUARE_SIZE = WINDOW_WIDTH / static_cast<float>(BOARD_SIZE);
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

    // Scale the menu background
    float scaleX = static_cast<float>(WINDOW_WIDTH) / menuTexture.getSize().x;
    float scaleY = static_cast<float>(WINDOW_HEIGHT) / menuTexture.getSize().y;
    menuSprite.setScale(scaleX, scaleY);

    // Calculate scalable positions based on window dimensions
    float centerX = WINDOW_WIDTH / 2.0f;
    float centerY = WINDOW_HEIGHT / 2.0f;
    float buttonWidth = WINDOW_WIDTH * 0.3f;       // 30% of window width
    float buttonHeight = WINDOW_HEIGHT * 0.08f;    // 8% of window height
    float verticalSpacing = WINDOW_HEIGHT * 0.03f; // 3% spacing

    // Create buttons
    RectangleShape twoPlayerButton(Vector2f(buttonWidth, buttonHeight));
    twoPlayerButton.setPosition(centerX - buttonWidth / 2, centerY - buttonHeight - verticalSpacing);
    twoPlayerButton.setFillColor(Color(50, 50, 50, 200));

    RectangleShape vsComputerButton(Vector2f(buttonWidth, buttonHeight));
    vsComputerButton.setPosition(centerX - buttonWidth / 2, centerY);
    vsComputerButton.setFillColor(Color(50, 50, 50, 200));

    RectangleShape exitButton(Vector2f(buttonWidth, buttonHeight));
    exitButton.setPosition(centerX - buttonWidth / 2, centerY + buttonHeight + verticalSpacing);
    exitButton.setFillColor(Color(50, 50, 50, 200));

    // Prepare text with appropriate font sizes
    float fontSize = WINDOW_HEIGHT * 0.04f; // 4% of window height

    Text twoPlayerText("Play Two-Player", font, fontSize);
    twoPlayerText.setPosition(
        twoPlayerButton.getPosition().x + (buttonWidth - twoPlayerText.getLocalBounds().width) / 2,
        twoPlayerButton.getPosition().y + (buttonHeight - twoPlayerText.getLocalBounds().height) / 2);
    twoPlayerText.setFillColor(Color::White);

    Text vsComputerText("Play vs Computer", font, fontSize);
    vsComputerText.setPosition(
        vsComputerButton.getPosition().x + (buttonWidth - vsComputerText.getLocalBounds().width) / 2,
        vsComputerButton.getPosition().y + (buttonHeight - vsComputerText.getLocalBounds().height) / 2);
    vsComputerText.setFillColor(Color::White);

    Text exitText("Exit Game", font, fontSize);
    exitText.setPosition(
        exitButton.getPosition().x + (buttonWidth - exitText.getLocalBounds().width) / 2,
        exitButton.getPosition().y + (buttonHeight - exitText.getLocalBounds().height) / 2);
    exitText.setFillColor(Color::White);

    bool needsRedraw = false;

    while (window.isOpen())
    {
        if (needsRedraw)
        {
            // Rebuild UI with new dimensions
            return showMenu();
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

                // Check if buttons are clicked
                if (twoPlayerButton.getGlobalBounds().contains(mousePosF))
                {
                    currentMode = GameMode::TwoPlayer;
                    return true; // Start game
                }
                else if (vsComputerButton.getGlobalBounds().contains(mousePosF))
                {
                    currentMode = GameMode::VsComputer;
                    if (!showComputerOptions())
                    {
                        return false; // User cancelled
                    }
                    return true; // Start game with computer
                }
                else if (exitButton.getGlobalBounds().contains(mousePosF))
                {
                    return false; // Exit game
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

        // Highlight buttons on hover
        twoPlayerButton.setFillColor(twoPlayerButton.getGlobalBounds().contains(mousePosF) ? Color(70, 70, 70, 220) : Color(50, 50, 50, 200));
        vsComputerButton.setFillColor(vsComputerButton.getGlobalBounds().contains(mousePosF) ? Color(70, 70, 70, 220) : Color(50, 50, 50, 200));
        exitButton.setFillColor(exitButton.getGlobalBounds().contains(mousePosF) ? Color(70, 70, 70, 220) : Color(50, 50, 50, 200));

        window.clear();
        window.draw(menuSprite);
        window.draw(twoPlayerButton);
        window.draw(vsComputerButton);
        window.draw(exitButton);
        window.draw(twoPlayerText);
        window.draw(vsComputerText);
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
    float centerX = WINDOW_WIDTH / 2.0f;
    float centerY = WINDOW_HEIGHT / 2.0f;
    float buttonWidth = WINDOW_WIDTH * 0.2f;
    float buttonHeight = WINDOW_HEIGHT * 0.08f;
    float difficultyButtonWidth = WINDOW_WIDTH * 0.1f; // Adjusted width for 5 buttons
    float horizontalSpacing = WINDOW_WIDTH * 0.01f;    // Spacing between difficulty buttons
    float totalDifficultyWidth = 5 * difficultyButtonWidth + 4 * horizontalSpacing;
    float firstDifficultyX = centerX - totalDifficultyWidth / 2.0f;
    float difficultyY = centerY + verticalOffset + buttonHeight + verticalSpacing * 3;
    float backButtonY = difficultyY + buttonHeight + verticalSpacing * 3;

    // Create buttons and text for computer options
    RectangleShape colorWhiteButton(Vector2f(buttonWidth, buttonHeight));
    colorWhiteButton.setPosition(centerX - buttonWidth - WINDOW_WIDTH * 0.02f, centerY + verticalOffset);
    colorWhiteButton.setFillColor(Color(50, 50, 50, 200));

    RectangleShape colorBlackButton(Vector2f(buttonWidth, buttonHeight));
    colorBlackButton.setPosition(centerX + WINDOW_WIDTH * 0.02f, centerY + verticalOffset);
    colorBlackButton.setFillColor(Color(50, 50, 50, 200));

    // Difficulty Buttons (5 buttons now)
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

    // Prepare text with appropriate font sizes
    float largeFontSize = WINDOW_HEIGHT * 0.04f;
    float mediumFontSize = WINDOW_HEIGHT * 0.035f;
    float smallFontSize = WINDOW_HEIGHT * 0.028f; // Slightly smaller for 5 buttons

    Text chooseColorText("Choose your color:", font, largeFontSize);
    chooseColorText.setPosition(centerX - chooseColorText.getLocalBounds().width / 2,
                                centerY + verticalOffset - buttonHeight);
    chooseColorText.setFillColor(Color::White);

    Text whiteText("White", font, mediumFontSize);
    whiteText.setPosition(colorWhiteButton.getPosition().x + (buttonWidth - whiteText.getLocalBounds().width) / 2,
                          colorWhiteButton.getPosition().y + (buttonHeight - whiteText.getLocalBounds().height) / 2);
    whiteText.setFillColor(Color::White);

    Text blackText("Black", font, mediumFontSize);
    blackText.setPosition(colorBlackButton.getPosition().x + (buttonWidth - blackText.getLocalBounds().width) / 2,
                          colorBlackButton.getPosition().y + (buttonHeight - blackText.getLocalBounds().height) / 2);
    blackText.setFillColor(Color::White);

    Text difficultyText("Choose difficulty:", font, largeFontSize);
    difficultyText.setPosition(centerX - difficultyText.getLocalBounds().width / 2,
                               difficultyY - buttonHeight);
    difficultyText.setFillColor(Color::White);

    // Difficulty Text Labels
    Text easyText("Easy", font, smallFontSize);
    easyText.setPosition(easyButton.getPosition().x + (difficultyButtonWidth - easyText.getLocalBounds().width) / 2,
                         easyButton.getPosition().y + (buttonHeight - easyText.getLocalBounds().height) / 2);
    easyText.setFillColor(Color::White);

    Text kindaEasyText("Kinda Easy", font, smallFontSize);
    kindaEasyText.setPosition(kindaEasyButton.getPosition().x + (difficultyButtonWidth - kindaEasyText.getLocalBounds().width) / 2,
                              kindaEasyButton.getPosition().y + (buttonHeight - kindaEasyText.getLocalBounds().height) / 2);
    kindaEasyText.setFillColor(Color::White);

    Text mediumText("Medium", font, smallFontSize);
    mediumText.setPosition(mediumButton.getPosition().x + (difficultyButtonWidth - mediumText.getLocalBounds().width) / 2,
                           mediumButton.getPosition().y + (buttonHeight - mediumText.getLocalBounds().height) / 2);
    mediumText.setFillColor(Color::White);

    Text kindaMediumText("Kinda Med", font, smallFontSize);
    kindaMediumText.setPosition(kindaMediumButton.getPosition().x + (difficultyButtonWidth - kindaMediumText.getLocalBounds().width) / 2,
                                kindaMediumButton.getPosition().y + (buttonHeight - kindaMediumText.getLocalBounds().height) / 2);
    kindaMediumText.setFillColor(Color::White);

    Text hardText("Hard", font, smallFontSize);
    hardText.setPosition(hardButton.getPosition().x + (difficultyButtonWidth - hardText.getLocalBounds().width) / 2,
                         hardButton.getPosition().y + (buttonHeight - hardText.getLocalBounds().height) / 2);
    hardText.setFillColor(Color::White);

    Text backText("Start", font, mediumFontSize);
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
        window.draw(kindaEasyButton); // Draw new button
        window.draw(mediumButton);
        window.draw(kindaMediumButton); // Draw new button
        window.draw(hardButton);
        window.draw(easyText);
        window.draw(kindaEasyText); // Draw new text
        window.draw(mediumText);
        window.draw(kindaMediumText); // Draw new text
        window.draw(hardText);
        window.draw(backButton);
        window.draw(backText);
        window.display();
    }

    return false;
}
#endif // CHESSBOARD_CPP_INCLUDED

void ChessBoard::resetGame()
{
    // Reset board to initial state
    initBoard();
    initializePieces();
    updateBoardAndPieceSizes();

    // Reset game state
    gameOver = false;
    whiteWon = false;

    // Clear move history
    moveHistory.clear();
    currentPosition = "";

    // Always start with white's turn
    whiteTurn = true;

    // For network games, set waiting state based on player color
    if (currentMode == GameMode::LANHost || currentMode == GameMode::LANClient)
    {
        // In LAN games, host is white, client is black
        waitingForMove = !playerIsWhite; // Wait for opponent's move if playing as black
    }

    // Note: If playing as black against computer, the first move for the computer (white)
    // will be handled in runGame() to ensure the window is fully rendered
}