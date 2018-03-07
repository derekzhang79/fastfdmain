#ifndef PUBLIC_PUSHBUTTON_MACHINE_TRANSACTION_H
#define PUBLIC_PUSHBUTTON_MACHINE_TRANSACTION_H

#include "public_pushbutton_machine.h"

class public_pushbutton_machine_transaction : public public_pushbutton_machine
{
    Q_OBJECT
public:
    explicit public_pushbutton_machine_transaction(QWidget *parent = 0);

protected:
    virtual  void mouseReleaseEvent(QMouseEvent *e);
};

#endif // PUBLIC_PUSHBUTTON_MACHINE_TRANSACTION_H
