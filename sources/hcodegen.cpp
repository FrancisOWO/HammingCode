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

//��ʼ�����ݳ�Ա
void HCodeGen::InitMembers()
{

}

//�����ź����
void HCodeGen::InitConnections()
{

}
