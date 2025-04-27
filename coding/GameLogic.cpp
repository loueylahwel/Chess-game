#include "GameLogic.h"
#include "ChessBoard.h"
#include <math.h>
#include <SFML/Audio.hpp>
#include <iostream>

using namespace std;

GameLogic::GameLogic(vector<vector<int>> &boardRef, ChessBoard &chessBoardRef)
    : board(boardRef), chessBoard(chessBoardRef),
      enPassantCol(-1), enPassantRow(-1), enPassantPossible(false) {}

bool GameLogic::isWhite(int x, int y) const
{
    if (x < 0 || x >= 8 || y < 0 || y >= 8)
        return false;
    return board[x][y] > 0;
}

bool GameLogic::isBlack(int x, int y) const
{
    if (x < 0 || x >= 8 || y < 0 || y >= 8)
        return false;
    return board[x][y] < 0;
}

string GameLogic::whatPiece(int x, int y)
{
    int piece = board[x][y];

    if (piece == 0)
    {
        return "Empty";
    }

    // White pieces
    if (piece > 0)
    {
        switch (piece)
        {
        case 6:
            return "Wr";
        case 8:
            return "Wk";
        case 7:
            return "Wb";
        case 11:
            return "Wq";
        case 9:
            return "Wp";
        case 10:
            return "Wn"; // Assuming 10 is knight (you didn't have this in original)
        default:
            return "UnknownWhite";
        }
    }
    // Black pieces
    else
    {
        switch (-piece)
        { // Use positive value for switch
        case 6:
            return "Br";
        case 8:
            return "Bk";
        case 7:
            return "Bb";
        case 11:
            return "Bq";
        case 9:
            return "Bp";
        case 10:
            return "Bn"; // Assuming -10 is black knight
        default:
            return "UnknownBlack";
        }
    }
}

