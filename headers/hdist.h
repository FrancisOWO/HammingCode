#ifndef HDIST_H
#define HDIST_H

#include <QWidget>
#include <QString>
#include <QPushButton>
#include "subwindow.h"

//输入输出
#define DATA_IN1    0
#define DATA_IN2    1
#define DATA_OUT    2

#define BIT_MAX 8

namespace Ui {
class HDist;
}

class HDist : public SubWindow
{
    Q_OBJECT

public:
    explicit HDist(SubWindow *parent = nullptr);
    ~HDist();

    void closeEvent(QCloseEvent *);

private:
    Ui::HDist *ui;
    QPushButton DataIn1[BIT_MAX];
    QPushButton DataIn2[BIT_MAX];
    QPushButton DataOut[BIT_MAX];

    QString pbtnStyle0, pbtnStyle1;

    void InitMembers();     //初始化数据成员
    void InitConnections(); //关联信号与槽

private slots:
    void updateIOBit(int val);          //更新输入输出位数
    void updateBlockData(int choice, int val); //根据十进制输入，更新二进制输入块
    void updateSpinData(int choice);    //根据二进制输入，更新十进制输入框

    void updateBlockIn1(int val);
    void updateBlockIn2(int val);

    void updateDataOut();

signals:
    void WinClosed();       //窗体关闭信号

};

#endif // HDIST_H
