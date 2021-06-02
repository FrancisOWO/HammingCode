#include "hcodegen.h"
#include "ui_hcodegen.h"
#include <QString>
#include <QPoint>
#include <QSpinBox>
#include <QPropertyAnimation>
#include <QRegExp>
#include <QValidator>
#include <QDebug>

//编码转换
static QString cvtStr2LocalQStr(const char *str)
{
    return QString::fromLocal8Bit(str);
}

HCodeGen::HCodeGen(SubWindow *parent) :
    SubWindow(parent),
    ui(new Ui::HCodeGen)
{
    ui->setupUi(this);

    InitMembers();
    InitConnections();
}

HCodeGen::~HCodeGen()
{
    delete ui;
}

//初始化数据成员
void HCodeGen::InitMembers()
{
    //单步动画状态
    stepStatus = INIT_STATUS;
    //海明码H对应的信息位D/校验位P
    for(int i = 0; i < HAMM_MAX; i++)
        HammLink[i] = 0;        //清零
    for(int base=1, i=1; base < HAMM_MAX; base<<=1, i++)
        HammLink[base-1] = -i;  //校验位P?记负数
    for(int i=2, cnt=1; i < HAMM_MAX; i++){
        if(HammLink[i] != 0)
            continue;
        HammLink[i] = cnt;      //信息位D?记正数
        DataLink[cnt-1] = i;
        cnt++;
    }
    //for(int i = 0; i < HAMM_MAX; i++)
    //  qDebug() << "H"+QString::number(i+1) << "=" << HammLink[i];

    //样式
    pbtnStyle0 = "QPushButton{background:#F0F0F0}";
    pbtnStyle1 = "QPushButton{background:#C0C0C0}";
    unknownStyle = "background:#E8F0FF";
    hoverStyle = "QPushButton:hover{background:#DCF0FC}";
    pressStyle = "QPushButton:pressed{background:#D0E0FC}";
    moveStyle = "background:#FFCCCC;border:1px solid grey";
    QString labStyle = "background:#F4F4F4";
    //数据块/标签初始化
    int blk_w = 30, blk_h = 30, delta_w = 10;
    int blk_x, blk_y = 10, ofs_x = 10;
    int lab_h = 16, lab_y = 2;
    QString valStr0 = "0", valStrUnknown = "?";
    //信息码
    blk_x = ofs_x;
    for(int i = 0; i < DATA_MAX; i++){
        //数据块
        DataBlk[i].setParent(ui->wgtDataBlk);
        DataBlk[i].setStyleSheet(pbtnStyle0+hoverStyle+pressStyle);
        DataBlk[i].setText(valStr0);
        DataBlk[i].setGeometry(blk_x, blk_y, blk_w, blk_h);
        //标签
        DataLab[i].setParent(ui->wgtDataLab);
        DataLab[i].setStyleSheet(labStyle);
        DataLab[i].setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        DataLab[i].setText("D" + QString::number(i+1));
        DataLab[i].setGeometry(blk_x, lab_y, blk_w, lab_h);
        //更新坐标
        blk_x += blk_w + delta_w;
    }
    //海明码
    blk_x = ofs_x;
    for(int i = 0; i < HAMM_MAX; i++){
        //数据块
        HammBlk[i].setParent(ui->wgtHammBlk);
        HammBlk[i].setStyleSheet(unknownStyle);
        HammBlk[i].setText(valStrUnknown);
        HammBlk[i].setGeometry(blk_x, blk_y, blk_w, blk_h);
        //标签
        HammLab[i].setParent(ui->wgtHammLab);
        HammLab[i].setStyleSheet(labStyle);
        HammLab[i].setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        HammLab[i].setText("H" + QString::number(i+1));
        HammLab[i].setGeometry(blk_x, lab_y, blk_w, lab_h);
        //更新坐标
        blk_x += blk_w + delta_w;
    }
    //校验码
    blk_x = ofs_x;
    for(int i = 0; i < PARITY_MAX; i++){
        //数据块
        ParityBlk[i].setParent(ui->wgtPrBlk);
        ParityBlk[i].setStyleSheet(unknownStyle);
        ParityBlk[i].setText(valStrUnknown);
        ParityBlk[i].setGeometry(blk_x, blk_y, blk_w, blk_h);
        //标签
        ParityLab[i].setParent(ui->wgtPrLab);
        ParityLab[i].setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ParityLab[i].setStyleSheet(labStyle);
        ParityLab[i].setText("P" + QString::number(i+1));
        ParityLab[i].setGeometry(blk_x, lab_y, blk_w, lab_h);
        //更新坐标
        blk_x += blk_w + delta_w;
    }
    //校验位
    //  //行头
    PrRowHead[0] = ui->labP1; PrRowHead[1] = ui->labP2;
    PrRowHead[2] = ui->labP3; PrRowHead[3] = ui->labP4;
    // //计算区
    wgtPrBlk[0] = ui->wgtP1Blk; wgtPrBlk[1] = ui->wgtP2Blk;
    wgtPrBlk[2] = ui->wgtP3Blk; wgtPrBlk[3] = ui->wgtP4Blk;
    // //标签区
    wgtPrLab[0] = ui->wgtP1Lab; wgtPrLab[1] = ui->wgtP2Lab;
    wgtPrLab[2] = ui->wgtP3Lab; wgtPrLab[3] = ui->wgtP4Lab;
    // //每个区域的数据块与标签
    delta_w = 20;
    HammingBack converter;
    // // //操作数和结果
    for(int i = 0; i < PARITY_MAX; i++){
        //计算校验码对应的所有信息位
        int dlen = converter.calCheckDatalen(DATA_MAX, i+1);
        converter.calCheckDnoList(DATA_MAX, i+1, PrDnos[i], dlen);
        blk_x = ofs_x;
        for(int j = 0; j < OPRD_MAX+1; j++){
            //数据块
            PrRowBlks[i][j].setParent(wgtPrBlk[i]);
            PrRowBlks[i][j].setStyleSheet(unknownStyle);
            PrRowBlks[i][j].setText(valStrUnknown);
            PrRowBlks[i][j].setGeometry(blk_x, blk_y, blk_w, blk_h);
            //标签
            PrRowLabs[i][j].setParent(wgtPrLab[i]);
            PrRowLabs[i][j].setStyleSheet(labStyle);
            PrRowLabs[i][j].setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
            PrRowLabs[i][j].setText("H?");
            PrRowLabs[i][j].setGeometry(blk_x, lab_y, blk_w, lab_h);
            //更新坐标
            blk_x += blk_w + delta_w;
        }
    }
    // // //操作符
    QString labStyle0 = "border:0px";
    for(int i = 0; i < PARITY_MAX; i++){
        blk_x = ofs_x + blk_w;
        for(int j = 0; j < OPTR_MAX; j++){
            //数据块
            PrRowOptrBlks[i][j].setParent(wgtPrBlk[i]);
            PrRowOptrBlks[i][j].setStyleSheet(labStyle0);
            PrRowOptrBlks[i][j].setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
            PrRowOptrBlks[i][j].setGeometry(blk_x, blk_y, delta_w, blk_h);
            //标签
            PrRowOptrLabs[i][j].setParent(wgtPrLab[i]);
            PrRowOptrLabs[i][j].setStyleSheet(labStyle0);
            PrRowOptrLabs[i][j].setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
            PrRowOptrLabs[i][j].setGeometry(blk_x, lab_y, delta_w, lab_h);
            //更新坐标
            blk_x += blk_w + delta_w;
            PrRowOptrBlks[i][j].setText("+");
            PrRowOptrLabs[i][j].setText("+");
        }
    }
    //设置可见性
    int data_bits = ui->spinDataBit->value();
    setBlkVis(data_bits);

}

