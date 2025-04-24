#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include "GameLogic.h"
#include "ChessBoard.h"

using namespace std;
using namespace sf;

ChessBoard::ChessBoard() : window(VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "ChessGame"),
                          board(BOARD_SIZE, vector<int>(BOARD_SIZE, 0)) {}

vector<vector<int>>& ChessBoard::getMatrix() { 
    return board; 
}

void ChessBoard::setPiece(int x, int y, int value) {
    if (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE) {
        board[x][y] = value;
    }
}

int ChessBoard::getPiece(int x, int y) const {
    if (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE) {
        return board[x][y];
    }
    return -1;
}

bool ChessBoard::loadTexture() {
    string pieceNames[6] = {"Pawn", "Rook", "Knight", "Bishop", "Queen", "King"};

    for (int i = 0; i < 6; i++) {
        if (!blackpieces[i].loadFromFile("C:/Users/BL9/Desktop/chess/coding/images/black" + pieceNames[i] + ".png")) {
            cout << "Failed to load black " << pieceNames[i] << " texture" << endl;
            return false;
        }
        if (!whitepieces[i].loadFromFile("C:/Users/BL9/Desktop/chess/coding/images/white" + pieceNames[i] + ".png")) {
            cout << "Failed to load white " << pieceNames[i] << " texture" << endl;
            return false;
        }

        blackSprites[i].setTexture(blackpieces[i]);
        float scale = (SQUARE_SIZE * PIECE_SCALE) / blackSprites[i].getTexture()->getSize().x;
        blackSprites[i].setScale(scale * SCALE_FACTOR, scale * SCALE_FACTOR);

        whiteSprites[i].setTexture(whitepieces[i]);
        scale = (SQUARE_SIZE * PIECE_SCALE) / whiteSprites[i].getTexture()->getSize().x;
        whiteSprites[i].setScale(scale * SCALE_FACTOR, scale * SCALE_FACTOR);
    }
    return true;
}

void ChessBoard::initBoard() {
    board = {
        {-6, -10, 0, 0, 0, 0, 10, 6},
        {-8, -10, 0, 0, 0, 0, 10, 8},
        {-7, -10, 0, 0, 0, 0, 10, 7},
        {-11,-10, 0, 0, 0, 0, 10,11},
        {-9, -10, 0, 0, 0, 0, 10, 9},
        {-7, -10, 0, 0, 0, 0, 10, 7},
        {-8, -10, 0, 0, 0, 0, 10, 8},
        {-6, -10, 0, 0, 0, 0, 10, 6}
    };
}

void ChessBoard::initializePieces() {
    // Initialize the 2D array (8x8) with empty sprites first
    pieceSprites.resize(BOARD_SIZE, vector<Sprite>(BOARD_SIZE));

    // Set up black pawns
    for (int i = 0; i < 8; ++i) {
        pieceSprites[i][1] = blackSprites[0]; // Pawn
    }

    // Set up white pawns
    for (int i = 0; i < 8; ++i) {
        pieceSprites[i][6] = whiteSprites[0]; // Pawn
    }

    // Set up other pieces
    int pieceIndices[] = {1, 2, 3, 4, 5}; // Rook, Knight, Bishop, Queen, King
    int pieceCols[][2] = {{0,7}, {1,6}, {2,5}, {3}, {4}}; // Positions

    for (int i = 0; i < 5; ++i) {
        int spriteIndex = pieceIndices[i];
        for (int j = 0; j < (i < 3 ? 2 : 1); ++j) {
            int col = pieceCols[i][j];
            // Black pieces (back row)
            pieceSprites[col][0] = blackSprites[spriteIndex];
            // White pieces (front row)
            pieceSprites[col][7] = whiteSprites[spriteIndex];
        }
    }
}

void ChessBoard::drawPiece(Sprite sprite, int x, int y) {
    sprite.setPosition(x * SQUARE_SIZE + (SQUARE_SIZE - sprite.getGlobalBounds().width) / 2,
                       y * SQUARE_SIZE + (SQUARE_SIZE - sprite.getGlobalBounds().height) / 2);
    window.draw(sprite);
}

