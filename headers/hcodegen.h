#ifndef HCODEGEN_H
#define HCODEGEN_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QPropertyAnimation>
#include "subwindow.h"
#include "HammingBack.h"

#define HAMM_MAX    9
#define DATA_MAX    5
#define PARITY_MAX  4
#define OPRD_MAX    (HAMM_MAX+1)/2
#define OPTR_MAX    OPRD_MAX

#define INIT_STATUS -1

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

    HammingResult HammResult;   //������ṹ��
    int dataBits, parityBits;   //��Ϣλ����У��λ��
    int HammLink[HAMM_MAX];     //�������Ӧ����Ϣλ/У��λ
    int DataLink[DATA_MAX];     //��Ϣλ��Ӧ�ĺ�����λ
    int PrHnos[PARITY_MAX][OPRD_MAX];   //У��λ��Ӧ�����к�����λ���
    int PrHnoLen[PARITY_MAX];   //У��λ��Ӧ�ĺ�����λ����

    int speedLevel;         //�����ٶ�
    int stepStatus;         //��������״̬
    QString stepStatusStr;  //״̬����
    //��ť��ʽ
    QString pbtnStyle0, pbtnStyle1, unknownStyle;
    QString hoverStyle, pressStyle, moveStyle;
    QString dataStyle, parityStyle;
    //���ݿ�
    QPushButton HammBlk[HAMM_MAX];
    QPushButton DataBlk[DATA_MAX];
    QPushButton ParityBlk[PARITY_MAX];
    QPushButton PrRowBlks[PARITY_MAX][OPRD_MAX+1];  //�������ͽ��
    //���ݿ��ǩ
    QLabel HammLab[HAMM_MAX];
    QLabel HammLinkLab[HAMM_MAX];
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
    void InitStyles();      //��ʼ����ʽ��

    int cvtDno2Hno(int index);      //��Ϣ���±�->�������±�
    int cvtPno2Hno(int index);      //У�����±�->�������±�
    QString getValStr(QPushButton *pbtn, int n);    //�����ݿ��ȡ���ִ�

    void updateDataCode();  //������Ϣ���

private slots:
    void changeSpeed();     //�ı䶯�������ٶ�

    void setBlkVis(int data_bits);  //�������ݿ�/��ǩ�ɼ���
    QPropertyAnimation *moveBlk(QPushButton *pStart, QPushButton *pEnd,
                 int duration_ms, QString startStyle, QString endStyle);

    void moveDataBlk(int dno);      //�ƶ���Ϣ��飺��Ϣ����->��������
    void updateDataBlk(const QString &dataStr);         //������Ϣ��飨��Ϣ������
    void setBlkStatus(QPushButton *pbtn, int status);   //�������ݿ�״̬
    void flipDataBlk(int dno);      //��ת��Ϣ���

    void genPrBlk(int pno);         //����У��λ������У��λ�����ݿ�
    void movePrBlk(int pno);        //�ƶ�У����飺У��λ��->У������
    void moveParityrBlk(int pno);   //�ƶ�У����飺У������->��������

    void setStepInit();         //���õ���������ʼ״̬
    void updateStepStatus();    //���µ�������״̬
    void setStepFinishStatus(); //���õ�����������״̬

    void setBlkUnknown();       //�������ݿ�״̬δ֪

signals:
    void moveFinished();

};

#endif // HCODEGEN_H
