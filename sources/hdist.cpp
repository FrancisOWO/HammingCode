#include "hdist.h"
#include "ui_hdist.h"

#include "HammingBack.h"
#include <QSpinBox>

HDist::HDist(SubWindow *parent) :
    SubWindow(parent),
    ui(new Ui::HDist)
{
    ui->setupUi(this);

    InitMembers();
    InitConnections();
}

HDist::~HDist()
{
    delete ui;
}

//初始化数据成员
void HDist::InitMembers()
{
    //设置按钮样式
    hoverStyle = "QPushButton:hover{background:#DCF0FC}";
    pressStyle = "QPushButton:pressed{background:#D0E0FC}";
    pbtnStyle0 = "QPushButton{background:#F0F0F0}";
    pbtnStyle1 = "QPushButton{background:#C0C0C0}";
    //设置默认输入位数
    int deftBit = BIT_MAX;
    ui->spinBit->setValue(deftBit);
    //设置输入输出数据块
    int block_w = 30, block_h = 30;
    int delta_x = 10, delta_y = 10;
    for(int i = 0; i < BIT_MAX; i++){
        //可点击
        DataIn1[i].setEnabled(true);
        DataIn2[i].setEnabled(true);
        DataOut[i].setEnabled(true);
        //设置位置
        DataIn1[i].setGeometry(delta_x + i*(block_w +delta_x), delta_y, block_w, block_h);
        DataIn2[i].setGeometry(delta_x + i*(block_w +delta_x), delta_y, block_w, block_h);
        DataOut[i].setGeometry(delta_x + i*(block_w +delta_x), delta_y, block_w, block_h);
        //绑定区域
        DataIn1[i].setParent(ui->wgtIn1);
        DataIn2[i].setParent(ui->wgtIn2);
        DataOut[i].setParent(ui->wgtOut);
    }
    //更新输入输出数据块状态
    updateIOBit(deftBit);

}

//关联信号与槽
void HDist::InitConnections()
{
    //改变输入位数，更新数据块
    connect(ui->spinBit, SIGNAL(valueChanged(int)), this, SLOT(updateIOBit(int)));
    //改变数字框输入值，更新数据块
    connect(ui->spinIn1, SIGNAL(valueChanged(int)), this, SLOT(updateBlockIn1(int)));
    connect(ui->spinIn2, SIGNAL(valueChanged(int)), this, SLOT(updateBlockIn2(int)));
    //点击按钮，计算输出
    connect(ui->pbtnCompute, SIGNAL(clicked()), this, SLOT(updateDataOut()));
    //点击数据块，改变数字框的值
    for(int i = 0; i < BIT_MAX; i++){
        ///由于存在数字框到数据块的连接，数字框更新会带动数据块变化，updateSpinData内部需暂时解除连接以防止联动
        connect(&(DataIn1[i]), &QPushButton::clicked, [=](){updateSpinData(DATA_IN1, i);});
        connect(&(DataIn2[i]), &QPushButton::clicked, [=](){updateSpinData(DATA_IN2, i);});
    }

}

//窗体关闭
void HDist::closeEvent(QCloseEvent *)
{
    emit WinClosed();
}

//更新输入输出限制
void HDist::updateIOBit(int val)
{
    QString style0 = pbtnStyle0 + hoverStyle + pressStyle;
    QString style1 = pbtnStyle1 + hoverStyle + pressStyle;
    //设置十进制输入上限
    ui->spinIn1->setMaximum((1<<val) - 1);
    ui->spinIn2->setMaximum((1<<val) - 1);
    //设置默认输入值
    int deftVal = 0;
    ui->spinIn1->setValue(deftVal);
    ui->spinIn2->setValue(deftVal);
    //设置输入输出数据块
    QString deftValStr = QString::number(deftVal);
    ui->labHDist->setText(deftValStr);
    QString valStr0;
    int bits = ui->spinBit->value();
    for(int i = 0; i < bits; i++)
        valStr0 = valStr0 + "0";
    ui->labOut->setText(valStr0);
    for(int i = 0; i < val; i++){
        //默认值
        DataIn1[i].setText(deftValStr);
        DataIn2[i].setText(deftValStr);
        DataOut[i].setText(deftValStr);
        //默认样式
        DataIn1[i].setStyleSheet(style0);
        DataIn2[i].setStyleSheet(style0);
        DataOut[i].setStyleSheet(pbtnStyle0);   //输出样式无hover,press
        //可见
        DataIn1[i].setVisible(true);
        DataIn2[i].setVisible(true);
        DataOut[i].setVisible(true);
    }
    for(int i = val; i < BIT_MAX; i++){
        //不可见
        DataIn1[i].setVisible(false);
        DataIn2[i].setVisible(false);
        DataOut[i].setVisible(false);
    }
}

