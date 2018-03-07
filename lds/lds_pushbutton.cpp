#include "lds_pushbutton.h"
#include <QEvent>
#include <QApplication>

lds_pushbutton::lds_pushbutton(QWidget *parent) :
    QPushButton(parent),
    _widget(0)
{
}

void lds_pushbutton::focusInEvent(QFocusEvent *e)
{
    QPushButton::focusInEvent(e);
    emit signalfocusin();
}

void lds_pushbutton::focusOutEvent(QFocusEvent *e)
{
    QPushButton::focusOutEvent(e);
    emit signalfocusout();
}

bool lds_pushbutton::eventFilter(QObject */*o*/, QEvent *e)
{
    if(_widget){
        switch(e->type()){
        case QEvent::KeyPress:
        case QEvent::InputMethod:
        case QEvent::ShortcutOverride:
            QApplication::sendEvent(_widget, e);\
            return true;
        default:
            break;
        }
    }
    return QObject::eventFilter(0, e);
}
