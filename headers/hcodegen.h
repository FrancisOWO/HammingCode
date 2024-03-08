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

#define CODE_OK     0
#define CODE_ERR    1

#define BLK_NORMAL  0
#define BLK_CHECK   1

#define STYLE_NORMAL    0
#define STYLE_HLIGHT    1

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

    int hammGenFlag;        //海明码生成标志
    int rowChanged;         //表格中高亮的行标号
    int speedLevel;         //动画速度
    int stepStatus;         //计算海明码单步动画状态
    int checkStatus;        //纠错检错单步动画状态

    bool codeChangeFlag[HAMM_MAX];  //待纠错数据改变标记
    bool CodeInStatus[HAMM_MAX];    //待纠错数据状态：正确/错误
    QString stepStatusStr;  //单步动画状态文字

    //样式
    QString pbtnStyle0, pbtnStyle1, unknownStyle;
    QString hoverStyle, pressStyle, moveStyle;
    QString dataStyle, parityStyle;
    QString okStyle, errStyle, chgStyle;

    //数据块
    QPushButton HammBlk[HAMM_MAX];      //海明码
    QPushButton DataBlk[DATA_MAX];      //信息码
    QPushButton ParityBlk[PARITY_MAX];  //校验码
    QPushButton PrRowBlks[PARITY_MAX][OPRD_MAX+2];  //操作数和结果
    QPushButton CodeBlk[2][HAMM_MAX];   //待校验数据/纠错后数据

    //数据块标签
    QLabel HammLab[HAMM_MAX];           //海明码
    QLabel HammLinkLab[HAMM_MAX];       //海明码位对应的信息位/校验位
    QLabel DataLab[DATA_MAX];           //信息码
    QLabel ParityLab[PARITY_MAX];       //校验码
    QLabel *PrRowHead[PARITY_MAX];      //校验位行首标记
    QLabel PrRowLabs[PARITY_MAX][OPRD_MAX+2];           //操作数和结果
    QLabel PrRowOptrLabs[PARITY_MAX][OPTR_MAX+1];       //操作符(标签区域)
    QLabel PrRowOptrBlkLabs[PARITY_MAX][OPTR_MAX+1];    //操作符(数据块区域)
    QLabel CodeLab[2][HAMM_MAX];        //待校验数据/纠错后数据
    QLabel HammLinkLab2[HAMM_MAX];      //待校验数据 海明码位对应的信息位/校验位

    //区域
    QWidget *wgtPrBlk[PARITY_MAX];      //校验区数据块
    QWidget *wgtPrLab[PARITY_MAX];      //校验区标签

    /********************* 初始化 *********************/
    void InitMembers();         //初始化数据成员
    void InitConnections();     //关联信号与槽
    void InitStyles();          //初始化样式表

    /******************** 属性设置 ********************/
    void setPbtnProp(QPushButton *pbtn, QWidget *parent, QString style, QString text);                  //设置按钮属性
    void setLabProp(QLabel *lab, QWidget *parent, QString style, Qt::Alignment align, QString text);    //设置标签属性

    /******************** 转换函数 ********************/
    int cvtDno2Hno(int index);      //信息码下标->海明码下标
    int cvtPno2Hno(int index);      //校验码下标->海明码下标
    int cvtPrHno2Hno(int pno, int index);   //计算校验码的操作数下标->海明码下标
    QString getValStr(QPushButton *pbtn, int n);    //从数据块获取数字串

    /******************* 组件状态更新 *******************/
    void updateDataCode();      //更新信息码框
    void updatePrBitLab();      //更新校验位二进制标签
    void setGenBtnsEnabled(bool flag);      //启用/禁用按钮组（生成海明码按钮组）
    void setCheckBtnsEnabled(bool flag);    //启用/禁用按钮组（检错纠错按钮组）

private slots:
    /******************** 动画设置 ********************/
    void changeSpeed();             //改变动画播放速度

    /******************** 合法编码表 ********************/
    void updateLegalCodeTable();    //填充合法编码表
    void updateCheckRow();          //更新纠错后的编码对应的行
    void changeCodeRowStyle(int row, int choice);   //改变行样式

    /****************** 数据块状态设置 ******************/
    void setBlkUnknown();           //设置数据块状态未知
    void setBlkStatus(QPushButton *pbtn, int status);   //设置数据块状态
    void flipDataBlk(QPushButton *pbtns, int dno, int blktype = BLK_NORMAL);    //反转信息码块

    /****************** 数据块移动/更新 ******************/
    QPropertyAnimation *moveBlk(QPushButton *pStart, QPushButton *pEnd,
                 int duration_ms, QString startStyle, QString endStyle);    //移动数据块

    void updateDataBlk(const QString &dataStr);     //根据数字串更新信息码块（信息码区）
    void moveDataBlk(int dno);      //移动信息码块：信息码区->海明码区

    void genPrBlk(int pno);         //计算校验位，生成校验位区数据块
    void movePrBlk(int pno);        //移动校验码块：校验位区->校验码区
    void moveParityrBlk(int pno);   //移动校验码块：校验码区->海明码区

    /******************* 海明码生成动画 *******************/
    void setBlkVis(int data_bits);  //设置数据块/标签可见性
    void setGenInit();              //海明码生成动画初始化
    void updateGenStatus();         //更新海明码生成动画状态
    void setGenFinish();            //海明码生成动画单步结束
    void getFinalHamm();            //一次性生成海明码

    /******************* 检错纠错动画 *******************/
    void getCheckResult(int pno);   //生成校验结果
    void genCheckBlk(int pno);      //一位校验动画
    void correctCode(QString &valStr, int hamm_bits, int errbit);   //纠错

    void setCheckInit();            //检错纠错动画初始化
    void updateCheckStatus();       //更新检错纠错动画状态
    void setCheckFinish();          //检错纠错动画单步结束
    void getFinalCheck();           //一次性生成校验结果

signals:
    void moveFinished();

};

#endif // HCODEGEN_H