//关联信号与槽
void HCodeGen::InitConnections()
{
    //输入信息码位数，改变数据块状态
    connect(ui->spinDataBit, SIGNAL(valueChanged(int)), this, SLOT(setBlkVis(int)));
    //输入信息码，更新信息码数据块的值
    connect(ui->lnDataCode, SIGNAL(textChanged(const QString &)), this, SLOT(updateDataBlk(const QString &)));
    for(int i = 0; i < DATA_MAX; i++)       //点击按钮，反转数据块的值
        connect(&(DataBlk[i]), &QPushButton::clicked, [=](){flipDataBlk(i);});

    //单步动画
    connect(ui->pbtnHamRestart, SIGNAL(clicked()), this, SLOT(setStepInit()));      //重新开始
    connect(ui->pbtnHamStep, SIGNAL(clicked()), this, SLOT(updateStepStatus()));    //下一步动画
    connect(this, SIGNAL(moveFinished()), this, SLOT(setStepFinishStatus()));       //设置动画结束状态
}

//设置数据块状态未知
void HCodeGen::setBlkUnknown()
{
    QString textUnKnown = "?";
    //海明码
    for(int i = 0; i < HAMM_MAX; i++){
        HammBlk[i].setText(textUnKnown);
        HammBlk[i].setStyleSheet(unknownStyle);
    }
    //校验码
    for(int i = 0; i < PARITY_MAX; i++){
        ParityBlk[i].setText(textUnKnown);
        ParityBlk[i].setStyleSheet(unknownStyle);
    }
    //校验位
    for(int i = 0; i < PARITY_MAX; i++){
        for(int j = 0; j < OPRD_MAX+1; j++){
            PrRowBlks[i][j].setText(textUnKnown);
            PrRowBlks[i][j].setStyleSheet(unknownStyle);
        }
    }

}

