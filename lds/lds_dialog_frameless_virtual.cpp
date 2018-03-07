#include "lds_dialog_frameless_virtual.h"
#include <QLabel>
#include <QtDebug>
#include "lds.h"

lds_dialog_frameless_virtual::lds_dialog_frameless_virtual(QWidget *parent) :
    QDialog(parent)
{
    lds::setwflagFrameless(this);
}

void lds_dialog_frameless_virtual::setWindowTitle(const QString &title)
{
    QDialog::setWindowTitle(title);
    if(label_title == 0) {
    } else {
        label_title->setText(title);
    }
}
