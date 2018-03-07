#include "lds_label_doublespinbox.h"
#include <QPainter>
#include "lds.h"
#include <QtDebug>

lds_label_doublespinbox::lds_label_doublespinbox(QWidget *parent)
    : QWidget(parent)
{
    if(lds::sysconf->value("system_setting/skin_name", "obsidian").toString() == "obsidian") {
        p_data.bgColor = QColor("#2b2b2b");
    } else {
        p_data.bgColor = QColor("#2291a3");
    }
}

void lds_label_doublespinbox::setDecimals(int decimals)
{
    p_data.decimals = decimals;
}

int lds_label_doublespinbox::decimals()
{
    return p_data.decimals;
}

void lds_label_doublespinbox::setValue(double v)
{
    p_data.edit_value.clear();
    p_data.v = v;
    selectAll();
    update();

    emit valueChanged(value());
}

void lds_label_doublespinbox::setValue(const QString &v)
{
    QString save_text = p_data.edit_value;
    if(isselectAll()) {
        p_data.edit_value.clear();
        selectCLear();
    }

    p_data.edit_value += v;
    if(p_data.edit_value.count(".") > 2) {
        p_data.edit_value = save_text;
    }
    if(p_data.edit_value.contains(".")
            && p_data.edit_value.length() - p_data.edit_value.indexOf(".") > (p_data.decimals + 1)/*00.*/) {
        p_data.edit_value = save_text;
    }
    p_data.v = p_data.edit_value.toDouble();
    update();

    emit valueChanged(value());
}

void lds_label_doublespinbox::setLabelText(const QString &labelText)
{
    p_data.labelText = labelText;
}

void lds_label_doublespinbox::setBgColor(const QColor &bgColor)
{
    p_data.bgColor = bgColor;
}

double lds_label_doublespinbox::value()
{
    return p_data.value();
}

double lds_label_doublespinbox::value_if_prefix(double else_value)
{
    if(prefix().length() > 0) {
        return else_value;
    }
    return value();
}

QString lds_label_doublespinbox::text()
{
    int f = 1;
    if(bShowNegativeValue) f = -1;
    QString retv;
    if(p_data.edit_value.isEmpty())
        retv = p_data.prefix+lds::septNumber(f * p_data.v, p_data.decimals);
    else
        retv = p_data.prefix+p_data.edit_value;

    return retv;
}

void lds_label_doublespinbox::enableSelectAll()
{
    p_data.enableSelectAll = true;
}

void lds_label_doublespinbox::selectAll()
{
    if(p_data.enableSelectAll)
        p_data.isselectAll = true;
}

void lds_label_doublespinbox::selectCLear()
{
    if(p_data.enableSelectAll)
        p_data.isselectAll = false;
}

bool lds_label_doublespinbox::isselectAll()
{
    return p_data.isselectAll;
}

void lds_label_doublespinbox::enableClickable()
{
    isclickable = true;
}

void lds_label_doublespinbox::setTipText(const QString &tip_text)
{
    this->tip_text = tip_text;
}

void lds_label_doublespinbox::enableShowNegativeValue()
{
    bShowNegativeValue = true;
}

QString lds_label_doublespinbox::falseText()
{
    return text();
}

void lds_label_doublespinbox::setPrefix(const QString &prefix)
{
    p_data.prefix = prefix;
}

QString lds_label_doublespinbox::prefix()
{
    return p_data.prefix;
}

void lds_label_doublespinbox::clear()
{
    p_data.v = 0;
    p_data.prefix.clear();
    p_data.edit_value.clear();
    update();
}

void lds_label_doublespinbox::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    int borderPix = 10;
    QRect rect0 = this->rect().adjusted(borderPix, borderPix, -borderPix, -borderPix);
    if(ispressed) {
        painter.fillRect(this->rect(), QColor("#ff650b"));
    } else {
        if(isEnabled())
            painter.fillRect(this->rect(), p_data.bgColor);
        else
            painter.fillRect(this->rect(), QColor("dimgrey"));
    }
    painter.setPen(QPen(QColor("white")));

    QFont f = painter.font();
    f.setPointSize(17);
    painter.setFont(f);

    painter.drawText(rect0, Qt::AlignLeft | Qt::AlignVCenter, p_data.labelText);

    if(isselectAll()) {
        f.setUnderline(true);
        painter.setFont(f);
    }
    painter.drawText(rect0, Qt::AlignRight | Qt::AlignVCenter, text());

    f.setPointSize(8);
    painter.setFont(f);
    painter.drawText(this->rect().adjusted(0, 0, -borderPix, 0), Qt::AlignRight | Qt::AlignBottom, tip_text);
}

void lds_label_doublespinbox::mousePressEvent(QMouseEvent *e)
{
    QWidget::mousePressEvent(e);
    //
    if(false == isclickable)  return;
    ispressed = true;
    update();
}

void lds_label_doublespinbox::mouseReleaseEvent(QMouseEvent *e)
{
    QWidget::mouseReleaseEvent(e);
    //
    if(false == isclickable)  return;
    ispressed = false;
    update();
    emit signalClicked();
}


lds_label_doublespinbox_data::lds_label_doublespinbox_data()
{
    v = 0;
    decimals = 2;
    isselectAll = false;
    enableSelectAll = false;
}

void lds_label_doublespinbox_data::setValue(double value)
{
    v = value;
}

double lds_label_doublespinbox_data::value()
{
    return QString::number(v, 'f', decimals).toDouble();
}

void lds_label_doublespinbox_data::setDecimals(int decimals)
{
    this->decimals = decimals;
}

QString lds_label_doublespinbox_data::text()
{
    return QString::number(v, 'f', decimals);
}
