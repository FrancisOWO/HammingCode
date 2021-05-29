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

    //�����Ӵ���
    for(int i = 0; i < SUBWIN_MAX; i++){
        if(pSubWin[i] != nullptr)
            delete pSubWin[i];
    }
}

//��ʼ�����ݳ�Ա
void MainWindow::InitMembers()
{
    for(int i = 0; i < SUBWIN_MAX; i++){
        pSubWin[i] = nullptr;
        tabno[i] = -1;
    }
    ui->tabWidget->clear();     //��ձ�ǩҳ
    ui->tabWidget->setTabsClosable(true);   //��ǩҳ�ɹر�

}

//�����ź����
void MainWindow::InitConnections()
{
    //�˵���
    connect(ui->actAboutApp, SIGNAL(triggered()), this, SLOT(MsgAboutApp()));   //����Ӧ��
    //���Ӵ���
    connect(ui->pbtnHDist, SIGNAL(clicked()), this, SLOT(openWinHDist()));
    connect(ui->pbtnHErr, SIGNAL(clicked()), this, SLOT(openWinHErr()));
    connect(ui->pbtnHCodeGen, SIGNAL(clicked()), this, SLOT(openWinHCodeGen()));
    //�رձ�ǩҳ
    connect(ui->tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
}

//����Ӧ��
void MainWindow::MsgAboutApp()
{
    QString title = QString::fromLocal8Bit("����Ӧ��");
    QString info = QString::fromLocal8Bit("�������롿����ԭ���������ʾ�������������ݣ�\n"
            "1. ��������\n" "2. ���λ�������λ��\n" "3. ���������ɹ���");
    QMessageBox::information(this, title, info);
}

//�رձ�ǩҳ
void MainWindow::closeTab(int index)
{
    ui->tabWidget->removeTab(index);    //ɾ����ǩҳ
    int tempno = -1;
    for(int i = 0; i < SUBWIN_MAX; i++){
        if(index == tabno[i]){
            delete pSubWin[i];          //�����Ӵ���
            pSubWin[i] = nullptr;
            if(i == SUBWIN_HDIST)
                ui->pbtnHDist->setEnabled(true);    //���ð�ť
            else if(i == SUBWIN_HERR)
                ui->pbtnHErr->setEnabled(true);
            else if(i == SUBWIN_HCGEN)
                ui->pbtnHCodeGen->setEnabled(true);
            tempno = tabno[i];
            tabno[i] = -1;
            break;
        }
    }
    //������ǩҳ���
    for(int i = 0; i < SUBWIN_MAX; i++){
        if(tabno[i] > tempno)
            tabno[i]--;
    }
}

//��"��������"�Ӵ���
void MainWindow::openWinHDist()
{
    if(nullptr != pSubWin[SUBWIN_HDIST])
        return;
    //�½��Ӵ��壬��ӱ�ǩҳ
    pSubWin[SUBWIN_HDIST] = new HDist;
    tabno[SUBWIN_HDIST] = ui->tabWidget->addTab(pSubWin[SUBWIN_HDIST], QString::fromLocal8Bit("��������"));
    ui->tabWidget->setCurrentIndex(tabno[SUBWIN_HDIST]);    //���û��ǩҳ
    pSubWin[SUBWIN_HDIST]->show();
    ui->pbtnHDist->setEnabled(false);   //���ð�ť
}

//��"��������"�Ӵ���
void MainWindow::openWinHErr()
{
    if(nullptr != pSubWin[SUBWIN_HERR])
        return;
    //�½��Ӵ��壬��ӱ�ǩҳ
    pSubWin[SUBWIN_HERR] = new HErr;
    tabno[SUBWIN_HERR] = ui->tabWidget->addTab(pSubWin[SUBWIN_HERR], QString::fromLocal8Bit("��������"));
    ui->tabWidget->setCurrentIndex(tabno[SUBWIN_HERR]);    //���û��ǩҳ
    pSubWin[SUBWIN_HERR]->show();
    ui->pbtnHErr->setEnabled(false);   //���ð�ť
}

//��"����������"�Ӵ���
void MainWindow::openWinHCodeGen()
{
    if(nullptr != pSubWin[SUBWIN_HCGEN])
        return;
    //�½��Ӵ��壬��ӱ�ǩҳ
    pSubWin[SUBWIN_HCGEN] = new HCodeGen;
    tabno[SUBWIN_HCGEN] = ui->tabWidget->addTab(pSubWin[SUBWIN_HCGEN], QString::fromLocal8Bit("����������"));
    ui->tabWidget->setCurrentIndex(tabno[SUBWIN_HCGEN]);    //���û��ǩҳ
    pSubWin[SUBWIN_HCGEN]->show();
    ui->pbtnHCodeGen->setEnabled(false);   //���ð�ť
}
