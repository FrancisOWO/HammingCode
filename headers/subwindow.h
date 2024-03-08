#ifndef SUBWINDOW_H
#define SUBWINDOW_H

#include <QWidget>

class SubWindow : public QWidget
{
    Q_OBJECT

public:
    explicit SubWindow(QWidget *parent = nullptr);
    virtual ~SubWindow();

};

#endif // SUBWINDOW_H