//根据十进制输入，更新二进制输入块
void HDist::updateBlockData(int choice, int val)
{
    HammingBack converter;
    QString valBStr = QString::fromStdString(converter.int2bstring(val));
    int len = valBStr.length();
    int bits = ui->spinBit->value();    //输入位数
    //选择对应的数据块
    QPushButton *pData = nullptr;
    if(choice == DATA_IN1)
        pData = DataIn1;
    else if(choice == DATA_IN2)
        pData = DataIn2;
    else
        return;
    //整数转字符串后，高位在前；整数占bits位，对应字符串的后bits位
    for(int i = 0; i < bits; i++){
        if(valBStr[len-bits+i] == '0')      //按字符串顺序
            setBlockStatus(&(pData[i]), 0);
        else
            setBlockStatus(&(pData[i]), 1);
    }
    return;
}

//根据二进制输入，更新十进制输入框
void HDist::updateSpinData(int choice, int pos)
{
    //选择数字框
    QSpinBox *pSpin = nullptr;
    QPushButton *pDataBlk = nullptr;
    if(choice == DATA_IN1){
        pSpin = ui->spinIn1;
        pDataBlk = DataIn1;
    }
    else if(choice == DATA_IN2){
        pSpin = ui->spinIn2;
        pDataBlk = DataIn2;
    }
    else
        return;
    //计算掩码
    int bits = ui->spinBit->value();
    int change = (1 << (bits-pos-1));
    //更新数字框的值
    int val = pSpin->value();
    val = val ^ change;
    ///设置数字框的值时，为防止数字框更新带动数据块变化，需暂时解除连接
    disconnect(pSpin, SIGNAL(valueChanged(int)), this, 0);    //解除连接
    pSpin->setValue(val);               //设置数字框
    flipBlockData(&(pDataBlk[pos]));    //反转数据块
    //恢复连接
    if(choice == DATA_IN1)
        connect(pSpin, SIGNAL(valueChanged(int)), this, SLOT(updateBlockIn1(int)));
    else if(choice == DATA_IN2)
        connect(pSpin, SIGNAL(valueChanged(int)), this, SLOT(updateBlockIn2(int)));

}

//设置数据块状态
void HDist::setBlockStatus(QPushButton *pbtn, int status)
{
    QString style0 = pbtnStyle0 + hoverStyle + pressStyle;
    QString style1 = pbtnStyle1 + hoverStyle + pressStyle;
    if(status == 0){
        pbtn->setStyleSheet(style0);
        pbtn->setText("0");
    }
    else {
        pbtn->setStyleSheet(style1);
        pbtn->setText("1");
    }
}

//数据块状态反转
void HDist::flipBlockData(QPushButton *pbtn)
{
    if(pbtn->text() == "0")
        setBlockStatus(pbtn, 1);
    else
        setBlockStatus(pbtn, 0);
}

//更新【输入1】对应的数据块
void HDist::updateBlockIn1(int val)
{
    updateBlockData(DATA_IN1, val);
}

//更新【输入2】对应的数据块
void HDist::updateBlockIn2(int val)
{
    updateBlockData(DATA_IN2, val);
}

//更新输出数据
void HDist::updateDataOut()
{
    int bits = ui->spinBit->value();
    int din1 = ui->spinIn1->value();
    int din2 = ui->spinIn2->value();
    int dout = din1^din2;

    HammingBack converter;
    int dist = converter.calHammingDistance(din1, din2);
    QString doutBStr = QString::fromStdString(converter.int2bstring(dout));
    int len = doutBStr.length();
    doutBStr = doutBStr.mid(len-bits, bits);

    ui->labOut->setText(doutBStr);
    ui->labHDist->setText(QString::number(dist));
    for(int i = 0; i < bits; i++){
        if(doutBStr[i] == '0'){     //顺序判断
            DataOut[i].setStyleSheet(pbtnStyle0);
            DataOut[i].setText("0");
        }
        else {
            DataOut[i].setStyleSheet(pbtnStyle1);
            DataOut[i].setText("1");
        }
    }
}

