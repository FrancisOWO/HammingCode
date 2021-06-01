#include "hcodegen.h"
#include "ui_hcodegen.h"
#include <QString>
#include <QSpinBox>

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
    //样式
    pbtnStyle0 = "QPushButton{background:#F0F0F0}";
    pbtnStyle1 = "QPushButton{background:#C0C0C0}";
    hoverStyle = "QPushButton:hover{background:#DCF0FC}";
    pressStyle = "QPushButton:pressed{background:#D0E0FC}";
    QString labStyle = "background:#F0F0F0";

    //数据块/标签初始化
    int blk_w = 30, blk_h = 30, delta_w = 10;
    int blk_x, blk_y = 10;
    int lab_h = 16, lab_y = 2;
    QString valStr0 = "0", valStrUnknown = "?";
    //信息码
    blk_x = delta_w;
    for(int i = 0; i < DATA_MAX; i++){
        //数据块
        DataBlk[i].setParent(ui->wgtDataBlk);
        DataBlk[i].setStyleSheet(pbtnStyle0+hoverStyle+pressStyle);
        DataBlk[i].setText(valStr0);
        DataBlk[i].setGeometry(blk_x, blk_y, blk_w, blk_h);
        //标签
        DataLab[i].setParent(ui->wgtDataLab);
        DataLab[i].setStyleSheet(labStyle);
        DataLab[i].setText("D" + QString::number(i+1));
        DataLab[i].setGeometry(blk_x, lab_y, blk_w, lab_h);
        //更新坐标
        blk_x += blk_w + delta_w;
    }
    //海明码
    blk_x = delta_w;
    for(int i = 0; i < HAMM_MAX; i++){
        //数据块
        HammBlk[i].setParent(ui->wgtHammBlk);
        HammBlk[i].setStyleSheet(pbtnStyle0);
        HammBlk[i].setText(valStrUnknown);
        HammBlk[i].setGeometry(blk_x, blk_y, blk_w, blk_h);
        //标签
        HammLab[i].setParent(ui->wgtHammLab);
        HammLab[i].setStyleSheet(labStyle);
        HammLab[i].setText("H" + QString::number(i+1));
        HammLab[i].setGeometry(blk_x, lab_y, blk_w, lab_h);
        //更新坐标
        blk_x += blk_w + delta_w;
    }
    //校验码
    blk_x = delta_w;
    for(int i = 0; i < PARITY_MAX; i++){
        //数据块
        ParityBlk[i].setParent(ui->wgtPrBlk);
        ParityBlk[i].setStyleSheet(pbtnStyle0);
        ParityBlk[i].setText(valStrUnknown);
        ParityBlk[i].setGeometry(blk_x, blk_y, blk_w, blk_h);
        //标签
        ParityLab[i].setParent(ui->wgtPrLab);
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
    for(int i = 0; i < PARITY_MAX; i++){
        blk_x = delta_w;
        for(int j = 0; j < OPTR_MAX; j++){
            //数据块
            PrRowBlks[i][j].setParent(wgtPrBlk[i]);
            PrRowBlks[i][j].setStyleSheet(pbtnStyle0);
            PrRowBlks[i][j].setText(valStrUnknown);
            PrRowBlks[i][j].setGeometry(blk_x, blk_y, blk_w, blk_h);
            //标签
            PrRowLabs[i][j].setParent(wgtPrLab[i]);
            PrRowLabs[i][j].setStyleSheet(labStyle);
            PrRowLabs[i][j].setText("D?");
            PrRowLabs[i][j].setGeometry(blk_x, lab_y, blk_w, lab_h);
            //更新坐标
            blk_x += blk_w + delta_w;
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

}

//设置可见性
void HCodeGen::setBlkVis(int data_bits)
{
    //信息码
    for(int i = 0; i < data_bits; i++){
        DataBlk[i].setVisible(true);
        DataLab[i].setVisible(true);
    }
    for(int i = data_bits; i < DATA_MAX; i++){
        DataBlk[i].setVisible(false);
        DataLab[i].setVisible(false);
    }

    //校验码
    HammingBack converter;
    int parity_bits = converter.calCheckLen(data_bits);     //校验码位数
    QString valStr, valUnknown = "?";
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
        int optr_len = converter.calCheckDatalen(data_bits, i+1);
        int *optr_list = new int[optr_len];
        converter.calCheckDnoList(data_bits, i+1, optr_list, optr_len);
        for(int j = 0; j < optr_len; j++){
            PrRowBlks[i][j].setVisible(true);   //数据块
            PrRowLabs[i][j].setVisible(true);   //标签
            PrRowLabs[i][j].setText("D"+QString::number(optr_list[j]));
        }
        for(int j = optr_len; j < OPTR_MAX; j++){
            PrRowBlks[i][j].setVisible(false);   //数据块
            PrRowLabs[i][j].setVisible(false);   //标签
        }
        delete []optr_list;
    }
}
