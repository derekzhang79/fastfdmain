#include "public_pushbutton_machine_transaction.h"

public_pushbutton_machine_transaction::public_pushbutton_machine_transaction(QWidget *parent) :
    public_pushbutton_machine(parent)
{
    this->setFocusPolicy(Qt::NoFocus);
}

void public_pushbutton_machine_transaction::mouseReleaseEvent(QMouseEvent *e)
{
    QPushButton::mouseReleaseEvent(e);
}
