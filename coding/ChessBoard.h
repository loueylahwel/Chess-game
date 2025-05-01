#ifndef CHESSBOARD_H
#define CHESSBOARD_H

// Prevent std::byte conflicts with Windows headers
#define _HAS_STD_BYTE 0

#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <string>
#include <memory>
#include "NetworkManager.h"
#include "GameLogic.h" // Add GameLogic header

// Include Windows headers specifically for StockfishEngine class definition
#ifdef _WIN32
#define _HAS_STD_BYTE 0 // Prevent std::byte conflicts
#include <windows.h>    // For HANDLE type
#endif

using namespace std;
using namespace sf;

// Reduce initial window size for better scaling
// Use extern to declare variables that will be defined once in a .cpp file
extern int WINDOW_WIDTH;   // Default: 700 (reduced from 773)
extern int WINDOW_HEIGHT;  // Default: 700 (reduced from 773)
extern float SCALE_FACTOR; // Default: 0.75
extern int BOARD_SIZE;     // Default: 8
extern float PIECE_SCALE;  // Default: 0.9f

enum class GameMode
{
    TwoPlayer,
    VsComputer,
    LANHost,  // Host a LAN game (play as white)
    LANClient // Join a LAN game (play as black)
};

enum class ComputerDifficulty
{
    Easy = 0, // Skill Level 0
    kindaEasy = 5,
    Medium = 10,
    kindaMedium = 15,
    Hard = 20 // Skill Level 20
};

class StockfishEngine
{
private:
#ifdef _WIN32
    HANDLE engineProcessHandle; // Process handle
    HANDLE hChildStd_IN_Rd;     // Pipe for engine stdin (read end)
    HANDLE hChildStd_IN_Wr;     // Pipe for engine stdin (write end)
    HANDLE hChildStd_OUT_Rd;    // Pipe for engine stdout (read end)
    HANDLE hChildStd_OUT_Wr;    // Pipe for engine stdout (write end)
#else
    FILE *engineProcess; // Use FILE* for popen on non-Windows
#endif
    bool initialized;
    int skillLevel;

public:
    StockfishEngine();
    ~StockfishEngine();

    bool initialize();
    void setDifficulty(int level);
    string getBestMove(const string &position, int moveTime = 1000);
    string sendCommand(const string &command);
    void close();
    bool isInitialized() const { return initialized; }
};

class ChessBoard
{
private:
    RenderWindow window;
    vector<vector<int>> board;
    GameLogic logic; // Add GameLogic member
    Texture blackpieces[6];
    Texture whitepieces[6];
    Sprite blackSprites[6];
    Sprite whiteSprites[6];
    vector<vector<Sprite>> pieceSprites; // 2D array for piece sprites
    Texture menuTexture;
    Sprite menuSprite;
    Font font;
    float SQUARE_SIZE; // Make SQUARE_SIZE a member variable
    bool gameOver;     // Flag to indicate if game is over
    bool whiteWon;     // Flag to indicate if white won
    bool whiteTurn;    // Added whiteTurn variable to track turns

    // Stockfish integration
    GameMode currentMode;
    bool playerIsWhite;
    ComputerDifficulty computerDifficulty;
    unique_ptr<StockfishEngine> engine;
    string currentPosition;
    vector<string> moveHistory;    // For UCI format moves
    vector<string> algebraicMoves; // For algebraic notation moves (PGN format)

    // Network game variables
    unique_ptr<NetworkManager> network;
    string serverAddress;
    unsigned short serverPort;
    bool waitingForOpponent;
    bool opponentConnected;
    bool waitingForMove;

    // For menu
    float centerX;
    float centerY;
    float buttonWidth;
    float buttonHeight;
    float verticalOffset;
    float verticalSpacing;
    float difficultyY;
    float backButtonY;
    Text chooseColorText;
    Text whiteText;
    Text blackText;
    Text difficultyText;
    Text backText;

    bool showMenu();
    bool showModeSelection();
    bool showComputerOptions();
    bool showNetworkOptions(bool isHost);
    bool showGameOverWindow(bool whiteWinner); // Method to show game over window
    void runGame();
    void updateBoardAndPieceSizes(); // Declaration for the new function
    void makeComputerMove();
    string boardToFen() const;
    string moveToUci(int fromX, int fromY, int toX, int toY) const;
    void applyUciMove(const string &uciMove);
    void resetGame();

    // Network methods
    bool startNetworkHost();
    bool joinNetworkGame(const string &address, unsigned short port);
    void handleNetworkMessages();
    void sendMoveToOpponent(int fromX, int fromY, int toX, int toY);
    void processNetworkMove(const string &moveData);
    void onNetworkMessage(const NetworkMessage &message);

    // PGN file handling
    void updatePgnFile();

public:
    ChessBoard();
    vector<vector<Sprite>> &getPieceSprites() { return pieceSprites; }
    vector<vector<int>> &getMatrix();
    void setPiece(int x, int y, int value);
    void initBoard();
    int getPiece(int x, int y) const;
    bool loadTexture();
    void initializePieces();
    void drawPieces();
    void drawPiece(Sprite sprite, int x, int y);
    void run();
    void addAlgebraicMove(const string &move) { algebraicMoves.push_back(move); }
};

#endif // CHESSBOARD_H