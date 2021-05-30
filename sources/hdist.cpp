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

//��ʼ�����ݳ�Ա
void HDist::InitMembers()
{
    //���ð�ť��ʽ
    pbtnStyle0 = "background:#F0F0F0";
    pbtnStyle1 = "background:#C0C0C0";
    //����Ĭ������λ��
    int deftBit = BIT_MAX;
    ui->spinBit->setValue(deftBit);
    //��������������ݿ�
    int block_w = 30, block_h = 30;
    int delta_x = 10, delta_y = 10;
    for(int i = 0; i < BIT_MAX; i++){
        //����λ��
        DataIn1[i].setGeometry(delta_x + i*(block_w +delta_x), delta_y, block_w, block_h);
        DataIn2[i].setGeometry(delta_x + i*(block_w +delta_x), delta_y, block_w, block_h);
        DataOut[i].setGeometry(delta_x + i*(block_w +delta_x), delta_y, block_w, block_h);
        //������
        DataIn1[i].setParent(ui->wgtIn1);
        DataIn2[i].setParent(ui->wgtIn2);
        DataOut[i].setParent(ui->wgtOut);
    }
    //��������������ݿ�״̬
    updateIOBit(deftBit);

}

//�����ź����
void HDist::InitConnections()
{
    connect(ui->spinBit, SIGNAL(valueChanged(int)), this, SLOT(updateIOBit(int)));
    connect(ui->spinIn1, SIGNAL(valueChanged(int)), this, SLOT(updateBlockIn1(int)));
    connect(ui->spinIn2, SIGNAL(valueChanged(int)), this, SLOT(updateBlockIn2(int)));
    connect(ui->pbtnCompute, SIGNAL(clicked()), this, SLOT(updateDataOut()));

    for(int i = 0; i < BIT_MAX; i++){

    }

}

//����ر�
void HDist::closeEvent(QCloseEvent *)
{
    emit WinClosed();
}

//���������������
void HDist::updateIOBit(int val)
{
    //����ʮ������������
    ui->spinIn1->setMaximum((1<<val) - 1);
    ui->spinIn2->setMaximum((1<<val) - 1);
    //����Ĭ������ֵ
    int deftVal = 0;
    ui->spinIn1->setValue(deftVal);
    ui->spinIn2->setValue(deftVal);
    //��������������ݿ�
    QString deftValStr = QString::number(deftVal);
    ui->labOut->setText(deftValStr);
    ui->labHDist->setText(deftValStr);
    for(int i = 0; i < val; i++){
        //Ĭ��ֵ
        DataIn1[i].setText(deftValStr);
        DataIn2[i].setText(deftValStr);
        DataOut[i].setText(deftValStr);
        //Ĭ����ʽ
        DataIn1[i].setStyleSheet(pbtnStyle0);
        DataIn2[i].setStyleSheet(pbtnStyle0);
        DataOut[i].setStyleSheet(pbtnStyle0);
        //�ɼ�
        DataIn1[i].setVisible(true);
        DataIn2[i].setVisible(true);
        DataOut[i].setVisible(true);
    }
    for(int i = val; i < BIT_MAX; i++){
        //���ɼ�
        DataIn1[i].setVisible(false);
        DataIn2[i].setVisible(false);
        DataOut[i].setVisible(false);
    }
}

//����ʮ�������룬���¶����������
void HDist::updateBlockData(int choice, int val)
{
    HammingBack converter;
    QString valBStr = QString::fromStdString(converter.int2bstring(val));
    int len = valBStr.length();
    if(choice == DATA_IN1){
        for(int i = 0; i < len; i++){
            if(valBStr[len-i-1] == '0'){   //�����ж�
                DataIn1[i].setStyleSheet(pbtnStyle0);
                DataIn1[i].setText("0");
            }
            else {
                DataIn1[i].setStyleSheet(pbtnStyle1);
                DataIn1[i].setText("1");
            }
        }
    }
    else if(choice == DATA_IN2){
        for(int i = 0; i < len; i++){
            if(valBStr[len-i-1] == '0'){   //�����ж�
                DataIn2[i].setStyleSheet(pbtnStyle0);
                DataIn2[i].setText("0");
            }
            else {
                DataIn2[i].setStyleSheet(pbtnStyle1);
                DataIn2[i].setText("1");
            }
        }
    }
    return;
}

//���ݶ��������룬����ʮ���������
void HDist::updateSpinData(int choice)
{

}

void HDist::updateBlockIn1(int val)
{
    updateBlockData(DATA_IN1, val);
}

void HDist::updateBlockIn2(int val)
{
    updateBlockData(DATA_IN2, val);
}

//�����������
void HDist::updateDataOut()
{
    int bits = ui->spinBit->value();
    int din1 = ui->spinIn1->value();
    int din2 = ui->spinIn2->value();
    int dout = din1^din2;

    HammingBack converter;
    int dist = converter.calHammingDistance(din1, din2);
    QString doutBStr = QString::fromStdString(converter.int2bstring(dout));

    ui->labOut->setText(QString::number(dout));
    ui->labHDist->setText(QString::number(dist));
    int len = doutBStr.length();
    for(int i = 0; i < bits; i++){
        if(doutBStr[len-i-1] == '0'){   //�����ж�
            DataOut[i].setStyleSheet(pbtnStyle0);
            DataOut[i].setText("0");
        }
        else {
            DataOut[i].setStyleSheet(pbtnStyle1);
            DataOut[i].setText("1");
        }
    }
}

