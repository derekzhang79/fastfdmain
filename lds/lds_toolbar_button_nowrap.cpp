#include "lds_toolbar_button_nowrap.h"

lds_toolbar_button_nowrap::lds_toolbar_button_nowrap(QWidget *parent) :
    QPushButton(parent)
{
    this->setMinimumHeight(40);
}
QSize lds_toolbar_button_nowrap::sizeHint() const
{
    int menu_width=0;
    if(this->menu()){
        menu_width=20;
    }
    QSize size=QPushButton::sizeHint();
    size.setWidth(qMax(menu_width+60, size.width()) +20);

    return size;
}

void lds_toolbar_button_nowrap::updateText(const QString &text)
{
    setText(text);
}
