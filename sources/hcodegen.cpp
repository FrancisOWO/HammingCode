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

//��ʼ�����ݳ�Ա
void HCodeGen::InitMembers()
{
    //��ʽ
    pbtnStyle0 = "QPushButton{background:#F0F0F0}";
    pbtnStyle1 = "QPushButton{background:#C0C0C0}";
    hoverStyle = "QPushButton:hover{background:#DCF0FC}";
    pressStyle = "QPushButton:pressed{background:#D0E0FC}";
    QString labStyle = "background:#F0F0F0";

    //���ݿ�/��ǩ��ʼ��
    int blk_w = 30, blk_h = 30, delta_w = 10;
    int blk_x, blk_y = 10, ofs_x = 10;
    int lab_h = 16, lab_y = 2;
    QString valStr0 = "0", valStrUnknown = "?";
    //��Ϣ��
    blk_x = ofs_x;
    for(int i = 0; i < DATA_MAX; i++){
        //���ݿ�
        DataBlk[i].setParent(ui->wgtDataBlk);
        DataBlk[i].setStyleSheet(pbtnStyle0+hoverStyle+pressStyle);
        DataBlk[i].setText(valStr0);
        DataBlk[i].setGeometry(blk_x, blk_y, blk_w, blk_h);
        //��ǩ
        DataLab[i].setParent(ui->wgtDataLab);
        DataLab[i].setStyleSheet(labStyle);
        DataLab[i].setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        DataLab[i].setText("D" + QString::number(i+1));
        DataLab[i].setGeometry(blk_x, lab_y, blk_w, lab_h);
        //��������
        blk_x += blk_w + delta_w;
    }
    //������
    blk_x = ofs_x;
    for(int i = 0; i < HAMM_MAX; i++){
        //���ݿ�
        HammBlk[i].setParent(ui->wgtHammBlk);
        HammBlk[i].setStyleSheet(pbtnStyle0);
        HammBlk[i].setText(valStrUnknown);
        HammBlk[i].setGeometry(blk_x, blk_y, blk_w, blk_h);
        //��ǩ
        HammLab[i].setParent(ui->wgtHammLab);
        HammLab[i].setStyleSheet(labStyle);
        HammLab[i].setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        HammLab[i].setText("H" + QString::number(i+1));
        HammLab[i].setGeometry(blk_x, lab_y, blk_w, lab_h);
        //��������
        blk_x += blk_w + delta_w;
    }
    //У����
    blk_x = ofs_x;
    for(int i = 0; i < PARITY_MAX; i++){
        //���ݿ�
        ParityBlk[i].setParent(ui->wgtPrBlk);
        ParityBlk[i].setStyleSheet(pbtnStyle0);
        ParityBlk[i].setText(valStrUnknown);
        ParityBlk[i].setGeometry(blk_x, blk_y, blk_w, blk_h);
        //��ǩ
        ParityLab[i].setParent(ui->wgtPrLab);
        ParityLab[i].setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ParityLab[i].setStyleSheet(labStyle);
        ParityLab[i].setText("P" + QString::number(i+1));
        ParityLab[i].setGeometry(blk_x, lab_y, blk_w, lab_h);
        //��������
        blk_x += blk_w + delta_w;
    }
    //У��λ
    //  //��ͷ
    PrRowHead[0] = ui->labP1; PrRowHead[1] = ui->labP2;
    PrRowHead[2] = ui->labP3; PrRowHead[3] = ui->labP4;
    // //������
    wgtPrBlk[0] = ui->wgtP1Blk; wgtPrBlk[1] = ui->wgtP2Blk;
    wgtPrBlk[2] = ui->wgtP3Blk; wgtPrBlk[3] = ui->wgtP4Blk;
    // //��ǩ��
    wgtPrLab[0] = ui->wgtP1Lab; wgtPrLab[1] = ui->wgtP2Lab;
    wgtPrLab[2] = ui->wgtP3Lab; wgtPrLab[3] = ui->wgtP4Lab;
    // //ÿ����������ݿ����ǩ
    delta_w = 20;
    // // //�������ͽ��
    for(int i = 0; i < PARITY_MAX; i++){
        blk_x = ofs_x;
        for(int j = 0; j < OPRD_MAX+1; j++){
            //���ݿ�
            PrRowBlks[i][j].setParent(wgtPrBlk[i]);
            PrRowBlks[i][j].setStyleSheet(pbtnStyle0);
            PrRowBlks[i][j].setText(valStrUnknown);
            PrRowBlks[i][j].setGeometry(blk_x, blk_y, blk_w, blk_h);
            //��ǩ
            PrRowLabs[i][j].setParent(wgtPrLab[i]);
            PrRowLabs[i][j].setStyleSheet(labStyle);
            PrRowLabs[i][j].setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
            PrRowLabs[i][j].setText("D?");
            PrRowLabs[i][j].setGeometry(blk_x, lab_y, blk_w, lab_h);
            //��������
            blk_x += blk_w + delta_w;
        }
    }
    // // //������
    QString labStyle0 = "border:0px";
    for(int i = 0; i < PARITY_MAX; i++){
        blk_x = ofs_x + blk_w;
        for(int j = 0; j < OPTR_MAX; j++){
            //���ݿ�
            PrRowOptrBlks[i][j].setParent(wgtPrBlk[i]);
            PrRowOptrBlks[i][j].setStyleSheet(labStyle0);
            PrRowOptrBlks[i][j].setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
            PrRowOptrBlks[i][j].setGeometry(blk_x, blk_y, delta_w, blk_h);
            //��ǩ
            PrRowOptrLabs[i][j].setParent(wgtPrLab[i]);
            PrRowOptrLabs[i][j].setStyleSheet(labStyle0);
            PrRowOptrLabs[i][j].setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
            PrRowOptrLabs[i][j].setGeometry(blk_x, lab_y, delta_w, lab_h);
            //��������
            blk_x += blk_w + delta_w;
            PrRowOptrBlks[i][j].setText("+");
            PrRowOptrLabs[i][j].setText("+");
        }
    }
    //���ÿɼ���
    int data_bits = ui->spinDataBit->value();
    setBlkVis(data_bits);

}

