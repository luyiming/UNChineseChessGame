//#include "stdafx.h"
#include "Define.h"
#include "Square.h"
#include "ClientSocket.h"
#include <windows.h>
#include <conio.h>

#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QPainter>
#include <QDebug>
#include <QMouseEvent>
#include <QTimer>
#include <QDateTime>
#include <QString>
#include <QScrollBar>
#include <QList>
#include <QtTest/QTest>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setMouseTracking(true);

    ui->idLine->setText(ID);
    ui->ipLine->setText(SERVER_IP);
    ui->pwdLine->setText(PASSWORD);
    ui->portLine->setText(QString::number(SERVER_PORT));

    countDownTimer = new QTimer(this);
    connect(countDownTimer, &QTimer::timeout, [&](){this->countDown(false);});
}

void MainWindow::countDown(bool restart)
{
    static int t = 0;
    if(restart)
        t = 10;
    if(t == 0)
    {
        ui->timeLabel->setText("time out");
    }
    else
    {
        ui->timeLabel->setText(QString::number(t));
        t--;
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    QPen pen;
    pen.setWidth(3);
    pen.setColor(QColor(0,0,0));
    p.setPen(pen);

    //draw lines
    for(int i = 1; i <= 5; i++)
        p.drawLine(80 * i, 80, 80 * i, 400);
    for(int i = 1; i <= 5; i++)
        p.drawLine(80, 80 * i, 400, 80 * i);

    for (int r = 0; r < ROWS; r++)
    {
        for (int c = 0; c < COLS; ++c)
        {
            if(board[r][c].empty)
                continue;
            else if(!board[r][c].valid)
            {
                p.drawPixmap(80 * c + 90, 80 * r + 90, QPixmap(":/new/prefix1/src/back.png"));
            }
            else
            {
                if(board[r][c].color == 0)
                {
                    switch (board[r][c].piece)
                    {
                    case 0: p.drawPixmap(80 * c + 90, 80 * r + 90, QPixmap(":/new/prefix1/src/r-0.png")); break;
                    case 1: p.drawPixmap(80 * c + 90, 80 * r + 90, QPixmap(":/new/prefix1/src/r-1.png")); break;
                    case 2: p.drawPixmap(80 * c + 90, 80 * r + 90, QPixmap(":/new/prefix1/src/r-2.png")); break;
                    case 3: p.drawPixmap(80 * c + 90, 80 * r + 90, QPixmap(":/new/prefix1/src/r-3.png")); break;
                    case 4: p.drawPixmap(80 * c + 90, 80 * r + 90, QPixmap(":/new/prefix1/src/r-4.png")); break;
                    case 5: p.drawPixmap(80 * c + 90, 80 * r + 90, QPixmap(":/new/prefix1/src/r-5.png")); break;
                    case 6: p.drawPixmap(80 * c + 90, 80 * r + 90, QPixmap(":/new/prefix1/src/r-6.png")); break;
                    }
                }
                else
                {
                    switch (board[r][c].piece)
                    {
                    case 0: p.drawPixmap(80 * c + 90, 80 * r + 90, QPixmap(":/new/prefix1/src/b-0.png")); break;
                    case 1: p.drawPixmap(80 * c + 90, 80 * r + 90, QPixmap(":/new/prefix1/src/b-1.png")); break;
                    case 2: p.drawPixmap(80 * c + 90, 80 * r + 90, QPixmap(":/new/prefix1/src/b-2.png")); break;
                    case 3: p.drawPixmap(80 * c + 90, 80 * r + 90, QPixmap(":/new/prefix1/src/b-3.png")); break;
                    case 4: p.drawPixmap(80 * c + 90, 80 * r + 90, QPixmap(":/new/prefix1/src/b-4.png")); break;
                    case 5: p.drawPixmap(80 * c + 90, 80 * r + 90, QPixmap(":/new/prefix1/src/b-5.png")); break;
                    case 6: p.drawPixmap(80 * c + 90, 80 * r + 90, QPixmap(":/new/prefix1/src/b-6.png")); break;
                    }
                }
            }

        }
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    int x = event->x();
    int y = event->y();
    if(x >= 80 && x <= 400 && y >= 80 && y <= 400)
    {
        int c = x / 80 - 1;
        int r = y / 80 - 1;
        if(!isSelected)
        {
            if(!board[r][c].valid)
                reversePiece(r, c);
            else
            {
                isSelected = true;
                srcRow = r;
                srcCol = c;
                return;
            }
        }
        else
        {
            desRow = r;
            desCol = c;
            isSelected = false;
            movePiece(srcRow, srcCol, desRow, desCol);
        }
        int rtn = observe();
        if (rtn == 1)
        {
            roundOver(curRound);
            curRound++;
            roundStart(curRound);
            update();
            return;
        }
        else if(rtn == 2)
        {
            gameOver();
            update();
            close();
            addInfo("Close Socket\n");
            ui->connectButton->setEnabled(true);
            return;
        }
        saveChessBoard();
        rtn = observe();
        if (rtn == 1)
        {
            roundOver(curRound);
            curRound++;
            roundStart(curRound);
            update();
            return;
        }
        else if(rtn == 2)
        {
            gameOver();
            update();
            close();
            addInfo("Close Socket\n");
            ui->connectButton->setEnabled(true);
            return;
        }
        saveChessBoard();

        countDownTimer->stop();
        countDown(true);
        countDownTimer->start(1000);

        update();
        qDebug() <<"row:"<< r <<"column:"<< c;
    }
    qDebug() <<"X:"<< x <<"Y:"<< y;
}


void MainWindow::authorize(const char *id, const char *pass) {
    connectServer();
    addInfo(QString("Authorize ") + id + "\n");
    char msgBuf[BUFSIZE];
    memset(msgBuf, 0, BUFSIZE);
    msgBuf[0] = 'A';
    memcpy(&msgBuf[1], id, 9);
    memcpy(&msgBuf[10], pass, 6);
    sendMsg(msgBuf);
}



void MainWindow::gameOver() {
    addInfo("Game Over\n");
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            board[r][c].reset();
        }
    }
    ownColor = oppositeColor = -1;
    curRound = 0;
}

