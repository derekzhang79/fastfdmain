#include "w_scr_dish_weixin_paybill.h"

w_scr_dish_weixin_paybill::w_scr_dish_weixin_paybill(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_scr_dish_weixin_paybill)
{
    ui->setupUi(this);
}

w_scr_dish_weixin_paybill::~w_scr_dish_weixin_paybill()
{
    delete ui;
}
