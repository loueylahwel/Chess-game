#pragma once
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;

class ChessBoard;

class GameLogic
{
private:
    vector<vector<int>> &board;
    ChessBoard &chessBoard;
    int enPassantCol;        // Column of pawn that just moved two squares
    int enPassantRow;        // Row where the capturing pawn would end up
    bool enPassantPossible;  // Flag indicating if en passant is possible this turn
    static int lastKingX[2]; // Cached king X positions [0] for black, [1] for white
    static int lastKingY[2]; // Cached king Y positions [0] for black, [1] for white

    // Track piece movement for castling
    bool whiteKingMoved;          // Has white king moved?
    bool blackKingMoved;          // Has black king moved?
    bool whiteQueensideRookMoved; // Has white queenside rook moved?
    bool whiteKingsideRookMoved;  // Has white kingside rook moved?
    bool blackQueensideRookMoved; // Has black queenside rook moved?
    bool blackKingsideRookMoved;  // Has black kingside rook moved?

    vector<string> moveHistory; // Store moves in algebraic notation

    // Helper methods for castling
    bool canCastle(bool isWhite, bool isKingside) const;
    bool squaresAreEmpty(int startX, int endX, int y) const;
    bool squaresAreNotAttacked(int startX, int endX, int y, bool color) const;

public:
    GameLogic(vector<vector<int>> &boardRef, ChessBoard &chessBoardRef);
    void reset(); // Reset all game logic state
    bool isWhite(int x, int y) const;
    bool isBlack(int x, int y) const;
    string whatPiece(int x, int y);
    vector<vector<int>> possibleMoves(int x, int y);
    vector<vector<int>> getAllMoves(bool color);
    void movePiece(int x, int y, int xx, int yy);
    bool check(bool color);
    bool checkMate(bool color);
    bool checkStaleMate(bool color);
    bool isValidMove(int x, int y, int xx, int yy);
    bool wouldBeInCheck(int fromX, int fromY, int toX, int toY, bool color);
    bool moveWouldCheck(int fromX, int fromY, int toX, int toY, bool color);
    void resetEnPassant();
    bool isEnPassantCapture(int fromX, int fromY, int toX, int toY) const;
    bool isCastlingMove(int fromX, int fromY, int toX, int toY) const;

    // New methods for PGN generation
    void addMoveToHistory(int fromX, int fromY, int toX, int toY);
    string generatePGN() const;
    string squareToAlgebraic(int x, int y) const;
};