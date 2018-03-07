#include "fastorder_dialog_saas_1366x768.h"
#include "ui_fastorder_dialog_saas_1366x768.h"

fastorder_dialog_saas_1366x768::fastorder_dialog_saas_1366x768(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::fastorder_dialog_saas_1366x768)
{
    ui->setupUi(this);
}

fastorder_dialog_saas_1366x768::~fastorder_dialog_saas_1366x768()
{
    delete ui;
}
