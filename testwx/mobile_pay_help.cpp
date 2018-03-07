#include "mobile_pay_help.h"
#include <QUuid>
#include <QHostInfo>
#include <QDateTime>
#include <QTimer>
#include <QSslConfiguration>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QEventLoop>
#include <QNetworkReply>
#include "alipay_help.h"


QString mobile_pay_help::errstring;
OnlinePayType mobile_pay_help::pay_type = OnlinePayTypeNone;
const QList<mobile_pay_help::SSPair> mobile_pay_help::mobile_pay_alipay_key_value_pair_list = QList<mobile_pay_help::SSPair>()
        << mobile_pay_help::SSPair("RSA", "")
        << mobile_pay_help::SSPair("RSA2", "_new");



mobile_pay_help::mobile_pay_help()
{
}
void mobile_pay_help::test()
{
    //金额为整数，单位分，如12.34元，实际传入1234
    //auth_code 自动替换清除空格
    QString appid;// = "2015121000954514";
    QString sub_mch_id = "1431014702";
    QString kbstore_id;// = "0000000100010005";
    QString auth_code;
    QString total_amount = "1";

    QString out_trade_no;
    QString body = QObject::tr("移动支付");
    QString sub_appid = "";
    QString device_info = "";
    QString transaction_id;

    out_trade_no = "01"+QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzz");
    auth_code = "1300 5768 2925 712750";

    ///微信扫码支付，测试可行
    //    if(codepay(
    //                transaction_id,

    //                appid,
    //                sub_mch_id,
    //                kbstore_id,
    //                auth_code,
    //                total_amount,

    //                out_trade_no,
    //                body
    //                )) {
    //        qDebug() << "codepay:" << "支付成功";
    //        qDebug() << "transaction_id" << transaction_id << total_amount;
    //        qDebug() << "out_trade_no" << out_trade_no << total_amount;
    //    } else {
    //        qDebug() << "codepay:" << "支付失败" << errstring;
    //    }

    ///微信二维码支付，测试可行
    QByteArray code_url;
    if(getwxpayurl(code_url, sub_mch_id, total_amount, out_trade_no, body)){
        QString transaction_id;
        if(wxOrderQuery(transaction_id, sub_mch_id, out_trade_no, sub_appid)) {
        } else {
        }
    } else {
    }
    ///微信退款，暂时不使用，需要门店签合同授权
}

void mobile_pay_help::test1()
{
    QString appid = "2015121000954514";
    QString sub_mch_id;// = "1431014702";
    QString kbstore_id = "0000000100010005";
    QString auth_code;
    QString total_amount = "1";//支付宝的金额也是以分为单位

    QString out_trade_no;
    QString body = QObject::tr("移动支付");
    QString sub_appid = "";
    QString device_info = "";
    QString transaction_id;

    out_trade_no = "01"+QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzz");
    auth_code = "2846 46994981 978715";

    ///支付宝扫码支付，测试可行
    //    if(codepay(
    //                transaction_id,

    //                appid,
    //                sub_mch_id,
    //                kbstore_id,
    //                auth_code,
    //                total_amount,

    //                out_trade_no,
    //                body
    //                )) {
    //        qDebug() << "codepay:" << "支付成功";
    //        qDebug() << "transaction_id" << transaction_id << total_amount;
    //        qDebug() << "out_trade_no" << out_trade_no << total_amount;

    //    } else {
    //        qDebug() << "codepay:" << "支付失败" << errstring;
    //    }
    ///支付宝二维码支付，测试可行
    QByteArray code_url;
    if(getzfbpayurl("RSA", code_url, appid, total_amount, out_trade_no, body, kbstore_id)){
        //        qDebug() << "getwxpayurl" << code_url;
        QString trade_no;
        if(zfbOrderQuery("RSA", trade_no, appid, out_trade_no)) {
            //            qDebug() << "zfbOrderQuery" << trade_no;
        } else {
            //            qDebug() << __LINE__ << errstring;
        }
    } else {
        //        qDebug() << __LINE__ << errstring;
    }
    ///支付宝退款操作，测试可行
    zfbrefund("RSA", "cz-desktop20170121150514781", appid, "", "1");
}

