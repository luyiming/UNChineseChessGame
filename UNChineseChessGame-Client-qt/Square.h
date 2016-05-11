#ifndef SQUARE_H
#define SQUARE_H

#include <QList>

#define ROWS 4
#define COLS 4

struct Square {
	bool empty;
	bool valid;
	int color;
	int piece;
    Square(): empty(false), valid(false), color(-1), piece(-1){}
	void moveTo(Square &des);
	void clear();
	void reset();
    Square(const Square &s);
    Square& operator = (Square& rhs);
    Square& operator = (const Square& rhs);
};

struct Board
{
private:
    Square board[ROWS][COLS];

public:
    Square* operator[](int i)
    {
        return board[i];
    }
    const Square* operator[](int i) const
    {
        return board[i];
    }
    Board()
    {
        for (int r = 0; r < ROWS; r++)
            for (int c = 0; c < COLS; c++)
                board[r][c].reset();
    }
    Board(const Board& b)
    {
        for (int r = 0; r < ROWS; r++)
            for (int c = 0; c < COLS; c++)
                board[r][c] = b[r][c];
    }
    Board& operator = (Board& rhs);
    Board& operator = (const Board& rhs);
};

struct Record
{
    int ownColor;
    int ownScore = -1, oppositeScore = -1;
    QList<Board> boards;
    Record() = default;
    Record(int ownColor) : ownColor(ownColor){}
};

#endif // MAINWINDOW_H