void ChessBoard::drawPieces() {
    for (int x = 0; x < BOARD_SIZE; ++x) {
        for (int y = 0; y < BOARD_SIZE; ++y) {
            // Check if there's a piece at this position
            if (pieceSprites[x][y].getTexture() != nullptr) {
                drawPiece(pieceSprites[x][y], x, y);
            }
        }
    }
}

void ChessBoard::run() {
    // Load menu background
    if (!menuTexture.loadFromFile("C:/Users/BL9/Desktop/chess/coding/images/mainscreen.jpg")) {
        cout << "Failed to load menu background!" << endl;
        return;
    }
    menuSprite.setTexture(menuTexture);
    
    // Scale the menu background to fit the window
    float scaleX = static_cast<float>(WINDOW_WIDTH) / menuTexture.getSize().x;
    float scaleY = static_cast<float>(WINDOW_HEIGHT) / menuTexture.getSize().y;
    menuSprite.setScale(scaleX, scaleY);

    // Load font for buttons
    if (!font.loadFromFile("C:/Users/BL9/Desktop/chess/coding/font/Arial.ttf")) {
        cout << "Failed to load font!" << endl;
        return;
    }

    // Show menu first
    if (!showMenu()) {
        window.close();
        return;
    }

    // If user selected start, run the game
    runGame();
}