void MainWindow::roundStart(int round) {
    addInfo(QString("Round ") + curRound + " Start\n");
    ui->roundLabel->setText(QString::number(round));

    recvMsg();
    switch (recvBuf[1]) {
        case 'R':
            ownColor = 0;
            oppositeColor = 1;
            sendMsg("BR");
            break;
        case 'B':
            ownColor = 1;
            oppositeColor = 0;
            sendMsg("BB");
            break;
    }

    if (ownColor == 0)
        this->ui->roleLabel->setText("红色");
    else
        this->ui->roleLabel->setText("黑色");
}


void MainWindow::roundOver(int round)
{
    addInfo(QString("Round ") + round + " Over\n");
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            board[r][c].reset();
        }
    }
    ownColor = oppositeColor = -1;
}

int MainWindow::observe() {
    int rtn = 0;
    recvMsg();
    switch (recvBuf[0]) {
        case 'R':
        {
            switch (recvBuf[1]) {
                case '0':
                    switch (recvBuf[2]) {
                        case 'R':
                        {
                            int srcRow = recvBuf[3] - '0', srcCol = recvBuf[4] - '0';
                            board[srcRow][srcCol].valid = true;
                            board[srcRow][srcCol].color = recvBuf[7] - '0';
                            board[srcRow][srcCol].piece = recvBuf[8] - '0';
                            break;
                        }
                        case 'M':
                        {
                            int srcRow = recvBuf[3] - '0', srcCol = recvBuf[4] - '0';
                            int desRow = recvBuf[5] - '0', desCol = recvBuf[6] - '0';
                            if (board[srcRow][srcCol].piece == board[desRow][desCol].piece)
                            {
                                board[srcRow][srcCol].clear();
                                board[desRow][desCol].clear();
                            }
                            else {
                                board[srcRow][srcCol].moveTo(board[desRow][desCol]);
                            }
                            break;
                        }
                        case 'N':
                        {
                            break;
                        }
                    }
                    break;
                case '1':
                    addInfo("Error -1: Msg format error\n");
                    rtn = -1;
                    break;
                case '2':
                    addInfo("Error -2: Coordinate error\n");
                    rtn = -2;
                    break;
                case '3':
                    addInfo("Error -3: Piece color error\n");
                    rtn = -3;
                    break;
                case '4':
                    addInfo("Error -4: Invalid step\n");
                    rtn = -4;
                    break;
                default:
                    addInfo("Error -5: Other error\n");
                    rtn = -5;
                    break;
            }
            break;
        }
        case 'E':
        {
            switch (recvBuf[1]) {
                case '0':
                    // game over
                    rtn = 2;
                    break;
                case '1':
                    // round over
                    rtn = 1;
                default:
                    break;
            }
            break;
        }
    }
    return rtn;
}

