#ifndef W_BT_DISH_KITCHENPRINTER_H
#define W_BT_DISH_KITCHENPRINTER_H

#include <QDialog>
#include<QSqlRelationalTableModel>
#include <QQueue>
#include <QPair>
#include "public_mainprinter.h"
#include <QDateTime>
class Printer_POS;
class Printer_POS_NET;
class Printer_POS_VIDPID;
class Ui_w_bt_dish_Dialog;
class lds_tableView;
//厨打管理的checkbox和combobox
#include "lds_tableview_delegate_check.h"
#include <QSqlRelationalTableModel>
//厨房打印机管理
class Ui_w_bt_dish_unitset_Dialog;
class lds_model_sqltablemodel;
class lds_tableView;
class w_bt_dish_Kitchenprinter_printermanager : public QDialog
{
    Q_OBJECT
public:
    explicit w_bt_dish_Kitchenprinter_printermanager(QWidget *parent = 0);
public slots:
    void tonew();
    void tochange();
    void todel();
    void torefresh();
    void toexit();
    void toprinttest();
    void tokitchenviewtest();
    void datachanged(const QModelIndex &, const QModelIndex &);
private:
    Ui_w_bt_dish_unitset_Dialog *ui;
    lds_model_sqltablemodel *tablemodel;
    bool dchange;
    QMap<QString, QVariant> ch_areano_keyValueMap;
};



#endif // W_BT_DISH_KITCHENPRINTER_H
