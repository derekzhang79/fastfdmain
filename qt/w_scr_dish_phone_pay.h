#ifndef W_SCR_DISH_PHONE_PAY_H
#define W_SCR_DISH_PHONE_PAY_H

#include <QDialog>
#include <QTimer>
#include <QMutex>
#include "alipay_help.h"
#include "testwx_help.h"
#include "lds.h"

namespace Ui {
class w_scr_dish_phone_pay;
}

class w_scr_dish_phone_pay : public QDialog
{
    Q_OBJECT
public:
    struct weixinqueryDATA{
        QString out_trade_no;
        QString appid;
        QString paykey;
        QString mch_id;
    };
    struct alipayqueryDATA{
        QString out_trade_no;
        alipay_help::alipay_public_request_argument arg0;
    };
    struct wxzfb_zfb_queryDATA{
        QString appid;// = "2016071601627725";
        QString total_amount;// = "1";//金额 单位为分
        QString out_trade_no;
        QString body;// = "移动支付";
        QString kbstore_id;// = "1233333333334";

        QString device_info;// = "";
    };
    struct wxzfb_wx_queryDATA{
        QString sub_mch_id;//      //微信子商户号
        QString total_amount;//    //金额 单位为分
        QString out_trade_no;//    //订单号
        QString body;//            //商品描述

        QString sub_appid;// = "";//  //微信子appid
        QString device_info;// = "";// //设备信息
    };
    struct quick_pay_queryDATA{
        QString terminal_code;//      //微信子商户号
        double total_amount;//    //金额
        QString walletType;//1：支付宝，2：微信
        QString out_trade_no;//订单号
        QString body;//            //商品描述
        QString device_info;// = "";// //设备信息
    };
public:
    explicit w_scr_dish_phone_pay(const QString &ch_paymodeno,
                                  double fee,
                                  QWidget *parent = 0);
    ~w_scr_dish_phone_pay();
    QString ret_out_trade_no;
    OnlinePayType ret_pay_type;
    static bool getInputPayFee(double &ret_fee, const QString &ch_paymodeno, double num_pay, QWidget *parent);
    static QString title(const QString &ch_paymodeno);
    static bool isPayRecord(const QString &ch_paymodeno);
private slots:
    void makeUrl();
    void toDDexit();
    void toDDaccept();
    void topolling();
    void tocodePay();

    void toprint();
    void to_save_audio(bool f);
private:
    Ui::w_scr_dish_phone_pay *ui;
    QString ch_paymodeno;
    QTimer *timer;
    double fee;

    weixinqueryDATA d_wechat;
    alipayqueryDATA d_alipay;
    wxzfb_wx_queryDATA d_mobile_wechat;
    wxzfb_zfb_queryDATA d_mobile_alipay;
    quick_pay_queryDATA d_quick_wechat;
    quick_pay_queryDATA d_quick_alipay;

    QString out_trade_no_alipay;
    QString out_trade_no_wechat;
    QString out_trade_no_barcode;
    QMutex mutex;//时间轮询时，只有一个有效

    lds::DOUBLE_DATA rb_ddata;

    class timer_auto_start{
    public:
        timer_auto_start(QTimer *t) : _t(t) {}
        ~timer_auto_start(){_t->start();}
        void stop() {_t->stop();}
    private:
        QTimer *_t;
    };
};

#endif // W_SCR_DISH_PHONE_PAY_H
