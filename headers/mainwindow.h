#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QScrollArea>
#include "hdist.h"
#include "herr.h"
#include "hcodegen.h"

#define SUBWIN_HDIST    0
#define SUBWIN_HERR     1
#define SUBWIN_HCGEN    2
#define SUBWIN_MAX      3

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    QScrollArea tabScrArea[SUBWIN_MAX]; //��ǩҳ������
    SubWindow *pSubWin[SUBWIN_MAX];     //�Ӵ���
    int tabno[SUBWIN_MAX];              //�Ӵ����Ӧ�ı�ǩҳ���

    void InitMembers();         //��ʼ�����ݳ�Ա
    void InitConnections();     //�����ź����

public slots:
    void closeTab(int index);   //�رձ�ǩҳ

private slots:
    void MsgAboutApp();         //����Ӧ��

    void openSubWin(int index); //���Ӵ���

};
#endif // MAINWINDOW_H
