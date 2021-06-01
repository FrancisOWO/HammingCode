#include "hcodegen.h"
#include "ui_hcodegen.h"
#include <QString>
#include <QPoint>
#include <QSpinBox>
#include <QPropertyAnimation>
#include <QDebug>

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
    hoverStyle = "QPushButton:hover{background:#DCF0FC}";
    pressStyle = "QPushButton:pressed{background:#D0E0FC}";
    QString labStyle = "background:#F0F0F0";
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
        HammBlk[i].setStyleSheet(pbtnStyle0);
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
        ParityBlk[i].setStyleSheet(pbtnStyle0);
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
            PrRowBlks[i][j].setStyleSheet(pbtnStyle0);
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
    connect(ui->spinDataBit, SIGNAL(valueChanged(int)), this, SLOT(setBlkVis(int)));
    //动画样例
    connect(ui->pbtnHamStep, &QPushButton::clicked, [=](){
        ui->lnDataCode->setReadOnly(true);        //不可修改
        ui->pbtnHamStep->setEnabled(false);     //禁用按钮
        moveDataBlk();
    });

}

//设置可见性
void HCodeGen::setBlkVis(int data_bits)
{
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
    int parity_bits = converter.calCheckLen(data_bits);     //校验码位数
    valStr = "";
    for(int i = 0; i < parity_bits; i++){
        valStr = valStr + valUnknown;
        ParityBlk[i].setVisible(true);
        ParityLab[i].setVisible(true);
        PrRowHead[i]->setVisible(true);
        wgtPrLab[i]->setVisible(true);
        wgtPrBlk[i]->setVisible(true);
    }
    for(int i = parity_bits; i < PARITY_MAX; i++){
        ParityBlk[i].setVisible(false);
        ParityLab[i].setVisible(false);
        PrRowHead[i]->setVisible(false);
        wgtPrLab[i]->setVisible(false);
        wgtPrBlk[i]->setVisible(false);
    }
    //填充文本框
    ui->lnPrBit->setText(QString::number(parity_bits));
    ui->lnPrCode->setText(valStr);

    //海明码
    int hamm_bits = data_bits + parity_bits;
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
    for(int i = 0; i < parity_bits; i++){
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

//移动信息码数据块
void HCodeGen::moveDataBlk()
{
    int duration_ms = 800;
    static int cnt = 0;
    if(cnt >= ui->spinDataBit->value())
        cnt = 0;
    moveBlk(&(DataBlk[cnt]), &(HammBlk[DataLink[cnt]]), duration_ms);
    cnt++;
}

//移动数据块
void HCodeGen::moveBlk(QPushButton *pStart, QPushButton *pEnd, int duration_ms)
{
    QPoint pt_start = pStart->mapTo(this, QPoint(0,0)); //起点
    QPoint pt_end = pEnd->mapTo(this, QPoint(0,0));     //终点
    QString blkText = pStart->text();   //标签
    QString blkStyle = pStart->styleSheet();
    int blk_w = pStart->width();
    int blk_h = pStart->height();
    //设置动画块
    QString tempStyle = "background:#FFCCCC;border:1px solid grey";
    QPushButton *pBlk = &TempMoveBlk;
    pBlk->setParent(this);
    pBlk->setStyleSheet(tempStyle);
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
        pEnd->setStyleSheet(blkStyle);
        ui->pbtnHamStep->setEnabled(true);  //启用按钮
        ui->lnDataCode->setReadOnly(false); //不可修改
        emit moveFinished();
    });

}
