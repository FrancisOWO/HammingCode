#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>

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
        pSubWin[i] = nullptr;
        tabno[i] = -1;
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
    connect(ui->pbtnHDist, SIGNAL(clicked()), this, SLOT(openWinHDist()));
    connect(ui->pbtnHErr, SIGNAL(clicked()), this, SLOT(openWinHErr()));
    connect(ui->pbtnHCodeGen, SIGNAL(clicked()), this, SLOT(openWinHCodeGen()));
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

//打开"海明距离"子窗体
void MainWindow::openWinHDist()
{
    if(nullptr != pSubWin[SUBWIN_HDIST])
        return;
    //新建子窗体，添加标签页
    pSubWin[SUBWIN_HDIST] = new HDist;
    tabno[SUBWIN_HDIST] = ui->tabWidget->addTab(pSubWin[SUBWIN_HDIST], QString::fromLocal8Bit("海明距离"));
    ui->tabWidget->setCurrentIndex(tabno[SUBWIN_HDIST]);    //设置活动标签页
    pSubWin[SUBWIN_HDIST]->show();
    ui->pbtnHDist->setEnabled(false);   //禁用按钮
}

//打开"检错与纠错"子窗体
void MainWindow::openWinHErr()
{
    if(nullptr != pSubWin[SUBWIN_HERR])
        return;
    //新建子窗体，添加标签页
    pSubWin[SUBWIN_HERR] = new HErr;
    tabno[SUBWIN_HERR] = ui->tabWidget->addTab(pSubWin[SUBWIN_HERR], QString::fromLocal8Bit("检错与纠错"));
    ui->tabWidget->setCurrentIndex(tabno[SUBWIN_HERR]);    //设置活动标签页
    pSubWin[SUBWIN_HERR]->show();
    ui->pbtnHErr->setEnabled(false);   //禁用按钮
}

//打开"海明码生成"子窗体
void MainWindow::openWinHCodeGen()
{
    if(nullptr != pSubWin[SUBWIN_HCGEN])
        return;
    //新建子窗体，添加标签页
    pSubWin[SUBWIN_HCGEN] = new HCodeGen;
    tabno[SUBWIN_HCGEN] = ui->tabWidget->addTab(pSubWin[SUBWIN_HCGEN], QString::fromLocal8Bit("海明码生成"));
    ui->tabWidget->setCurrentIndex(tabno[SUBWIN_HCGEN]);    //设置活动标签页
    pSubWin[SUBWIN_HCGEN]->show();
    ui->pbtnHCodeGen->setEnabled(false);   //禁用按钮
}
