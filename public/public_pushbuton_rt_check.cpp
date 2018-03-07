#include "public_pushbuton_rt_check.h"
#include <QStylePainter>
#include <QStyleOptionButton>

public_pushbuton_rt_check::public_pushbuton_rt_check(QWidget *parent) :
    lds_toolbar_button(parent)
{
    _state = none;
}

QString public_pushbuton_rt_check::get_m_belong_state()
{
    return m_belong_state;
}

void public_pushbuton_rt_check::set_rt_checked(const vch_membernoData &m)
{
    _state = none;
    m_belong_state = m.ch_member_belong_state;
    if(!m.vch_memberno.isEmpty()) {
        if("Y" == m_belong_state) {
            _state = yunm;
        } else {
            _state = info;
        }
    }
    update();
}

void public_pushbuton_rt_check::set_rt_checked(bool f)
{
    _state = none;
    if(f) _state = info;
    update();
}

void public_pushbuton_rt_check::set_rt_checked_warn(bool f)
{
    _state = none;
    if(f)    _state = warn;
    update();
}

void public_pushbuton_rt_check::paintEvent(QPaintEvent *e)
{
    QPushButton::paintEvent(e);
    QPainter p(this);
    QPixmap pg;
    switch(_state) {
    case info:
        pg = QPixmap(":/image/widgets_pic/radio_on.png").scaledToWidth(20, Qt::SmoothTransformation);
        break;
    case yunm:
        pg = QPixmap(":/image/widgets_pic/radio_on_yun.png").scaledToWidth(20, Qt::SmoothTransformation);
        break;
    case warn:
        pg = QPixmap(":/image/widgets_pic/radio_on_warn.png").scaledToWidth(20, Qt::SmoothTransformation);
        break;
    default:
        break;
    }

    if(!pg.isNull()) {
        p.drawPixmap(QRect(width()-pg.width(), 0, pg.width(), pg.height()), pg);
    }
}

void public_pushbuton_rt_check::resizeEvent(QResizeEvent *event)
{
    lds_toolbar_button::resizeEvent(event);
//    int w = qMin(this->width(), this->height()) * 2 / 3;
//    setIconSize(QSize(w, w));
}
