#include "lds_radiobutton_no_exclusive.h"


lds_radiobutton_no_exclusive::lds_radiobutton_no_exclusive(QWidget *parent) :
    QRadioButton(parent)
{
    this->setAutoExclusive(false);
    connect(this, SIGNAL(clicked(bool)),this,SLOT(tocheckstate(bool)));
}

void lds_radiobutton_no_exclusive::tochecked()
{
    this->setChecked(true);
}

void lds_radiobutton_no_exclusive::tounchecked()
{
    this->setChecked(false);

}

void lds_radiobutton_no_exclusive::tocheckstate(bool f)
{
    if(f)    emit signal_checked();
}
