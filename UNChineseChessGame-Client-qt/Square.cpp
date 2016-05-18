//#include "stdafx.h"
#include "Square.h"

void Square::moveTo(Square &des) {
	des.empty = false;
	des.valid = true;
	des.color = color;
	des.piece = piece;
	clear();
}

void Square::clear() {
	empty = true;
	valid = true;
	color = -1;
	piece = -1;
}

void Square::reset() {
	empty = false;
	valid = false;
	color = -1;
	piece = -1;
}

Square::Square(const Square &s)
{
    empty = s.empty;
    valid = s.valid;
    color = s.color;
    piece = s.piece;
}
Square& Square::operator = (Square &rhs)
{
    empty = rhs.empty;
    valid = rhs.valid;
    color = rhs.color;
    piece = rhs.piece;
    return *this;
}
Square& Square::operator = (const Square &rhs)
{
    empty = rhs.empty;
    valid = rhs.valid;
    color = rhs.color;
    piece = rhs.piece;
    return *this;
}
Board& Board::operator = (Board& rhs)
{
    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++)
            board[r][c] = rhs[r][c];
    for(int i = 0; i < 20; i++)
        this->remainPieces[i] = rhs.remainPieces[i];
    return *this;
}

Board& Board::operator = (const Board& rhs)
{
    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++)
            board[r][c] = rhs[r][c];
    for(int i = 0; i < 20; i++)
        this->remainPieces[i] = rhs.remainPieces[i];
    return *this;
}

bool Board::empty()
{
    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++)
            if(this->board[r][c].color != -1)
                return false;
    return true;
}

void Board::reset()
{
    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++)
            this->board[r][c].reset();

    for(int i = 0; i < 20; i++)
        remainPieces[i] = 1;

    remainPieces[0] = 2;
    remainPieces[10] = 2;
    return;
}
