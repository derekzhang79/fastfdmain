#include "lds_checkbox.h"

lds_CheckBox::lds_CheckBox(QWidget *parent) :
    lds_checkbox_wrap(parent)
{
    connect(this, SIGNAL(toggled(bool)),this,SLOT(toToggled(bool)));
    connect(this, SIGNAL(clicked(bool)),this,SLOT(toClicked(bool)));
}

void lds_CheckBox::setChecked1(QString YN)
{
    setChecked(YN=="Y");
}

QString lds_CheckBox::isChecked1()
{
    return isChecked()? "Y":"N";
}

void lds_CheckBox::toToggled(bool f)
{
    emit toggled(f?"Y":"N");
}

void lds_CheckBox::toClicked(bool f)
{
    emit clicked(f?"Y":"N");
}
