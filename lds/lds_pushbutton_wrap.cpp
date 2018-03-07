#include "lds_pushbutton_wrap.h"
#include <QStylePainter>
#include <QStyleOptionButton>
#include<QPainterPath>
#include <QTimeLine>
#include <QEventLoop>
#include <QTimer>
#include <QLabel>
#include <QMovie>
#include <QMouseEvent>
#include <QtDebug>
#include <QEventLoop>
#include <QMap>
#include "lds.h"


lds_pushbutton_wrap::lds_pushbutton_wrap(QWidget *parent) :
    QPushButton(parent)
{
    xp1 = 0;
    yp1 = 0;
    xp2 = 0;
    yp2 = 0;
    textflags = int(Qt::AlignCenter|Qt::TextWordWrap);

    do_longpress = new lds_record_do_longpress(this);
    connect(do_longpress, SIGNAL(timeout()), this, SIGNAL(timeout()));
}

lds_pushbutton_wrap::~lds_pushbutton_wrap()
{
}

void lds_pushbutton_wrap::paintEvent(QPaintEvent *)
{
    QStylePainter p(this);
    QString t;
    QStyleOptionButton o;
    QRect rect0 = this->rect().adjusted(xp1, yp1, xp2, yp2);
    QPixmap iconpix = this->icon().pixmap(iconSize());
    initStyleOption(&o);
    t = o.text;
    o.text = "";
    o.icon = QIcon();
    p.drawControl(QStyle::CE_PushButton, o);
    //
    if(false == iconpix.isNull()) {
        rect0.adjust(iconpix.width() + 5, 0, 0, 0);
        p.drawPixmap(QRect(xp1 + 5, yp1 + (rect0.height() - iconpix.height()) / 2, iconpix.width(), iconpix.height()), iconpix);
    }
    //
    if(b_text_right_bottom) {
        QStringList keyValue = t.split("\n");
        p.drawText(rect0, Qt::AlignLeft | Qt::AlignTop, keyValue.value(0));
        //        p.setPen(QColor("#ff650b"));
        p.drawText(rect0, Qt::AlignRight | Qt::AlignBottom, keyValue.value(1));

    } else {
        p.drawText(rect0, textflags, t);
    }

    int w = rect0.width();
    int h= rect0.height();
    int off = 15;
    ///
    if(show_top_right) {//三角
        QPainterPath path0;

        path0.addPolygon(QVector<QPointF>()
                         << QPointF(rect0.x() + w - off, rect0.y() + 0)
                         << QPointF(rect0.x() + w, rect0.y() + 0)
                         << QPointF(rect0.x() + w, rect0.y() + off)
                         ) ;
        p.fillPath(
                    path0,
                    QColor("#50c600")
                    );
    }
    ///~


    ///
    if(show_bottom_right) {//三角
        QPainterPath path1;
        path1.addPolygon(QVector<QPointF>()
                         << QPointF(rect0.x() + w, rect0.y() + h - off)
                         << QPointF(rect0.x() + w, rect0.y() + h)
                         << QPointF(rect0.x() + w - off, rect0.y() + h)
                         ) ;
        p.fillPath(
                    path1,
                    show_bottom_right_color
                    );
    }
    ///~

    ///
    if(b_enable_frame && isDefault()&&hasFocus() ){
        p.save();
        p.setBrush(Qt::NoBrush);
        p.setPen(QPen(QColor("crimson"), 3));
        p.drawRect(this->rect().adjusted(0,0,-3.0/2,-3.0/2));
        p.restore();
    }
    ///~

    ///
    if(!right_pixmap.isNull()) {
//        p.drawPixmap(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignRight | Qt::AlignVCenter, QSize(right_pixmap.width() * rect0.height() * 1.0 / right_pixmap.height() , rect0.height()) * 0.7, rect0), right_pixmap);
//        p.drawText(rect0, Qt::AlignRight | Qt::AlignVCenter, ">");
    }
    ///~
}

void lds_pushbutton_wrap:: show_flag_top_right(bool f)
{
    show_top_right = f;
    update();
}

void lds_pushbutton_wrap::show_flag_bottom_right(bool f, const QColor &show_bottom_right_color)
{
    show_bottom_right = f;
    this->show_bottom_right_color = show_bottom_right_color;
    update();
}

void lds_pushbutton_wrap::show_text_right_bottom(const QString &mainColor)
{
    Q_UNUSED(mainColor)

    xp1 = 3;
    yp1 = 3;
    xp2 = -3;
    yp2 = -3;

    b_text_right_bottom = true;
}

void lds_pushbutton_wrap::show_right_pixmap(const QPixmap &right_pixmap)
{
    this->right_pixmap = right_pixmap;
}

void lds_pushbutton_wrap::disableFrame()
{
    b_enable_frame = false;
}

QVariant lds_pushbutton_wrap::getData(int role, const QVariant &def)
{
    return role_value_map.value(role, def);
}

void lds_pushbutton_wrap::setData(int role, const QVariant &d)
{
    role_value_map.insert(role, d);
}

void lds_pushbutton_wrap::enableLongPress(bool flag)
{
    do_longpress->setEnable(flag);
}

void lds_pushbutton_wrap::mousePressEvent(QMouseEvent *event)
{
    QPushButton::mousePressEvent(event);
    do_longpress->start(event->pos());
}

void lds_pushbutton_wrap::mouseReleaseEvent(QMouseEvent *event)
{
    if(do_longpress->isprepared_stop()) {
        QPushButton::mouseReleaseEvent(event);
    }
}
