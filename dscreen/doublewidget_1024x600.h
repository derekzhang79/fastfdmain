#ifndef DOUBLEWIDGET_1024X600_H
#define DOUBLEWIDGET_1024X600_H

#include "doublewidget.h"

class doublewidget_1024x600 : public doublewidget
{
    Q_OBJECT
public:
    doublewidget_1024x600(QSize dsize ,QWidget *parent = 0);

protected slots:
    virtual void doublewidget_layout();
    virtual void adc_timer_start();
    virtual void adc_timer_out();
    virtual void standmodel_setdata(int row, const QString &p0, const QString &p1, const QString &p2, const QString &p3, const QString &p4);
    virtual void standmodel_insetdata(int row, const QString &p0, const QString &p1, const QString &p2, const QString &p3, const QString &p4);
    virtual void total_settext(const QString &text);
    virtual void num_settext(const QString &text);
    virtual void adc_show();
    virtual QString class_name();

private slots:
    void delayShowPayTest();
private:
    QString blank;

};

#endif // DOUBLEWIDGET_1024X600_H
