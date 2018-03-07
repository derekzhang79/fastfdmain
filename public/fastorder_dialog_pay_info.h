#ifndef FASTORDER_DIALOG_PAY_INFO_H
#define FASTORDER_DIALOG_PAY_INFO_H

#include <QDialog>

namespace Ui {
class fastorder_dialog_pay_info;
}

class fastorder_dialog_pay_info : public QDialog
{
    Q_OBJECT

public:
    explicit fastorder_dialog_pay_info(QWidget *parent = 0);
    ~fastorder_dialog_pay_info();

private:
    Ui::fastorder_dialog_pay_info *ui;
};

#endif // FASTORDER_DIALOG_PAY_INFO_H
