#ifndef DOUBLEWIDGET_NORMAL_H
#define DOUBLEWIDGET_NORMAL_H

#include "doublewidget.h"

class doublewidget_normal : public doublewidget
{
public:
    doublewidget_normal(QSize dsize ,QWidget *parent = 0);
    virtual void doublewidget_layout();
};

#endif // DOUBLEWIDGET_NORMAL_H
