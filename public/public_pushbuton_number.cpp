#include "public_pushbuton_number.h"
#include <QStylePainter>
#include <QStyleOptionButton>
#include "lds.h"

public_pushbuton_number::public_pushbuton_number(QWidget *parent) :
    QPushButton(parent)
{
}

void public_pushbuton_number::setNumber(double number)
{
    _numberStr = lds::septNumber(number);
    QPushButton::setText(_text + ":" + _numberStr);
}

void public_pushbuton_number::setNumberUnit(const QString &unit)
{
    _unit = unit;
}

double public_pushbuton_number::number()
{
    return lds::groupSeparatorStr2v(_numberStr);
}

void public_pushbuton_number::setText(const QString &text)
{
    _text = text;
    QPushButton::setText(_text + ":" + _numberStr);
}

void public_pushbuton_number::paintEvent(QPaintEvent *e)
{
    QStylePainter p(this);
    QStyleOptionButton o;
    initStyleOption(&o);
    o.text = "";
    p.drawControl(QStyle::CE_PushButton, o);

    QRect r = this->rect();
    r.adjust(5, 5, -5, -5);
    p.drawText(r, int(Qt::AlignVCenter | Qt::AlignLeft), _text + ":");

    QFontMetrics fm(p.font());
//    QFont f = p.font();
//    f.setBold(true);
//    p.setFont(f);
    r.adjust(fm.width(_text + ":"), 0, -fm.width(_unit), 0);
    p.setPen(QColor("#ff650b"));
    p.drawText(r, int(Qt::AlignVCenter | Qt::AlignRight), _numberStr);

    p.drawText(QRect(r.left() + r.width(), r.top(), fm.width(_unit), r.height()), _unit);
}

