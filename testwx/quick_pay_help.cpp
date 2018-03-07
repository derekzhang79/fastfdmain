#include "quick_pay_help.h"
#include <QHostInfo>
#include <QDateTime>
#include <QTimer>
#include <QSslConfiguration>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QEventLoop>
#include <QNetworkReply>
#include <QObject>
#include <QTextCodec>

static QString trans_g2u(const QByteArray &in)
{
    QTextCodec *c = QTextCodec::codecForName("gb2312");
    return c->toUnicode(in);
}

static QByteArray trans_u2g(const QString &in)
{
    QByteArray b;
    QTextCodec *c = QTextCodec::codecForName("gb2312");
    b.append(c->fromUnicode(in));
    return b;
}

quick_pay_help::quick_pay_help()
{
}

bool quick_pay_help::barCodePay(const QString &terminal_code, double total_amount, const QString &qrcode, const QString &out_trade_no, const QString &body, const QString &device_info, QString &ret_errstring, OnlinePayType &ret_pay_type)
{
    QString param =
            " { \n "
            "  \"terminal_code\": \""+terminal_code+"\", \n " +
            "  \"total_amount\": "+QString::number(total_amount, 'f', 2)+", \n " +
            "  \"qrcode\": \""+QString(qrcode).replace(" ", "")+"\", \n " +
            "  \"out_trade_no\": \""+out_trade_no+"\" ,\n " +
            "  \"body\": \""+body+"\" ,\n " +
            "  \"device_info\": \""+device_info+"\" \n " +
            " }"
            ;
    QString walletType;
    QNetworkAccessManager manager;
    QUrl url;
//#ifdef QT_DEBUG
//    url = QUrl(QString("http://192.168.1.81/bankPay/tradeActiveScanCodePay-mobile"));
//#else
    url = QUrl(QString("http://www.yqspos.com/bankPay/tradeActiveScanCodePay-mobile"));
//#endif
    QNetworkReply *reply = post(&manager, param, url);
    int httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    QByteArray readall = reply->readAll();


    qDebug() << url.toString() << param << "httpStatus:" << httpStatus << readall.data();
    qDebug() << "";
    qDebug() << "";
    switch(httpStatus) {
    case 200:
        if(getcJSONvalue(readall, "txnStatus") == "s") {
            walletType = getcJSONvalue(readall, "walletType").toString();
            if("1" == walletType)
                ret_pay_type = OnlinePayTypeAliPay;
            if("2" == walletType)
                ret_pay_type = OnlinePayTypeWeChat;
            return true;
        }
        ret_errstring = getcJSONvalue(readall, "message").toString();
        break;
    case 550:
        ret_errstring = getcJSONvalue(readall, "message").toString();
        break;
    default:
        ret_errstring = reply->errorString();
        break;
    }
    return false;
}

bool quick_pay_help::cancelPay(const QString &terminal_code, const QString &out_trade_no, QString &ret_errstring)
{
    QString param =
            " { \n "
            "  \"terminal_code\": \""+terminal_code+"\", \n " +
            "  \"out_trade_no\": \""+out_trade_no+"\" \n " +
            " }"
            ;
    QNetworkAccessManager manager;
    QUrl url;
//#ifdef QT_DEBUG
//    url = QUrl(QString("http://192.168.1.81/bankPay/cancelTradePay-mobile"));
//#else
    url = QUrl(QString("http://www.yqspos.com/bankPay/tradeECodePay-mobile"));
//#endif
    QNetworkReply *reply = post(&manager, param, url);
    int httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    QByteArray readall = reply->readAll();
    qDebug() << url.toString() << param << "httpStatus:" << httpStatus << readall.data();
    qDebug() << "";
    qDebug() << "";
    switch(httpStatus) {
    case 200:
        return true;
    case 550:
        ret_errstring = getcJSONvalue(readall, "message").toString();
        break;
    default:
        ret_errstring = reply->errorString();
        break;
    }
    return false;
}

