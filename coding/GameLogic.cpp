#include "GameLogic.h"
#include "ChessBoard.h"
#include <math.h>
#include <SFML/Audio.hpp>

GameLogic::GameLogic(vector<vector<int>>& boardRef, ChessBoard& chessBoardRef) 
    : board(boardRef), chessBoard(chessBoardRef) {}

bool GameLogic::isWhite(int x, int y) const {
    if (x < 0 || x >= 8 || y < 0 || y >= 8) return false;
    return board[x][y] > 0;
}

bool GameLogic::isBlack(int x, int y) const {
    if (x < 0 || x >= 8 || y < 0 || y >= 8) return false;
    return board[x][y] < 0;
}

string GameLogic::whatPiece(int x, int y) {
    int piece = board[x][y];
    
    if (piece == 0) {
        return "Empty";
    }

    // White pieces
    if (piece > 0) {
        switch (piece) {
            case 6:  return "Wr";
            case 8:  return "Wk";
            case 7:  return "Wb";
            case 11: return "Wq";
            case 9:  return "Wp";
            case 10: return "Wn"; // Assuming 10 is knight (you didn't have this in original)
            default: return "UnknownWhite";
        }
    } 
    // Black pieces
    else {
        switch (-piece) { // Use positive value for switch
            case 6:  return "Br";
            case 8:  return "Bk";
            case 7:  return "Bb";
            case 11: return "Bq";
            case 9:  return "Bp";
            case 10: return "Bn"; // Assuming -10 is black knight
            default: return "UnknownBlack";
        }
    }
}

vector<vector<int>> GameLogic::possibleMoves(int x, int y) {
    vector<vector<int>> moves;
    int piece = board[x][y];
    if (piece == 0) return moves;
    
    bool isWhitePiece = piece > 0;
    
    if (piece == 10) { // White pawn
        // Forward move
        if (y > 0 && board[x][y-1] == 0) {
            moves.push_back({x, y-1});
            // Initial two-square move
            if (y == 6 && board[x][y-2] == 0) {
                moves.push_back({x, y-2});
            }
        }
        // Captures
        if (y > 0 && x > 0 && isBlack(x-1, y-1)) {
            moves.push_back({x-1, y-1});
        }
        if (y > 0 && x < 7 && isBlack(x+1, y-1)) {
            moves.push_back({x+1, y-1});
        }
    } 
    else if (piece == -10) { // Black pawn
        // Forward move
        if (y < 7 && board[x][y+1] == 0) {
            moves.push_back({x, y+1});
            // Initial two-square move
            if (y == 1 && board[x][y+2] == 0) {
                moves.push_back({x, y+2});
            }
        }
        // Captures
        if (y < 7 && x > 0 && isWhite(x-1, y+1)) {
            moves.push_back({x-1, y+1});
        }
        if (y < 7 && x < 7 && isWhite(x+1, y+1)) {
            moves.push_back({x+1, y+1});
        }
    }
    else if (abs(piece) == 6) { // Rook
        int directions[4][2] = {{1,0}, {-1,0}, {0,1}, {0,-1}};
        for (auto dir : directions) {
            for (int i = 1; i < 8; ++i) {
                int newX = x + i*dir[0];
                int newY = y + i*dir[1];
                if (newX < 0 || newX >= 8 || newY < 0 || newY >= 8) break;
                
                if (board[newX][newY] == 0) {
                    moves.push_back({newX, newY});
                } else {
                    if ((isWhitePiece && isBlack(newX, newY)) || 
                        (!isWhitePiece && isWhite(newX, newY))) {
                        moves.push_back({newX, newY});
                    }
                    break;
                }
            }
        }
    }
    else if (abs(piece) == 7) { // Bishop
        int directions[4][2] = {{1,1}, {1,-1}, {-1,1}, {-1,-1}};
        for (auto dir : directions) {
            for (int i = 1; i < 8; ++i) {
                int newX = x + i*dir[0];
                int newY = y + i*dir[1];
                if (newX < 0 || newX >= 8 || newY < 0 || newY >= 8) break;
                
                if (board[newX][newY] == 0) {
                    moves.push_back({newX, newY});
                } else {
                    if ((isWhitePiece && isBlack(newX, newY)) || 
                        (!isWhitePiece && isWhite(newX, newY))) {
                        moves.push_back({newX, newY});
                    }
                    break;
                }
            }
        }
    }
    else if (piece == 9) { // White king
        for (int i = -1; i <= 1; ++i) {
            for (int j = -1; j <= 1; ++j) {
                if (i == 0 && j == 0) continue;
                int newX = x + i;
                int newY = y + j;
                if (newX >= 0 && newX < 8 && newY >= 0 && newY < 8) {
                    // Check if square is empty or contains black piece
                    if (board[newX][newY] <= 0) {
                        // Check if this square is under attack by black pieces
                        bool isUnderAttack = false;
                        
                        // Check all black pieces to see if they attack this square
                        for (int checkX = 0; checkX < 8 && !isUnderAttack; checkX++) {
                            for (int checkY = 0; checkY < 8 && !isUnderAttack; checkY++) {
                                if (board[checkX][checkY] < 0) { // Black piece
                                    // Temporarily remove king to avoid blocking checks
                                    int originalKing = board[x][y];
                                    board[x][y] = 0;
                                    
                                    // Check if this black piece can move to the target square
                                    if (isValidMove(checkX, checkY, newX, newY)) {
                                        isUnderAttack = true;
                                    }
                                    
                                    // Restore king
                                    board[x][y] = originalKing;
                                }
                            }
                        }
                        
                        if (!isUnderAttack) {
                            moves.push_back({newX, newY});
                        }
                    }
                }
            }
        }
    }
    else if (piece == -9) { // Black king
        for (int i = -1; i <= 1; ++i) {
            for (int j = -1; j <= 1; ++j) {
                if (i == 0 && j == 0) continue;
                int newX = x + i;
                int newY = y + j;
                if (newX >= 0 && newX < 8 && newY >= 0 && newY < 8) {
                    // Check if square is empty or contains white piece
                    if (board[newX][newY] >= 0) {
                        // Check if this square is under attack by white pieces
                        bool isUnderAttack = false;
                        
                        // Check all white pieces to see if they attack this square
                        for (int checkX = 0; checkX < 8 && !isUnderAttack; checkX++) {
                            for (int checkY = 0; checkY < 8 && !isUnderAttack; checkY++) {
                                if (board[checkX][checkY] > 0) { // White piece
                                    // Temporarily remove king to avoid blocking checks
                                    int originalKing = board[x][y];
                                    board[x][y] = 0;
                                    
                                    // Check if this white piece can move to the target square
                                    if (isValidMove(checkX, checkY, newX, newY)) {
                                        isUnderAttack = true;
                                    }
                                    
                                    // Restore king
                                    board[x][y] = originalKing;
                                }
                            }
                        }
                        
                        if (!isUnderAttack) {
                            moves.push_back({newX, newY});
                        }
                    }
                }
            }
        }
    }
    else if (abs(piece) == 11) { // Queen (combines rook and bishop)
        int directions[8][2] = {{1,0}, {-1,0}, {0,1}, {0,-1}, 
                               {1,1}, {1,-1}, {-1,1}, {-1,-1}};
        for (auto dir : directions) {
            for (int i = 1; i < 8; ++i) {
                int newX = x + i*dir[0];
                int newY = y + i*dir[1];
                if (newX < 0 || newX >= 8 || newY < 0 || newY >= 8) break;
                
                if (board[newX][newY] == 0) {
                    moves.push_back({newX, newY});
                } else {
                    if ((isWhitePiece && isBlack(newX, newY)) || 
                        (!isWhitePiece && isWhite(newX, newY))) {
                        moves.push_back({newX, newY});
                    }
                    break;
                }
            }
        }
    }
    else if (abs(piece) == 8) { // Knight
        int knightMoves[8][2] = {
            {2, 1}, {2, -1}, {-2, 1}, {-2, -1},
            {1, 2}, {1, -2}, {-1, 2}, {-1, -2}
        };
        for (int i = 0; i < 8; ++i) {
            int newX = x + knightMoves[i][0];
            int newY = y + knightMoves[i][1];
            if (newX >= 0 && newX < 8 && newY >= 0 && newY < 8) {
                if (board[newX][newY] == 0 || 
                    (isWhitePiece && isBlack(newX, newY)) || 
                    (!isWhitePiece && isWhite(newX, newY))) {
                    moves.push_back({newX, newY});
                }
            }
        }
    }
    
    return moves;
}

