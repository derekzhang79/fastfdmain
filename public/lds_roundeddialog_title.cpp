#include "lds_roundeddialog_title.h"
#include <QPainter>
#include "lds.h"
#include "saas_pay_detail.h"
#include <QtDebug>

lds_roundeddialog_title::lds_roundeddialog_title(QWidget *parent)
    : QWidget(parent){
    titleColor = 0;
    isbottomBorder = false;
}

lds_roundeddialog_title::lds_roundeddialog_title(QWidget *parent, const QString &title, QColor *titleColor)
    : QWidget(parent){
    setTitle(title);
    this->titleColor = titleColor;
    isbottomBorder = false;

}
void lds_roundeddialog_title::setTitle(const QString &title)
{
    this->title = title;
    update();
}

void lds_roundeddialog_title::setTitleColor(QColor *titleColor)
{
    this->titleColor = titleColor;
    update();
}

void lds_roundeddialog_title::enableBottomBorder()
{
    isbottomBorder = true;
}

void lds_roundeddialog_title::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    QRect rect0 = this->rect();
    //bg RIGHT
    if(titleColor){
        p.fillRect(rect0, *titleColor);
        {
            QPainterPath path;
            path.moveTo(this->width(), 0);
            path.lineTo(this->width() - 70, 0);
            path.lineTo(this->width() - 30, this->height());
            path.closeSubpath();
            p.fillPath(path, titleColor->light(90));
        }

        {
            QPainterPath path;
            path.moveTo(this->width(), 0);
            path.lineTo(this->width() - 30, this->height());
            path.lineTo(this->width(), this->height());
            path.closeSubpath();
            p.fillPath(path, titleColor->dark(90));
        }
    }

    //top left
    saas_pay_detail::paintLeftTop(this, rect0, &p);
    //text
    if(title.count() > 0) {
        QFont f = p.font();
        QFontMetrics fm(f);
        qreal fmrate =  fm.height() *1.0 / fm.width(title);
        f.setPixelSize(qMin(rect0.height(), int(fmrate * rect0.width()))  - 6 );
        p.setFont(f);
    }
    p.setPen(QColor("white"));
    p.drawText(rect0, Qt::AlignCenter, title);

    if(isbottomBorder) {
        p.fillRect(rect0.x() , rect0.y() + rect0.height() - 2, rect0.width(), 2,
                   "obsidian" == lds::sysconf->value("system_setting/skin_name", "obsidian").toString() ?  QColor("black") :  QColor("darkgrey")
                                                                                                           );
    }
}
