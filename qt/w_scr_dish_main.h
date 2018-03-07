#ifndef W_SCR_DISH_MAIN_H
#define W_SCR_DISH_MAIN_H

#include <QDialog>
#include "fexpandmain_model_sqltablemodel.h"
#include "lds_dialog.h"
#include "lds_roundeddialog_rect.h"
#include <QTableView>
#include <QPointer>
#include "w_scr_dish_main_pay_info.h"
#include <QPointer>
#include "lds_tableview_virtual.h"
#include "ftableview_standmodel_sql.h"

class ftableview_standmodel_virtual;
class QFrame;
class lds_roundeddialog_rect_align;
class w_scr_dish_pay;
class ftableview;
class fexpandmain_tableview;
class QLabel;
class fexpandmain_dish_btn;
class lds_lineedit;
class public_pushbutton_pay;
class public_pushbuton_rt_check;

class w_scr_dish_main : public lds_roundeddialog_rect0
{
    Q_OBJECT
public:
    static const QString Q000;
    enum windowType{
        windowTypeKuaiCan,
        windowTypeZhongCan,
    };

    //paybill 效果 结算后,本单u_table ,u_orderdish,checkout_master,赋予ch_payno
    //                    分单结算, 若是全部付款,则分单付款的同时,留下的空的桌台也要付款
    //                    快餐,暂时不管

    static bool separatePaybill(QWidget *parent,
                                fexpandmain_model_sqltablemodel_data *tablemodel,
                                const QList<int> &rowListAsc,
                                bool table_pay = false);

    static int paysomedishGetRows(QWidget *parent, fexpandmain_model_sqltablemodel_data *orderdish_mode, QList<int> &rowListAsc, bool preprint );
    static void initLogo(QLabel *label_logo ,QLabel *label_text);

    QString cur_ch_billno();
protected:
    explicit w_scr_dish_main(const QString &ch_tableno,  const QString &ch_billno, windowType window_type, QWidget *parent = 0);
protected:
    void initLogo();
    void torefresh_dish();
    void torefresh_dish_sql();
    void torefresh_control(const QString &ch_billno, bool forQ000 = false);
    void torefresh_control_Q000();

    int autoDiscount(QString &discount_desc, int &int_discount_value);

protected slots:
    void refreshChBillno();

    void toAllbillcancel();
    void tochange();//换菜
    void toquitdish();
    void toExitbill();

    void todetail();
    void topresent();
    void tomhangpay();
    void toTakebill();
    void toTakeweight();

    void topaybill();
    void topaybillSelectPayMode();
    void toMemberPayBill();
    void toTuichu();
    void toMRead();
    void towaimai();
    void towaimaipay();
    void toweixinwaimai();
    void toweixinquitbill();
    void toweixinbill_import();

    void tosysyingyedanju();
    void toLanguageSwitch();
    void toLanguageSet();
    void tosysyingyeqingkuang();
    void tosysjiaoban();
    void tosyschongzhi();
    void tosysqukuan();
    void tosysxiugaimima();
    void tosysswitchback();
    void tosyskaiqianxiang();
    void tochangyongcai();
    void tosyssuoping();
    void tosysguqing();
    void toRestaurant();
    void toScanbar();
    void tosub();
    void toadd();
    void tochangenum();
    void todishup();
    void todishdown();

    void showBianMa();

    void toLongPressUpdateDishImage(const QModelIndex &index);
    void toLongPressUpdateDishImageAdd();
    void toLongPressTypeSet();

    void to_suit_fold(const QModelIndex &index);

    ///菜品的修改删除新增
    void toadddish(const QString &ch_dishno, const QString &vch_print_memo, float num_price, int int_discount, const bool time_promotion);
    void todishclick(const QString &ch_dishno);
    bool to_dish_del(const int row, QString *errstring = 0);
    bool to_dish_del(const QModelIndex &index);

    bool add_dish_by_scanbar(const QString &pay_barcode);

