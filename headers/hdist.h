#ifndef HDIST_H
#define HDIST_H

#include <QWidget>
#include <QString>
#include <QPushButton>
#include "subwindow.h"

//�������
#define DATA_IN1    0
#define DATA_IN2    1
#define DATA_OUT    2

#define BIT_MAX 8

namespace Ui {
class HDist;
}

class HDist : public SubWindow
{
    Q_OBJECT

public:
    explicit HDist(SubWindow *parent = nullptr);
    ~HDist();

    void closeEvent(QCloseEvent *);

private:
    Ui::HDist *ui;
    QPushButton DataIn1[BIT_MAX];
    QPushButton DataIn2[BIT_MAX];
    QPushButton DataOut[BIT_MAX];

    QString pbtnStyle0, pbtnStyle1;

    void InitMembers();     //��ʼ�����ݳ�Ա
    void InitConnections(); //�����ź����

private slots:
    void updateIOBit(int val);          //�����������λ��
    void updateBlockData(int choice, int val); //����ʮ�������룬���¶����������
    void updateSpinData(int choice);    //���ݶ��������룬����ʮ���������

    void updateBlockIn1(int val);
    void updateBlockIn2(int val);

    void updateDataOut();

signals:
    void WinClosed();       //����ر��ź�

};

#endif // HDIST_H
