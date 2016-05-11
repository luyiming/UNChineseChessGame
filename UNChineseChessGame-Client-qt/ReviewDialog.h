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
    explicit ReviewDialog(QWidget *parent = 0, QList<Record> &record = QList<Record>());
    ~ReviewDialog();

signals:
    void signal_review(int round, int num);

private slots:
    void on_jumpButton_clicked();

    void on_nextButton_clicked();

    void on_backButton_clicked();

    void on_roundBox_currentTextChanged(const QString &arg1);

private:
    Ui::ReviewDialog *ui;
    //TitleBar *titleBar;
    QList<Record> record;
};

#endif // REVIEWDIALOG_H