void MainWindow::reversePiece(int row, int col) {
    char msg[6];
    msg[0] = 'S';
    msg[1] = 'R';
    msg[2] = '0' + row;
    msg[3] = '0' + col;
    msg[4] = '0' + row;
    msg[5] = '0' + col;
    sendMsg(msg);
}

void MainWindow::movePiece(int srcRow, int srcCol, int desRow, int desCol) {
    char msg[6];
    msg[0] = 'S';
    msg[1] = 'M';
    msg[2] = '0' + srcRow;
    msg[3] = '0' + srcCol;
    msg[4] = '0' + desRow;
    msg[5] = '0' + desCol;
    sendMsg(msg);
}

void MainWindow::noStep() {
    sendMsg("SN");
}

void MainWindow::saveChessBoard() {

}


void MainWindow::on_connectButton_clicked()
{
    authorize(ID, PASSWORD);
    addInfo("Game Start\n");
    roundStart(curRound);
    ui->connectButton->setDisabled(true);
}

int MainWindow::alphaBetaMax(Board& b, int depth, int alpha, int beta)
{
   if (depth == 0)
       return evaluate(b);
   QList<Board> moves;
   makeMoves(b, ownColor, moves);
   for(auto it = moves.begin(); it != moves.end(); ++it)
   {
      int score = alphaBetaMin(*it, depth - 1, alpha, beta);
      if(score >= beta)
         return beta;   //beta-cutoff
      if(score > alpha)
         alpha = score; // alpha acts like max in MiniMax
   }
   return alpha;
}

int MainWindow::alphaBetaMin(Board& b, int depth, int alpha, int beta)
{
   if (depth == 0)
       return -evaluate(b);
   QList<Board> moves;
   makeMoves(b, oppositeColor, moves);
   for(auto it = moves.begin(); it != moves.end(); ++it)
   {
      int score = alphaBetaMax(*it, depth - 1, alpha, beta);
      if(score <= alpha)
         return alpha; //alpha-cutoff
      if(score < beta)
         beta = score; // beta acts like min in MiniMax
   }
   return beta;
}


