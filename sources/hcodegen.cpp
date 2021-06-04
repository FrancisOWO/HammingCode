#include "hcodegen.h"
#include "ui_hcodegen.h"
#include <QString>
#include <QPoint>
#include <QSpinBox>
#include <QValidator>
#include <QRegExp>
#include <QSequentialAnimationGroup>
#include <QDebug>

//编码转换
static QString cvtStr2LocalQStr(const char *str)
{
    return QString::fromLocal8Bit(str);
}

//信息码下标->海明码下标
int HCodeGen::cvtDno2Hno(int index)
{
    if(index < 0 || index >= DATA_MAX)
        return -1;
    return DataLink[index];
}

//校验码下标->海明码下标
int HCodeGen::cvtPno2Hno(int index)
{
    if(index < 0 || index >= PARITY_MAX)
        return -1;
    return (1 << index) - 1;
}

//从数据块获取数字串
QString HCodeGen::getValStr(QPushButton *pbtn, int n)
{
    QString valStr;
    for(int i = 0; i < n; i++)
        valStr = valStr + pbtn[i].text();
    return valStr;
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
    //样式初始化
    InitStyles();
    QString labStyle = "background:#F4F4F4";
    //单步动画状态
    stepStatus = INIT_STATUS;
    speedLevel = 1;      //动画原速播放
    //海明码H对应的信息位D/校验位P
    for(int i = 0; i < HAMM_MAX; i++)
        HammLink[i] = 0;        //清零
    for(int base=1, i=1; base < HAMM_MAX; base<<=1, i++)
        HammLink[base-1] = -i;  //校验位Px记负数
    for(int i=2, cnt=1; i < HAMM_MAX; i++){
        if(HammLink[i] != 0)
            continue;
        HammLink[i] = cnt;      //信息位Dx记正数
        DataLink[cnt-1] = i;
        cnt++;
    }
    //for(int i = 0; i < HAMM_MAX; i++)
    //  qDebug() << "H"+QString::number(i+1) << "=" << HammLink[i];

    //数据块/标签初始化
    int blk_w = 30, blk_h = 30, delta_w = 10;
    int blk_x, blk_y = 10, ofs_x = 10;
    int lab_h = 16, lab_y = 2;
    QString valStr0 = "0", valStrUnknown = "?";
    //信息码
    blk_x = ofs_x;
    QString style0 = pbtnStyle0 + hoverStyle + pressStyle;
    for(int i = 0; i < DATA_MAX; i++){
        //数据块
        DataBlk[i].setParent(ui->wgtDataBlk);
        DataBlk[i].setStyleSheet(style0);
        DataBlk[i].setText(valStr0);
        DataBlk[i].setGeometry(blk_x, blk_y, blk_w, blk_h);
        //标签
        DataLab[i].setParent(ui->wgtDataLab);
        DataLab[i].setStyleSheet(dataStyle);
        DataLab[i].setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        DataLab[i].setText("D" + QString::number(i+1));
        DataLab[i].setGeometry(blk_x, lab_y, blk_w, lab_h);
        //更新坐标
        blk_x += blk_w + delta_w;
    }
    //海明码
    blk_x = ofs_x;
    QString hammStr, hammStyle;
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
        //海明码对应的信息位/校验位标签
        if(HammLink[i] > 0){     //信息位
            hammStr = "D" + QString::number(HammLink[i]);
            hammStyle = dataStyle;
        }
        else {    //校验位
            hammStr = "P" + QString::number(-HammLink[i]);
            hammStyle = parityStyle;
        }
        HammLinkLab[i].setParent(ui->wgtHammLink);
        HammLinkLab[i].setStyleSheet(hammStyle);
        HammLinkLab[i].setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        HammLinkLab[i].setText(hammStr);
        HammLinkLab[i].setGeometry(blk_x, lab_y, blk_w, lab_h);
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
        ParityLab[i].setStyleSheet(parityStyle);
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
        converter.calCheckDnoList(DATA_MAX, i+1, PrHnos[i], dlen);
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
    if(data_bits < 3 || data_bits > 5)
        data_bits = 4;
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

    //动画速度
    connect(ui->pbtnSpeed, SIGNAL(clicked()), this, SLOT(changeSpeed()));
    //单步动画
    connect(ui->pbtnHamRestart, SIGNAL(clicked()), this, SLOT(setStepInit()));      //重新开始
    connect(ui->pbtnHamStep, SIGNAL(clicked()), this, SLOT(updateStepStatus()));    //下一步动画
    connect(this, SIGNAL(moveFinished()), this, SLOT(setStepFinishStatus()));       //设置动画结束状态
}

