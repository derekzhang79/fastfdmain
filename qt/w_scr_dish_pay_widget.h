#ifndef W_SCR_DISH_PAY_WIDGET_H
#define W_SCR_DISH_PAY_WIDGET_H

#include <QWidget>
#include <QStandardItemModel>
#include "w_sys_manage_cloudsync.h"
#include "fexpandmain_2.h"
#include "lds_roundeddialog_rect.h"
#include <QLabel>
#include <QDoubleSpinBox>
#include "lds_pushbutton_wrap.h"
#include "lds_roundeddialog_rect_pop.h"
#include "lds_label_doublespinbox.h"

class lds_label_doublespinbox;
class fexpandmain_model_sqltablemodel;
class ftableview_standmodel_sql_none;
class lds_menu;
class lds_doublespinbox;

////////////////////////////////////////////////
namespace Ui {
class w_scr_dish_pay_widget;
}


class w_scr_dish_pay_widget : public QFrame
{
    Q_OBJECT
public:
    enum operatMode{
        OPERAT_NONE = 0,
        OPERAT_QUITBILL = 1,
        OPERAT_PAY = 2,
        OPERAT_WAIMAI = 4,

        TYPE_RESTAURANT = 8,
        TYPE_FAST = 16,
        RESTAURANT_PAY = TYPE_RESTAURANT | OPERAT_PAY,
    };
    Q_DECLARE_FLAGS(operatModes, operatMode)

    static double valuerountbysys(double value);

    static bool cey_u_orderdish_direct_pay_transaction(const QString &ch_billno, QString *errstring);

    static bool canQuitbill(QString key);

    static void toYunPaydataUploadWithTransaction(QWidget *parent, const QString &ch_payno, const QDateTime &pay_dt);//【事务】
public:
    explicit w_scr_dish_pay_widget(QWidget *parent = 0);
    w_scr_dish_pay_widget(w_scr_dish_pay_widget::operatModes m, QWidget *parent,
                          const fexpandmain_model_sqltablemodel_data *tablemodel, int print_int_flowid_max = -1);

    ~w_scr_dish_pay_widget();
    void initdataByM(const fexpandmain_model_sqltablemodel_data *tablemodel, int print_int_flowid_max = -1);
    void initGeometry(w_scr_dish_pay_widget::operatModes m, bool strechLayout = true);
    void retranslateView();
    //通过付款单号来初始化付款列表，只用于反结账中的退单
    void initdataPayModel(const QString &ch_payno);

    int mread(bool b_always_disable_num_pay, const QString &title);
private:
    void first_go_in();
    double origin_pay_low;//快餐下的付款
private slots:
    void pay_data_init();
    void refreshLowServiceControl();
    void refreshQuitbillControl();
public:
    //////////会员读卡模块
public:
    void refreshControl();
    void updateMuyBillPay(member_up_yun &muy);

signals:
    void accepted();
    void rejected();
public slots:
    void accept();
    void reject();
public:
    void m_save();
    void m_restore();
    //////////
private:
    QString getMaxSerialnoAdd1(const QString &ch_billno);
private slots:
    void tocancel();
    void todiscount();
    void tomember();
    void tobank();
    void tovouchers();//代金券
    void torecharge();
    void amountClear();//数字键清除
    void updateAmount();//数字键插入
    void setAmount();//数字键settext
    void updateChange();//更新找零
    void updateDD(double v);

    void toChooseSpecialDispose(const QString &arg1);//选择特殊方式模式
    void toLowcostFree();//免低消
    void toPaysettings();//付款设置
    void toServiceFree();//免服务费
    void toMemberHang();//挂账
    void toBillPay();//付款
    void toBillFree();//免单
    void toOtherPay(const QString &ch_paymodeno);
    void towxpay();
    void toalipay();
    void tomobilepay();
    void toquickpay();
    void tonuomi();
    void todianping();
    void tomeituan();
    bool towaimaipaymodeno(const QString &ch_paymodeno);
    void towaimaicheck(const QModelIndex &index);//外卖验证
    void tospecialcheck(const QModelIndex &index);//特殊处理

    void toMenuTg(QPushButton *btn_parent);
    void toMenuOther(QPushButton *btn_parent);

    void toplayloop();
    void playloop();
public slots:
    //若是 initData_by_m 之后立刻 to_scrpay 则需要 initData_by_m(xx, -1, false)
    void toScrpay();//to_scrpay---【事务】---to_m_pay---to_pay---payAndYunCommit---datauploadAndprint【事务】
    void toExitbill();//to_exitbill---【事务】---to_m_pay---to_pay---payAndYunCommit---datauploadAndprint【事务】
private:
    void toMPay(const QDateTime &dt, QString &errstring , QString &ch_payno, const QString &serial_no);//买单
    bool otherModePay(const QString &ch_paymodeno, double amount = 0, const QString & wtitle = "");
    /*付款时本地云端提交*/
    void payAndYunCommit(const QDateTime &dt, QString &errstring , QString &ch_payno, QString serial_no);
    /*营业数据上传和打印*/
    void dataupload(const QString &ch_payno, const QDateTime &dt);
    void dataprintBill(const QString &languageKey);
    double getMNumPoint(const vch_membernoData &md);
    /*打印*/
    void printLabel(const QString &ch_billno);
    qlonglong print_int_flowid_max;

    bool checkNumFree(const QString &id, double num_free);
    bool getNewOperid(QString &errstring, QString &id);

private slots://功能模块
    void tostackinfo0(bool f);
    void tostackinfo1(bool f);

    void negativeallControl();
    void negativeallControl(lds_label_doublespinbox *l);
public:
    vch_membernoData vch_membernod;
    fexpandmain_keepData pay_keep_data;
    operatModes pay_operate_mode;
    bool get_pay_finished();

    const fexpandmain_model_sqltablemodel_data *_tablemodel;
    lds_model_sqltablemodel *payModel();
    double pay_without_low_service();//快餐下的付款
    double hasPaid();
    QString label_paylist_remain_str();
    double label_paylist_remain();

    int pay_all_discount;//全场折扣
    bool pay_finished;
private:
    lds_model_sqltablemodel *table_model;//付款方式的MODE
private:
    void updatePaylistCashpay();
    ///shortcut
protected:
    void keyPressEvent(QKeyEvent *e);
private slots:
    void toShortCutFunction(const QString &k);
    ///~shortcut
private:
    class pushbuttonAutoEnable{
    public:
        pushbuttonAutoEnable(QAbstractButton *b) : _b(b){b->setEnabled(false);b->repaint();}
        ~pushbuttonAutoEnable(){_b->setEnabled(true);}
    private:
        QAbstractButton *_b;
    };

    ///pay_section
private:
    QList<lds_pushbutton_wrap *> pay_section;
    void resetPaySection();
    lds_pushbutton_wrap *paySectionValue(const QString &key);
    void savePayModuleFile();
    void updatePayModuleStylesheet(lds_pushbutton_wrap *b);

private slots:
    void toModuleSelect();
    void toPaySectionModule();
    ///~pay_section
public:
    Ui::w_scr_dish_pay_widget *ui;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(w_scr_dish_pay_widget::operatModes)

#endif // W_SCR_DISH_PAY_WIDGET_H