void MainWindow::makeMoves(Board& b, int color, QList<Board> & moves)
{
    moves.clear();
    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++)
        {
            if(b[r][c].empty)
                continue;
            if(b[r][c].valid && b[r][c].color == color)
            {
                if(b[r][c].piece == 1)//pao
                {
                    for(int i = -2; i <= 2; i += 4)
                        for(int j = -2; j <= 2; j+= 4)
                        {
                            int nr = r + i;
                            int nc = c + j;
                            if(nr < 0 || nr >= ROWS || nc < 0 || nc >= COLS)
                                continue;
                            if(!b[r+i/2][c+j/2].empty && !b[nr][nc].empty && b[nr][nc].valid && b[nr][nc].color != color)
                            {
                                Board nb = b;
                                nb[r][c].moveTo(nb[nr][nc]);
                                moves.push_back(nb);
                            }
                        }
                    for(int i = -1; i <= 1; i += 2)
                        for(int j = -1; j <= 1; j+= 2)
                        {
                            int nr = r + i;
                            int nc = c + j;
                            if(nr < 0 || nr >= ROWS || nc < 0 || nc >= COLS)
                                continue;
                            if(!b[nr][nc].empty)
                                continue;
                            Board nb = b;
                            nb[r][c].moveTo(nb[nr][nc]);
                            moves.push_back(nb);
                        }

                }
                else
                {
                    for(int i = -1; i <= 1; i += 2)
                        for(int j = -1; j <= 1; j+= 2)
                        {
                            int nr = r + i;
                            int nc = c + j;
                            if(nr < 0 || nr >= ROWS || nc < 0 || nc >= COLS)
                                continue;
                            if(!b[nr][nc].empty && !b[nr][nc].valid)
                                continue;
                            else if(!b[nr][nc].empty && b[nr][nc].color == color)
                                continue;
                            if(b[nr][nc].piece == b[r][c].piece)
                            {
                                Board nb = b;
                                nb[r][c].clear();
                                nb[nr][c].clear();
                                moves.push_back(nb);
                            }
                            else if((b[nr][nc].piece == 6 && b[r][c].piece == 0) || b[nr][nc].piece < b[r][c].piece)
                            {
                                if((b[nr][nc].piece == 0 && b[r][c].piece == 6))
                                    continue;
                                Board nb = b;
                                nb[r][c].moveTo(nb[nr][nc]);
                                moves.push_back(nb);
                            }
                        }
                }
            }
            else if(!b[r][c].valid)
            {
//                Board nb = b;
//                nb[r][c].clear();
//                nb[nr][c].clear();
//                moves.push_back(nb);
                continue;
            }
        }
}
int MainWindow::evaluate(Board& b)
{
    int score = 0;

    //piese value
    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++)
        {
            if(b[r][c].empty)
                continue;
            if(b[r][c].valid)
            {
                if(b[r][c].color == ownColor)
                    score += b[r][c].piece + 1;
                else
                    score -= b[r][c].piece + 1;
            }
        }
    return score;
}

void MainWindow::addInfo(QString s)
{
    ui->statusText->setText(ui->statusText->toPlainText() + s);
    QScrollBar *sb = ui->statusText->verticalScrollBar();
    sb->setValue(sb->maximum());
}

void MainWindow::on_quitButton_clicked()
{
    Board nb = board;
    board[0][0].moveTo(board[0][1]);
    update();
    addInfo(QString::number(evaluate(nb)) + "\n");
}

