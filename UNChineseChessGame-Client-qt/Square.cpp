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
    return *this;
}

Board& Board::operator = (const Board& rhs)
{
    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++)
            board[r][c] = rhs[r][c];
    return *this;
}
