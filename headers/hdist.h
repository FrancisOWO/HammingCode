#ifndef HDIST_H
#define HDIST_H

#include <QWidget>
#include "subwindow.h"

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

    void InitMembers();     //初始化数据成员
    void InitConnections(); //关联信号与槽

signals:
    void WinClosed();       //窗体关闭信号

};

#endif // HDIST_H
