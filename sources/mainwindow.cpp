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
        tabScrArea[i].setStyleSheet("background:white");    //������ɫ
        QString barColor = "background:#E8E8E8";            //������ǳ��ɫ
        tabScrArea[i].verticalScrollBar()->setStyleSheet(barColor);
        tabScrArea[i].horizontalScrollBar()->setStyleSheet(barColor);
        tabScrArea[i].setFrameShape(QFrame::NoFrame);       //�ޱ߿�
        pSubWin[i] = nullptr;   //�����Ӵ���
        tabno[i] = -1;      //��ǩҳ����÷Ƿ�ֵ
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
    connect(ui->pbtnHDist, &QPushButton::clicked, [=](){openSubWin(SUBWIN_HDIST);});
    connect(ui->pbtnHErr, &QPushButton::clicked, [=](){openSubWin(SUBWIN_HERR);});
    connect(ui->pbtnHCodeGen, &QPushButton::clicked, [=](){openSubWin(SUBWIN_HCGEN);});
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

//���Ӵ���
void MainWindow::openSubWin(int index)
{
    //index�Ƿ����˳�
    if(index < 0 || index > SUBWIN_MAX)
        return;
    //�Ӵ����Ѵ��ڣ���Ϊ���ǩҳ
    if(pSubWin[index] != nullptr){
        if(ui->tabWidget->currentIndex() != tabno[index])
            ui->tabWidget->setCurrentIndex(tabno[index]);
        return;
    }
    //�½��Ӵ��壬��ӱ�ǩҳ
    QString tabTitle;
    if(index == SUBWIN_HDIST){       //��������
        pSubWin[index] = new HDist;
        tabTitle = QString::fromLocal8Bit("��������");
    }
    else if(index == SUBWIN_HERR){   //��������
        pSubWin[index] = new HErr;
        tabTitle = QString::fromLocal8Bit("��������");
    }
    else if(index == SUBWIN_HCGEN){  //����������
        pSubWin[index] = new HCodeGen;
        tabTitle = QString::fromLocal8Bit("����������");
    }
    else
        return;
    //�Ӵ���󶨵�����������
    tabScrArea[index].setWidget(pSubWin[index]);
    ///���谴ť��ʽ��֮ǰ����ʽ��ScrollArea����ʽ���ǣ�
    ///�˲����Ϊ�ڸ��Ӵ���Ĺ��캯���ڽ��У������Ӵ��幹��ʱ���õ���ʽ��������
    /*
    QList<QPushButton *> pbtns = pSubWin[index]->findChildren<QPushButton *>();
    foreach (QPushButton *pbtn, pbtns){
        pbtn->setStyleSheet("background:#F0F0F0");
    } */
    //����������󶨵���ǩҳ
    tabno[index] = ui->tabWidget->addTab(&(tabScrArea[index]), tabTitle);
    ui->tabWidget->setCurrentIndex(tabno[index]);    //���û��ǩҳ
    pSubWin[index]->show();

}
