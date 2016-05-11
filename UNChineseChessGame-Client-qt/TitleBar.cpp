#include "TitleBar.h"
#include "ui_TitleBar.h"
#include <QLabel>
#include <QFont>
#include <QString>
#include <QPixmap>
#include <QToolButton>
#include <QObjectList>
#include <QHBoxLayout>
#include <QEvent>
#include <QMouseEvent>

static const int TITLE_H = 30;
static const int FRAME_BORDER = 2;
static const int VALUE_DIS = 3;

TitleBar::TitleBar(QWidget *parent, const QString &title) :
    QWidget(parent),
    ui(new Ui::TitleBar)
{
    ui->setupUi(this);
    this->title = title;
//    this->setWindowFlags(Qt::FramelessWindowHint);
//    this->setAttribute(Qt::WA_TranslucentBackground);
//    this->setMouseTracking(true);

    CreateWidget();
    SetWidgetStyle();
    CreateLayout();

    minButton->installEventFilter(this);
    closeButton->installEventFilter(this);
}

TitleBar::~TitleBar()
{
    delete ui;
}

//创建子部件
void TitleBar::CreateWidget()
{
    //图像标签--logo
    iconLabel = new QLabel(this);
    QPixmap objPixmap(":/src/chess/r-1.png");
    iconLabel->setPixmap(objPixmap.scaled(TITLE_H,TITLE_H));
    //文本标签--标题
    titleLabel = new QLabel(this);
    titleLabel->setText(this->title);
    //文本标签--版本
    versionLabel = new QLabel(this);
    versionLabel->setText(QString("version 1.0"));
    //设置鼠标形状
    versionLabel->setCursor(Qt::PointingHandCursor);
    //按钮--最小化
//    SetBtnIcon(minButton,btnDefault,true);
    minButton = new QToolButton();
    QPixmap minPixmap = this->style()->standardPixmap(QStyle::SP_TitleBarMinButton);
    minButton->setIcon(minPixmap);
    connect(minButton, SIGNAL(clicked()), this, SLOT(slotShowSmall()));
    //按钮--关闭
//    SetBtnIcon(closeButton,btnDefault,true);
    closeButton = new QToolButton();
    QPixmap closePixmap = this->style()->standardPixmap(QStyle::SP_TitleBarCloseButton);
    closeButton->setIcon(closePixmap);
    connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));

    //获得子部件
    const QObjectList &objList = children();
    for(int i=0; i < objList.count(); ++i)
    {
        //设置子部件的MouseTracking属性
        ((QWidget*)(objList.at(i)))->setMouseTracking(true);
        //如果是QToolButton部件
        if(0 == qstrcmp(objList.at(i)->metaObject()->className(),"QToolButton"))
        {
            //连接pressed信号为slot_btnpress
            connect(((QToolButton*)(objList.at(i))),SIGNAL(pressed()),this,SLOT(slot_btnpress()));
            //连接clicked信号为slot_btnclick
            connect(((QToolButton*)(objList.at(i))),SIGNAL(clicked()),this,SLOT(slot_btnclick()));
            //设置顶部间距
            ((QToolButton*)(objList.at(i)))->setContentsMargins(0,VALUE_DIS,0,0);
        }
    }
}

//设置子部件样式(qss)
void TitleBar::SetWidgetStyle()
{
    //设置标签的文本颜色，大小等以及按钮的边框
    setStyleSheet("QLabel{color:#CCCCCC;font-size:12px;font-weight:bold;}QToolButton{border:0px;}");
    //设置左边距
    titleLabel->setStyleSheet("margin-left:6px;");
    //设置右边距以及鼠标移上去时的文本颜色
    versionLabel->setStyleSheet("QLabel{margin-right:10px;}QLabel:hover{color:#00AA00;}");
}

//创建设置布局
void TitleBar::CreateLayout()
{
    //水平布局
    QHBoxLayout *titleLayout = new QHBoxLayout(this);
    //添加部件
    titleLayout->addWidget(iconLabel);
    titleLayout->addWidget(titleLabel);
    //添加伸缩项
//    titleLabel->setContentsMargins(5, 0, 0, 0);
    titleLayout->addStretch(1);
    //添加部件
    titleLayout->addWidget(versionLabel);
    titleLayout->addWidget(minButton, 0, Qt::AlignTop);
    titleLayout->addWidget(closeButton, 0, Qt::AlignTop);
    //设置Margin
    titleLayout->setContentsMargins(0,0,VALUE_DIS,0);
    //设置部件之间的space
    titleLayout->setSpacing(0);
    setLayout(titleLayout);
}

//设置按钮不同状态下的图标
void TitleBar::setButttonIcon(QToolButton *btn,eButtonState state, bool init/*=false*/)
{
    //获得图片路径
    QString imagePath = getBtnImagePath(btn,init);
    //创建QPixmap对象
    QPixmap pixmap(imagePath);
    //得到图像宽和高
    int nPixWidth = pixmap.width();
    int nPixHeight = pixmap.height();
    //如果状态不是无效值
    if(state != btnNone)
    {
        btn->setIcon(pixmap.copy((nPixWidth/4)*(state-1),0,nPixWidth/4,nPixHeight));
        //设置按钮图片大小
        btn->setIconSize(QSize(nPixWidth/4,nPixHeight));
    }
}

//获得图片路径(固定值)
const QString TitleBar::getBtnImagePath(QToolButton *btn, bool init/*=false*/)
{
    QString path;
    //最小化
    if(minButton==btn)
    {
        path = ":/image/sys_button_min.png";
    }
    //关闭按钮
    if(closeButton==btn)
    {
        path = ":/image/sys_button_close.png";
    }
    return path;
}

//事件过滤
bool TitleBar::eventFilter(QObject *obj, QEvent *event)
{
    //按钮状态
    eButtonState btnState = btnNone;
    //判断事件类型--QEvent::Enter
    if (event->type() == QEvent::Enter)
    {
        btnState = btnHover;
    }
    //判断事件类型--QEvent::Leave
    if (event->type() == QEvent::Leave)
    {
        btnState = btnDefault;
    }
    //判断事件类型--QEvent::MouseButtonPress
    if (event->type() == QEvent::MouseButtonPress && ((QMouseEvent*)(event))->button()== Qt::LeftButton)
    {
        btnState = btnPressed;
    }
    //判断目标
    if( minButton == obj || closeButton == obj)
    {
        //如果状态有效
        if(btnState != btnNone)
        {
            //根据状态设置按钮图标
            setButttonIcon((QToolButton *)obj, btnState);
            return false;
        }
    }
    return QWidget::eventFilter(obj,event);
}

//槽函数--slot_btnclick
void TitleBar::slot_btnclick()
{
    QToolButton *btn = (QToolButton*)(sender());
    if(btn == minButton)
    {
        emit signal_min();
    }
    if(btn == closeButton)
    {
        emit signal_close();
    }
}

void TitleBar::mousePressEvent(QMouseEvent *e)
{
    startPos = e->globalPos();
    clickPos = e->pos();

    if (e->button() == Qt::LeftButton)
    {
        if (e->type() == QEvent::MouseButtonPress)
        {
            isPress = true;
        }
    }
}

void TitleBar::mouseMoveEvent(QMouseEvent *e)
{
    if (isMax || !isPress)
    {
        return;
    }
    this->move(e->globalPos() - clickPos);
}

void TitleBar::mouseReleaseEvent(QMouseEvent *)
{
    isPress = false;
}
