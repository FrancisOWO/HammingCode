#include "hcodegen.h"
#include "ui_hcodegen.h"
#include <QString>
#include <QPoint>
#include <QSpinBox>
#include <QPropertyAnimation>
#include <QRegExp>
#include <QValidator>
#include <QDebug>

//����ת��
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

//��ʼ�����ݳ�Ա
void HCodeGen::InitMembers()
{
    //��������״̬
    stepStatus = INIT_STATUS;
    //������H��Ӧ����ϢλD/У��λP
    for(int i = 0; i < HAMM_MAX; i++)
        HammLink[i] = 0;        //����
    for(int base=1, i=1; base < HAMM_MAX; base<<=1, i++)
        HammLink[base-1] = -i;  //У��λP?�Ǹ���
    for(int i=2, cnt=1; i < HAMM_MAX; i++){
        if(HammLink[i] != 0)
            continue;
        HammLink[i] = cnt;      //��ϢλD?������
        DataLink[cnt-1] = i;
        cnt++;
    }
    //for(int i = 0; i < HAMM_MAX; i++)
    //  qDebug() << "H"+QString::number(i+1) << "=" << HammLink[i];

    //��ʽ
    pbtnStyle0 = "QPushButton{background:#F0F0F0}";
    pbtnStyle1 = "QPushButton{background:#C0C0C0}";
    unknownStyle = "background:#E8F0FF";
    hoverStyle = "QPushButton:hover{background:#DCF0FC}";
    pressStyle = "QPushButton:pressed{background:#D0E0FC}";
    moveStyle = "background:#FFCCCC;border:1px solid grey";
    QString labStyle = "background:#F4F4F4";
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
        HammBlk[i].setStyleSheet(unknownStyle);
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
        ParityBlk[i].setStyleSheet(unknownStyle);
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
    HammingBack converter;
    // // //�������ͽ��
    for(int i = 0; i < PARITY_MAX; i++){
        //����У�����Ӧ��������Ϣλ
        int dlen = converter.calCheckDatalen(DATA_MAX, i+1);
        converter.calCheckDnoList(DATA_MAX, i+1, PrDnos[i], dlen);
        blk_x = ofs_x;
        for(int j = 0; j < OPRD_MAX+1; j++){
            //���ݿ�
            PrRowBlks[i][j].setParent(wgtPrBlk[i]);
            PrRowBlks[i][j].setStyleSheet(unknownStyle);
            PrRowBlks[i][j].setText(valStrUnknown);
            PrRowBlks[i][j].setGeometry(blk_x, blk_y, blk_w, blk_h);
            //��ǩ
            PrRowLabs[i][j].setParent(wgtPrLab[i]);
            PrRowLabs[i][j].setStyleSheet(labStyle);
            PrRowLabs[i][j].setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
            PrRowLabs[i][j].setText("H?");
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
    //������Ϣ��λ�����ı����ݿ�״̬
    connect(ui->spinDataBit, SIGNAL(valueChanged(int)), this, SLOT(setBlkVis(int)));
    //������Ϣ�룬������Ϣ�����ݿ��ֵ
    connect(ui->lnDataCode, SIGNAL(textChanged(const QString &)), this, SLOT(updateDataBlk(const QString &)));
    for(int i = 0; i < DATA_MAX; i++)       //�����ť����ת���ݿ��ֵ
        connect(&(DataBlk[i]), &QPushButton::clicked, [=](){flipDataBlk(i);});

    //��������
    connect(ui->pbtnHamRestart, SIGNAL(clicked()), this, SLOT(setStepInit()));      //���¿�ʼ
    connect(ui->pbtnHamStep, SIGNAL(clicked()), this, SLOT(updateStepStatus()));    //��һ������
    connect(this, SIGNAL(moveFinished()), this, SLOT(setStepFinishStatus()));       //���ö�������״̬
}

//�������ݿ�״̬δ֪
void HCodeGen::setBlkUnknown()
{
    QString textUnKnown = "?";
    //������
    for(int i = 0; i < HAMM_MAX; i++){
        HammBlk[i].setText(textUnKnown);
        HammBlk[i].setStyleSheet(unknownStyle);
    }
    //У����
    for(int i = 0; i < PARITY_MAX; i++){
        ParityBlk[i].setText(textUnKnown);
        ParityBlk[i].setStyleSheet(unknownStyle);
    }
    //У��λ
    for(int i = 0; i < PARITY_MAX; i++){
        for(int j = 0; j < OPRD_MAX+1; j++){
            PrRowBlks[i][j].setText(textUnKnown);
            PrRowBlks[i][j].setStyleSheet(unknownStyle);
        }
    }

}

//���õ���������ʼ״̬
void HCodeGen::setStepInit()
{
    stepStatusStr = cvtStr2LocalQStr("����������\n(��������)");
    setStepFinishStatus();
    stepStatus = INIT_STATUS;
    //��������
    ui->lnDataCode->setReadOnly(false);     //�������޸�
    for(int i = 0; i < dataBits; i++)       //���ݿ�ɸı�
        DataBlk[i].setEnabled(true);
    //�ָ���Ϣ�����ݿ�״̬
    QString valStr;
    for(int i = 0; i < dataBits; i++)
        valStr = valStr + DataBlk[i].text();
    updateDataBlk(valStr);
    //����δ֪���ݿ�
    setBlkUnknown();

}

//���µ�������״̬
void HCodeGen::updateStepStatus()
{
    ui->pbtnHamStep->setEnabled(false); //���ð�ť
    stepStatus++;
    if(stepStatus == 0){
        stepStatusStr = cvtStr2LocalQStr("��������\n����D1");
        //��������
        ui->lnDataCode->setReadOnly(true);  //����򲻿��޸�
        for(int i = 0; i < dataBits; i++)   //���ݿ鲻�ɸı�
            DataBlk[i].setEnabled(false);
        setStepFinishStatus();
    }
    else if(stepStatus < dataBits){
        stepStatusStr = cvtStr2LocalQStr("��������\n����D")+QString::number(stepStatus+1);
        moveDataBlk(stepStatus-1);
    }
    else if(stepStatus == dataBits){
        stepStatusStr = cvtStr2LocalQStr("��������\n����P1");
        moveDataBlk(stepStatus-1);
    }
    else {
        setStepInit();      //�ָ���ʼ״̬
    }
}

//���õ�����������״̬
void HCodeGen::setStepFinishStatus()
{
    ui->pbtnHamStep->setText(stepStatusStr);//���°�ť����
    ui->pbtnHamStep->setEnabled(true);      //���ð�ť
    //������һ���Ľ���
    if(stepStatus < dataBits){
        //�ı佫Ҫ�ƶ������ݿ�״̬
        DataBlk[stepStatus].setStyleSheet(moveStyle);
    }

}

//���ÿɼ���
void HCodeGen::setBlkVis(int data_bits)
{
    //���������ָ���ʼ״̬
    setStepInit();
    //������Ϣ�������ֻ����ָ�����ȵ�0/1��
    dataBits = data_bits;
    QString rgx0 = QString("^[0-1]{%1}$").arg(data_bits);
    QValidator *vdt = new QRegExpValidator(QRegExp(rgx0));
    ui->lnDataCode->setValidator(vdt);
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
    parityBits = converter.calCheckLen(data_bits);     //У����λ��
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
    //����ı���
    ui->lnPrBit->setText(QString::number(parityBits));
    ui->lnPrCode->setText(valStr);

    //������
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
    //����ı���
    ui->lnHammBit->setText(QString::number(hamm_bits));     //������λ��
    ui->lnHammCode->setText(valStr);

    //У��λ
    for(int i = 0; i < parityBits; i++){
        //�������У��λ���õ�����λ
        int dlen = converter.calCheckDatalen(data_bits, i+1) - 1;
        //��ʾ��������
        for(int j = 0; j < dlen; j++){
            PrRowBlks[i][j].setVisible(true);   //���ݿ�
            PrRowLabs[i][j].setVisible(true);   //��ǩ
            PrRowLabs[i][j].setText("H"+QString::number(PrDnos[i][j+1]));
            //���ò�����
            PrRowOptrBlks[i][j].setVisible(true);
            PrRowOptrLabs[i][j].setVisible(true);
            PrRowOptrBlks[i][j].setText("+");
            PrRowOptrLabs[i][j].setText("+");
        }
        PrRowOptrBlks[i][dlen-1].setText("=");
        PrRowOptrLabs[i][dlen-1].setText("=");
        //��ʾ�����
        PrRowBlks[i][dlen].setVisible(true);
        PrRowLabs[i][dlen].setVisible(true);
        PrRowLabs[i][dlen].setText("H"+QString::number(PrDnos[i][0]));
        //���ض����
        for(int j = dlen; j < OPTR_MAX; j++){
            PrRowOptrBlks[i][j].setVisible(false);  //������(���ݿ���)
            PrRowOptrLabs[i][j].setVisible(false);  //������(��ǩ��)
        }
        for(int j = dlen+1; j < OPRD_MAX+1; j++){
            PrRowBlks[i][j].setVisible(false);   //���ݿ�
            PrRowLabs[i][j].setVisible(false);   //��ǩ
        }
    }
}

//��ת���ݿ�
void HCodeGen::flipDataBlk(int index)
{
    //��ť��ʽ
    QString style0 = pbtnStyle0 + hoverStyle + pressStyle;
    QString style1 = pbtnStyle1 + hoverStyle + pressStyle;
    //������Ϣ��
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
    ///������Ϣ�������ǰ��Ϊ��ֹ�������´������ݿ���£�����ʱ�������
    disconnect(ui->lnDataCode, SIGNAL(textChanged(const QString &)), this, SLOT(updateDataBlk(const QString &)));   //�������
    ui->lnDataCode->setText(valStr);    //������Ϣ��
    connect(ui->lnDataCode, SIGNAL(textChanged(const QString &)), this, SLOT(updateDataBlk(const QString &)));      //�ָ�����
}

//������Ϣ�����ݿ�
void HCodeGen::updateDataBlk(const QString &dataStr)
{
    int len = dataStr.length();
    for(int i = 0; i < len; i++){
        if(dataStr[i] == '0'){   //˳���ж�
            DataBlk[i].setStyleSheet(pbtnStyle0);
            DataBlk[i].setText("0");
        }
        else {
            DataBlk[i].setStyleSheet(pbtnStyle1);
            DataBlk[i].setText("1");
        }
    }
}

//�ƶ���Ϣ�����ݿ�
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

//�ƶ����ݿ�
void HCodeGen::moveBlk(QPushButton *pStart, QPushButton *pEnd, int duration_ms,
                       QString startStyle, QString endStyle)
{
    QPoint pt_start = pStart->mapTo(this, QPoint(0,0)); //���
    QPoint pt_end = pEnd->mapTo(this, QPoint(0,0));     //�յ�
    QString blkText = pStart->text();   //��ǩ
    int blk_w = pStart->width();
    int blk_h = pStart->height();
    //���ö�����
    QPushButton *pBlk = &TempMoveBlk;
    pBlk->setParent(this);
    pBlk->setStyleSheet(moveStyle);
    pBlk->setText(blkText);
    pBlk->setGeometry(pt_start.x(), pt_start.y(), blk_w, blk_h);
    pBlk->show();
    //ƽ�ƶ���
    QPropertyAnimation *pMove = new QPropertyAnimation(pBlk, "pos");
    pMove->setDuration(duration_ms);    //��ʱ
    pMove->setStartValue(pt_start);     //���
    pMove->setEndValue(pt_end);         //�յ�
    pMove->setEasingCurve(QEasingCurve::InOutQuad); //��������
    pMove->start();
    //�������������ض�����
    connect(pMove, &QPropertyAnimation::finished, [=](){
        pBlk->setVisible(false);
        pEnd->setText(blkText);
        pEnd->setStyleSheet(endStyle);
        pStart->setStyleSheet(startStyle);
        emit moveFinished();
    });

}
