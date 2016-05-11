#ifndef TITLEBAR_H
#define TITLEBAR_H

#include <QWidget>
#include <QString>
#include <QToolButton>
#include <QPoint>
#include <QLabel>
#include <QEvent>

//枚举，按钮状态
enum eButtonState{
     btnNone,//无效
     btnDefault,//默认值(如按钮初始显示)
     btnHover,//鼠标移到按钮上状态
     btnPressed//鼠标按下按钮时状态
};

namespace Ui {
class TitleBar;
}

class TitleBar : public QWidget
{
    Q_OBJECT

public slots:
    void slot_btnclick();

signals:
    void signal_min();
    void signal_close();

protected:
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    bool eventFilter(QObject *, QEvent *);

public:
    explicit TitleBar(QWidget *parent = 0, const QString &title = "");
    ~TitleBar();
    void CreateWidget();
    void SetWidgetStyle();
    void CreateLayout();
    void setButttonIcon(QToolButton *btn,eButtonState state, bool init=false);
    const QString getBtnImagePath(QToolButton *btn, bool init=false);

private:
    Ui::TitleBar *ui;
    QString title;
    QToolButton *minButton, *maxButton, *closeButton;
    QLabel *iconLabel, *titleLabel, *versionLabel;
    bool isPress, isMax;
    QPoint startPos;
    QPoint clickPos;
    QPixmap maxPixmap;
    QPixmap restorePixmap;
};

#endif // TITLEBAR_H
