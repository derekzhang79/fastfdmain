#include "lds_toolbar_button.h"
#include <QVariant>
#include <QtDebug>

lds_toolbar_button::lds_toolbar_button(QWidget *parent) :lds_pushbutton_wrap(parent)
{
    this->setMinimumHeight(30);
}

lds_toolbar_button::lds_toolbar_button(const QString &outer_stylesheet, QWidget *parent)
    :lds_pushbutton_wrap(parent)
{
    this->setProperty("outer_stylesheet", outer_stylesheet);
    this->setMinimumHeight(30);
}

lds_toolbar_button::~lds_toolbar_button()
{
}

QSize lds_toolbar_button::sizeHint() const
{
    int menu_width=0;
    if(this->menu()){
        menu_width=20;
    }
    QSize size=QPushButton::sizeHint();
    size.setWidth(qMax(menu_width+60, size.width()) +20);

    return size;
}

void lds_toolbar_button::updateText(const QString &text)
{
    setText(text);
}
