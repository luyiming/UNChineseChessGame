#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Define.h"
#include "Square.h"
#include "ClientSocket.h"
#include "UNChineseChess.h"
#include "TitleBar.h"
#include "ui_TitleBar.h"
#include "ReviewDialog.h"
#include "ui_ReviewDialog.h"

#include <QMainWindow>
#include <QPainter>
#include <QDebug>
#include <QTimer>
#include <QStack>
#include <QList>
#include <QString>


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

    void on_nextButton_clicked();

    void on_reviewButton_clicked();

public slots:
    void addInfo(QString s);

private:
    Ui::MainWindow *ui;

    QTimer *countDownTimer;

    Game *game = new Game;

    int hr, hc;
    bool selected = false;

    void countDown(bool restart);

};

#endif // MAINWINDOW_H
