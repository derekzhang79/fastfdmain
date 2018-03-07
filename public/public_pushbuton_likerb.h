#ifndef PUBLIC_PUSHBUTON_LIKERB_H
#define PUBLIC_PUSHBUTON_LIKERB_H

#include <QPushButton>
#include "lds_pushbutton_wrap.h"

class public_pushbuton_likerb : public lds_pushbutton_wrap
{
    Q_OBJECT
public:
    explicit public_pushbuton_likerb(QWidget *parent = 0);

    void setChecked1(bool f);
signals:
    void signalclicked();
protected:
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

    void focusInEvent(QFocusEvent *e);
    void focusOutEvent(QFocusEvent *e);
private:
    void checkallSendclicked();
};

#endif // PUBLIC_PUSHBUTON_LIKERB_H