bool GameLogic::isValidMove(int x, int y, int xx, int yy) {
    const vector<vector<int>>& moves = possibleMoves(x,y);//vector made by possible moves
    for (const auto& move : moves) {
        if (move[0] == xx && move[1] == yy) {
            return true;
        }
    }
    return false;
}

void GameLogic::movePiece(int x, int y, int xx, int yy) {
    // Play move sound
    static sf::SoundBuffer buffer;
    static sf::Sound sound;  // Corrected: sf::Sound instead of sf::sound
    if (board[xx][yy] == 0) buffer.loadFromFile("C:/Users/BL9/Desktop/chess/coding/sounds/move-self.wav");
    else buffer.loadFromFile("C:/Users/BL9/Desktop/chess/coding/sounds/capture.wav");
    sound.setBuffer(buffer);
    sound.play();
    sound.setVolume(100);
    // Move the piece on the board
    board[xx][yy] = board[x][y];
    board[x][y] = 0;

    // Update the sprites
    auto& pieceSprites = chessBoard.getPieceSprites();
    pieceSprites[xx][yy] = pieceSprites[x][y];
    pieceSprites[x][y] = Sprite();
}

vector <vector<int>> GameLogic::getAllMoves(bool color){
    vector <vector<int>> allMoves;
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            if ((color && isWhite(i,j)) || (!color && isBlack(i,j))) {
                vector<vector<int>> moves = possibleMoves(i,j);
                for (const auto& move : moves) {
                    allMoves.push_back({i, j, move[0], move[1]});
                }
            }
        }
    }
    return allMoves;
}

bool GameLogic::check(bool color) {
    vector<vector<int>> enemyMoves = getAllMoves(!color);
    int kingColor = color ? 1*9 : -1*9; // 1 for white, -1 for black
    
    // Locate the king
    int x = -1, y = -1;
    for (size_t i = 0; i < 8; i++) {
        for (size_t j = 0; j < 8; j++) {
            if (board[i][j] == kingColor) {
                x = i;
                y = j;
                goto king_found; // Exit both loops when king is found
            }
        }
    }
    
king_found:
    // If king wasn't found (shouldn't happen in chess)
    if (x == -1 || y == -1) {
        return false;
    }
    
    // Check if any enemy move targets the king's position
    for (const auto& move : enemyMoves) {
        if (move[0] == x && move[1] == y) {
            return true;
        }
    }
    return false;
}

bool GameLogic::checkMate(bool color) {
    if (!check(color)) return false; // Not in check, so not checkmate
    
    vector<vector<int>> allMoves = getAllMoves(color);
    
    return true; // No valid moves found, so it's checkmate
}