QString mobile_pay_help::get_mobile_pay_alipay_key_value_pair_list_value(const QString &key)
{
    for(int k = 0; k < mobile_pay_alipay_key_value_pair_list.count(); k ++) {
        if(mobile_pay_alipay_key_value_pair_list[k].first == key) {
            return mobile_pay_alipay_key_value_pair_list[k].second;
        }
    }
    return "";
}

QStringList mobile_pay_help::mobile_pay_alipay_keys()
{
    QStringList list;
    for(int k = 0; k < mobile_pay_alipay_key_value_pair_list.count(); k ++) {
        list << mobile_pay_alipay_key_value_pair_list[k].first;
    }
    return list;
}

bool mobile_pay_help::codepay(QString sign_type,QString &transaction_id, const QString &appid, const QString &sub_mch_id, const QString &kbstore_id, const QString &auth_code, const QString &total_amount, const QString &out_trade_no, const QString &body, const QString &sub_appid, const QString &device_info)
{
    errstring = "";pay_type = OnlinePayTypeNone;
    QByteArray recv;
    QByteArray data;

    data += "out_trade_no="+out_trade_no+"&";
    data += "total_amount="+total_amount+"&";
    data += "appid="+appid+"&";
    data += "auth_code="+auth_code.trimmed().replace(" ", "")+"&";
    data += "kbstore_id="+kbstore_id+"&";

    data += "body="+body+"&";
    data += "sub_mch_id="+sub_mch_id+"&";
    data += "sub_appid="+sub_appid+"&";
    data += "device_info="+device_info;
    if(auth_code.isEmpty()) {
        errstring = "auth_code_not_null";
        return false;
    }

    if( do_post("http://pay.scpos.com/SystemManage/wxzfb/codepay" + get_mobile_pay_alipay_key_value_pair_list_value(sign_type), data,recv, &errstring)){
        //        qDebug()<<QObject::trUtf8( recv.data());
        cJSON*p = cJSON_Parse(recv.data());
        cJSON_auto_del d(p);
        QByteArray state = getcJSONvalue(p, "state").toByteArray();
        QByteArray message = getcJSONvalue(p, "message").toByteArray();
        if("success" == state) {
            cJSON *root = cJSON_Parse(message.data());
            if(root) {
                cJSON_auto_del d(root);
                //支付宝
                cJSON *item = cJSON_GetObjectItem(root,"alipay_trade_pay_response");
                if(item) {
                    transaction_id = getcJSONvalue(item, "trade_no").toString();
                    pay_type = OnlinePayTypeAliPay;
                }
                //微信
                if(transaction_id.isEmpty()) {
                    pay_type = OnlinePayTypeWeChat;
                    transaction_id = getcJSONvalue(root, "transaction_id").toString();
                }
                if(!transaction_id.isEmpty()) {
                    return true;
                }
                //微信
                if(item) {
                    errstring = getcJSONvalue(item, "sub_msg").toString();
                }
                if(errstring.isEmpty()) {
                    errstring = getcJSONvalue(root, "err_code_des").toString();
                }
                if(errstring.isEmpty()) {
                    errstring = "transaction_id_no_null";
                }
            }
        }
        if("error" == state) {
            cJSON *root = cJSON_Parse(message.data());
            if(root) {
                cJSON_auto_del d(root);
                //支付宝
                cJSON *item = cJSON_GetObjectItem(root,"alipay_trade_pay_response");
                if(item) {
                    errstring = getcJSONvalue(item, "sub_msg").toString();
                    return false;
                }
                //微信
                errstring = getcJSONvalue(item, "err_code_des").toString();
            }
            if(errstring.isEmpty()) {
                errstring = message;
            }
        }
    }
    if(errstring.isEmpty()) {
        errstring = "unknow error";
    }
    return false;
}

