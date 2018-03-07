#ifndef W_SCR_DISH_WEIXIN_PAYBILL_H
#define W_SCR_DISH_WEIXIN_PAYBILL_H

#include <QDialog>
#include "ui_w_scr_dish_weixin_paybill.h"

namespace Ui {
class w_scr_dish_weixin_paybill;
}

class w_scr_dish_weixin_paybill : public QDialog
{
    Q_OBJECT

public:
    explicit w_scr_dish_weixin_paybill(QWidget *parent);
    ~w_scr_dish_weixin_paybill();
    Ui::w_scr_dish_weixin_paybill *ui;
};

#endif // W_SCR_DISH_WEIXIN_PAYBILL_H
