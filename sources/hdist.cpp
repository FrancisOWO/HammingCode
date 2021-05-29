#include "hdist.h"
#include "ui_hdist.h"

HDist::HDist(SubWindow *parent) :
    SubWindow(parent),
    ui(new Ui::HDist)
{
    ui->setupUi(this);

    InitMembers();
    InitConnections();
}

HDist::~HDist()
{
    delete ui;
}

//初始化数据成员
void HDist::InitMembers()
{

}

//关联信号与槽
void HDist::InitConnections()
{

}

//窗体关闭
void HDist::closeEvent(QCloseEvent *)
{
    emit WinClosed();
}
