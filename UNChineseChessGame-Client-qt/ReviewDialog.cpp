#include "ReviewDialog.h"
#include "ui_ReviewDialog.h"
#include "TitleBar.h"
#include "ui_TitleBar.h"
#include <QVBoxLayout>
#include <QPainter>
#include <QList>
#include "Square.h"
#include <QValidator>

ReviewDialog::ReviewDialog(QWidget *parent, QList<Record> &record) :
    QDialog(parent),
    ui(new Ui::ReviewDialog)
{
//    this->setWindowFlags(Qt::FramelessWindowHint);
//    this->setAttribute(Qt::WA_TranslucentBackground);
//    this->setMouseTracking(true);
    ui->setupUi(this);
    this->record = record;
    QIntValidator v(0, record[0].boards.size() - 1, this);
    ui->numLine->setValidator(&v);
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

    p.drawPixmap(0,0,width(),height(),QPixmap(":/src/background03.png"));

}

void ReviewDialog::on_jumpButton_clicked()
{
    int round = ui->roundBox->currentText().toInt();
    int num = ui->numLine->text().toInt();
    emit signal_review(round, num);
}

void ReviewDialog::on_nextButton_clicked()
{
    int round = ui->roundBox->currentText().toInt();
    int num = ui->numLine->text().toInt() + 1;
    if(num >= record[round].boards.size())
        return;
    ui->numLine->setText(QString::number(num));
    emit signal_review(round, num);
}

void ReviewDialog::on_backButton_clicked()
{
    int round = ui->roundBox->currentText().toInt();
    int num = ui->numLine->text().toInt() - 1;
    if(num < 0)
        return;
    ui->numLine->setText(QString::number(num));
    emit signal_review(round, num);
}

void ReviewDialog::on_roundBox_currentTextChanged(const QString &text)
{
    int round = text.toInt();
    QIntValidator v(0, record[round].boards.size() - 1, this);
    ui->numLine->setValidator(&v);
    ui->numLine->setText(QString::number(0));
}
