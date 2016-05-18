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
    void boardChanged();

public:

    Board board;
    QList<Record> record;

    Game();

    void setBoard(Board &board);
    void resetBoard();
    int tryMove(int r, int c);
    void setAiMode(bool mode);
    void connectToServer(QString ip, int port);
    void authorize(const char *id, const char *pass);

    void step();
    void minimaxStep();

    void learn(int cnt);

private:

    bool b_roundStart = false;
    bool b_aimove = false;
    int curPlayColor = -1;
    int curRound = 0;
    int ownColor = -1, oppositeColor = -1;
    bool isSelected = false;
    Board board_copy;
    QTcpSocket *tcpSocket;

    void gameStart();
    void gameOver();
    void roundStart(int round);
    void oneRound();
    void roundOver(int round);
    int observe();
    void reversePiece(int row, int col);
    void movePiece(int srcRow, int srcCol, int desRow, int desCol);
    void noStep();

    void saveChessBoard(bool saveToFile = false);
    double alphaBetaMax(Board& b, int depth, double alpha, double beta);
    double alphaBetaMin(Board& b, int depth, double alpha, double beta);
    void makeMoves(Board& b, int color, QList<Board>& moves, QList<QList<int> > &msg);
    double evaluate(const Board& b);
    double evaluateDark(const Board &b, int r, int c);

    void receiveMessage();
    void displayError(QAbstractSocket::SocketError);
    int sendMessage(const char* msg);

    void getScore(const Board& b, int &ownScore, int &oppositeScore);


    bool b_learn = false;
    int learn_cnt = -1;
    QString id_;
    QString pwd_;
    QString ip_;
    int port_;
    double ownScore = 0.0, oppoScore = 0.0;
};

#endif // MAINWINDOW_H
