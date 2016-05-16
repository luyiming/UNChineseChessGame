#ifndef GAME_H
#define GAME_H

#include "Define.h"
#include "Square.h"
#include "ClientSocket.h"
#include <QObject>
#include <QString>
#include <QList>
#include <QStack>

class Game : public QObject
{
    Q_OBJECT

signals:
    void statusChanged(QString msg);
    void roundChanged(int round, int ownColor);

public:

    int curRound = 0;
    bool saveToFile = false;
    int ownColor = -1, oppositeColor = -1;
    bool isSelected = false;
    Board board;
    QList<Record> record;

public:
    void authorize(const char *id, const char *pass);
    void gameStart();
    void gameOver();
    void roundStart(int round);
    void oneRound();
    void roundOver(int round);
    int observe();
    void reversePiece(int row, int col);
    void movePiece(int srcRow, int srcCol, int desRow, int desCol);
    void noStep();
    void step();

    void setBoard(Board &board);
    int tryMove(int r, int c);
    void saveChessBoard();
    int alphaBetaMax(Board& b, int depth, int alpha, int beta);
    int alphaBetaMin(Board& b, int depth, int alpha, int beta);
    void makeMoves(Board& b, int color, QList<Board>& moves, QList<QList<int> > &msg);
    int evaluate(Board& b);
    void minimax();
    void minimaxStep();
    void reset();
};

#endif // MAINWINDOW_H
