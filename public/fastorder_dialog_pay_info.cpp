#include "fastorder_dialog_pay_info.h"
#include "ui_fastorder_dialog_pay_info.h"

fastorder_dialog_pay_info::fastorder_dialog_pay_info(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::fastorder_dialog_pay_info)
{
    ui->setupUi(this);
}

fastorder_dialog_pay_info::~fastorder_dialog_pay_info()
{
    delete ui;
}