//初始化样式表
void HCodeGen::InitStyles()
{
    pbtnStyle0 = "QPushButton{background:#F0F0F0}";     //值为0
    pbtnStyle1 = "QPushButton{background:#C0C0C0}";     //值为1
    unknownStyle = "background:#E8F0FF";    //值未知

    hoverStyle = "QPushButton:hover{background:#DCF0FC}";       //悬停
    pressStyle = "QPushButton:pressed{background:#D0E0FC}";     //按压

    dataStyle = "background:#FFCCCC;border:1px solid grey";     //信息块
    parityStyle = "background:#CCCCFF;border:1px solid grey";   //校验块
    moveStyle = "background:#FFCCCC;border:1px solid grey";     //移动
}

//更新信息码框
void HCodeGen::updateDataCode()
{
    //从数据块获取数字串
    QString valStr = getValStr(DataBlk, dataBits);
    //为防止信息码框变化带动数据块变化，暂时解除连接
    disconnect(ui->lnDataCode, SIGNAL(textChanged(const QString &)), this, SLOT(updateDataBlk(const QString &)));   //解除连接
    ui->lnDataCode->setText(valStr);    //设置信息码框
    connect(ui->lnDataCode, SIGNAL(textChanged(const QString &)), this, SLOT(updateDataBlk(const QString &)));      //恢复连接
}

//改变动画播放速度
void HCodeGen::changeSpeed()
{
    if(speedLevel == 1){
        speedLevel = 2;
        ui->pbtnSpeed->setText(cvtStr2LocalQStr("原速播放"));
    }
    else {
        speedLevel = 1;
        ui->pbtnSpeed->setText(cvtStr2LocalQStr("倍速播放"));
    }
}

//设置数据块状态未知
void HCodeGen::setBlkUnknown()
{
    QString textUnKnown = "?";
    //海明码
    QString valStr;
    int fullBits = dataBits + parityBits;
    for(int i = 0; i < fullBits; i++){
        valStr = valStr + textUnKnown;
        HammBlk[i].setText(textUnKnown);
        HammBlk[i].setStyleSheet(unknownStyle);
    }
    ui->lnHammCode->setText(valStr);    //设置显示框
    //校验码
    valStr = "";
    for(int i = 0; i < parityBits; i++){
        valStr = valStr + textUnKnown;
        ParityBlk[i].setText(textUnKnown);
        ParityBlk[i].setStyleSheet(unknownStyle);
    }
    ui->lnPrCode->setText(valStr);      //设置显示框
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
    QString valStr = getValStr(DataBlk, dataBits);
    updateDataBlk(valStr);
    //设置未知数据块
    setBlkUnknown();

}