vector<vector<int>> GameLogic::possibleMoves(int x, int y)
{
    vector<vector<int>> moves;
    int piece = board[x][y];
    if (piece == 0)
        return moves;

    bool isWhitePiece = piece > 0;

    if (abs(piece) == 10)
    { // Pawn
        if (isWhitePiece)
        { // White pawn
            // Forward move
            if (y > 0 && board[x][y - 1] == 0)
            {
                moves.push_back({x, y - 1});
                // Initial two-square move
                if (y == 6 && board[x][y - 2] == 0)
                {
                    moves.push_back({x, y - 2});
                }
            }
            // Regular captures
            if (y > 0 && x > 0 && isBlack(x - 1, y - 1))
            {
                moves.push_back({x - 1, y - 1});
            }
            if (y > 0 && x < 7 && isBlack(x + 1, y - 1))
            {
                moves.push_back({x + 1, y - 1});
            }

            // En passant captures
            if (enPassantPossible && y == 3)
            {
                if (x > 0 && enPassantCol == x - 1 && enPassantRow == 2)
                {
                    moves.push_back({x - 1, 2});
                }
                if (x < 7 && enPassantCol == x + 1 && enPassantRow == 2)
                {
                    moves.push_back({x + 1, 2});
                }
            }
        }
        else
        { // Black pawn
            // Forward move
            if (y < 7 && board[x][y + 1] == 0)
            {
                moves.push_back({x, y + 1});
                // Initial two-square move
                if (y == 1 && board[x][y + 2] == 0)
                {
                    moves.push_back({x, y + 2});
                }
            }
            // Regular captures
            if (y < 7 && x > 0 && isWhite(x - 1, y + 1))
            {
                moves.push_back({x - 1, y + 1});
            }
            if (y < 7 && x < 7 && isWhite(x + 1, y + 1))
            {
                moves.push_back({x + 1, y + 1});
            }

            // En passant captures
            if (enPassantPossible && y == 4)
            {
                if (x > 0 && enPassantCol == x - 1 && enPassantRow == 5)
                {
                    moves.push_back({x - 1, 5});
                }
                if (x < 7 && enPassantCol == x + 1 && enPassantRow == 5)
                {
                    moves.push_back({x + 1, 5});
                }
            }
        }
    }
    else if (abs(piece) == 6)
    { // Rook
        int directions[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
        for (auto dir : directions)
        {
            for (int i = 1; i < 8; ++i)
            {
                int newX = x + i * dir[0];
                int newY = y + i * dir[1];
                if (newX < 0 || newX >= 8 || newY < 0 || newY >= 8)
                    break;

                if (board[newX][newY] == 0)
                {
                    moves.push_back({newX, newY});
                }
                else
                {
                    if ((isWhitePiece && isBlack(newX, newY)) ||
                        (!isWhitePiece && isWhite(newX, newY)))
                    {
                        moves.push_back({newX, newY});
                    }
                    break;
                }
            }
        }
    }
    else if (abs(piece) == 7)
    { // Bishop
        int directions[4][2] = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
        for (auto dir : directions)
        {
            for (int i = 1; i < 8; ++i)
            {
                int newX = x + i * dir[0];
                int newY = y + i * dir[1];
                if (newX < 0 || newX >= 8 || newY < 0 || newY >= 8)
                    break;

                if (board[newX][newY] == 0)
                {
                    moves.push_back({newX, newY});
                }
                else
                {
                    if ((isWhitePiece && isBlack(newX, newY)) ||
                        (!isWhitePiece && isWhite(newX, newY)))
                    {
                        moves.push_back({newX, newY});
                    }
                    break;
                }
            }
        }
    }
    else if (piece == 9)
    { // White king
        for (int i = -1; i <= 1; ++i)
        {
            for (int j = -1; j <= 1; ++j)
            {
                if (i == 0 && j == 0)
                    continue;
                int newX = x + i;
                int newY = y + j;
                if (newX >= 0 && newX < 8 && newY >= 0 && newY < 8)
                {
                    // Check if square is empty or contains black piece
                    if (board[newX][newY] <= 0)
                    {
                        // Check if this square is under attack by black pieces
                        bool isUnderAttack = false;

                        // Check all black pieces to see if they attack this square
                        for (int checkX = 0; checkX < 8 && !isUnderAttack; checkX++)
                        {
                            for (int checkY = 0; checkY < 8 && !isUnderAttack; checkY++)
                            {
                                if (board[checkX][checkY] < 0)
                                { // Black piece
                                    // Temporarily remove king to avoid blocking checks
                                    int originalKing = board[x][y];
                                    board[x][y] = 0;

                                    // Check if this black piece can move to the target square
                                    if (isValidMove(checkX, checkY, newX, newY))
                                    {
                                        isUnderAttack = true;
                                    }

                                    // Restore king
                                    board[x][y] = originalKing;
                                }
                            }
                        }

                        if (!isUnderAttack)
                        {
                            moves.push_back({newX, newY});
                        }
                    }
                }
            }
        }
    }
    else if (piece == -9)
    { // Black king
        for (int i = -1; i <= 1; ++i)
        {
            for (int j = -1; j <= 1; ++j)
            {
                if (i == 0 && j == 0)
                    continue;
                int newX = x + i;
                int newY = y + j;
                if (newX >= 0 && newX < 8 && newY >= 0 && newY < 8)
                {
                    // Check if square is empty or contains white piece
                    if (board[newX][newY] >= 0)
                    {
                        // Check if this square is under attack by white pieces
                        bool isUnderAttack = false;

                        // Check all white pieces to see if they attack this square
                        for (int checkX = 0; checkX < 8 && !isUnderAttack; checkX++)
                        {
                            for (int checkY = 0; checkY < 8 && !isUnderAttack; checkY++)
                            {
                                if (board[checkX][checkY] > 0)
                                { // White piece
                                    // Temporarily remove king to avoid blocking checks
                                    int originalKing = board[x][y];
                                    board[x][y] = 0;

                                    // Check if this white piece can move to the target square
                                    if (isValidMove(checkX, checkY, newX, newY))
                                    {
                                        isUnderAttack = true;
                                    }

                                    // Restore king
                                    board[x][y] = originalKing;
                                }
                            }
                        }

                        if (!isUnderAttack)
                        {
                            moves.push_back({newX, newY});
                        }
                    }
                }
            }
        }
    }
    else if (abs(piece) == 11)
    { // Queen (combines rook and bishop)
        int directions[8][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
        for (auto dir : directions)
        {
            for (int i = 1; i < 8; ++i)
            {
                int newX = x + i * dir[0];
                int newY = y + i * dir[1];
                if (newX < 0 || newX >= 8 || newY < 0 || newY >= 8)
                    break;

                if (board[newX][newY] == 0)
                {
                    moves.push_back({newX, newY});
                }
                else
                {
                    if ((isWhitePiece && isBlack(newX, newY)) ||
                        (!isWhitePiece && isWhite(newX, newY)))
                    {
                        moves.push_back({newX, newY});
                    }
                    break;
                }
            }
        }
    }
    else if (abs(piece) == 8)
    { // Knight
        int knightMoves[8][2] = {
            {2, 1}, {2, -1}, {-2, 1}, {-2, -1}, {1, 2}, {1, -2}, {-1, 2}, {-1, -2}};
        for (int i = 0; i < 8; ++i)
        {
            int newX = x + knightMoves[i][0];
            int newY = y + knightMoves[i][1];
            if (newX >= 0 && newX < 8 && newY >= 0 && newY < 8)
            {
                if (board[newX][newY] == 0 ||
                    (isWhitePiece && isBlack(newX, newY)) ||
                    (!isWhitePiece && isWhite(newX, newY)))
                {
                    moves.push_back({newX, newY});
                }
            }
        }
    }

    return moves;
}

bool GameLogic::isValidMove(int x, int y, int xx, int yy)
{
    const vector<vector<int>> &moves = possibleMoves(x, y); // vector made by possible moves
    for (const auto &move : moves)
    {
        if (move[0] == xx && move[1] == yy)
        {
            // Check if the move would leave the king in check
            bool isWhitePiece = board[x][y] > 0;
            if (wouldBeInCheck(x, y, xx, yy, isWhitePiece))
            {
                return false; // Move would leave the king in check
            }
            return true;
        }
    }
    return false;
}

void GameLogic::movePiece(int x, int y, int xx, int yy)
{
    // Determine if this move puts the opponent in check
    bool isWhitePiece = board[x][y] > 0;
    bool putsInCheck = moveWouldCheck(x, y, xx, yy, isWhitePiece);

    // Reset en passant flag
    bool wasEnPassant = isEnPassantCapture(x, y, xx, yy);

    // Check if this move enables en passant
    bool enPassantMove = false;
    if (abs(board[x][y]) == 10) // Pawn
    {
        // White pawn moving 2 squares
        if (board[x][y] > 0 && y == 6 && yy == 4)
        {
            enPassantPossible = true;
            enPassantCol = x;
            enPassantRow = 5;
            enPassantMove = true;
        }
        // Black pawn moving 2 squares
        else if (board[x][y] < 0 && y == 1 && yy == 3)
        {
            enPassantPossible = true;
            enPassantCol = x;
            enPassantRow = 2;
            enPassantMove = true;
        }
    }

    if (!enPassantMove && !wasEnPassant)
    {
        resetEnPassant();
    }

    // Play move sound
    static sf::SoundBuffer buffer;
    static sf::Sound sound; // Corrected: sf::Sound instead of sf::sound

    if (putsInCheck)
        buffer.loadFromFile("coding/sounds/move-check.wav");
    else if (board[xx][yy] == 0 && !wasEnPassant)
        buffer.loadFromFile("coding/sounds/move-self.wav");
    else
        buffer.loadFromFile("coding/sounds/capture.wav");

    sound.setBuffer(buffer);
    sound.play();
    sound.setVolume(100);

    // Handle en passant capture (remove the captured pawn)
    if (wasEnPassant)
    {
        if (isWhitePiece)
        {
            // White capturing black - captured pawn is on the same column but one row below
            board[xx][yy + 1] = 0;
            chessBoard.getPieceSprites()[xx][yy + 1] = Sprite();
        }
        else
        {
            // Black capturing white - captured pawn is on the same column but one row above
            board[xx][yy - 1] = 0;
            chessBoard.getPieceSprites()[xx][yy - 1] = Sprite();
        }
    }

    // Move the piece on the board
    board[xx][yy] = board[x][y];
    board[x][y] = 0;

    // Update the sprites
    auto &pieceSprites = chessBoard.getPieceSprites();
    pieceSprites[xx][yy] = pieceSprites[x][y];
    pieceSprites[x][y] = Sprite();
}

vector<vector<int>> GameLogic::getAllMoves(bool color)
{
    vector<vector<int>> allMoves;
    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 8; ++j)
        {
            if ((color && isWhite(i, j)) || (!color && isBlack(i, j)))
            {
                vector<vector<int>> moves = possibleMoves(i, j);
                for (const auto &move : moves)
                {
                    allMoves.push_back({i, j, move[0], move[1]});
                }
            }
        }
    }
    return allMoves;
}

