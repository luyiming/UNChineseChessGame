//#include "stdafx.h"
#include "Define.h"
#include "Square.h"
#include "ClientSocket.h"
#include <windows.h>
#include <conio.h>

#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "UNChineseChess.h"
#include "TitleBar.h"
#include "ui_TitleBar.h"
#include "ReviewDialog.h"
#include "ui_ReviewDialog.h"

#include <QPainter>
#include <QDebug>
#include <QMouseEvent>
#include <QTimer>
#include <QDateTime>
#include <QString>
#include <QScrollBar>
#include <QList>
#include <QPixmap>
#include <QIcon>
#include <QBitmap>
#include <QFile>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setMouseTracking(true);
    this->setWindowIcon(QIcon(":windowIcon"));

    ui->idLine->setText(ID);
    ui->ipLine->setText(SERVER_IP);
    ui->pwdLine->setText(PASSWORD);
    ui->portLine->setText(QString::number(SERVER_PORT));

    setStyleSheet("#connectButton{border-image:url(:/src/button/button-normal.png);}"
                  "#connectButton:hover{border-image: url(:/src/button/button-hover.png);}"
                  "#connectButton:pressed{border-image: url(:/src/button/button-pressed.png);}"
                  "#aiButton{border-image:url(:/src/button/button-normal.png);}"
                  "#aiButton:hover{border-image: url(:/src/button/button-hover.png);}"
                  "#aiButton:pressed{border-image: url(:/src/button/button-pressed.png);}"
                  "#nextButton{border-image:url(:/src/button/button-normal.png);}"
                  "#nextButton:hover{border-image: url(:/src/button/button-hover.png);}"
                  "#nextButton:pressed{border-image: url(:/src/button/button-pressed.png);}"
                  "#reviewButton{border-image:url(:/src/button/button-normal.png);}"
                  "#reviewButton:hover{border-image: url(:/src/button/button-hover.png);}"
                  "#reviewButton:pressed{border-image: url(:/src/button/button-pressed.png);}"
                  "#configButton{border-image:url(:/src/button/button-normal.png);}"
                  "#configButton:hover{border-image: url(:/src/button/button-hover.png);}"
                  "#configButton:pressed{border-image: url(:/src/button/button-pressed.png);}"
                  "#quitButton{border-image:url(:/src/button/button-normal.png);}"
                  "#quitButton:hover{border-image: url(:/src/button/button-hover.png);}"
                  "#quitButton:pressed{border-image: url(:/src/button/button-pressed.png);}"
                  "#backButton{border-image:url(:/src/button/button-normal.png);}"
                  "#backButton:hover{border-image: url(:/src/button/button-hover.png);}"
                  "#backButton:pressed{border-image: url(:/src/button/button-pressed.png);}"
                  "#jumpButton{border-image:url(:/src/button/button-normal.png);}"
                  "#jumpButton:hover{border-image: url(:/src/button/button-hover.png);}"
                  "#jumpButton:pressed{border-image: url(:/src/button/button-pressed.png);}"
                  "#importButton{border-image:url(:/src/button/button-normal.png);}"
                  "#importButton:hover{border-image: url(:/src/button/button-hover.png);}"
                  "#importButton:pressed{border-image: url(:/src/button/button-pressed.png);}"
                  "#deleteButton{border-image:url(:/src/button/button-normal.png);}"
                  "#deleteButton:hover{border-image: url(:/src/button/button-hover.png);}"
                  "#deleteButton:pressed{border-image: url(:/src/button/button-pressed.png);}");

