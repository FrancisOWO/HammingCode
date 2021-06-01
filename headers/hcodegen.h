#ifndef HCODEGEN_H
#define HCODEGEN_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include "subwindow.h"
#include "HammingBack.h"

#define HAMM_MAX    9
#define DATA_MAX    5
#define PARITY_MAX  4
#define OPRD_MAX    (HAMM_MAX+1)/2
#define OPTR_MAX    OPRD_MAX

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

    HammingResult HammStruct;   //������ṹ��
    int HammLink[HAMM_MAX];     //�������Ӧ����Ϣλ/У��λ
    int DataLink[DATA_MAX];     //��Ϣλ��Ӧ�ĺ�����λ
    int PrDnos[PARITY_MAX][OPRD_MAX];   //У��λ��Ӧ����������λ���
    //��ť��ʽ
    QString pbtnStyle0, pbtnStyle1;
    QString hoverStyle, pressStyle;
    //���ݿ�
    QPushButton HammBlk[HAMM_MAX];
    QPushButton DataBlk[DATA_MAX];
    QPushButton ParityBlk[PARITY_MAX];
    QPushButton PrRowBlks[PARITY_MAX][OPRD_MAX+1];  //�������ͽ��
    QPushButton TempMoveBlk;
    //���ݿ��ǩ
    QLabel HammLab[HAMM_MAX];
    QLabel DataLab[DATA_MAX];
    QLabel ParityLab[PARITY_MAX];
    QLabel *PrRowHead[PARITY_MAX];
    QLabel PrRowLabs[PARITY_MAX][OPRD_MAX+1];   //�������ͽ��
    QLabel PrRowOptrLabs[PARITY_MAX][OPTR_MAX]; //������(��ǩ����)
    QLabel PrRowOptrBlks[PARITY_MAX][OPTR_MAX]; //������(���ݿ�����)
    QWidget *wgtPrBlk[PARITY_MAX];
    QWidget *wgtPrLab[PARITY_MAX];

    void InitMembers();     //��ʼ�����ݳ�Ա
    void InitConnections(); //�����ź����

private slots:
    void setBlkVis(int data_bits);  //�������ݿ�/��ǩ�ɼ���
    void moveBlk(QPushButton *pStart, QPushButton *pEnd, int duration_ms);

    void moveDataBlk();
    void updateDataBlk(const QString &dataStr);
    void flipDataBit(int index);

signals:
    void moveFinished();

};

#endif // HCODEGEN_H
