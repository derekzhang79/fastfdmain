#include "lds_keytoolbutton.h"
#include "lds_keytoolbutton_pop.h"
#include <QApplication>
#include <QLineEdit>
#include "lds.h"
#include <QtDebug>
#include <QTimer>
#ifdef Q_OS_UNIX
#include <QCopChannel>
#endif
#include "lds_lineedit.h"

lds_keytoolbutton::lds_keytoolbutton(QWidget *parent, KEYTYPE type) :
    QToolButton(parent),
    _type(type)
{
    parentl=0;
    this->setFocusPolicy(Qt::NoFocus);
    this->setAutoRaise(true);
    this->setToolButtonStyle(Qt::ToolButtonIconOnly);
    this->setIcon(QIcon(":/image/widgets_pic/keyboard.png"));
    this->setIconSize(QSize(80,40));

    connect(this,SIGNAL(clicked()),this,SLOT(showpop()));

}

void lds_keytoolbutton::setKeyType(lds_keytoolbutton::KEYTYPE type)
{
    _type = type;
}

lds_keytoolbutton::KEYTYPE lds_keytoolbutton::Keytype()
{
    return _type;
}

void lds_keytoolbutton::tocenter(const QString &value)
{
    if(!parentl) return;
    if(value=="<<"){
        parentl->backspace();
    } else if(value=="<-"){
        parentl->cursorBackward(false);
    } else if(value=="->"){
        parentl->cursorForward(false);
    } else {
        parentl->insert(value);
    }
}

void lds_keytoolbutton::showpop()
{
#ifdef Q_OS_UNIX
    if(_type == SYSKEY){
#ifdef QT_DEBUG
#else
        QByteArray data;
        QCopChannel::send(QString("/sys/zwserver"),"keyboard",data);
#endif
    }
#endif
    if(_type == NUMKEY){
        QWidget *fw=qApp->focusWidget();
        if(fw
                &&fw->inherits("QLineEdit")
                ){
            parentl=qobject_cast<QLineEdit*>(fw);
            oldstylesheet = parentl->styleSheet();

            parentl->setStyleSheet(oldstylesheet+"color:indigo;font:bold 15pt;");
            parentl->selectAll();
            QTimer::singleShot(100, this,SLOT(showpop_time()));
        }
    }
}

void lds_keytoolbutton::showpop_time()
{
    lds_keytoolbutton_pop *pop=new lds_keytoolbutton_pop;
    pop->setAttribute(Qt::WA_DeleteOnClose);
    connect(pop,SIGNAL(signal_click(QString)),this,SLOT(tocenter(QString)));
    pop->raise();
    pop->move(lds::getPopPoint_size(parentl, pop));
    pop->show();
}