bool GameLogic::check(bool color)
{
    // Cache the king position for performance
    static int lastKingX[2] = {-1, -1}; // [0] for black, [1] for white
    static int lastKingY[2] = {-1, -1}; // [0] for black, [1] for white

    int kingValue = color ? 9 : -9; // 9 for white king, -9 for black king
    int kingX = -1, kingY = -1;
    int colorIndex = color ? 1 : 0;

    // Only search for king if position has changed or not yet found
    if (lastKingX[colorIndex] == -1 || board[lastKingX[colorIndex]][lastKingY[colorIndex]] != kingValue)
    {
        // Locate the king
        for (int i = 0; i < 8; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                if (board[i][j] == kingValue)
                {
                    kingX = i;
                    kingY = j;
                    // Cache the position
                    lastKingX[colorIndex] = kingX;
                    lastKingY[colorIndex] = kingY;
                    break;
                }
            }
            if (kingX != -1)
                break;
        }
    }
    else
    {
        // Use cached king position
        kingX = lastKingX[colorIndex];
        kingY = lastKingY[colorIndex];
    }

    // If king not found (shouldn't happen), return false
    if (kingX == -1 || kingY == -1)
    {
        cout << "ERROR: King not found on board! Color: " << (color ? "White" : "Black") << endl;
        return false;
    }

    // Optimize by only checking pieces that could potentially attack the king
    // For knights
    int knightMoves[8][2] = {{1, 2}, {2, 1}, {2, -1}, {1, -2}, {-1, -2}, {-2, -1}, {-2, 1}, {-1, 2}};
    for (int i = 0; i < 8; i++)
    {
        int checkX = kingX + knightMoves[i][0];
        int checkY = kingY + knightMoves[i][1];
        if (checkX >= 0 && checkX < 8 && checkY >= 0 && checkY < 8)
        {
            int piece = board[checkX][checkY];
            // If this is an opponent's knight
            if ((color && piece == -8) || (!color && piece == 8))
                return true;
        }
    }

    // For pieces that move in straight lines (rook, queen)
    int straightDirections[4][2] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
    for (int d = 0; d < 4; d++)
    {
        int dirX = straightDirections[d][0];
        int dirY = straightDirections[d][1];
        for (int step = 1; step < 8; step++)
        {
            int checkX = kingX + dirX * step;
            int checkY = kingY + dirY * step;
            if (checkX < 0 || checkX >= 8 || checkY < 0 || checkY >= 8)
                break; // Off the board

            int piece = board[checkX][checkY];
            if (piece != 0) // Not empty
            {
                if ((color && (piece == -6 || piece == -11)) || // Black rook or queen
                    (!color && (piece == 6 || piece == 11)))    // White rook or queen
                    return true;
                else
                    break; // Blocked by another piece
            }
        }
    }

    // For pieces that move diagonally (bishop, queen)
    int diagonalDirections[4][2] = {{1, 1}, {1, -1}, {-1, -1}, {-1, 1}};
    for (int d = 0; d < 4; d++)
    {
        int dirX = diagonalDirections[d][0];
        int dirY = diagonalDirections[d][1];
        for (int step = 1; step < 8; step++)
        {
            int checkX = kingX + dirX * step;
            int checkY = kingY + dirY * step;
            if (checkX < 0 || checkX >= 8 || checkY < 0 || checkY >= 8)
                break; // Off the board

            int piece = board[checkX][checkY];
            if (piece != 0) // Not empty
            {
                if ((color && (piece == -7 || piece == -11)) || // Black bishop or queen
                    (!color && (piece == 7 || piece == 11)))    // White bishop or queen
                    return true;
                else
                    break; // Blocked by another piece
            }
        }
    }

    // For pawns (need to check specific squares)
    if (color) // White king
    {
        // Check for black pawns that can capture
        if (kingY > 0)
        {
            if (kingX > 0 && board[kingX - 1][kingY - 1] == -10) // Black pawn capturing diagonally
                return true;
            if (kingX < 7 && board[kingX + 1][kingY - 1] == -10) // Black pawn capturing diagonally
                return true;
        }
    }
    else // Black king
    {
        // Check for white pawns that can capture
        if (kingY < 7)
        {
            if (kingX > 0 && board[kingX - 1][kingY + 1] == 10) // White pawn capturing diagonally
                return true;
            if (kingX < 7 && board[kingX + 1][kingY + 1] == 10) // White pawn capturing diagonally
                return true;
        }
    }

    // Check for opponent king (in case of adjacent kings)
    int enemyKingValue = color ? -9 : 9;
    for (int dx = -1; dx <= 1; dx++)
    {
        for (int dy = -1; dy <= 1; dy++)
        {
            if (dx == 0 && dy == 0)
                continue;

            int checkX = kingX + dx;
            int checkY = kingY + dy;
            if (checkX >= 0 && checkX < 8 && checkY >= 0 && checkY < 8)
            {
                if (board[checkX][checkY] == enemyKingValue)
                    return true;
            }
        }
    }

    return false;
}