bool quick_pay_help::getPayUrl(QByteArray &pay_url, const QString &terminal_code, double total_amount, const QString &walletType, const QString &out_trade_no, const QString &body, const QString &device_info, QString &ret_errstring)
{
    QString param =
            " { \n "
            "  \"terminal_code\": \""+terminal_code+"\", \n " +
            "  \"total_amount\": "+QString::number(total_amount, 'f', 2)+", \n " +
            "  \"walletType\": \""+walletType+"\", \n " +
            "  \"out_trade_no\": \""+out_trade_no+"\", \n " +
            "  \"body\": \""+body+"\", \n " +
            "  \"device_info\": \""+device_info+"\" \n " +
            " }"
            ;
    QNetworkAccessManager manager;
    QUrl url;
//#ifdef QT_DEBUG
//    url = QUrl(QString("http://192.168.1.81/bankPay/tradePassiveScanCodePay-mobile"));
//#else
    url = QUrl(QString("http://www.yqspos.com/bankPay/tradePassiveScanCodePay-mobile"));
//#endif
    QNetworkReply *reply = post(&manager, param, url);
    int httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    QByteArray readall = reply->readAll();
    qDebug() << url.toString() << param << "httpStatus:" << httpStatus << readall;
    qDebug() << "";
    qDebug() << "";
    switch(httpStatus) {
    case 200:
        pay_url = getcJSONvalue(readall, "ecodeText").toByteArray();
        return true;
    case 550:
        ret_errstring = getcJSONvalue(readall, "message").toString();
        qDebug() << __LINE__ << trans_g2u(ret_errstring.toLocal8Bit());
        qDebug() << __LINE__ << trans_u2g(ret_errstring);
        break;
    default:
        ret_errstring = reply->errorString();
        break;
    }
    return false;
}

/*
"http://www.yqspos.com/bankPay/queryTradePay-mobile" " {
"terminal_code": "FA80310092516736C",
"out_trade_no": "AA81020232410800C20171227171233"
}" httpStatus: 200 {"txnStatus":"s","walletSerialNumber":"JN1013825845","walletType":"1","message":"查询成功","totalSrcAmt":"0.01"}

*/
bool quick_pay_help::query(const QString &terminal_code, const QString &out_trade_no, QString &ret_errstring, OnlinePayType &ret_pay_type)
{
    QString param =
            " { \n "
            "  \"terminal_code\": \""+terminal_code+"\", \n " +
            "  \"out_trade_no\": \""+out_trade_no+"\" \n " +
            " }"
            ;
    QNetworkAccessManager manager;
    QUrl url;
//#ifdef QT_DEBUG
//    url = QUrl(QString("http://192.168.1.81/bankPay/queryTradePay-mobile"));
//#else
    url = QUrl(QString("http://www.yqspos.com/bankPay/queryTradePay-mobile"));
//#endif
    QNetworkReply *reply = post(&manager, param, url);
    int httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    QByteArray readall = reply->readAll();
    qDebug() << url.toString() << param << "httpStatus:" << httpStatus << readall.data();
    qDebug() << "";
    qDebug() << "";
    switch(httpStatus) {
    case 200:
        if(getcJSONvalue(readall, "txnStatus") == "s") {
            ret_pay_type = OnlinePayTypeNone;
            if(getcJSONvalue(readall, "walletType") == "1") {
                ret_pay_type = OnlinePayTypeAliPay;
            }
            if(getcJSONvalue(readall, "walletType") == "2") {
                ret_pay_type = OnlinePayTypeWeChat;
            }
            return true;
        }
        ret_errstring = getcJSONvalue(readall, "message").toString();
        break;
    case 550:
        ret_errstring = getcJSONvalue(readall, "message").toString();
        break;
    default:
        ret_errstring = reply->errorString();
        break;
    }

    return false;
}

QNetworkReply *quick_pay_help::post(QNetworkAccessManager *networkAccessManager, const QString &param, const QUrl &url)
{
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/java;charset=utf-8");
    request.setUrl(url);
    int mesc = 60 * 1000;
    QEventLoop eloop;
    QTimer timer;
    timer.setSingleShot(true);
    QObject::connect(&timer,SIGNAL(timeout()),&eloop,SLOT(quit()));
    QObject::connect(networkAccessManager,SIGNAL(finished(QNetworkReply*)),&eloop,SLOT(quit()));
    QNetworkReply *reply = networkAccessManager->post(request, param.toLocal8Bit());
    timer.start(mesc);
    eloop.exec();
    return reply;
}
QVariant quick_pay_help::getItemValue(cJSON *item)
{
    QVariant ret;
    if(0 == item) {
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

QVariant quick_pay_help::getcJSONvalue(cJSON *root, const char *string)
{
    if(root == 0) return QVariant();

    cJSON *item = cJSON_GetObjectItem(root,string);
    return getItemValue(item);
}

QVariant quick_pay_help::getcJSONvalue(const QByteArray &d, const char *string)
{
    cJSON * p = cJSON_Parse(d.data());
    if(p) {
        QVariant retd = getcJSONvalue(p, string);
        delete p;
        return retd;
    }
    return QVariant();
}