void MainWindow::oneRound() {
    switch (ownColor) {
    case 0:
        while (true) {
            step();
            if (observe() == 1) break;
            saveChessBoard();
            if (observe() == 1) break;
            saveChessBoard();
        }
        break;
    case 1:
        while (true) {
            step();
            if (observe() == 1) break;
            saveChessBoard();
            if (observe() == 1) break;
            saveChessBoard();
        }
        break;
    default:
        break;
    }
}
void MainWindow::step()
{
    int srcRow = -1, srcCol = -1, desRow = -1, desCol = -1;
    int score = -1000;
    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++)
        {
            if(board[r][c].empty)
                continue;
            if(board[r][c].valid && board[r][c].color == ownColor)
            {
                if(board[r][c].piece == 1)//pao
                {
                    for(int i = -2; i <= 2; i += 2)
                        for(int j = -2; j <= 2; j += 2)
                        {
                            if(i !=0 && j != 0)
                                continue;
                            if(i == 0 && j == 0)
                                continue;
                            int nr = r + i;
                            int nc = c + j;
                            if(nr < 0 || nr >= ROWS || nc < 0 || nc >= COLS)
                                continue;
                            if(!board[r+i/2][c+j/2].empty && !board[nr][nc].empty && board[nr][nc].valid && board[nr][nc].color != ownColor)
                            {
                                Board nb = board;
                                nb[r][c].moveTo(nb[nr][nc]);
                                int tempScore = evaluate(nb);
                                if(tempScore > score)
                                {
                                    score = tempScore;
                                    srcRow = r;
                                    srcCol = c;
                                    desRow = nr;
                                    desCol = nc;
                                }
                            }
                        }
                    for(int i = -1; i <= 1; i += 1)
                        for(int j = -1; j <= 1; j+= 1)
                        {
                            if(i !=0 && j != 0)
                                continue;
                            if(i == 0 && j == 0)
                                continue;
                            int nr = r + i;
                            int nc = c + j;
                            if(nr < 0 || nr >= ROWS || nc < 0 || nc >= COLS)
                                continue;
                            if(!board[nr][nc].empty)
                                continue;
                            Board nb = board;
                            nb[r][c].moveTo(nb[nr][nc]);
                            int tempScore = evaluate(nb);
                            if(tempScore > score)
                            {
                                score = tempScore;
                                srcRow = r;
                                srcCol = c;
                                desRow = nr;
                                desCol = nc;
                            }
                        }

                }
                else
                {
                    for(int i = -1; i <= 1; i += 1)
                        for(int j = -1; j <= 1; j+= 1)
                        {
                            if(i != 0 && j != 0)
                                continue;
                            if(i == 0 && j == 0)
                                continue;
                            int nr = r + i;
                            int nc = c + j;
                            if(nr < 0 || nr >= ROWS || nc < 0 || nc >= COLS)
                                continue;
                            if(!board[nr][nc].empty && !board[nr][nc].valid)
                                continue;
                            else if(!board[nr][nc].empty && board[nr][nc].color == ownColor)
                                continue;
                            if(board[nr][nc].piece == board[r][c].piece)
                            {
                                Board nb = board;
                                nb[r][c].clear();
                                nb[nr][nc].clear();
                                int tempScore = evaluate(nb);
                                if(tempScore > score)
                                {
                                    score = tempScore;
                                    srcRow = r;
                                    srcCol = c;
                                    desRow = nr;
                                    desCol = nc;
                                }
                            }
                            else if((board[nr][nc].piece == 6 && board[r][c].piece == 0) || board[nr][nc].piece < board[r][c].piece)
                            {
                                if((board[nr][nc].piece == 0 && board[r][c].piece == 6))
                                    continue;
                                Board nb = board;
                                nb[r][c].moveTo(nb[nr][nc]);
                                int tempScore = evaluate(nb);
                                if(tempScore > score)
                                {
                                    score = tempScore;
                                    srcRow = r;
                                    srcCol = c;
                                    desRow = nr;
                                    desCol = nc;
                                }
                            }
                        }
                }
            }
            else if(!board[r][c].valid)
            {
                for(int i = -2; i <= 2; i += 2)
                    for(int j = -2; j <= 2; j+= 2)
                    {
                        if(i !=0 && j != 0)
                            continue;
                        if(i == 0 && j == 0)
                            continue;
                        int nr = r + i;
                        int nc = c + j;
                        if(nr < 0 || nr >= ROWS || nc < 0 || nc >= COLS)
                            continue;
                        if(board[nr][nc].piece == 1 && board[nr][nc].color == ownColor)
                        {
                            int tempScore = evaluate(board) + 2;
                            if(tempScore > score)
                            {
                                score = tempScore;
                                srcRow = r;
                                srcCol = c;
                                desRow = -1;
                                desCol = -1;
                            }
                        }
                    }
                int tempScore = evaluate(board) + 1;
                if(tempScore > score)
                {
                    score = tempScore;
                    srcRow = r;
                    srcCol = c;
                    desRow = -1;
                    desCol = -1;
                }
            }
        }
    if(srcRow == -1 || srcCol == -1)
        noStep();
    else if(desRow == -1 || desCol == -1)
        reversePiece(srcRow, srcCol);
    else
        movePiece(srcRow, srcCol, desRow, desCol);
}
void MainWindow::on_aiButton_clicked()
{
    authorize(ID, PASSWORD);

    for (int round = 0; round < ROUNDS; round++) {
        roundStart(round);
        oneRound();
        roundOver(round);
    }
    gameOver();
    close();
}
