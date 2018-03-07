#ifndef W_SCR_DISH_WEIXINBILL_IMPORT_H
#define W_SCR_DISH_WEIXINBILL_IMPORT_H

#include <QWidget>
#include "frontviewdialog_virtual.h"
#include "w_scr_dish_weixin_getbill.h"
#include "w_scr_dish.h"

class lds_model_sqltablemodel;
class Ui_w_scr_dish_weixinwaimai_dialog;
class w_scr_dish_weixinbill_import : public QDialog
{
    Q_OBJECT

public:
    w_scr_dish_weixinbill_import(QWidget *parent);
    w_scr_dish::WEIXIN_DATA wdata;
private slots:
    void totake_waimai();
    void toexit();
    void tosearch(int row=-1);
    void torefresh();
    void toprint();
private:
    Ui_w_scr_dish_weixinwaimai_dialog *ui;
    lds_model_sqltablemodel *tablemodel;
    lds_model_sqltablemodel *tablemodel_2;
};

#endif // W_SCR_DISH_WEIXINBILL_IMPORT_H
