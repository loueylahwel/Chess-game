#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>

using namespace std;
using namespace sf;

const int WINDOW_WIDTH = 773;
const int WINDOW_HEIGHT = 773;
const float SCALE_FACTOR = 0.75;
const int BOARD_SIZE = 8;
const float SQUARE_SIZE = WINDOW_WIDTH / static_cast<float>(BOARD_SIZE);
const float PIECE_SCALE = 0.9f;

class ChessBoard {
private:
    RenderWindow window;
    vector<vector<int>> board;
    Texture blackpieces[6];
    Texture whitepieces[6];
    Sprite blackSprites[6];
    Sprite whiteSprites[6];
    vector<vector<Sprite>> pieceSprites; // 2D array for piece sprites

public:
    ChessBoard();
    vector<vector<Sprite>>& getPieceSprites() { return pieceSprites; }
    vector<vector<int>>& getMatrix();
    void setPiece(int x, int y, int value);
    void initBoard();
    int getPiece(int x, int y) const;
    bool loadTexture();
    void initializePieces();
    void drawPieces();
    void drawPiece(Sprite sprite, int x, int y);
    void run();
};

#endif // CHESSBOARD_H