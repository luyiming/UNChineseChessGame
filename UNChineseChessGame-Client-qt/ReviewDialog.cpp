#include "ReviewDialog.h"
#include "ui_ReviewDialog.h"
#include "TitleBar.h"
#include "ui_TitleBar.h"
#include <QVBoxLayout>
#include <QPainter>
#include <QList>
#include "Square.h"
#include <QValidator>
#include <QDir>
#include <QFile>
#include <QDebug>
#include <QTextStream>
#include <QMessageBox>

ReviewDialog::ReviewDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ReviewDialog)
{
//    this->setWindowFlags(Qt::FramelessWindowHint);
//    this->setAttribute(Qt::WA_TranslucentBackground);
//    this->setMouseTracking(true);
    ui->setupUi(this);
    QDir dir("record");
    if(!dir.exists())
        ui->infoLabel->setText("record path not exist");
    dir.setFilter(QDir::Files);
    QFileInfoList fileList = dir.entryInfoList();
    for(int i = 0; i < fileList.size(); i++)
    {
        QString file = fileList.at(i).baseName();
        file.replace(8, 1, ":");
        file.replace(11, 1, ":");
        file.replace(14, 1, "");
        ui->recordList->addItem(file);
    }

//    titleBar = new TitleBar(this, "review");
//    connect(titleBar, &TitleBar::signal_close, this, &QDialog::close);

//    QVBoxLayout *mainLayout = new QVBoxLayout();
//    mainLayout->addWidget(titleBar);
//    mainLayout->setSpacing(0);
//    mainLayout->setMargin(5);
//    this->setLayout(mainLayout);

//    setStyleSheet("QPushButton{border-image:url(:/src/button/button-normal.png);}"
//                                      "QPushButton:hover{border-image: url(:/src/button/button-hover.png);}"
//                                      "QPushButton:pressed{border-image: url(:/src/button/button-pressed.png);}");
}

ReviewDialog::~ReviewDialog()
{
    delete ui;
}

void ReviewDialog::paintEvent(QPaintEvent *)
{
    QPainter p(this);

    p.drawPixmap(0,0,width(),height(),QPixmap(":/reviewBackground"));

}

void ReviewDialog::on_jumpButton_clicked()
{
    if(record.size() == 0)
        return;
    int round = ui->roundBox->currentText().toInt();
    int num = ui->numLine->text().toInt();
    emit signal_review(record.at(round).boards.at(num));
}

void ReviewDialog::on_nextButton_clicked()
{
    if(record.size() == 0)
        return;
    int round = ui->roundBox->currentText().toInt();
    int num = ui->numLine->text().toInt() + 1;
    if(num >= record[round].boards.size())
        return;
    ui->numLine->setText(QString::number(num));
    emit signal_review(record.at(round).boards.at(num));
}

void ReviewDialog::on_backButton_clicked()
{
    if(record.size() == 0)
        return;
    int round = ui->roundBox->currentText().toInt();
    int num = ui->numLine->text().toInt() - 1;
    if(num < 0)
        return;
    ui->numLine->setText(QString::number(num));
    emit signal_review(record.at(round).boards.at(num));
}

void ReviewDialog::on_roundBox_currentTextChanged(const QString &text)
{
    if(record.size() == 0)
        return;
    int round = text.toInt();
    QIntValidator v(0, record[round].boards.size() - 1, this);
    ui->numLine->setValidator(&v);
    ui->numLine->setText(QString::number(0));
    Board b;
    emit signal_review(b);
}

void ReviewDialog::on_importButton_clicked()
{
    if(ui->recordList->currentRow() < 0)
        return;
    QString filePath = ui->recordList->currentItem()->text();
    filePath.replace(8, 1, "h");
    filePath.replace(11, 1, "m");
    filePath.append("s.dat");

    QFile file(QString("record/") + filePath);
    file.open(QFile::ReadOnly);

    QTextStream fin(&file);

    record.clear();
    QString str;
    while(!fin.atEnd())
    {
        str = fin.readLine();
        if(str.length() == 2)
        {
            Record temp;
            temp.ownColor = str.at(1).toLatin1() - '0';
            record.append(temp);
        }
        else
        {
            Board b;
            for(int r = 0; r < ROWS; ++r)
            {
                for(int c = 0; c < COLS; ++c)
                {
                    if(str.at(r * 8 + c * 2) == "#")
                        b[r][c].reset();
                    else if(str.at(r * 8 + c * 2) == "-")
                        b[r][c].clear();
                    else
                    {
                        b[r][c].color = str.at(r * 8 + c * 2).toLatin1() - '0';
                        b[r][c].piece = str.at(r * 8 + c * 2 + 1).toLatin1() - '0';
                        b[r][c].empty = false;
                        b[r][c].valid = true;
                    }
                }
            }
            record.back().boards.append(b);
        }
    }
    QIntValidator v(0, record[0].boards.size() - 1, this);
    ui->numLine->setValidator(&v);
    ui->numLine->setText(QString::number(0));
    Board b;
    emit signal_review(b);
}

void ReviewDialog::on_deleteButton_clicked()
{
    QString filePath = ui->recordList->currentItem()->text();
    if(QMessageBox::Yes == QMessageBox::question(this, QString::fromUtf8("删除确认"), QString::fromUtf8("删除确认")))
    {
        filePath.replace(8, 1, "h");
        filePath.replace(11, 1, "m");
        filePath.append("s.dat");

        QFile file(QString("record/") + filePath);
        file.remove();

        ui->recordList->clear();
        QDir dir("record");
        if(!dir.exists())
            ui->infoLabel->setText("record path not exist");
        dir.setFilter(QDir::Files);
        QFileInfoList fileList = dir.entryInfoList();
        for(int i = 0; i < fileList.size(); i++)
        {
            QString file = fileList.at(i).baseName();
            file.replace(8, 1, ":");
            file.replace(11, 1, ":");
            file.replace(14, 1, "");
            ui->recordList->addItem(file);
        }
    }
    else
        return;
}

void ReviewDialog::on_quitButton_clicked()
{
    close();
}
