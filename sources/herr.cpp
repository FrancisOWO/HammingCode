#include "herr.h"
#include "ui_herr.h"

HErr::HErr(SubWindow *parent) :
    SubWindow(parent),
    ui(new Ui::HErr)
{
    ui->setupUi(this);

    InitMembers();
    InitConnections();
}

HErr::~HErr()
{
    delete ui;
}

//初始化数据成员
void HErr::InitMembers()
{

}

//关联信号与槽
void HErr::InitConnections()
{

}