//更新单步动画状态
void HCodeGen::updateStepStatus()
{
    ui->pbtnHamStep->setEnabled(false); //禁用按钮
    stepStatus++;
    int lastParityStatus = dataBits + parityBits;
    int finalStatus = lastParityStatus + parityBits;
    if(stepStatus == 0){
        stepStatusStr = cvtStr2LocalQStr("单步动画\n填入D1");
        updateDataCode();   //更新信息码框
        //禁用输入
        ui->lnDataCode->setReadOnly(true);  //输入框不可修改
        for(int i = 0; i < dataBits; i++)   //数据块不可改变
            DataBlk[i].setEnabled(false);
        setStepFinishStatus();
        //计算海明码
        HammingBack converter;
        std::string valStr = ui->lnDataCode->text().toStdString();
        HammResult = converter.calHammingResult(valStr);
    }
    else if(stepStatus < dataBits){     //信息码区的信息块->海明码区的信息块
        stepStatusStr = cvtStr2LocalQStr("单步动画\n填入D")+QString::number(stepStatus+1);
        moveDataBlk(stepStatus-1);
    }
    else if(stepStatus == dataBits){    //信息码区的信息块->海明码区的信息块（最后一个）
        stepStatusStr = cvtStr2LocalQStr("单步动画\n计算P1");
        moveDataBlk(stepStatus-1);
    }
    else if(stepStatus < lastParityStatus){     //计算校验位
        int index = stepStatus-dataBits;
        stepStatusStr = cvtStr2LocalQStr("单步动画\n计算P")+QString::number(index+1);
        genPrBlk(index-1);
    }
    else if(stepStatus == lastParityStatus){    //计算校验位（最后一个）
        int index = stepStatus-dataBits;
        stepStatusStr = cvtStr2LocalQStr("单步动画\n填入P1");
        genPrBlk(index-1);
    }
    else if(stepStatus < finalStatus){      //校验码区的校验块->海明码区的校验块
        int index = stepStatus - lastParityStatus;
        stepStatusStr = cvtStr2LocalQStr("单步动画\n填入P")+QString::number(index+1);
        moveParityrBlk(index-1);
    }
    else if(stepStatus == finalStatus){     //校验码区的校验块->海明码区的校验块（最后一个）
        int index = stepStatus - lastParityStatus;
        stepStatusStr = cvtStr2LocalQStr("单步动画\n结束");
        moveParityrBlk(index-1);
    }
    else if(stepStatus > 0){
        stepStatusStr = cvtStr2LocalQStr("单步动画\n结束");
        setStepInit();      //恢复初始状态
    }
}

