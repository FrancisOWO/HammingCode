#ifndef HCODEGEN_H
#define HCODEGEN_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QPropertyAnimation>
#include "subwindow.h"
#include "HammingBack.h"

#define HAMM_MAX    9
#define DATA_MAX    5
#define PARITY_MAX  4
#define OPRD_MAX    (HAMM_MAX+1)/2
#define OPTR_MAX    OPRD_MAX

#define INIT_STATUS -1

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

    HammingResult HammResult;   //海明码结构体
    int dataBits, parityBits;   //信息位数、校验位数
    int HammLink[HAMM_MAX];     //海明码对应的信息位/校验位
    int DataLink[DATA_MAX];     //信息位对应的海明码位
    int PrHnos[PARITY_MAX][OPRD_MAX];   //校验位对应的所有海明码位标号
    int PrHnoLen[PARITY_MAX];   //校验位对应的海明码位个数

    int speedLevel;         //动画速度
    int stepStatus;         //单步动画状态
    QString stepStatusStr;  //状态文字
    //按钮样式
    QString pbtnStyle0, pbtnStyle1, unknownStyle;
    QString hoverStyle, pressStyle, moveStyle;
    QString dataStyle, parityStyle;
    //数据块
    QPushButton HammBlk[HAMM_MAX];
    QPushButton DataBlk[DATA_MAX];
    QPushButton ParityBlk[PARITY_MAX];
    QPushButton PrRowBlks[PARITY_MAX][OPRD_MAX+1];  //操作数和结果
    //数据块标签
    QLabel HammLab[HAMM_MAX];
    QLabel HammLinkLab[HAMM_MAX];
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
    void InitStyles();      //初始化样式表

    int cvtDno2Hno(int index);      //信息码下标->海明码下标
    int cvtPno2Hno(int index);      //校验码下标->海明码下标
    QString getValStr(QPushButton *pbtn, int n);    //从数据块获取数字串

    void updateDataCode();  //更新信息码框

private slots:
    void changeSpeed();     //改变动画播放速度

    void setBlkVis(int data_bits);  //设置数据块/标签可见性
    QPropertyAnimation *moveBlk(QPushButton *pStart, QPushButton *pEnd,
                 int duration_ms, QString startStyle, QString endStyle);

    void moveDataBlk(int dno);      //移动信息码块：信息码区->海明码区
    void updateDataBlk(const QString &dataStr);         //更新信息码块（信息码区）
    void setBlkStatus(QPushButton *pbtn, int status);   //设置数据块状态
    void flipDataBlk(int dno);      //反转信息码块

    void genPrBlk(int pno);         //计算校验位，生成校验位区数据块
    void movePrBlk(int pno);        //移动校验码块：校验位区->校验码区
    void moveParityrBlk(int pno);   //移动校验码块：校验码区->海明码区

    void setStepInit();         //设置单步动画初始状态
    void updateStepStatus();    //更新单步动画状态
    void setStepFinishStatus(); //设置单步动画结束状态

    void setBlkUnknown();       //设置数据块状态未知

signals:
    void moveFinished();

};

#endif // HCODEGEN_H
