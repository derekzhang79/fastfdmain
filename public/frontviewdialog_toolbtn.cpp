#include "frontviewdialog_toolbtn.h"
#include <QPainter>
#include <QtDebug>
#include <QMouseEvent>
#include <QStylePainter>
#include <QStyleOptionButton>
#include <QLinearGradient>
#include "lds.h"
#include "public_sql.h"
#include "n_func.h"


frontViewDialog_toolbtn::frontViewDialog_toolbtn(QWidget *parent) :
    QRadioButton(parent),
    is_use_pixmap(false)
{
    isdollar = false;
    num = 1;
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    //    this->setFocusPolicy(Qt::NoFocus);
}

void frontViewDialog_toolbtn::set_btn_property(const QString &property)
{
    this->setProperty("outer_stylesheet", property);
}

void frontViewDialog_toolbtn::setNum(float n)
{
    num = n;
    update();
}

float frontViewDialog_toolbtn::getNum()
{
    return num;
}

void frontViewDialog_toolbtn::mousePressEvent(QMouseEvent *e)
{
    emit thispressed(this);
    QRadioButton::mousePressEvent(e);
}

void frontViewDialog_toolbtn::mouseDoubleClickEvent(QMouseEvent *e)
{
    //    QRadioButton::mouseDoubleClickEvent(e);
}

void frontViewDialog_toolbtn::mouseReleaseEvent(QMouseEvent *e)
{
    QRadioButton::mouseReleaseEvent(e);
    if(this->rect().contains(e->pos())){
        emit thisclicked(index);
    }
}

void frontViewDialog_toolbtn::paintEvent(QPaintEvent *e)
{
    //1
    QStylePainter p(this);
    QStyleOptionButton opt;
    initStyleOption(&opt);
    _icon_size=opt.iconSize;
    _icon=opt.icon;
    opt.icon=QIcon();
    _text=opt.text;
    opt.text.clear();
    QStyle::State org_state = opt.state;
    opt.state = QStyle::State_Enabled;
    if(pay_unit_prefix.isEmpty()) {
        pay_unit_prefix = n_func::f_get_sysparm("pay_unit_prefix", " ");
    }
    //2
    //    p.setRenderHint(QPainter::Antialiasing);
    QRect parent_rect=QRect(2,2,this->width()-4,this->height()-4);
    QRect pixmap_rect=QStyle::alignedRect(lds::SYS_LAYOUT_DIRECTION, Qt::AlignHCenter|Qt::AlignTop,_icon_size, parent_rect);
    QFont f = this->font();
    setPenColor(&p, Qt::red);
    p.drawControl(QStyle::CE_PushButton, opt);

    if(is_use_pixmap){
        _icon_size=QSize(parent_rect.width(), 55);//p.size();
        if(!_icon.isNull()) {
            p.drawPixmap(pixmap_rect, _icon.pixmap(_icon_size).scaled(_icon_size, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
    }
    p.setRenderHint(QPainter::Antialiasing);

    //3
    QRect text_rect;
    if(is_use_pixmap){
        text_rect=QRect(3, parent_rect.top()+_icon_size.height()+3, this->width()-6, this->height()-_icon_size.height()-6);
    } else {
        text_rect=QRect(3, parent_rect.top()+3, this->width()-6, this->height()-6);
    }

    setPenColor(&p, QColor("black"));
    if(index >= 0 &&  org_state & QStyle::State_On){
        QPixmap pg = QPixmap(":/image/widgets_pic/radio_on.png").scaledToWidth(20, Qt::SmoothTransformation);
        p.drawPixmap(QRect(text_rect.x()+text_rect.width()-pg.width(), text_rect.y(), pg.width(), pg.height()), pg);
    }


    f.setPointSize(12);
    p.setFont(f);
    QStringList plist = _text.split("\n");
    if(plist.count() == 1){
        p.drawText(text_rect, int(Qt::AlignCenter|Qt::TextWordWrap), plist.value(0));
    } else {
        QString name_x_num;
        if(num == 1) {
            name_x_num = QString("%1").arg(plist.value(0));
        } else {
            name_x_num = QString("%1x%2").arg(plist.value(0)).arg(num);
        }

        p.drawText(QRect(text_rect.x(), text_rect.y(),text_rect.width(), text_rect.height()/2),  int(Qt::AlignBottom|Qt::AlignHCenter|Qt::TextWordWrap), name_x_num);
        setPenColor(&p, QColor("forestgreen"));

        p.drawText(QRect(text_rect.x(), text_rect.y()+text_rect.height()/2,text_rect.width(), text_rect.height()/2),  int(Qt::AlignTop|Qt::AlignHCenter|Qt::TextWordWrap),
                   (isdollar?pay_unit_prefix : "")+plist.value(1));
    }
}

void frontViewDialog_toolbtn::setPenColor(QStylePainter *p, QColor c)
{
    if(this->isEnabled())p->setPen(c);
    else p->setPen(QColor("grey"));
}

