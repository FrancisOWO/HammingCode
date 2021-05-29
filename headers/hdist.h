#ifndef HDIST_H
#define HDIST_H

#include <QWidget>
#include "subwindow.h"

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

    void InitMembers();     //��ʼ�����ݳ�Ա
    void InitConnections(); //�����ź����

signals:
    void WinClosed();       //����ر��ź�

};

#endif // HDIST_H