bool ChessBoard::showMenu() {
    // Set window icon
    Image icon;
    if (!icon.loadFromFile("C:/Users/BL9/Desktop/chess/coding/images/logo.png")) {
        cout << "Failed to load window icon!" << endl;
    } else {
        window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
    }
    // Create start and exit buttons
    RectangleShape startButton(Vector2f(200, 60));
    startButton.setPosition(WINDOW_WIDTH/2 - 100, WINDOW_HEIGHT/2 - 40);
    startButton.setFillColor(Color(50, 50, 50, 200));

    RectangleShape exitButton(Vector2f(200, 60));
    exitButton.setPosition(WINDOW_WIDTH/2 - 100, WINDOW_HEIGHT/2 + 60);
    exitButton.setFillColor(Color(50, 50, 50, 200));

    Text startText("Start Game", font, 30);
    startText.setPosition(WINDOW_WIDTH/2 - 80, WINDOW_HEIGHT/2 - 30);
    startText.setFillColor(Color::White);

    Text exitText("Exit Game", font, 30);
    exitText.setPosition(WINDOW_WIDTH/2 - 75, WINDOW_HEIGHT/2 + 70);
    exitText.setFillColor(Color::White);

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
                return false;
            }

            if (event.type == Event::MouseButtonPressed) {
                Vector2i mousePos = Mouse::getPosition(window);
                if (startButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    return true; // Start game
                } else if (exitButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    return false; // Exit game
                }
            }
        }

        // Highlight buttons on hover
        Vector2i mousePos = Mouse::getPosition(window);
        if (startButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
            startButton.setFillColor(Color(70, 70, 70, 220));
        } else {
            startButton.setFillColor(Color(50, 50, 50, 200));
        }

        if (exitButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
            exitButton.setFillColor(Color(70, 70, 70, 220));
        } else {
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

void ChessBoard::runGame() {
    // Set window icon
    Image icon;
    if (!icon.loadFromFile("C:/Users/BL9/Desktop/chess/coding/images/logo.png")) {
        cout << "Failed to load window icon!" << endl;
    } else {
        window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
    }

    initBoard();
    if (!loadTexture()) {
        cout << "Failed to load textures!" << endl;
        return;
    }
    initializePieces();
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
    
    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
            }
            if (event.type == Event::MouseButtonPressed) {
                Vector2i mousePos(event.mouseButton.x, event.mouseButton.y);
                int boardX = mousePos.x / SQUARE_SIZE;
                int boardY = mousePos.y / SQUARE_SIZE;
                
                if (boardX >= 0 && boardX < BOARD_SIZE && 
                    boardY >= 0 && boardY < BOARD_SIZE) {
                    cout << "Position: [" << boardX << "][" << boardY 
                         << "] Value: " << getPiece(boardX, boardY) << endl;

                    if (!pieceSelected) {
                        // Only allow selecting pieces of the current turn's color
                        if ((whiteTurn && getPiece(boardX, boardY) > 0) || 
                            (!whiteTurn && getPiece(boardX, boardY) < 0)) {
                            selectedX = boardX;
                            selectedY = boardY;
                            pieceSelected = true;
                            validMoves.clear();
                            for (int x = 0; x < BOARD_SIZE; x++) {
                                for (int y = 0; y < BOARD_SIZE; y++) {
                                    if (logic.isValidMove(selectedX, selectedY, x, y)) {
                                        // Only add moves that are either empty or opponent pieces
                                        if (getPiece(x, y) == 0 || 
                                            (whiteTurn && getPiece(x, y) < 0) || 
                                            (!whiteTurn && getPiece(x, y) > 0)) {
                                            validMoves.emplace_back(x, y);
                                        }
                                    }
                                }
                            }
                        }
                    } else {
                        // Check if the move is valid
                        bool isValid = false;
                        for (const auto& move : validMoves) {
                            if (move.first == boardX && move.second == boardY) {
                                isValid = true;
                                break;
                            }
                        }
                        if (isValid) {
                            logic.movePiece(selectedX, selectedY, boardX, boardY);
                            whiteTurn = !whiteTurn; // Switch turns
                            pieceSelected = false;
                            validMoves.clear();
                        } else if ((whiteTurn && getPiece(boardX, boardY) > 0) || 
                                  (!whiteTurn && getPiece(boardX, boardY) < 0)) {
                            // Allow selecting a different piece of the same color
                            selectedX = boardX;
                            selectedY = boardY;
                            validMoves.clear();
                            for (int x = 0; x < BOARD_SIZE; x++) {
                                for (int y = 0; y < BOARD_SIZE; y++) {
                                    if (logic.isValidMove(selectedX, selectedY, x, y)) {
                                        if (getPiece(x, y) == 0 || 
                                            (whiteTurn && getPiece(x, y) < 0) || 
                                            (!whiteTurn && getPiece(x, y) > 0)) {
                                            validMoves.emplace_back(x, y);
                                        }
                                    }
                                }
                            }
                        } else {
                            // Clicked on invalid square - deselect
                            pieceSelected = false;
                            validMoves.clear();
                        }
                    }
                }
            }
        }

        window.clear(Color::White);
        // Draw board
        for (int x = 0; x < BOARD_SIZE; x++) {
            for (int y = 0; y < BOARD_SIZE; y++) {
                RectangleShape square(Vector2f(SQUARE_SIZE, SQUARE_SIZE));
                square.setPosition(x * SQUARE_SIZE, y * SQUARE_SIZE);
                
                // Highlight selected piece
                if (pieceSelected && x == selectedX && y == selectedY) {
                    square.setFillColor(Color(247, 247, 105)); // Yellow highlight
                } else {
                    square.setFillColor((x + y) % 2 == 0 ? Color(244, 244, 244) : Color	(105,146,62)); // green + white board
                }
                window.draw(square);
            }
        }
        
        // Draw valid move indicators
        for (const auto& move : validMoves) {
            int x = move.first;
            int y = move.second;
            float centerX = x * SQUARE_SIZE + SQUARE_SIZE/2;
            float centerY = y * SQUARE_SIZE + SQUARE_SIZE/2;
            
            if (getPiece(x, y) == 0) {
                // Empty square - gray circle
                moveIndicator.setFillColor(Color(50, 50, 50, 180));
                moveIndicator.setPosition(centerX, centerY);
                window.draw(moveIndicator);
            } else {
                // Opponent's piece - red circle
                captureIndicator.setPosition(centerX, centerY);
                window.draw(captureIndicator);
            }
        }
        drawPieces();
        window.display();
    }
}