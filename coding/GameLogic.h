#ifndef GAMELOGIC_H
#define GAMELOGIC_H

#include <vector>
#include "ChessBoard.h"
#include <string.h>

using namespace std;

class GameLogic {
private:
    vector<vector<int>>& board;
    ChessBoard& chessBoard;

public:
    GameLogic(vector<vector<int>>& boardRef, ChessBoard& chessBoardRef);
    bool isWhite(int x,int y) const;
    bool isBlack(int x,int y) const;
    string whatPiece(int x,int y);
    vector <vector<int>> possibleMoves(int x,int y);
    vector <vector<int>> getAllMoves(bool color);
    void movePiece(int x, int y, int xx, int yy);
    bool check(bool color);
    bool checkMate(bool color);
    bool checkStaleMate(bool color);
    bool isValidMove(int x, int y, int xx, int yy);
};

#endif 