//    ui->connectButton->resize(QPixmap(":/src/button/connect-normal.png").size());
//    //ui->pushButton->setIconSize(img.size());
//    //ui->pushButton->setIcon(QIcon(img));
//    ui->connectButton->setMask(QPixmap(":/src/button/connect-normal.png").mask());
//    ui->connectButton->setStyleSheet("QPushButton{border-image:url(:/src/button/connect-normal.png);}"
//                                  "QPushButton:hover{border-image: url(:/src/button/connect-hover.png);}"
//                                  "QPushButton:pressed{border-image: url(:/src/button/connect-pressed.png);}");

    countDownTimer = new QTimer(this);
    connect(countDownTimer, &QTimer::timeout, [&](){this->countDown(false);});

    connect(game, &Game::statusChanged, this, &MainWindow::addInfo);
    connect(game,
            &Game::roundChanged,
            [&](int round, int ownColor)
    {
        this->ui->roundLabel->setText(QString::number(round));
        if (ownColor == 0)
            this->ui->roleLabel->setText("红色");
        else
            this->ui->roleLabel->setText("黑色");
    });

    connect(game, &Game::boardChanged, [&](){this->update();});
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
    const int sx = 80, sy = 100; //start x, start y
    const int cell_w = 110, offset = 18;    //cell_width

    QPainter p(this);

    p.drawPixmap(0,0,width(),height(),QPixmap(":/background"));
    p.drawPixmap(sx, sy, QPixmap(":/src/chess/half-board.png"));

    if(selected)
        p.drawPixmap(sx + cell_w * hc + 16, sy + cell_w * hr + 14, QPixmap(":/src/chess/selected.png").scaled(QSize(101,100)));

    for (int r = 0; r < ROWS; r++)
    {
        for (int c = 0; c < COLS; ++c)
        {
            if(game->board[r][c].empty)
                continue;
            else if(!game->board[r][c].valid)
            {
                p.drawPixmap(sx + cell_w * c + offset, sy + cell_w * r + offset, QPixmap(":/src/chess/back.png"));
            }
            else
            {
                if(game->board[r][c].color == 0)
                {
                    switch (game->board[r][c].piece)
                    {
                    case 0: p.drawPixmap(sx + cell_w * c + offset, sy + cell_w * r + offset, QPixmap(":/src/chess/r-0.png")); break;
                    case 1: p.drawPixmap(sx + cell_w * c + offset, sy + cell_w * r + offset, QPixmap(":/src/chess/r-1.png")); break;
                    case 2: p.drawPixmap(sx + cell_w * c + offset, sy + cell_w * r + offset, QPixmap(":/src/chess/r-2.png")); break;
                    case 3: p.drawPixmap(sx + cell_w * c + offset, sy + cell_w * r + offset, QPixmap(":/src/chess/r-3.png")); break;
                    case 4: p.drawPixmap(sx + cell_w * c + offset, sy + cell_w * r + offset, QPixmap(":/src/chess/r-4.png")); break;
                    case 5: p.drawPixmap(sx + cell_w * c + offset, sy + cell_w * r + offset, QPixmap(":/src/chess/r-5.png")); break;
                    case 6: p.drawPixmap(sx + cell_w * c + offset, sy + cell_w * r + offset, QPixmap(":/src/chess/r-6.png")); break;
                    }
                }
                else
                {
                    switch (game->board[r][c].piece)
                    {
                    case 0: p.drawPixmap(sx + cell_w * c + offset, sy + cell_w * r + offset, QPixmap(":/src/chess/b-0.png")); break;
                    case 1: p.drawPixmap(sx + cell_w * c + offset, sy + cell_w * r + offset, QPixmap(":/src/chess/b-1.png")); break;
                    case 2: p.drawPixmap(sx + cell_w * c + offset, sy + cell_w * r + offset, QPixmap(":/src/chess/b-2.png")); break;
                    case 3: p.drawPixmap(sx + cell_w * c + offset, sy + cell_w * r + offset, QPixmap(":/src/chess/b-3.png")); break;
                    case 4: p.drawPixmap(sx + cell_w * c + offset, sy + cell_w * r + offset, QPixmap(":/src/chess/b-4.png")); break;
                    case 5: p.drawPixmap(sx + cell_w * c + offset, sy + cell_w * r + offset, QPixmap(":/src/chess/b-5.png")); break;
                    case 6: p.drawPixmap(sx + cell_w * c + offset, sy + cell_w * r + offset, QPixmap(":/src/chess/b-6.png")); break;
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

    if(x >= 90 && x <= 525 && y >= 90 && y <= 525)
    {
        int c = (x - 90) / 110;
        int r = (y - 90) / 110;
        if(0 == game->tryMove(r, c))
        {
            hr = r;
            hc = c;
            selected = true;
        }
        else
            selected = false;
        countDownTimer->stop();
        countDown(true);
        countDownTimer->start(1000);
        qDebug() <<"row:"<< r <<"column:"<< c;
    }
    else
    {
        game->tryMove(-1, -1);
        selected = false;
    }
    update();
    qDebug() <<"X:"<< x <<"Y:"<< y;
}

void MainWindow::on_connectButton_clicked()
{
    game->connectToServer(ui->ipLine->text(), ui->portLine->text().toInt());
    char id_[100] = {0};
    char password_[100] = {0};
    memcpy(id_, ui->idLine->text().toStdString().data(), ui->idLine->text().size());
    memcpy(password_, ui->pwdLine->text().toStdString().data(), ui->pwdLine->text().size());

    game->authorize(id_, password_);
//    //game->authorize(ID, PASSWORD);

}

void MainWindow::addInfo(QString s)
{
    ui->statusText->setText(ui->statusText->toPlainText() + s);
    QScrollBar *sb = ui->statusText->verticalScrollBar();
    sb->setValue(sb->maximum());
}

void MainWindow::on_quitButton_clicked()
{
    this->close();
}


void MainWindow::on_aiButton_clicked()
{
    game->setAiMode(true);

    game->connectToServer(ui->ipLine->text(), ui->portLine->text().toInt());

    char id_[100] = {0};
    char password_[100] = {0};
    memcpy(id_, ui->idLine->text().toStdString().data(), ui->idLine->text().size());
    memcpy(password_, ui->pwdLine->text().toStdString().data(), ui->pwdLine->text().size());
    game->authorize(id_, password_);
}

void MainWindow::on_nextButton_clicked()
{
    //game->step();
    game->minimaxStep();

    countDownTimer->stop();
    countDown(true);
    countDownTimer->start(1000);

//    game->learn(9);
//    game->connectToServer(ui->ipLine->text(), ui->portLine->text().toInt());

//    char id_[100] = {0};
//    char password_[100] = {0};
//    memcpy(id_, ui->idLine->text().toStdString().data(), ui->idLine->text().size());
//    memcpy(password_, ui->pwdLine->text().toStdString().data(), ui->pwdLine->text().size());
//    game->authorize(id_, password_);
    update();
}


void MainWindow::on_reviewButton_clicked()
{
    ReviewDialog *reviewDialog = new ReviewDialog(this);
    connect(reviewDialog, &ReviewDialog::signal_review, [&](Board board){game->setBoard(board); update();});
    connect(reviewDialog, &ReviewDialog::reset_board, [&](){game->resetBoard();});
    reviewDialog->show();
}




