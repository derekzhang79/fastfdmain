#include "public_pushbuton_likerb.h"

public_pushbuton_likerb::public_pushbuton_likerb(QWidget *parent) :
    lds_pushbutton_wrap(parent)
{
    this->setFocusPolicy(Qt::NoFocus);
    textflags = int(Qt::AlignCenter|Qt::TextWordWrap);
    xp1 = 10;
}

void public_pushbuton_likerb::setChecked1(bool f)
{
    this->setDown(f);
    foreach(QObject *b, this->parent()->children()) {
        if(b == this) continue;
        public_pushbuton_likerb *t0 = qobject_cast<public_pushbuton_likerb *>(b);
        if(t0){
            t0->setDown(false);
        }
    }
}

void public_pushbuton_likerb::mousePressEvent(QMouseEvent *e)
{
    QPushButton::mousePressEvent(e);
    checkallSendclicked();
}

void public_pushbuton_likerb::mouseReleaseEvent(QMouseEvent *e)
{

}

void public_pushbuton_likerb::focusInEvent(QFocusEvent *e)
{

}

void public_pushbuton_likerb::focusOutEvent(QFocusEvent *e)
{

}

void public_pushbuton_likerb::checkallSendclicked()
{
    foreach(QObject *b, this->parent()->children()) {
        if(b == this) continue;
        public_pushbuton_likerb *t0 = qobject_cast<public_pushbuton_likerb *>(b);
        if(t0){
            t0->setDown(false);
        }
        emit signalclicked();
    }
}
