#include "qcheckboxlongpress.h"
#include <QTimeLine>
#include <QEventLoop>
#include <QTimer>
#include <QLabel>
#include <QMovie>
#include <QMouseEvent>
#include <QtDebug>
#include <QEventLoop>
#include <QMap>

QCheckBoxLongPress::QCheckBoxLongPress(QWidget *parent)
    : QCheckBox(parent)
{
    do_longpress = new lds_record_do_longpress(this);
    connect(do_longpress, SIGNAL(timeout()), this, SIGNAL(timeout()));
}

void QCheckBoxLongPress::enableLongPress(bool enabled)
{
    do_longpress->setEnable(enabled);
    if(enabled)
        this->setStyleSheet("color:#ff650b;");
}

void QCheckBoxLongPress::mousePressEvent(QMouseEvent *event)
{
    QCheckBox::mousePressEvent(event);
    do_longpress->start(event->pos());
}

void QCheckBoxLongPress::mouseReleaseEvent(QMouseEvent *event)
{
    if(do_longpress->isprepared_stop()) {
        QCheckBox::mouseReleaseEvent(event);
    }
}
