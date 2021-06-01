#ifndef HCODEGEN_H
#define HCODEGEN_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include "subwindow.h"
#include "HammingBack.h"

#define HAMM_MAX    9
#define DATA_MAX    5
#define PARITY_MAX  4
#define OPRD_MAX    (HAMM_MAX+1)/2
#define OPTR_MAX    OPRD_MAX

namespace Ui {
class HCodeGen;
}

class HCodeGen : public SubWindow
{
    Q_OBJECT

public:
    explicit HCodeGen(SubWindow *parent = nullptr);
    ~HCodeGen();

private:
    Ui::HCodeGen *ui;

    HammingResult HammStruct;   //海明码结构体
    //按钮样式
    QString pbtnStyle0, pbtnStyle1;
    QString hoverStyle, pressStyle;
    //数据块
    QPushButton HammBlk[HAMM_MAX];
    QPushButton DataBlk[DATA_MAX];
    QPushButton ParityBlk[PARITY_MAX];
    QPushButton PrRowBlks[PARITY_MAX][OPRD_MAX+1];  //操作数和结果
    //数据块标签
    QLabel HammLab[HAMM_MAX];
    QLabel DataLab[DATA_MAX];
    QLabel ParityLab[PARITY_MAX];
    QLabel *PrRowHead[PARITY_MAX];
    QLabel PrRowLabs[PARITY_MAX][OPRD_MAX+1];   //操作数和结果
    QLabel PrRowOptrLabs[PARITY_MAX][OPTR_MAX]; //操作符(标签区域)
    QLabel PrRowOptrBlks[PARITY_MAX][OPTR_MAX]; //操作符(数据块区域)
    QWidget *wgtPrBlk[PARITY_MAX];
    QWidget *wgtPrLab[PARITY_MAX];

    void InitMembers();     //初始化数据成员
    void InitConnections(); //关联信号与槽

private slots:
    void setBlkVis(int data_bits);  //设置数据块/标签可见性
};

#endif // HCODEGEN_H
