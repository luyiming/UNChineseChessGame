#ifndef REVIEWDIALOG_H
#define REVIEWDIALOG_H

#include <QDialog>
#include "TitleBar.h"
#include "ui_TitleBar.h"
#include <QList>
#include "Square.h"

namespace Ui {
class ReviewDialog;
}

class ReviewDialog : public QDialog
{
    Q_OBJECT

protected:
    void paintEvent(QPaintEvent *);

public:
    explicit ReviewDialog(QWidget *parent = 0);
    ~ReviewDialog();

signals:
    void signal_review(Board board);
    void reset_board();

private slots:
    void on_jumpButton_clicked();

    void on_nextButton_clicked();

    void on_backButton_clicked();

    void on_roundBox_currentTextChanged(const QString &arg1);

    void on_importButton_clicked();

    void on_deleteButton_clicked();

    void on_quitButton_clicked();

private:
    Ui::ReviewDialog *ui;
    //TitleBar *titleBar;
    QList<Record> record;
};

#endif // REVIEWDIALOG_H