//设置单步动画初始状态
void HCodeGen::setStepInit()
{
    stepStatusStr = cvtStr2LocalQStr("海明码生成\n(单步动画)");
    setStepFinishStatus();
    stepStatus = INIT_STATUS;
    //允许输入
    ui->lnDataCode->setReadOnly(false);     //输入框可修改
    for(int i = 0; i < dataBits; i++)       //数据块可改变
        DataBlk[i].setEnabled(true);
    //恢复信息码数据块状态
    QString valStr;
    for(int i = 0; i < dataBits; i++)
        valStr = valStr + DataBlk[i].text();
    updateDataBlk(valStr);
    //设置未知数据块
    setBlkUnknown();

}

//更新单步动画状态
void HCodeGen::updateStepStatus()
{
    ui->pbtnHamStep->setEnabled(false); //禁用按钮
    stepStatus++;
    if(stepStatus == 0){
        stepStatusStr = cvtStr2LocalQStr("单步动画\n填入D1");
        //禁用输入
        ui->lnDataCode->setReadOnly(true);  //输入框不可修改
        for(int i = 0; i < dataBits; i++)   //数据块不可改变
            DataBlk[i].setEnabled(false);
        setStepFinishStatus();
    }
    else if(stepStatus < dataBits){
        stepStatusStr = cvtStr2LocalQStr("单步动画\n填入D")+QString::number(stepStatus+1);
        moveDataBlk(stepStatus-1);
    }
    else if(stepStatus == dataBits){
        stepStatusStr = cvtStr2LocalQStr("单步动画\n计算P1");
        moveDataBlk(stepStatus-1);
    }
    else {
        setStepInit();      //恢复初始状态
    }
}

//设置单步动画结束状态
void HCodeGen::setStepFinishStatus()
{
    ui->pbtnHamStep->setText(stepStatusStr);//更新按钮文字
    ui->pbtnHamStep->setEnabled(true);      //启用按钮
    //设置下一步的界面
    if(stepStatus < dataBits){
        //改变将要移动的数据块状态
        DataBlk[stepStatus].setStyleSheet(moveStyle);
    }

}

