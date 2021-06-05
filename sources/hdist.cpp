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
    hoverStyle = "QPushButton:hover{background:#DCF0FC}";
    pressStyle = "QPushButton:pressed{background:#D0E0FC}";
    pbtnStyle0 = "QPushButton{background:#F0F0F0}";
    pbtnStyle1 = "QPushButton{background:#C0C0C0}";
    //����Ĭ������λ��
    int deftBit = BIT_MAX;
    ui->spinBit->setValue(deftBit);
    //��������������ݿ�
    int block_w = 30, block_h = 30;
    int delta_x = 10, delta_y = 10;
    for(int i = 0; i < BIT_MAX; i++){
        //�ɵ��
        DataIn1[i].setEnabled(true);
        DataIn2[i].setEnabled(true);
        DataOut[i].setEnabled(true);
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
    //�ı�����λ�����������ݿ�
    connect(ui->spinBit, SIGNAL(valueChanged(int)), this, SLOT(updateIOBit(int)));
    //�ı����ֿ�����ֵ���������ݿ�
    connect(ui->spinIn1, SIGNAL(valueChanged(int)), this, SLOT(updateBlockIn1(int)));
    connect(ui->spinIn2, SIGNAL(valueChanged(int)), this, SLOT(updateBlockIn2(int)));
    //�����ť���������
    connect(ui->pbtnCompute, SIGNAL(clicked()), this, SLOT(updateDataOut()));
    //������ݿ飬�ı����ֿ��ֵ
    for(int i = 0; i < BIT_MAX; i++){
        ///���ڴ������ֿ����ݿ�����ӣ����ֿ���»�������ݿ�仯��updateSpinData�ڲ�����ʱ��������Է�ֹ����
        connect(&(DataIn1[i]), &QPushButton::clicked, [=](){updateSpinData(DATA_IN1, i);});
        connect(&(DataIn2[i]), &QPushButton::clicked, [=](){updateSpinData(DATA_IN2, i);});
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
    QString style0 = pbtnStyle0 + hoverStyle + pressStyle;
    QString style1 = pbtnStyle1 + hoverStyle + pressStyle;
    //����ʮ������������
    ui->spinIn1->setMaximum((1<<val) - 1);
    ui->spinIn2->setMaximum((1<<val) - 1);
    //����Ĭ������ֵ
    int deftVal = 0;
    ui->spinIn1->setValue(deftVal);
    ui->spinIn2->setValue(deftVal);
    //��������������ݿ�
    QString deftValStr = QString::number(deftVal);
    ui->labHDist->setText(deftValStr);
    QString valStr0;
    int bits = ui->spinBit->value();
    for(int i = 0; i < bits; i++)
        valStr0 = valStr0 + "0";
    ui->labOut->setText(valStr0);
    for(int i = 0; i < val; i++){
        //Ĭ��ֵ
        DataIn1[i].setText(deftValStr);
        DataIn2[i].setText(deftValStr);
        DataOut[i].setText(deftValStr);
        //Ĭ����ʽ
        DataIn1[i].setStyleSheet(style0);
        DataIn2[i].setStyleSheet(style0);
        DataOut[i].setStyleSheet(pbtnStyle0);   //�����ʽ��hover,press
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
    int bits = ui->spinBit->value();    //����λ��
    //ѡ���Ӧ�����ݿ�
    QPushButton *pData = nullptr;
    if(choice == DATA_IN1)
        pData = DataIn1;
    else if(choice == DATA_IN2)
        pData = DataIn2;
    else
        return;
    //����ת�ַ����󣬸�λ��ǰ������ռbitsλ����Ӧ�ַ����ĺ�bitsλ
    for(int i = 0; i < bits; i++){
        if(valBStr[len-bits+i] == '0')      //���ַ���˳��
            setBlockStatus(&(pData[i]), 0);
        else
            setBlockStatus(&(pData[i]), 1);
    }
    return;
}

//���ݶ��������룬����ʮ���������
void HDist::updateSpinData(int choice, int pos)
{
    //ѡ�����ֿ�
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
    //��������
    int bits = ui->spinBit->value();
    int change = (1 << (bits-pos-1));
    //�������ֿ��ֵ
    int val = pSpin->value();
    val = val ^ change;
    ///�������ֿ��ֵʱ��Ϊ��ֹ���ֿ���´������ݿ�仯������ʱ�������
    disconnect(pSpin, SIGNAL(valueChanged(int)), this, 0);    //�������
    pSpin->setValue(val);               //�������ֿ�
    flipBlockData(&(pDataBlk[pos]));    //��ת���ݿ�
    //�ָ�����
    if(choice == DATA_IN1)
        connect(pSpin, SIGNAL(valueChanged(int)), this, SLOT(updateBlockIn1(int)));
    else if(choice == DATA_IN2)
        connect(pSpin, SIGNAL(valueChanged(int)), this, SLOT(updateBlockIn2(int)));

}

//�������ݿ�״̬
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

//���ݿ�״̬��ת
void HDist::flipBlockData(QPushButton *pbtn)
{
    if(pbtn->text() == "0")
        setBlockStatus(pbtn, 1);
    else
        setBlockStatus(pbtn, 0);
}

//���¡�����1����Ӧ�����ݿ�
void HDist::updateBlockIn1(int val)
{
    updateBlockData(DATA_IN1, val);
}

//���¡�����2����Ӧ�����ݿ�
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
    int len = doutBStr.length();
    doutBStr = doutBStr.mid(len-bits, bits);

    ui->labOut->setText(doutBStr);
    ui->labHDist->setText(QString::number(dist));
    for(int i = 0; i < bits; i++){
        if(doutBStr[i] == '0'){     //˳���ж�
            DataOut[i].setStyleSheet(pbtnStyle0);
            DataOut[i].setText("0");
        }
        else {
            DataOut[i].setStyleSheet(pbtnStyle1);
            DataOut[i].setText("1");
        }
    }
}

