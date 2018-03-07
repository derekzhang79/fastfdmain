#include "public_pushbuton_likerb_pur.h"
#include <QtDebug>
#include <QMouseEvent>

public_pushbuton_likerb_pur::public_pushbuton_likerb_pur(QWidget *parent) :
    QPushButton(parent)
{
    isexclusive = true;
    isnormalbtn = false;
    do_longpress = new lds_record_do_longpress(this);
    connect(do_longpress, SIGNAL(timeout()), this, SIGNAL(timeout()));
}

void public_pushbuton_likerb_pur::setChecked1(bool f)
{
    if(isnormalbtn) {
        return;
    }
    this->setDown(f);
    scan_all_except_self();
}

void public_pushbuton_likerb_pur::setExclusive(bool isexclusive)
{
    this->isexclusive = isexclusive;
}

void public_pushbuton_likerb_pur::enableLongPress(bool flag)
{
    do_longpress->setEnable(flag);
}

void public_pushbuton_likerb_pur::setNormalBtn(bool flag)
{
    isnormalbtn = flag;
}

void public_pushbuton_likerb_pur::mousePressEvent(QMouseEvent *e)
{
    bool isd = isDown();

    QPushButton::mousePressEvent(e);
    if(isnormalbtn) {
        return;
    }

    do_longpress->start(e->pos());

    if(false == isd) {//release
        scan_all_except_self();
    } else {
        if(false == isexclusive) {
            this->setDown(false);
        }
    }
    emit signalpressed(false == isd);
}

void public_pushbuton_likerb_pur::mouseReleaseEvent(QMouseEvent *e)
{
    if(isnormalbtn) {
        QPushButton::mouseReleaseEvent(e);
        return;
    }
    do_longpress->isprepared_stop();
}

void public_pushbuton_likerb_pur::mouseMoveEvent(QMouseEvent *e)
{
    //do nothing
}

void public_pushbuton_likerb_pur::focusInEvent(QFocusEvent *e)
{
    //do nothing
}

void public_pushbuton_likerb_pur::focusOutEvent(QFocusEvent *e)
{
    //do nothing
}

void public_pushbuton_likerb_pur::resizeEvent(QResizeEvent *event)
{
    QPushButton::resizeEvent(event);
//    int w = qMin(this->width(), this->height());
//    setIconSize(QSize(w, w));
//    qDebug() << __LINE__ << __FUNCTION__ << this->objectName() << this->text() << "size:" << w <<w;
}

void public_pushbuton_likerb_pur::paintEvent(QPaintEvent *e)
{
    QPushButton::paintEvent(e);

    qDebug() << __LINE__ << __FUNCTION__ << this->objectName() << this->text() <<  "size:" << this->iconSize();
}


void public_pushbuton_likerb_pur::scan_all_except_self()
{
    foreach(QObject *b, this->parent()->children()) {
        if(b == this) continue;
        public_pushbuton_likerb_pur *t0 = qobject_cast<public_pushbuton_likerb_pur *>(b);
        if(t0){
            t0->setDown(false);
        }
    }
}


public_pushbuton_likerb_pur_unexclusive::public_pushbuton_likerb_pur_unexclusive(QWidget *parent)
    : public_pushbuton_likerb_pur(parent)
{
    setExclusive(false);
}


public_pushbuton_likerb_pur_auto_release::public_pushbuton_likerb_pur_auto_release(public_pushbuton_likerb_pur *b)
{
    this->b = b;
}

public_pushbuton_likerb_pur_auto_release::~public_pushbuton_likerb_pur_auto_release()
{
    if(b) {
        b->setDown(false);
    }
}

public_pushbuton_likerb_pur *public_pushbuton_likerb_pur_auto_release::current_checked_rb(QWidget *parent)
{
    foreach(QObject *b, parent->children()) {
        public_pushbuton_likerb_pur *t0 = qobject_cast<public_pushbuton_likerb_pur *>(b);
        if(t0){
            if(t0->isDown()) {
                return t0;
            }
        }
    }
    return 0;
}

public_pushbuton_likerb_pur_false::public_pushbuton_likerb_pur_false(QWidget *parent)
{
    setNormalBtn(true);
}