bool mobile_pay_help::getwxpayurl(QByteArray &ret_code_url, const QString &sub_mch_id, const QString &total_amount, const QString &out_trade_no, const QString &body, const QString &sub_appid, const QString &device_info)
{
    errstring = "";pay_type = OnlinePayTypeNone;
    QByteArray recv;
    QByteArray data;

    data += "out_trade_no="+out_trade_no+"&";
    data += "total_amount="+total_amount+"&";
    data += "body="+body+"&";
    data += "sub_mch_id="+sub_mch_id+"&";

    data += "sub_appid="+sub_appid+"&";
    data += "device_info="+device_info;
    if(sub_mch_id.isEmpty()) {
        errstring = "sub_mch_id_no_null";
        return false;
    }

    if(do_post("http://pay.scpos.com/SystemManage/wxzfb/getwxpayurl",data,recv, &errstring)){
        cJSON*p = cJSON_Parse(recv.data());
        cJSON_auto_del d(p);
        QByteArray state = getcJSONvalue(p, "state").toByteArray();
        QByteArray message = getcJSONvalue(p, "message").toByteArray();
        if("success" == state){
            //微信判断
            if(!message.isEmpty()){
                ret_code_url = message;
                return true;
            }
        }
        errstring = getcJSONvalue(message, "err_code_des").toString();
        if(errstring.isEmpty()) {
            errstring = message;
        }
    }

    if(errstring.isEmpty()) {
        errstring = "unknow error";
    }
    return false;
}

bool mobile_pay_help::getzfbpayurl(QString sign_type,QByteArray &ret_qr_code, const QString &appid, const QString &total_amount, const QString &out_trade_no, const QString &body, const QString &kbstore_id, const QString &device_info)
{
    errstring = "";pay_type = OnlinePayTypeNone;
    QByteArray recv;
    QByteArray data;

    data += "appid="+appid+"&";
    data += "total_amount="+total_amount+"&";
    data += "out_trade_no="+out_trade_no+"&";
    data += "body="+body+"&";
    data += "kbstore_id="+kbstore_id+"&";

    data += "device_info="+device_info;

    if( do_post("http://pay.scpos.com/SystemManage/wxzfb/getzfbpayurl" + get_mobile_pay_alipay_key_value_pair_list_value(sign_type) ,data,recv, &errstring)){
        cJSON*p = cJSON_Parse(recv.data());
        cJSON_auto_del d(p);
        QByteArray state = getcJSONvalue(p, "state").toByteArray();
        QByteArray message = getcJSONvalue(p, "message").toByteArray();
        errstring = message;
        if("success" == state){
            errstring = "qr_code_no_null";
            if(!message.isEmpty()){
                ret_qr_code = message;
                errstring = "";
                return true;
            }
        }

        if("error" == state) {
            cJSON *root = cJSON_Parse(message.data());
            if(root) {
                cJSON_auto_del d(root);
                cJSON *item = cJSON_GetObjectItem(root,"alipay_trade_precreate_response");
                errstring = getcJSONvalue(item, "sub_msg").toString();
            }
        }
    }
    if(errstring.isEmpty()) {
        errstring = "unknow error";
    }
    return false;
}

bool mobile_pay_help::zfbOrderQuery(QString sign_type,QString &trade_no, const QString &appid, const QString &out_trade_no)
{
    errstring = "";pay_type = OnlinePayTypeNone;
    QByteArray recv;
    QByteArray data;

    data += "appid="+appid+"&";
    data += "out_trade_no="+out_trade_no;

    if( do_post("http://pay.scpos.com/SystemManage/wxzfb/zfbOrderQuery" + get_mobile_pay_alipay_key_value_pair_list_value(sign_type) ,data,recv, &errstring)){
        cJSON*p = cJSON_Parse(recv.data());
        cJSON_auto_del d(p);
        QByteArray state = getcJSONvalue(p, "state").toByteArray();
        QByteArray message = getcJSONvalue(p, "message").toByteArray();
        errstring = message;
        if("success" == state){
            cJSON *root = cJSON_Parse(message.data());
            if(root) {
                cJSON_auto_del d(root);
                cJSON *item = cJSON_GetObjectItem(root,"alipay_trade_query_response");
                trade_no = getcJSONvalue(item, "trade_no").toString();
                if(!trade_no.isEmpty()){
                    pay_type = OnlinePayTypeAliPay;
                    return true;
                }
                errstring = getcJSONvalue(item, "sub_msg").toString();
            }
        }

        if("error" == state) {
            cJSON *root = cJSON_Parse(message.data());
            if(root) {
                cJSON_auto_del d(root);
                cJSON *item = cJSON_GetObjectItem(root,"alipay_trade_query_response");
                errstring = getcJSONvalue(item, "sub_msg").toString();
            }
        }
    }
    if(errstring.isEmpty()) {
        errstring = "unknow error";
    }

    return false;
}