//设置单步动画结束状态
void HCodeGen::setStepFinishStatus()
{
    ui->pbtnHamStep->setText(stepStatusStr);    //更新按钮文字
    ui->pbtnHamStep->setEnabled(true);          //启用按钮
    //设置下一步的界面
    int lastParityStatus = dataBits + parityBits;
    int finalStatus = lastParityStatus + parityBits;
    //qDebug() << "dataBits :" << dataBits;
    //qDebug() << "prBits :" << parityBits;
    //qDebug() << "status :" << stepStatus;
    if(stepStatus < 0)
        ;
    else if(stepStatus < dataBits){
        //改变将要移动的数据块状态
        DataBlk[stepStatus].setStyleSheet(dataStyle);
    }
    else if(stepStatus < lastParityStatus){
        ;
    }
    else if(stepStatus < finalStatus){
        int index = stepStatus - lastParityStatus;
        ParityBlk[index].setStyleSheet(parityStyle);
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
        HammLinkLab[i].setVisible(true);
    }
    for(int i = hamm_bits; i < HAMM_MAX; i++){
        HammBlk[i].setVisible(false);
        HammLab[i].setVisible(false);
        HammLinkLab[i].setVisible(false);
    }
    //填充文本框
    ui->lnHammBit->setText(QString::number(hamm_bits));     //海明码位数
    ui->lnHammCode->setText(valStr);

    //校验位
    for(int i = 0; i < parityBits; i++){
        //求出计算校验位所需的海明码位数
        int dlen = converter.calCheckDatalen(data_bits, i+1) - 1;
        PrHnoLen[i] = dlen;
        //显示操作数块
        for(int j = 0; j < dlen; j++){
            PrRowBlks[i][j].setVisible(true);   //数据块
            PrRowLabs[i][j].setVisible(true);   //标签
            PrRowLabs[i][j].setText("H"+QString::number(PrHnos[i][j+1]));
            PrRowLabs[i][j].setStyleSheet(dataStyle);
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
        PrRowLabs[i][dlen].setText("H"+QString::number(PrHnos[i][0]));
        PrRowLabs[i][dlen].setStyleSheet(parityStyle);
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

//设置数据块状态
void HCodeGen::setBlkStatus(QPushButton *pbtn, int status)
{
    //按钮样式
    QString style[2];
    style[0] = pbtnStyle0 + hoverStyle + pressStyle;
    style[1] = pbtnStyle1 + hoverStyle + pressStyle;
    pbtn->setStyleSheet(style[status]);
    if(status == 0){
        pbtn->setText("0");
    }
    else if(status == 1){
        pbtn->setText("1");
    }
    else{
        pbtn->setText("?");
        pbtn->setStyleSheet(unknownStyle);
    }
}

//反转数据块
void HCodeGen::flipDataBlk(int dno)
{
    //按钮样式
    QString style0 = pbtnStyle0 + hoverStyle + pressStyle;
    QString style1 = pbtnStyle1 + hoverStyle + pressStyle;
    //计算信息码
    int bits = ui->spinDataBit->value();
    QString valStr = getValStr(DataBlk, bits);
    if(DataBlk[dno].text() == "0"){
        valStr[dno] = '1';
        DataBlk[dno].setText("1");
        DataBlk[dno].setStyleSheet(style1);
    }
    else {
        valStr[dno] = '0';
        DataBlk[dno].setText("0");
        DataBlk[dno].setStyleSheet(style0);
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
void HCodeGen::moveDataBlk(int dno)
{
    //过滤非法值
    if(dno < 0 || dno >= dataBits)
        return;
    //设置起点块/终点块样式
    QString startStyle, endStyle;
    if(DataBlk[dno].text() == "0"){
        startStyle = pbtnStyle0 + hoverStyle + pressStyle;
        endStyle = pbtnStyle0;
    }
    else {
        startStyle = pbtnStyle1 + hoverStyle + pressStyle;
        endStyle = pbtnStyle1;
    }
    //平移动画
    QPropertyAnimation *pMove = nullptr;
    int duration_ms = 800;
    int hno = cvtDno2Hno(dno);
    moveStyle = dataStyle;     //移动样式
    //获取动画指针
    pMove = moveBlk(&(DataBlk[dno]), &(HammBlk[hno]), duration_ms, startStyle, endStyle);
    connect(pMove, &QPropertyAnimation::finished, [=](){
        //设置海明码框
        QString val = ui->lnHammCode->text();
        val[hno] = HammResult.data[dno];
        ui->lnHammCode->setText(val);
        //发送动画结束信号
        emit moveFinished();
    });
    pMove->start(QAbstractAnimation::DeleteWhenStopped);
}

//计算校验位，生成校验位数据块
void HCodeGen::genPrBlk(int pno)
{
    //过滤非法值
    if(pno < 0 || pno >= parityBits)
        return;
    //串行动画组
    QSequentialAnimationGroup *pSeqGrp = new QSequentialAnimationGroup;
    int n = PrHnoLen[pno];
    QPropertyAnimation **pMove = new QPropertyAnimation *[n];
    int duration_ms = 800;
    //海明码区的信息位->校验位的操作数
    QString startStyle, endStyle;   //起点块/终点块样式
    moveStyle = dataStyle;     //移动样式
    for(int i = 0; i < n; i++){
        int hno = PrHnos[pno][i+1] - 1;
        if(HammBlk[hno].text() == "0"){
            startStyle = pbtnStyle0 + hoverStyle + pressStyle;
            endStyle = pbtnStyle0;
        }
        else {
            startStyle = pbtnStyle1 + hoverStyle + pressStyle;
            endStyle = pbtnStyle1;
        }
        //qDebug() << "add anim" << QString::number(k);
        //获取动画指针
        pMove[i] = moveBlk(&(HammBlk[hno]), &(PrRowBlks[pno][i]), duration_ms, startStyle, endStyle);
        pSeqGrp->addAnimation(pMove[i]);
    }
    //串行动画组结束后，释放动画块及其指针数组，启动结果填入校验区动画
    connect(pSeqGrp, &QSequentialAnimationGroup::finished, [=](){
        for(int i = 0; i < n; i++)
            delete pMove[i];
        delete []pMove;
        //启动结果填入校验区动画
        movePrBlk(pno);
    });
    //启动串行动画组
    pSeqGrp->start(QAbstractAnimation::DeleteWhenStopped);
}

//移动校验码块：校验位区->校验码区
void HCodeGen::movePrBlk(int pno)
{
    //过滤非法值
    if(pno < 0 || pno >= parityBits)
        return;
    //校验位区的计算结果->校验码区的校验块
    QPropertyAnimation *pMove = nullptr;
    int duration_ms = 800;
    int n = PrHnoLen[pno];
    int prStatus = HammResult.check[pno] - '0';
    setBlkStatus(&(PrRowBlks[pno][n]), prStatus); //设置起始块的值，用来给动画块赋值
    //起点块/终点块样式
    QString startStyle, endStyle;
    if(PrRowBlks[pno][n].text() == "0"){
        startStyle = pbtnStyle0;
        endStyle = pbtnStyle0;
    }
    else {
        startStyle = pbtnStyle1;
        endStyle = pbtnStyle1;
    }
    moveStyle = parityStyle;     //移动样式
    //获取动画指针
    pMove = moveBlk(&(PrRowBlks[pno][n]), &(ParityBlk[pno]), duration_ms, startStyle, endStyle);
    connect(pMove, &QPropertyAnimation::finished, [=](){
        //设置校验码框
        QString val = ui->lnPrCode->text();
        val[pno] = HammResult.check[pno];
        ui->lnPrCode->setText(val);
        //发送结束信号
        emit moveFinished();
    });
    //启动动画
    pMove->start(QAbstractAnimation::DeleteWhenStopped);
}

//移动校验码块：校验码区->海明码区
void HCodeGen::moveParityrBlk(int pno)
{
    //过滤非法值
    if(pno < 0 || pno >= parityStyle)
        return;
    //平移动画
    QPropertyAnimation *pMove = nullptr;
    int duration_ms = 800;
    int hno = cvtPno2Hno(pno);
    //设置起点块/终点块样式
    QString startStyle, endStyle;
    if(ParityBlk[pno].text() == "0"){
        startStyle = pbtnStyle0;
        endStyle = pbtnStyle0;
    }
    else {
        startStyle = pbtnStyle1;
        endStyle = pbtnStyle1;
    }
    moveStyle = parityStyle;     //移动样式
    pMove = moveBlk(&(ParityBlk[pno]), &(HammBlk[hno]), duration_ms, startStyle, endStyle);
    connect(pMove, &QPropertyAnimation::finished, [=](){
        //设置海明码框
        QString val = ui->lnHammCode->text();
        val[hno] = HammResult.check[pno];
        ui->lnHammCode->setText(val);
        //发送动画结束信号
        emit moveFinished();
    });
    pMove->start(QAbstractAnimation::DeleteWhenStopped);
}

//移动数据块
QPropertyAnimation *HCodeGen::moveBlk(QPushButton *pStart, QPushButton *pEnd,
                        int duration_ms, QString startStyle, QString endStyle)
{
    QPoint pt_start = pStart->mapTo(this, QPoint(0,0)); //起点
    QPoint pt_end = pEnd->mapTo(this, QPoint(0,0));     //终点
    QString blkText = pStart->text();   //标签
    int blk_w = pStart->width();
    int blk_h = pStart->height();
    //设置动画块
    QPushButton *pBlk = new QPushButton;
    pBlk->setParent(this);
    pBlk->setStyleSheet(moveStyle);
    pBlk->setText(blkText);
    pBlk->setGeometry(pt_start.x(), pt_start.y(), blk_w, blk_h);
    pBlk->show();
    //平移动画
    QPropertyAnimation *pMove = new QPropertyAnimation(pBlk, "pos");
    duration_ms /= speedLevel;      //调整延时
    pMove->setDuration(duration_ms);    //延时
    pMove->setStartValue(pt_start);     //起点
    pMove->setEndValue(pt_end);         //终点
    pMove->setEasingCurve(QEasingCurve::InOutQuad); //动画曲线
    //动画结束后，释放动画块，重置数据块样式
    connect(pMove, &QPropertyAnimation::finished, [=](){
        delete pBlk;
        pStart->setStyleSheet(startStyle);
        pEnd->setText(blkText);
        pEnd->setStyleSheet(endStyle);
    });
    return pMove;
}
