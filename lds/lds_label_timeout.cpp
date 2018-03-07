#include "lds_label_timeout.h"
#include <QTimer>

lds_label_timeout::lds_label_timeout(QWidget *parent) :
    QLabel(parent)
{
    timer =  new QTimer(this);
    timer->setInterval(1000);
    connect(timer,SIGNAL(timeout()),this,SLOT(totrigger()));
    this->setStyleSheet("color:crimson;font:14pt;");
}

void lds_label_timeout::start(const QString &title, int timeout)
{
    resultTimeout = timeout;
    this->title = title;
    setText(QString("%1(%2)").arg(title).arg(resultTimeout));

    timer->start();
}

void lds_label_timeout::totrigger()
{
    resultTimeout--;
    setText(QString("%1(%2)").arg(title).arg(resultTimeout));
    if(resultTimeout <= 0){
        emit signaltimeout();
        timer->stop();
    }
}
