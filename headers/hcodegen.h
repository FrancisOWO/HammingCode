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

#define CODE_IN     0
#define CODE_OUT    1

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
    int PrHnos[PARITY_MAX][OPRD_MAX+1];     //校验位对应的所有海明码位标号
    int PrHnoLen[PARITY_MAX];   //校验位对应的海明码位个数

    int speedLevel;         //动画速度
    int stepStatus;         //计算海明码单步动画状态
    int checkStatus;        //纠错检错单步动画状态
    QString stepStatusStr;  //状态文字
    //按钮样式
    QString pbtnStyle0, pbtnStyle1, unknownStyle;
    QString hoverStyle, pressStyle, moveStyle;
    QString dataStyle, parityStyle;
    //数据块
    QPushButton HammBlk[HAMM_MAX];      //海明码
    QPushButton DataBlk[DATA_MAX];      //信息码
    QPushButton ParityBlk[PARITY_MAX];  //校验码
    QPushButton PrRowBlks[PARITY_MAX][OPRD_MAX+2];  //操作数和结果
    QPushButton CodeBlk[2][HAMM_MAX];   //待校验数据/纠错后数据

    //数据块标签
    QLabel HammLab[HAMM_MAX];       //海明码
    QLabel HammLinkLab[HAMM_MAX];   //海明码对应的信息位/校验位
    QLabel DataLab[DATA_MAX];       //信息码
    QLabel ParityLab[PARITY_MAX];   //校验码
    QLabel *PrRowHead[PARITY_MAX];
    QLabel PrRowLabs[PARITY_MAX][OPRD_MAX+2];     //操作数和结果
    QLabel PrRowOptrLabs[PARITY_MAX][OPTR_MAX+1]; //操作符(标签区域)
    QLabel PrRowOptrBlks[PARITY_MAX][OPTR_MAX+1]; //操作符(数据块区域)
    QLabel CodeLab[2][HAMM_MAX];
    QLabel HammLinkLab2[HAMM_MAX];

    QWidget *wgtPrBlk[PARITY_MAX];
    QWidget *wgtPrLab[PARITY_MAX];

    void InitMembers();     //初始化数据成员
    void InitConnections(); //关联信号与槽
    void InitStyles();      //初始化样式表

    void setPbtnProp(QPushButton *pbtn, QWidget *parent, QString style, QString text);
    void setLabProp(QLabel *lab, QWidget *parent, QString style, Qt::Alignment align, QString text);

    int cvtDno2Hno(int index);      //信息码下标->海明码下标
    int cvtPno2Hno(int index);      //校验码下标->海明码下标
    int cvtPrHno2Hno(int pno, int index);   //计算校验码的操作数下标->海明码下标
    QString getValStr(QPushButton *pbtn, int n);    //从数据块获取数字串

    void updateDataCode();  //更新信息码框
    void updatePrBitLab();  //更新校验位二进制标签
    void setGenBtnsEnabled(bool flag);      //启用/禁用按钮组（生成海明码按钮组）
    void setCheckBtnsEnabled(bool flag);    //启用/禁用按钮组（检错纠错按钮组）

    void setCheckStatusInit();

private slots:
    void changeSpeed();     //改变动画播放速度

    void setBlkUnknown();       //设置数据块状态未知
    void setBlkVis(int data_bits);  //设置数据块/标签可见性
    QPropertyAnimation *moveBlk(QPushButton *pStart, QPushButton *pEnd,
                 int duration_ms, QString startStyle, QString endStyle);

    void moveDataBlk(int dno);      //移动信息码块：信息码区->海明码区
    void updateDataBlk(const QString &dataStr);         //更新信息码块（信息码区）
    void setBlkStatus(QPushButton *pbtn, int status);   //设置数据块状态
    void flipDataBlk(QPushButton *pbtns, int dno);      //反转信息码块

    void genPrBlk(int pno);         //计算校验位，生成校验位区数据块
    void movePrBlk(int pno);        //移动校验码块：校验位区->校验码区
    void moveParityrBlk(int pno);   //移动校验码块：校验码区->海明码区

    void setStepInit();         //设置单步动画初始状态
    void updateStepStatus();    //更新单步动画状态
    void setStepFinishStatus(); //设置单步动画结束状态
    void genAllBlk();           //一次性生成海明码

    void updateCheckStep();     //检错纠错单步动画状态
    void setCheckStepFinish();  //动画结束

    void genCheckBlk(int pno);  //数据校验
    void getCheckResult(int pno);   //校验结果
    void genAllCheck();         //一次性生成校验结果

    void updateLegalCodeTable();  //填充合法编码表

signals:
    void moveFinished();

};

#endif // HCODEGEN_H
