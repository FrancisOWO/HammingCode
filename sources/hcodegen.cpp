#include "hcodegen.h"
#include "ui_hcodegen.h"

HCodeGen::HCodeGen(SubWindow *parent) :
    SubWindow(parent),
    ui(new Ui::HCodeGen)
{
    ui->setupUi(this);

    InitMembers();
    InitConnections();
}

HCodeGen::~HCodeGen()
{
    delete ui;
}

//初始化数据成员
void HCodeGen::InitMembers()
{

}

//关联信号与槽
void HCodeGen::InitConnections()
{

}
