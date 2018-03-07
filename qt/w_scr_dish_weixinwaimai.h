#ifndef W_SCR_DISH_WEIXINWAIMAI_H
#define W_SCR_DISH_WEIXINWAIMAI_H

#include <QDialog>
#include <QStandardItemModel>
#include "frontviewdialog_virtual.h"

class lds_model_sqltablemodel;
class Ui_w_scr_dish_weixinwaimai_dialog;
class w_scr_dish_weixinwaimai:public QDialog{
    Q_OBJECT
public:
    w_scr_dish_weixinwaimai(QWidget *parent = 0);
    static QString getweixinorderStatename(const QString &orderState);
    static QString getweixinorderTypename(const QString &orderType);
private slots:
    void torefuse();//拒绝订单
    void totake_waimai();
    void toexit();
    void tosearch(int row=-1);
    void torefresh();
    void toprintbill();
private:
    Ui_w_scr_dish_weixinwaimai_dialog *ui;
    lds_model_sqltablemodel *tablemodel;
    lds_model_sqltablemodel *tablemodel_2;
};

#endif // W_SCR_DISH_WEIXINWAIMAI_H
