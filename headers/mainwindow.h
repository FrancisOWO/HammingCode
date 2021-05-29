#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
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

    SubWindow *pSubWin[SUBWIN_MAX];
    int tabno[SUBWIN_MAX];

    void InitMembers();         //初始化数据成员
    void InitConnections();     //关联信号与槽

public slots:
    void closeTab(int index);   //关闭标签页

private slots:
    void MsgAboutApp();

    void openWinHDist();
    void openWinHErr();
    void openWinHCodeGen();

};
#endif // MAINWINDOW_H
