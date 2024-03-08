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

    QScrollArea tabScrArea[SUBWIN_MAX]; //标签页滚动条
    SubWindow *pSubWin[SUBWIN_MAX];     //子窗体
    int tabno[SUBWIN_MAX];              //子窗体对应的标签页编号

    void InitMembers();         //初始化数据成员
    void InitConnections();     //关联信号与槽

public slots:
    void closeTab(int index);   //关闭标签页

private slots:
    void MsgAboutApp();         //关于应用

    void openSubWin(int index); //打开子窗体

};
#endif // MAINWINDOW_H
