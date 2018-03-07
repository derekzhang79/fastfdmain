#include "lds_radiobutton_wrap.h"
#include <QPainter>
#include <QStylePainter>
#include <QStyleOptionButton>
#include <QMouseEvent>
#include <QtDebug>

lds_radiobutton_wrap::lds_radiobutton_wrap(QWidget *parent) :
    QRadioButton(parent)
{
}

void lds_radiobutton_wrap::paintEvent(QPaintEvent *e)
{
    //1
    QStylePainter p(this);
    QStyleOptionButton opt;
    initStyleOption(&opt);
    //2
    QString text = opt.text;
    opt.text.clear();
    QBrush brush = p.brush();

    //3
    p.drawControl(QStyle::CE_RadioButton, opt);

    //4
    p.setBrush(brush);
    p.drawText(textrect(), int(Qt::TextWordWrap|Qt::AlignLeft|Qt::AlignVCenter),
               text);
}

bool lds_radiobutton_wrap::hitButton(const QPoint &p) const
{
    return textrect().contains(p);
}

QRect lds_radiobutton_wrap::textrect() const
{
    return this->rect().adjusted(2*style()->pixelMetric(QStyle::PM_RadioButtonLabelSpacing)+this->iconSize().width(), 0, 0, 0);
}