//设置可见性
void HCodeGen::setBlkVis(int data_bits)
{
    //单步动画恢复初始状态
    setStepInit();
    //限制信息码输入框只能输指定长度的0/1串
    dataBits = data_bits;
    QString rgx0 = QString("^[0-1]{%1}$").arg(data_bits);
    QValidator *vdt = new QRegExpValidator(QRegExp(rgx0));
    ui->lnDataCode->setValidator(vdt);
    //信息码
    QString valStr, valStr0 = "0", valUnknown = "?";
    for(int i = 0; i < data_bits; i++){
        valStr = valStr + valStr0;
        DataBlk[i].setVisible(true);
        DataLab[i].setVisible(true);
    }
    for(int i = data_bits; i < DATA_MAX; i++){
        DataBlk[i].setVisible(false);
        DataLab[i].setVisible(false);
    }
    ui->lnDataCode->setText(valStr);

    //校验码
    HammingBack converter;
    parityBits = converter.calCheckLen(data_bits);     //校验码位数
    valStr = "";
    for(int i = 0; i < parityBits; i++){
        valStr = valStr + valUnknown;
        ParityBlk[i].setVisible(true);
        ParityLab[i].setVisible(true);
        PrRowHead[i]->setVisible(true);
        wgtPrLab[i]->setVisible(true);
        wgtPrBlk[i]->setVisible(true);
    }
    for(int i = parityBits; i < PARITY_MAX; i++){
        ParityBlk[i].setVisible(false);
        ParityLab[i].setVisible(false);
        PrRowHead[i]->setVisible(false);
        wgtPrLab[i]->setVisible(false);
        wgtPrBlk[i]->setVisible(false);
    }
    //填充文本框
    ui->lnPrBit->setText(QString::number(parityBits));
    ui->lnPrCode->setText(valStr);

    //海明码
    int hamm_bits = data_bits + parityBits;
    valStr = "";
    for(int i = 0; i < hamm_bits; i++){
        valStr = valStr + valUnknown;
        HammBlk[i].setVisible(true);
        HammLab[i].setVisible(true);
    }
    for(int i = hamm_bits; i < HAMM_MAX; i++){
        HammBlk[i].setVisible(false);
        HammLab[i].setVisible(false);
    }
    //填充文本框
    ui->lnHammBit->setText(QString::number(hamm_bits));     //海明码位数
    ui->lnHammCode->setText(valStr);

    //校验位
    for(int i = 0; i < parityBits; i++){
        //求出计算校验位所用的数据位
        int dlen = converter.calCheckDatalen(data_bits, i+1) - 1;
        //显示操作数块
        for(int j = 0; j < dlen; j++){
            PrRowBlks[i][j].setVisible(true);   //数据块
            PrRowLabs[i][j].setVisible(true);   //标签
            PrRowLabs[i][j].setText("H"+QString::number(PrDnos[i][j+1]));
            //设置操作符
            PrRowOptrBlks[i][j].setVisible(true);
            PrRowOptrLabs[i][j].setVisible(true);
            PrRowOptrBlks[i][j].setText("+");
            PrRowOptrLabs[i][j].setText("+");
        }
        PrRowOptrBlks[i][dlen-1].setText("=");
        PrRowOptrLabs[i][dlen-1].setText("=");
        //显示结果块
        PrRowBlks[i][dlen].setVisible(true);
        PrRowLabs[i][dlen].setVisible(true);
        PrRowLabs[i][dlen].setText("H"+QString::number(PrDnos[i][0]));
        //隐藏多余块
        for(int j = dlen; j < OPTR_MAX; j++){
            PrRowOptrBlks[i][j].setVisible(false);  //操作符(数据块区)
            PrRowOptrLabs[i][j].setVisible(false);  //操作符(标签区)
        }
        for(int j = dlen+1; j < OPRD_MAX+1; j++){
            PrRowBlks[i][j].setVisible(false);   //数据块
            PrRowLabs[i][j].setVisible(false);   //标签
        }
    }
}

