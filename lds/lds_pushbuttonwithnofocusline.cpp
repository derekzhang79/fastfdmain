#include "lds_pushbuttonwithnofocusline.h"
#include <QEvent>
#include <QtDebug>
#include <QKeyEvent>
#include <QApplication>

lds_pushbuttonwithnofocusline::lds_pushbuttonwithnofocusline(QWidget *parent)
    : QPushButton(parent),
      _widget(0)
{
    this->installEventFilter(this);
}

bool lds_pushbuttonwithnofocusline::eventFilter(QObject */*o*/, QEvent *e)
{
    if(_widget){
        switch(e->type()){
        case QEvent::KeyPress:
        case QEvent::InputMethod:
        case QEvent::ShortcutOverride:
            QApplication::sendEvent(_widget, e);\
            return true;
        default:
            return QObject::eventFilter(0, e);
        }
    }
    return QObject::eventFilter(0, e);
}
