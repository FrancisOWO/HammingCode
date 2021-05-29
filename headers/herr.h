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

    void InitMembers();     //初始化数据成员
    void InitConnections(); //关联信号与槽

};

#endif // HERR_H