bool mobile_pay_help::wxOrderQuery(QString &transaction_id, const QString &sub_mch_id, const QString &out_trade_no, const QString &sub_appid)
{
    errstring = "";pay_type = OnlinePayTypeNone;
    QByteArray recv;
    QByteArray data;

    data += "out_trade_no="+out_trade_no+"&";
    data += "sub_mch_id="+sub_mch_id+"&";

    data += "sub_appid="+sub_appid;

    if( do_post("http://pay.scpos.com/SystemManage/wxzfb/wxOrderQuery",data,recv, &errstring)){
        cJSON*p = cJSON_Parse(recv.data());
        cJSON_auto_del d(p);
        QByteArray state = getcJSONvalue(p, "state").toByteArray();
        QByteArray message = getcJSONvalue(p, "message").toByteArray();
        qDebug() << data;
        qDebug() << recv;
        qDebug() << state;
        qDebug() << message.data();
        if("success" == state){
            //微信判断
            QString trade_state = getcJSONvalue(message, "trade_state").toString();
            transaction_id = getcJSONvalue(message, "transaction_id").toString();
            if(!transaction_id.isEmpty()//原订单号
                    &&trade_state =="SUCCESS"){
                pay_type = OnlinePayTypeWeChat;
                return true;
            }
        }
        errstring = getcJSONvalue(message, "trade_state_desc").toString();
    }

    if(errstring.isEmpty()) {
        errstring = "unknow error";
    }
    return false;
}

bool mobile_pay_help::zfbrefund(QString sign_type,const QString &trade_no, const QString &appid, const QString &refund_reason, const QString &refund_amount)
{
    errstring = "";pay_type = OnlinePayTypeNone;
    QByteArray recv;
    QByteArray data;

    data += "out_trade_no="+trade_no+"&";
    data += "appid="+appid+"&";
    data += "refund_reason="+refund_reason+"&";
    data += "refund_amount="+refund_amount;

    if( do_post("http://pay.scpos.com/SystemManage/wxzfb/zfbrefund" + get_mobile_pay_alipay_key_value_pair_list_value(sign_type) ,data,recv, &errstring)){
        cJSON*p = cJSON_Parse(recv.data());
        cJSON_auto_del d(p);
        QByteArray state = getcJSONvalue(p, "state").toByteArray();
        QByteArray message = getcJSONvalue(p, "message").toByteArray();
        errstring = message;
        if("success" == state){

        }
        if("error" == state){
            cJSON *root = cJSON_Parse(message.data());
            if(root) {
                cJSON_auto_del d(root);
                //支付宝
                cJSON *item = cJSON_GetObjectItem(root,"alipay_trade_refund_response");
                if(item){
                    errstring = getcJSONvalue(item, "sub_msg").toString();
                }
            }
        }
    }
    return false;
}

