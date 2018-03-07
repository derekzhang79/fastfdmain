#include "lds_label_text.h"
#include <QPainter>
#include "lds.h"

lds_label_text::lds_label_text(QWidget *parent)
    : QWidget(parent)
{
    if(lds::sysconf->value("system_setting/skin_name", "obsidian").toString() == "obsidian") {
        bgColor = QColor("#2b2b2b");
    } else {
        bgColor = QColor("#2291a3");
    }

    QFont f = this->font();
    f.setPointSize(17);
    this->setFont(f);
}

void lds_label_text::setLabelText(const QString &labelText)
{
    this->labelText = labelText;
}

void lds_label_text::setText(const QString &text)
{
    this->txt = text;
}

void lds_label_text::setValue(double value)
{
    this->txt = QString::number(value);
}

void lds_label_text::setBgColor(const QColor &bgColor)
{
    this->bgColor = bgColor;
}

double lds_label_text::value()
{
    return this->txt.toDouble();
}

QString lds_label_text::text()
{
    return txt;
}

void lds_label_text::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QRect rect0 = this->rect().adjusted(10, 10, -10, -10);
    painter.fillRect(this->rect(), bgColor);
    painter.setPen(QPen(QColor("white")));

    painter.drawText(rect0, Qt::AlignLeft | Qt::AlignVCenter, labelText);
    painter.drawText(rect0, Qt::AlignRight | Qt::AlignVCenter, text());
}
