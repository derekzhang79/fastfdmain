#ifndef MOBILE_PAY_HELP_H
#define MOBILE_PAY_HELP_H

#include <QVariant>
#include <QHostInfo>
#include "cJSON.h"
#include "testwx_help.h"

/// 本支付、微信支付宝的金额都是整数、以分为单位
///  微信支付宝的 的 transaction_id 和 out_trade_no 都可以作为退款凭证，目前只需保存 out_trade_no 作为退款用
///
/// errstring用于返回错误信息
///
/// paytype用于返回付款类型，例如扫码支付时，若是手机出示的是支付宝，则返回pay_zfb
/// 用于 codepay、wxOrderQuery、zfbOrderQuery
///
/// 扫码支付时，条码需要做清除空格操作

/// 微信支付：
/// 1.只需要获取【微信子商户号】即可，向售后申请
/// 支付宝支付：
/// *****************
/// 1.已有的账户、根据软件提供的【公钥】(getzfbpublic_key)重新设置【公钥】
/// *****************
/// 2.【appid】必填、【口碑门店】选填（影响门店优惠）

/// 微信支付，第一次会比较慢
/// 支付宝支付，第一次会比较慢
class mobile_pay_help
{
public:
    mobile_pay_help();

    //测试微信部分
    static void test();
    //测试支付部分
    static void test1();
    static QString get_mobile_pay_alipay_key_value_pair_list_value(const QString &key);
    static QStringList mobile_pay_alipay_keys();
    typedef QPair<QString, QString> SSPair;

    ////////////////////微信、支付宝
    //微信、支付宝，扫码支付
    static bool codepay(
            QString sign_type,

            QString &transaction_id,
            const QString &appid,           //支付宝appid
            const QString &sub_mch_id,      //微信子商户号
            const QString &kbstore_id,      //口碑门店
            const QString &auth_code,       //条形码
            const QString &total_amount,    //金额 单位为分

            const QString &out_trade_no,    //订单号
            const QString &body,            //商品描述
            const QString &sub_appid = "",  //微信子appid 选填 ，子商户有公众账 号时填写
            const QString &device_info = "" //设备信息
            );
    ////////////////////微信
    //微信、二维码支付
    static bool getwxpayurl(
            QByteArray &ret_code_url,
            const QString &sub_mch_id,      //微信子商户号
            const QString &total_amount,    //金额 单位为分
            const QString &out_trade_no,    //订单号
            const QString &body,            //商品描述

            const QString &sub_appid = "",  //微信子appid
            const QString &device_info = "" //设备信息
            );

    //微信、订单查询
    static bool wxOrderQuery(
            QString &transaction_id,

            const QString &sub_mch_id,      //微信子商户号
            const QString &out_trade_no,    //订单号
            const QString &sub_appid = ""  //微信子appid
            );

    //微信、退款
    static bool wxrefund(
            const QString &transaction_id,    //商户系统内部的订单号

            const QString &total_fee,      //订单总金额,单位为分,只能为整数
            const QString &refund_fee,      //退款总金额,单位为分,只能为整数,可部分退款
            const QString &sub_mch_id,    //微信支付分配的子商户号
            const QString &sub_appid = ""       //微信分配的子商户公众账号ID
            );


    ////////////////////支付宝
    //支付宝、退款
    static bool getzfbpayurl(
            QString sign_type,

            QByteArray &ret_qr_code,

            const QString &appid,           //支付宝appid
            const QString &total_amount,    //金额 单位为分
            const QString &out_trade_no,    //订单号
            const QString &body,            //商品描述
            const QString &kbstore_id,            //口碑门店

            const QString &device_info = "" //设备信息
            );

    //支付宝、订单查询
    static bool zfbOrderQuery(
            QString sign_type,
            QString &trade_no,

            const QString &appid,           //支付宝appid
            const QString &out_trade_no    //订单号
            );

    //支付宝、获取公钥
    static bool getzfbpublic_key(
            QString sign_type,
            QByteArray &ret_rsa_public_key);
    //支付宝、退款
    static bool zfbrefund(
            QString sign_type,
            const QString &trade_no,      //原订单号
            const QString &appid,    //支付宝appid
            const QString &refund_reason , //退单原因
            const QString &refund_amount  //退款金额
            );

    /////////////////////////
    static QString errstring;
    static OnlinePayType pay_type;

private:
    static bool do_post(const QString &url, const QByteArray &content, QByteArray&responseData, QString *errstring = 0);

    ///cjson
    static QVariant getcJSONvalue(cJSON *root, const char *string);
    static QVariant getcJSONvalue(const QByteArray &d, const char *string);
    static QVariant getItemValue(cJSON *item);
    class cJSON_auto_del{
    public:
        cJSON_auto_del(cJSON* o) {this->o = o;}
        ~cJSON_auto_del(){if(o)delete o;o = 0;}

    private:
        cJSON *o;
    };
    ///~cjson

    static const QList<SSPair>mobile_pay_alipay_key_value_pair_list;
};

#endif // MOBILE_PAY_HELP_H
