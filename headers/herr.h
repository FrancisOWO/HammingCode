#ifndef HERR_H
#define HERR_H

#include <QWidget>
#include "subwindow.h"

namespace Ui {
class HErr;
}

class HErr : public SubWindow
{
    Q_OBJECT

public:
    explicit HErr(SubWindow *parent = nullptr);
    ~HErr();

private:
    Ui::HErr *ui;

    void InitMembers();     //��ʼ�����ݳ�Ա
    void InitConnections(); //�����ź����

};

#endif // HERR_H
