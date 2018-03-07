#include "public_toolbuton_wrap.h"
#include <QStylePainter>
#include <QStyleOptionToolButton>
#include <QFontMetrics>
#include <QtDebug>

public_toolbuton_wrap::public_toolbuton_wrap(QWidget *parent) :
    QToolButton(parent)
{
    xp1 = 70;
    yp1 = 0;
    xp2 = 0;
    yp2 = 0;

    textfont_size = 12;
    textfont_weight = QFont::Normal;
    textflags =int(Qt::TextWordWrap|Qt::AlignLeft|Qt::AlignVCenter);
}

void public_toolbuton_wrap::paintEvent(QPaintEvent *)
{
    QStylePainter p(this);
    QStyleOptionToolButton o;
    QString ptext;
    initStyleOption(&o);
    ptext = o.text;
    o.text = "";
    QBrush brush = p.brush();
    p.drawComplexControl(QStyle::CC_ToolButton, o);

    QFont f = this->font();
    f.setPointSize(textfont_size);
    f.setWeight(textfont_weight);
    p.setFont(f);
    p.setBrush(brush);
    p.drawText(o.rect.adjusted(xp1, yp1, xp2, yp2), textflags, ptext);
}
