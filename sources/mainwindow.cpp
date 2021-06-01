#include "mainwindow.h"
#include "ui_mainwindow.h"

//#include <QList>
#include <QString>
#include <QMessageBox>
#include <QScrollBar>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    InitMembers();
    InitConnections();

}

MainWindow::~MainWindow()
{
    delete ui;

    //销毁子窗体
    for(int i = 0; i < SUBWIN_MAX; i++){
        if(pSubWin[i] != nullptr)
            delete pSubWin[i];
    }
}

//初始化数据成员
void MainWindow::InitMembers()
{
    for(int i = 0; i < SUBWIN_MAX; i++){
        tabScrArea[i].setStyleSheet("background:white");    //背景白色
        QString barColor = "background:#E8E8E8";            //滚动条浅灰色
        tabScrArea[i].verticalScrollBar()->setStyleSheet(barColor);
        tabScrArea[i].horizontalScrollBar()->setStyleSheet(barColor);
        tabScrArea[i].setFrameShape(QFrame::NoFrame);       //无边框
        pSubWin[i] = nullptr;   //暂无子窗体
        tabno[i] = -1;      //标签页编号置非法值
    }
    ui->tabWidget->clear();     //清空标签页
    ui->tabWidget->setTabsClosable(true);   //标签页可关闭

}

//关联信号与槽
void MainWindow::InitConnections()
{
    //菜单栏
    connect(ui->actAboutApp, SIGNAL(triggered()), this, SLOT(MsgAboutApp()));   //关于应用
    //打开子窗体
    connect(ui->pbtnHDist, &QPushButton::clicked, [=](){openSubWin(SUBWIN_HDIST);});
    connect(ui->pbtnHErr, &QPushButton::clicked, [=](){openSubWin(SUBWIN_HERR);});
    connect(ui->pbtnHCodeGen, &QPushButton::clicked, [=](){openSubWin(SUBWIN_HCGEN);});
    //关闭标签页
    connect(ui->tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
}

//关于应用
void MainWindow::MsgAboutApp()
{
    QString title = QString::fromLocal8Bit("关于应用");
    QString info = QString::fromLocal8Bit("【海明码】编码原理与过程演示，包括以下内容：\n"
            "1. 海明距离\n" "2. 检错位数与纠错位数\n" "3. 海明码生成过程");
    QMessageBox::information(this, title, info);
}

//关闭标签页
void MainWindow::closeTab(int index)
{
    ui->tabWidget->removeTab(index);    //删除标签页
    int tempno = -1;
    for(int i = 0; i < SUBWIN_MAX; i++){
        if(index == tabno[i]){
            delete pSubWin[i];          //销毁子窗体
            pSubWin[i] = nullptr;
            if(i == SUBWIN_HDIST)
                ui->pbtnHDist->setEnabled(true);    //启用按钮
            else if(i == SUBWIN_HERR)
                ui->pbtnHErr->setEnabled(true);
            else if(i == SUBWIN_HCGEN)
                ui->pbtnHCodeGen->setEnabled(true);
            tempno = tabno[i];
            tabno[i] = -1;
            break;
        }
    }
    //调整标签页编号
    for(int i = 0; i < SUBWIN_MAX; i++){
        if(tabno[i] > tempno)
            tabno[i]--;
    }
}

//打开子窗体
void MainWindow::openSubWin(int index)
{
    //index非法，退出
    if(index < 0 || index > SUBWIN_MAX)
        return;
    //子窗体已存在，置为活动标签页
    if(pSubWin[index] != nullptr){
        if(ui->tabWidget->currentIndex() != tabno[index])
            ui->tabWidget->setCurrentIndex(tabno[index]);
        return;
    }
    //新建子窗体，添加标签页
    QString tabTitle;
    if(index == SUBWIN_HDIST){       //海明距离
        pSubWin[index] = new HDist;
        tabTitle = QString::fromLocal8Bit("海明距离");
    }
    else if(index == SUBWIN_HERR){   //检错与纠错
        pSubWin[index] = new HErr;
        tabTitle = QString::fromLocal8Bit("检错与纠错");
    }
    else if(index == SUBWIN_HCGEN){  //海明码生成
        pSubWin[index] = new HCodeGen;
        tabTitle = QString::fromLocal8Bit("海明码生成");
    }
    else
        return;
    //子窗体绑定到滚动条区域
    tabScrArea[index].setWidget(pSubWin[index]);
    ///重设按钮样式（之前的样式被ScrollArea的样式覆盖）
    ///此步骤改为在各子窗体的构造函数内进行，否则子窗体构造时设置的样式将被覆盖
    /*
    QList<QPushButton *> pbtns = pSubWin[index]->findChildren<QPushButton *>();
    foreach (QPushButton *pbtn, pbtns){
        pbtn->setStyleSheet("background:#F0F0F0");
    } */
    //滚动条区域绑定到标签页
    tabno[index] = ui->tabWidget->addTab(&(tabScrArea[index]), tabTitle);
    ui->tabWidget->setCurrentIndex(tabno[index]);    //设置活动标签页
    pSubWin[index]->show();

}
