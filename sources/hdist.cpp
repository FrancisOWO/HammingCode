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

//��ʼ�����ݳ�Ա
void HDist::InitMembers()
{

}

//�����ź����
void HDist::InitConnections()
{

}

//����ر�
void HDist::closeEvent(QCloseEvent *)
{
    emit WinClosed();
}
