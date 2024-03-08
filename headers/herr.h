#ifndef HERR_H
#define HERR_H

#include <QWidget>
#include <QPushButton>
#include "subwindow.h"

#define BIT_MAX 8

namespace Ui {
class HErr;
}

class HErr : public SubWindow
{
    Q_OBJECT

public:
    explicit HErr(SubWindow *parent = nullptr);
    ~HErr();

private:
    Ui::HErr *ui;
    QPushButton DataIn1[BIT_MAX];
    QPushButton DataIn2[BIT_MAX];

    QString pbtnStyle0, pbtnStyle1;
    QString hoverStyle, pressStyle;


    void InitMembers();     //初始化数据成员
    void InitConnections(); //关联信号与槽

private slots:
    void updateNumBit(int val);
    void updateSpinRelated(int d);
    void newRandomData();
    void updateData2ByBit(int i);
    void setBlockStatus(QPushButton *pbtn, int status);
    void updateResultTable();
};

#endif // HERR_H
