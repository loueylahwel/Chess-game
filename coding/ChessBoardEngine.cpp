#include "ChessBoard.h"
#include "GameLogic.h"
#include <sstream>
#include <iostream>
#include <fstream>
#include <direct.h> // For _getcwd

using namespace std;
using namespace sf;

string ChessBoard::moveToUci(int fromX, int fromY, int toX, int toY) const
{
    // Convert board coordinates to algebraic notation
    char fromFile = 'a' + fromX;
    char fromRank = '8' - fromY;
    char toFile = 'a' + toX;
    char toRank = '8' - toY;

    // Build UCI string (e.g., "e2e4")
    string uciMove;
    uciMove += fromFile;
    uciMove += fromRank;
    uciMove += toFile;
    uciMove += toRank;

    return uciMove;
}

void ChessBoard::applyUciMove(const string &uciMove)
{
    if (uciMove.length() < 4)
        return; // Invalid move

    // Convert algebraic notation to board coordinates
    int fromX = uciMove[0] - 'a';
    int fromY = '8' - uciMove[1];
    int toX = uciMove[2] - 'a';
    int toY = '8' - uciMove[3];

    // Validate coordinates
    if (fromX < 0 || fromX >= 8 || fromY < 0 || fromY >= 8 ||
        toX < 0 || toX >= 8 || toY < 0 || toY >= 8)
    {
        cout << "Invalid UCI move: " << uciMove << endl;
        return;
    }

    // Execute the move
    GameLogic logic(board, *this);
    logic.movePiece(fromX, fromY, toX, toY);

    // Update PGN file after computer move
    updatePgnFile();

    // Add move to history
    if (!currentPosition.empty())
    {
        currentPosition += " ";
    }
    currentPosition += uciMove;

    moveHistory.push_back(uciMove);
}

void ChessBoard::makeComputerMove()
{
    if (!engine || !engine->isInitialized() || gameOver)
        return;

    cout << "Computer is thinking..." << endl;

    // Calculate difficulty-based move time (in milliseconds)
    int moveTime = 100; // Default 100ms
    switch (computerDifficulty)
    {
    case ComputerDifficulty::Easy:
        moveTime = 200;
        break;
    case ComputerDifficulty::Medium:
        moveTime = 500;
        break;
    case ComputerDifficulty::Hard:
        moveTime = 2000;
        break;
    case ComputerDifficulty::kindaEasy:
        moveTime = 300;
        break;
    case ComputerDifficulty::kindaMedium:
        moveTime = 1000;
        break;
    }

    // Get best move from Stockfish
    string bestMove = engine->getBestMove(currentPosition, moveTime);

    if (bestMove.empty())
    {
        cout << "Engine failed to find a move!" << endl;
        return;
    }

    cout << "Computer plays: " << bestMove << endl;

    // Apply the move
    applyUciMove(bestMove);
}

string ChessBoard::boardToFen() const
{
    stringstream fen;

    // Board position
    for (int y = 0; y < 8; ++y)
    {
        int emptyCount = 0;

        for (int x = 0; x < 8; ++x)
        {
            int piece = board[x][y];

            if (piece == 0)
            {
                emptyCount++;
            }
            else
            {
                if (emptyCount > 0)
                {
                    fen << emptyCount;
                    emptyCount = 0;
                }

                char pieceLetter;

                switch (abs(piece))
                {
                case 6:
                    pieceLetter = 'r';
                    break; // Rook
                case 7:
                    pieceLetter = 'b';
                    break; // Bishop
                case 8:
                    pieceLetter = 'n';
                    break; // Knight
                case 9:
                    pieceLetter = 'k';
                    break; // King
                case 10:
                    pieceLetter = 'p';
                    break; // Pawn
                case 11:
                    pieceLetter = 'q';
                    break; // Queen
                default:
                    pieceLetter = '?';
                    break; // Unknown
                }

                if (piece > 0)
                { // White piece
                    pieceLetter = toupper(pieceLetter);
                }

                fen << pieceLetter;
            }
        }

        if (emptyCount > 0)
        {
            fen << emptyCount;
        }

        if (y < 7)
        {
            fen << '/';
        }
    }

    return fen.str();
}