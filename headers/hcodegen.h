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

#define CODE_IN     0
#define CODE_OUT    1

#define CODE_OK     0
#define CODE_ERR    1

#define BLK_NORMAL  0
#define BLK_CHECK   1

#define STYLE_NORMAL    0
#define STYLE_HLIGHT    1

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
    int PrHnos[PARITY_MAX][OPRD_MAX+1];     //У��λ��Ӧ�����к�����λ���
    int PrHnoLen[PARITY_MAX];   //У��λ��Ӧ�ĺ�����λ����

    int hammGenFlag;        //���������ɱ�־
    int rowChanged;         //����и������б��
    int speedLevel;         //�����ٶ�
    int stepStatus;         //���㺣���뵥������״̬
    int checkStatus;        //������������״̬

    bool codeChangeFlag[HAMM_MAX];  //���������ݸı���
    bool CodeInStatus[HAMM_MAX];    //����������״̬����ȷ/����
    QString stepStatusStr;  //��������״̬����

    //��ʽ
    QString pbtnStyle0, pbtnStyle1, unknownStyle;
    QString hoverStyle, pressStyle, moveStyle;
    QString dataStyle, parityStyle;
    QString okStyle, errStyle, chgStyle;

    //���ݿ�
    QPushButton HammBlk[HAMM_MAX];      //������
    QPushButton DataBlk[DATA_MAX];      //��Ϣ��
    QPushButton ParityBlk[PARITY_MAX];  //У����
    QPushButton PrRowBlks[PARITY_MAX][OPRD_MAX+2];  //�������ͽ��
    QPushButton CodeBlk[2][HAMM_MAX];   //��У������/���������

    //���ݿ��ǩ
    QLabel HammLab[HAMM_MAX];           //������
    QLabel HammLinkLab[HAMM_MAX];       //������λ��Ӧ����Ϣλ/У��λ
    QLabel DataLab[DATA_MAX];           //��Ϣ��
    QLabel ParityLab[PARITY_MAX];       //У����
    QLabel *PrRowHead[PARITY_MAX];      //У��λ���ױ��
    QLabel PrRowLabs[PARITY_MAX][OPRD_MAX+2];           //�������ͽ��
    QLabel PrRowOptrLabs[PARITY_MAX][OPTR_MAX+1];       //������(��ǩ����)
    QLabel PrRowOptrBlkLabs[PARITY_MAX][OPTR_MAX+1];    //������(���ݿ�����)
    QLabel CodeLab[2][HAMM_MAX];        //��У������/���������
    QLabel HammLinkLab2[HAMM_MAX];      //��У������ ������λ��Ӧ����Ϣλ/У��λ

    //����
    QWidget *wgtPrBlk[PARITY_MAX];      //У�������ݿ�
    QWidget *wgtPrLab[PARITY_MAX];      //У������ǩ

    /********************* ��ʼ�� *********************/
    void InitMembers();         //��ʼ�����ݳ�Ա
    void InitConnections();     //�����ź����
    void InitStyles();          //��ʼ����ʽ��

    /******************** �������� ********************/
    void setPbtnProp(QPushButton *pbtn, QWidget *parent, QString style, QString text);                  //���ð�ť����
    void setLabProp(QLabel *lab, QWidget *parent, QString style, Qt::Alignment align, QString text);    //���ñ�ǩ����

    /******************** ת������ ********************/
    int cvtDno2Hno(int index);      //��Ϣ���±�->�������±�
    int cvtPno2Hno(int index);      //У�����±�->�������±�
    int cvtPrHno2Hno(int pno, int index);   //����У����Ĳ������±�->�������±�
    QString getValStr(QPushButton *pbtn, int n);    //�����ݿ��ȡ���ִ�

    /******************* ���״̬���� *******************/
    void updateDataCode();      //������Ϣ���
    void updatePrBitLab();      //����У��λ�����Ʊ�ǩ
    void setGenBtnsEnabled(bool flag);      //����/���ð�ť�飨���ɺ����밴ť�飩
    void setCheckBtnsEnabled(bool flag);    //����/���ð�ť�飨������ť�飩

private slots:
    /******************** �������� ********************/
    void changeSpeed();             //�ı䶯�������ٶ�

    /******************** �Ϸ������ ********************/
    void updateLegalCodeTable();    //���Ϸ������
    void updateCheckRow();          //���¾����ı����Ӧ����
    void changeCodeRowStyle(int row, int choice);   //�ı�����ʽ

    /****************** ���ݿ�״̬���� ******************/
    void setBlkUnknown();           //�������ݿ�״̬δ֪
    void setBlkStatus(QPushButton *pbtn, int status);   //�������ݿ�״̬
    void flipDataBlk(QPushButton *pbtns, int dno, int blktype = BLK_NORMAL);    //��ת��Ϣ���

    /****************** ���ݿ��ƶ�/���� ******************/
    QPropertyAnimation *moveBlk(QPushButton *pStart, QPushButton *pEnd,
                 int duration_ms, QString startStyle, QString endStyle);    //�ƶ����ݿ�

    void updateDataBlk(const QString &dataStr);     //�������ִ�������Ϣ��飨��Ϣ������
    void moveDataBlk(int dno);      //�ƶ���Ϣ��飺��Ϣ����->��������

    void genPrBlk(int pno);         //����У��λ������У��λ�����ݿ�
    void movePrBlk(int pno);        //�ƶ�У����飺У��λ��->У������
    void moveParityrBlk(int pno);   //�ƶ�У����飺У������->��������

    /******************* ���������ɶ��� *******************/
    void setBlkVis(int data_bits);  //�������ݿ�/��ǩ�ɼ���
    void setGenInit();              //���������ɶ�����ʼ��
    void updateGenStatus();         //���º��������ɶ���״̬
    void setGenFinish();            //���������ɶ�����������
    void getFinalHamm();            //һ�������ɺ�����

    /******************* �������� *******************/
    void getCheckResult(int pno);   //����У����
    void genCheckBlk(int pno);      //һλУ�鶯��
    void correctCode(QString &valStr, int hamm_bits, int errbit);   //����

    void setCheckInit();            //����������ʼ��
    void updateCheckStatus();       //���¼�������״̬
    void setCheckFinish();          //����������������
    void getFinalCheck();           //һ��������У����

signals:
    void moveFinished();

};

#endif // HCODEGEN_H
