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

    void InitMembers();     //��ʼ�����ݳ�Ա
    void InitConnections(); //�����ź����

};

#endif // HCODEGEN_H
