#ifndef FOUNDING_COOK_H
#define FOUNDING_COOK_H

#include <QDialog>
#include "frontviewdialog_virtual.h"
#include "ui_founding_cook.h"
#include "lds_roundeddialog_rect.h"

class w_scr_dish_restaurant_div_pop_only_table;
class lds_model_sqltablemodel;
class QDataWidgetMapper;
namespace Ui {
class founding_cook;
}
class w_scr_dish_restaurant_dialog;
class founding_cook : public QDialog
{
    Q_OBJECT
public:
    enum closeType{
        Rejected = QDialog::Rejected,
        Accepted = QDialog::Accepted,

        EXIT = 1,
        DISH_ORDER = 2,
        TABLE_OPEN = 4,
        TABLE_CANCEL = 8,

        DISH_APPEND = 16,
        SAVE = 32,
        PRE_PAYBILL = 64,

        closeTypeCancel = EXIT | TABLE_CANCEL | SAVE
    };
    Q_DECLARE_FLAGS(closeTypes, closeType)

    static void pre_print(QWidget *parent, const QString&ch_tableno);
    static bool payBill(QWidget *parent,
                        const QString &ch_billno, //付款单号
                        bool ispaysomedish);
public:
     founding_cook(const QString &ch_billnos, const QString &ch_tableno, const QString &ch_billno, int sqr, QWidget *parent = 0);
    ~founding_cook();
     int get_sqr_by_curindex();
     QString orderSn;
     Ui::founding_cook *ui;
     void updateVch_memo(const QString &vch_memo);
     void hideOpenTable();
private:
    void update_9Box(const QString &ch_billnos, const QString &ch_tableno, const QString &ch_billno, int sqr);
private:
    void initData();
private slots:
    void on_pushButton_14_clicked();//取消

    void on_pushButton_9_clicked();//选择操作员

    void on_pushButton_13_clicked();//点单

    void on_pushButton_open_clicked();//开台

    void toed_exit();
    void toed_order();
    void toed_pay();
    void toed_pay_somedish();
    void toed_save();
    void toed_inputonline();
    void toBillDiv(const QModelIndex &index);
    void toIsShowDiv();
    void toinlne();
    void toreprint();
    void tobarclear();
private:
    bool saveData(bool inf = true);
    void initData(QString ch_billno, int sqr);

    w_scr_dish_restaurant_div_pop_only_table *get9Box();
private:
    w_scr_dish_restaurant_div_pop_only_table *_9Box;

    lds_model_sqltablemodel *tablemodel;
    QDataWidgetMapper *mapper;

    QString ch_tableno;
    QString ch_billnos;
};

#endif // FOUNDING_COOK_H