//反转数据块
void HCodeGen::flipDataBlk(int index)
{
    //按钮样式
    QString style0 = pbtnStyle0 + hoverStyle + pressStyle;
    QString style1 = pbtnStyle1 + hoverStyle + pressStyle;
    //计算信息码
    int bits = ui->spinDataBit->value();
    QString valStr;
    for(int i = 0; i < bits; i++)
        valStr = valStr + DataBlk[i].text();
    if(DataBlk[index].text() == "0"){
        valStr[index] = '1';
        DataBlk[index].setText("1");
        DataBlk[index].setStyleSheet(style1);
    }
    else {
        valStr[index] = '0';
        DataBlk[index].setText("0");
        DataBlk[index].setStyleSheet(style0);
    }
    ///设置信息码输入框前，为防止输入框更新带动数据块更新，需暂时解除连接
    disconnect(ui->lnDataCode, SIGNAL(textChanged(const QString &)), this, SLOT(updateDataBlk(const QString &)));   //解除连接
    ui->lnDataCode->setText(valStr);    //设置信息码
    connect(ui->lnDataCode, SIGNAL(textChanged(const QString &)), this, SLOT(updateDataBlk(const QString &)));      //恢复连接
}

//更新信息码数据块
void HCodeGen::updateDataBlk(const QString &dataStr)
{
    int len = dataStr.length();
    for(int i = 0; i < len; i++){
        if(dataStr[i] == '0'){   //顺序判断
            DataBlk[i].setStyleSheet(pbtnStyle0);
            DataBlk[i].setText("0");
        }
        else {
            DataBlk[i].setStyleSheet(pbtnStyle1);
            DataBlk[i].setText("1");
        }
    }
}

//移动信息码数据块
void HCodeGen::moveDataBlk(int index)
{
    int duration_ms = 800;
    if(index < 0 || index >= ui->spinDataBit->value())
        return;
    QString startStyle, endStyle;
    if(DataBlk->text() == "0"){
        startStyle = pbtnStyle0 + hoverStyle + pressStyle;
        endStyle = pbtnStyle0;
    }
    else {
        startStyle = pbtnStyle1 + hoverStyle + pressStyle;
        endStyle = pbtnStyle1;
    }
    moveBlk(&(DataBlk[index]), &(HammBlk[DataLink[index]]), duration_ms, startStyle, endStyle);
}

//移动数据块
void HCodeGen::moveBlk(QPushButton *pStart, QPushButton *pEnd, int duration_ms,
                       QString startStyle, QString endStyle)
{
    QPoint pt_start = pStart->mapTo(this, QPoint(0,0)); //起点
    QPoint pt_end = pEnd->mapTo(this, QPoint(0,0));     //终点
    QString blkText = pStart->text();   //标签
    int blk_w = pStart->width();
    int blk_h = pStart->height();
    //设置动画块
    QPushButton *pBlk = &TempMoveBlk;
    pBlk->setParent(this);
    pBlk->setStyleSheet(moveStyle);
    pBlk->setText(blkText);
    pBlk->setGeometry(pt_start.x(), pt_start.y(), blk_w, blk_h);
    pBlk->show();
    //平移动画
    QPropertyAnimation *pMove = new QPropertyAnimation(pBlk, "pos");
    pMove->setDuration(duration_ms);    //延时
    pMove->setStartValue(pt_start);     //起点
    pMove->setEndValue(pt_end);         //终点
    pMove->setEasingCurve(QEasingCurve::InOutQuad); //动画曲线
    pMove->start();
    //动画结束后，隐藏动画块
    connect(pMove, &QPropertyAnimation::finished, [=](){
        pBlk->setVisible(false);
        pEnd->setText(blkText);
        pEnd->setStyleSheet(endStyle);
        pStart->setStyleSheet(startStyle);
        emit moveFinished();
    });

}
