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

//��ʼ�����ݳ�Ա
void HErr::InitMembers()
{

}

//�����ź����
void HErr::InitConnections()
{

}
