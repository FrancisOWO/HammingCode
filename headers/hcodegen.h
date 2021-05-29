#ifndef HCODEGEN_H
#define HCODEGEN_H

#include <QWidget>
#include "subwindow.h"

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

    void InitMembers();     //初始化数据成员
    void InitConnections(); //关联信号与槽

};

#endif // HCODEGEN_H