bool GameLogic::checkMate(bool color)
{
    // If not in check, can't be checkmate
    if (!check(color))
        return false;

    // Try every possible move to see if any can get out of check
    for (int x = 0; x < 8; x++)
    {
        for (int y = 0; y < 8; y++)
        {
            // If this is the player's piece who might be in checkmate
            if ((color && board[x][y] > 0) || (!color && board[x][y] < 0))
            {
                // Get raw possible moves
                vector<vector<int>> moves = possibleMoves(x, y);

                // Try each move
                for (const auto &move : moves)
                {
                    int toX = move[0];
                    int toY = move[1];

                    // Save the board state
                    int capturedPiece = board[toX][toY];
                    int movingPiece = board[x][y];

                    // Make the move
                    board[toX][toY] = movingPiece;
                    board[x][y] = 0;

                    // Check if still in check
                    bool stillInCheck = check(color);

                    // Restore the board
                    board[x][y] = movingPiece;
                    board[toX][toY] = capturedPiece;

                    // If this move gets out of check, it's not checkmate
                    if (!stillInCheck)
                        return false;
                }
            }
        }
    }

    // No move can get out of check, it's checkmate
    return true;
}

// Validate if a move would leave the king in check
bool GameLogic::wouldBeInCheck(int fromX, int fromY, int toX, int toY, bool color)
{
    // Save the current state
    int capturedPiece = board[toX][toY];
    int movingPiece = board[fromX][fromY];

    // Account for en passant
    bool isEnPassantMove = isEnPassantCapture(fromX, fromY, toX, toY);
    int enPassantCapturedX = -1, enPassantCapturedY = -1;
    int enPassantCapturedPiece = 0;

    if (isEnPassantMove)
    {
        if (color) // White capturing black
        {
            enPassantCapturedX = toX;
            enPassantCapturedY = toY + 1;
        }
        else // Black capturing white
        {
            enPassantCapturedX = toX;
            enPassantCapturedY = toY - 1;
        }

        if (enPassantCapturedX >= 0 && enPassantCapturedX < 8 &&
            enPassantCapturedY >= 0 && enPassantCapturedY < 8)
        {
            enPassantCapturedPiece = board[enPassantCapturedX][enPassantCapturedY];
            board[enPassantCapturedX][enPassantCapturedY] = 0;
        }
    }

    // Make the move
    board[toX][toY] = movingPiece;
    board[fromX][fromY] = 0;

    // Check if this move leaves the king in check
    bool inCheck = check(color);

    // Restore the board
    board[fromX][fromY] = movingPiece;
    board[toX][toY] = capturedPiece;

    // Restore en passant captured piece if applicable
    if (isEnPassantMove && enPassantCapturedX != -1)
    {
        board[enPassantCapturedX][enPassantCapturedY] = enPassantCapturedPiece;
    }

    return inCheck;
}