bool mobile_pay_help::wxrefund(const QString &transaction_id, const QString &total_fee, const QString &refund_fee, const QString &sub_mch_id, const QString &sub_appid)
{
    errstring = "";pay_type = OnlinePayTypeNone;
    QByteArray recv;
    QByteArray data;

    data += "out_trade_no="+transaction_id+"&";
    data += "total_fee="+total_fee+"&";
    data += "refund_fee="+refund_fee+"&";
    data += "sub_appid="+sub_appid+"&";
    data += "sub_mch_id="+sub_mch_id;

    if( do_post("http://pay.scpos.com/SystemManage/wxzfb/wxrefund",data,recv, &errstring)){
        cJSON*p = cJSON_Parse(recv.data());
        cJSON_auto_del d(p);
        QByteArray state = getcJSONvalue(p, "state").toByteArray();
        QByteArray message = getcJSONvalue(p, "message").toByteArray();
        if("success" == state){
            return true;
        }

        errstring = getcJSONvalue(message, "err_code_des").toString();
        if(errstring.isEmpty()) {
            errstring = message;
        }
    }
    if(errstring.isEmpty()) {
        errstring = "unknow error";
    }
    return false;
}

bool mobile_pay_help::getzfbpublic_key(QString sign_type,QByteArray &ret_rsa_public_key)
{
    QByteArray recv;
    if( do_post("http://pay.scpos.com/SystemManage/wxzfb/getzfbpublic_key" + get_mobile_pay_alipay_key_value_pair_list_value(sign_type) ,"",recv, &errstring)){
        cJSON*p = cJSON_Parse(recv.data());
        cJSON_auto_del d(p);
        QByteArray state = getcJSONvalue(p, "state").toByteArray();
        QByteArray message = getcJSONvalue(p, "message").toByteArray();
        if("success" == state){
            ret_rsa_public_key = message;
            return true;
        }
    }
    return false;
}

bool mobile_pay_help::do_post(const QString &url, const QByteArray &content, QByteArray &responseData, QString *errstring)
{
    int contentLength = content.length();
    QNetworkRequest req;
    QNetworkAccessManager manager;
    QEventLoop eloop;
    QTimer timer;
    req.setUrl(QUrl(url));
    //application/octet-stream
    req.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");
    req.setHeader(QNetworkRequest::ContentLengthHeader,contentLength);

    qDebug() << "url:" << url ;

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#else
    QSslConfiguration config = QSslConfiguration::defaultConfiguration();
    config.setProtocol(QSsl::TlsV1);
    config.setPeerVerifyMode(QSslSocket::VerifyNone);
    req.setSslConfiguration(config);
#endif

    timer.setSingleShot(true);
    QObject::connect(&timer,SIGNAL(timeout()),&eloop,SLOT(quit()));
    QObject::connect(&manager,SIGNAL(finished(QNetworkReply*)),&eloop,SLOT(quit()));
    QNetworkReply *reply=manager.post(req,content);
    timer.start(60*1000);//10s的超时eply*)),&eloop,SLOT(quit()));
    eloop.exec(QEventLoop::ExcludeUserInputEvents);

    if(timer.isActive()){
        if(reply->error()==QNetworkReply::NoError){
            responseData = reply->readAll();
            return true;
        }
        if(errstring) {
            *errstring = reply->errorString();
        }
        return false;
    }
    if(errstring) {
        *errstring = "timeout";
    }
    return false;
}

QVariant mobile_pay_help::getcJSONvalue(cJSON *root, const char *string)
{
    cJSON *item = cJSON_GetObjectItem(root,string);
    return getItemValue(item);
}

QVariant mobile_pay_help::getcJSONvalue(const QByteArray &d, const char *string)
{
    cJSON * p = cJSON_Parse(d.data());
    if(p) {
        cJSON_auto_del x(p);
        return getcJSONvalue(p, string);
    }
    return QVariant();
}

QVariant mobile_pay_help::getItemValue(cJSON *item)
{
    QVariant ret;
    if(!item) {
        qDebug("json return item is null , please check ");
        return ret;
    }
    if(item->type == cJSON_String){
        ret = item->valuestring;
    } else if(item->type == cJSON_Number){
        if(item->valuedouble != double(item->valueint)){
            ret = item->valuedouble;
        } else {
            ret = item->valueint;
        }
    }
    return ret;
}
