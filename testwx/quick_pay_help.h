#ifndef QUICK_PAY_HELP_H
#define QUICK_PAY_HELP_H

#include <QVariant>
#include <QHostInfo>
#include "cJSON.h"
#include "testwx_help.h"
class QNetworkReply;
class QNetworkAccessManager;

class quick_pay_help
{
public:
    quick_pay_help();
    //    -----------------银行支付
    //    1、扫码枪扫码支付
    static bool barCodePay(
            const QString &terminal_code,    //终端号
            double total_amount,    //金额 22.22
            const QString &qrcode,// 授权码
            const QString &out_trade_no,    //订单号
            const QString &body,            //商品描述
            const QString &device_info, //设备信息
            QString &ret_errstring,
            OnlinePayType &ret_pay_type
            );
    //    2、取消支付交易（民生银行暂不支持）
    static bool cancelPay(
            const QString &terminal_code,  //终端号
            const QString &out_trade_no,      //微信子商户号
            QString &ret_errstring
            );

    //银行，二维码支付
    static bool getPayUrl(
            QByteArray &pay_url,
            const QString &terminal_code,    //终端号
            double total_amount,    //金额 22.22
            const QString &walletType,// 1：支付宝，2：微信
            const QString &out_trade_no,    //订单号
            const QString &body,            //商品描述
            const QString &device_info, //设备信息
            QString &ret_errstring
            );
    //银行, 订单查询
    static bool query(
            const QString &terminal_code,  //终端号
            const QString &out_trade_no ,     //微信子商户号
            QString &ret_errstring,
            OnlinePayType &ret_pay_type
            );

    static QNetworkReply * post(QNetworkAccessManager *networkAccessManager, const QString &param, const QUrl &url);

    static QVariant getcJSONvalue(cJSON *root, const char *string);
    static QVariant getcJSONvalue(const QByteArray &d, const char *string);
    static QVariant getItemValue(cJSON *item);
};

#endif // QUICK_PAY_HELP_H