bool GameLogic::moveWouldCheck(int fromX, int fromY, int toX, int toY, bool color)
{
    // Save the current state
    int capturedPiece = board[toX][toY];
    int movingPiece = board[fromX][fromY];

    // Account for en passant
    bool isEnPassantMove = isEnPassantCapture(fromX, fromY, toX, toY);
    int enPassantCapturedX = -1, enPassantCapturedY = -1;
    int enPassantCapturedPiece = 0;

    if (isEnPassantMove)
    {
        if (color) // White capturing black
        {
            enPassantCapturedX = toX;
            enPassantCapturedY = toY + 1;
        }
        else // Black capturing white
        {
            enPassantCapturedX = toX;
            enPassantCapturedY = toY - 1;
        }

        if (enPassantCapturedX >= 0 && enPassantCapturedX < 8 &&
            enPassantCapturedY >= 0 && enPassantCapturedY < 8)
        {
            enPassantCapturedPiece = board[enPassantCapturedX][enPassantCapturedY];
            board[enPassantCapturedX][enPassantCapturedY] = 0;
        }
    }

    // Make the move
    board[toX][toY] = movingPiece;
    board[fromX][fromY] = 0;

    // Check if this move puts the opponent in check
    bool putsInCheck = check(!color);

    // Restore the board
    board[fromX][fromY] = movingPiece;
    board[toX][toY] = capturedPiece;

    // Restore en passant captured piece if applicable
    if (isEnPassantMove && enPassantCapturedX != -1)
    {
        board[enPassantCapturedX][enPassantCapturedY] = enPassantCapturedPiece;
    }

    return putsInCheck;
}

void GameLogic::resetEnPassant()
{
    enPassantPossible = false;
    enPassantCol = -1;
    enPassantRow = -1;
}

bool GameLogic::isEnPassantCapture(int fromX, int fromY, int toX, int toY) const
{
    // Check if en passant is possible
    if (!enPassantPossible)
        return false;

    // Check if this is a pawn
    int piece = board[fromX][fromY];
    bool isWhitePiece = piece > 0;

    if (abs(piece) != 10) // Not a pawn
        return false;

    // Check if the move is diagonal
    if (abs(fromX - toX) != 1)
        return false;

    // For white pawns moving up (decreasing y)
    if (isWhitePiece && fromY - toY == 1 && toX == enPassantCol && toY == enPassantRow)
        return true;

    // For black pawns moving down (increasing y)
    if (!isWhitePiece && toY - fromY == 1 && toX == enPassantCol && toY == enPassantRow)
        return true;

    return false;
}