    ///中餐

    void toprint();//落单

    void tocuicaipiece();//单个催菜
    void tocuicaitable();//整桌
    void tocuicaitype();//类别

    void toguaqi();

    void tojiaodanpiece();
    void tojiaodantable();
    void tojiaodantype();

    void toDishTmp();
    void toPaySomeDish();
    void toWaiDai();
    void toTelComing(const QString &telNo = "");

protected:
    bool paybill();
    void removeAllInsertDish();

    ///~中餐
    void todish_fill_data(lds_query &query, const QDateTime &dt, const QString &vch_print_memo = "");
    bool get_num_price_when_dishclick(const QString &ch_dishno, float &num_price, int &int_discount, bool &time_promotion);
    bool can_promotion();
    void dish_add(int f, bool autoScale);//-1 1
    //菜品的修改删除新增
protected slots:
    void to_update_num_total(const QString &n, const QString &t);
    void update_pay_info_set_empty();
    void to_payfor_info(const QString &ch_payno, int paytype);
    void to_payfor_info_ch_billno(const QString &ch_billno, int paytype);
    void toSwitchSkin();
protected:
    void update_type_subtype_view();
    void showWidget(QWidget *w);
    void hideWidget(QWidget *w);

    ///中餐
    bool printdish_tran_saction(QString &errstring);//落单
    bool try_commit_data_update_num_cost(QString &errstring);
    void loadSn(const QString & orderSn);
    bool update_num_cost(QString &errstring);
    ///~中餐

    void keyPressEvent(QKeyEvent *e);
protected:
    QString ch_tableno;
    QString ch_billno;//在分单付款时,起到还原的功能
    lds_bool_default_false promtion_not_pop;//读卡下次不弹出
protected:
    void loadControl(
            ftableview *tableView_dish,
            ftableview *tableView_type,
            ftableview *tableView_subtype,
            fexpandmain_tableview *tableView,

            QLabel *label_logo,
            QLabel *label_text,
            QLabel *label_num,
            QPushButton *label_total,
            lds_lineedit *lineEdit_scan,
            public_pushbutton_pay *pushButton_pay
            );
protected slots:
    virtual void menuhide();
    virtual void updateTakebillState();
    virtual void updateTakebillDesc();
    virtual void retranslateView();
    virtual void loadData();
    virtual void updatequitDish(const QModelIndex &/*current*/, const QModelIndex &/*previous*/) ;
    virtual void updatememberchecked();
protected:
    QPointer<ftableview> tableView_dish;
    QPointer<ftableview> tableView_type;
    QPointer<ftableview> tableView_subtype;

    QPointer<QLabel> label_logo;
    QPointer<QLabel> label_text;
    QPointer<QLabel> label_num;
    QPointer<QPushButton> label_total;
    QPointer<lds_lineedit> lineEdit_scan;
    QPointer<public_pushbutton_pay> pushButton_pay;

    QPointer<fexpandmain_tableview> tableView;
    QPointer<w_scr_dish_main_pay_info> pay_info_widget;

    windowType window_type;

    fexpandmain_model_sqltablemodel_data *tablemodel;
    ftableview_standmodel_sql *model_type;
    ftableview_standmodel_sql *model_subtype;
    ftableview_standmodel_sql *model_dish;
};

class w_scr_dish_main_auto_exit_multiSelection{
public:
    w_scr_dish_main_auto_exit_multiSelection(QTableView *tableview){
        row = tableview->currentIndex().row();
        this->tableview = tableview;
        this->tableview->setSelectionMode(QTableView::MultiSelection);
    }
    ~w_scr_dish_main_auto_exit_multiSelection(){
        this->tableview->setSelectionMode(QTableView::ExtendedSelection);
        this->tableview->clearSelection();
        this->tableview->selectRow(row);
    }
private:
    QTableView *tableview;
    int row;
};
#endif // W_SCR_DISH_MAIN_H
