#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Define.h"
#include "Square.h"
#include "ClientSocket.h"

#include <QMainWindow>
#include <QPainter>
#include <QDebug>
#include <QTimer>
#include <QStack>
#include <QList>

#define ROWS 4
#define COLS 4
#define ROUNDS 3


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *ev);

private slots:
    void on_connectButton_clicked();


    void on_quitButton_clicked();
    void on_aiButton_clicked();

signals:
    void test();
private:
    Ui::MainWindow *ui;
    int srcRow, srcCol, desRow, desCol;
    int curRound = 0;
    int ownColor = -1, oppositeColor = -1;
    bool isSelected = false;
    Board board;
    QStack<Board> boards;
    QTimer *countDownTimer;

    void authorize(const char *id, const char* password);
    void gameOver();
    void roundStart(int round);
    void roundOver(int round);
    int observe();
    void reversePiece(int row, int col);
    void movePiece(int srcRow, int srcCol, int desRow, int desCol);
    void noStep();
    void step();
    void oneRound();
    void saveChessBoard();

    void addInfo(QString s);

    void countDown(bool restart);

    int alphaBetaMax(Board& b, int depth, int alpha, int beta);
    int alphaBetaMin(Board& b, int depth, int alpha, int beta);
    void makeMoves(Board& b, int color, QList<Board>& moves);
    int evaluate(Board& b);
};

#endif // MAINWINDOW_H