//�����ź����
void HCodeGen::InitConnections()
{
    connect(ui->spinDataBit, SIGNAL(valueChanged(int)), this, SLOT(setBlkVis(int)));

}

//���ÿɼ���
void HCodeGen::setBlkVis(int data_bits)
{
    //��Ϣ��
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

    //У����
    HammingBack converter;
    int parity_bits = converter.calCheckLen(data_bits);     //У����λ��
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
    //����ı���
    ui->lnPrBit->setText(QString::number(parity_bits));
    ui->lnPrCode->setText(valStr);

    //������
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
    //����ı���
    ui->lnHammBit->setText(QString::number(hamm_bits));     //������λ��
    ui->lnHammCode->setText(valStr);

    //У��λ
    for(int i = 0; i < parity_bits; i++){
        //�������У��λ���õ�����λ
        int optr_len = converter.calCheckDatalen(data_bits, i+1);
        int *optr_list = new int[optr_len];
        converter.calCheckDnoList(data_bits, i+1, optr_list, optr_len);
        //��ʾ��������
        for(int j = 0; j < optr_len; j++){
            PrRowBlks[i][j].setVisible(true);   //���ݿ�
            PrRowLabs[i][j].setVisible(true);   //��ǩ
            PrRowLabs[i][j].setText("D"+QString::number(optr_list[j]));
            //���ò�����
            PrRowOptrBlks[i][j].setVisible(true);
            PrRowOptrLabs[i][j].setVisible(true);
            PrRowOptrBlks[i][j].setText("+");
            PrRowOptrLabs[i][j].setText("+");
        }
        PrRowOptrBlks[i][optr_len-1].setText("=");
        PrRowOptrLabs[i][optr_len-1].setText("=");
        delete []optr_list;
        //��ʾ�����
        PrRowBlks[i][optr_len].setVisible(true);
        PrRowLabs[i][optr_len].setVisible(true);
        PrRowLabs[i][optr_len].setText("P"+QString::number(i+1));
        //���ض����
        for(int j = optr_len+1; j < OPRD_MAX+1; j++){
            PrRowBlks[i][j].setVisible(false);   //���ݿ�
            PrRowLabs[i][j].setVisible(false);   //��ǩ
        }
        for(int j = optr_len; j < OPTR_MAX; j++){
            PrRowOptrBlks[i][j].setVisible(false);  //������(���ݿ���)
            PrRowOptrLabs[i][j].setVisible(false);  //������(��ǩ��)
        }
    }
}
