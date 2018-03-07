#include "w_sys_manage_cloudsync_transfer.h"
#include <QtDebug>
#include <QTimer>
#include "w_sys_manage_cloudsync.h"
#include "networkhttprequest.h"
#include "saas_pay_detail.h"
#include "cjson_help.h"
#include <QFile>
#include <QEventLoop>

w_sys_manage_cloudsync_transfer::w_sys_manage_cloudsync_transfer(QObject *parent) :
    QObject(parent)
{
}

cJSONHttpData w_sys_manage_cloudsync_transfer::sendPost2(const QString &url, const QString &param, const QMap<QString, QString> &headInfo, QString &retparam, int mesc, QString *errstring)
{
    cJSONHttpData httpd;
    QByteArray  strResponse;
    QList<networkHttpRequest::httpData> httpdList;
    QMap<QString, QString>::const_iterator i;
    httpdList << networkHttpRequest::httpData("Content-Type", "application/json;charset=utf-8");

    if(saas_pay_detail::isSaaS())
        httpdList << networkHttpRequest::httpData("terminalCode", saas_pay_detail::terminalCodeGet().toLocal8Bit());

    for( i=headInfo.constBegin(); i!=headInfo.constEnd(); ++i) {
        httpdList << networkHttpRequest::httpData(i.key().toLocal8Bit(), i.value().toLocal8Bit());
    }

    httpd.httpStatus = networkHttpRequest::post1(saas_pay_detail::networkFilterHttpUrl(url), httpdList, param, strResponse, mesc, errstring);
    switch(httpd.httpStatus){
    case 200:
        retparam =strResponse;
        httpd.json=cJSON_Parse(retparam.toLocal8Bit().data());
        break;
    case 550:
        retparam =strResponse;
        httpd.json=cJSON_Parse(retparam.toLocal8Bit().data());
        httpd.message= cJSON_help::getcJSONvalue(httpd.json, "message").toString();
        httpd.code= cJSON_help::getcJSONvalue(httpd.json, "code").toString();

        cJSON_Delete(httpd.json);
        httpd.json = 0;

        if(errstring) *errstring = httpd.message;
        break;
    default:
        httpd.message=strResponse;
    }
    qDebug() << __FUNCTION__ << "httpStatus:" << httpd.httpStatus<< httpd.code << httpd.message;
    return httpd;
}
/**
若是错误，则返回retparam，包含错误信息
*/
cJSONHttpData w_sys_manage_cloudsync_transfer::sendGet(const QString &url, const QMap<QString, QString> &headInfo, QString &retparam, int mesc, QString *errstring)
{
    cJSONHttpData httpd;
    QByteArray  strResponse;
    QList<networkHttpRequest::httpData> httpdList;
    QMap<QString, QString>::const_iterator i;
    httpdList << networkHttpRequest::httpData("Content-Type", "application/json;charset=utf-8");
    if(saas_pay_detail::isSaaS()) {
        httpdList << networkHttpRequest::httpData("terminalCode", saas_pay_detail::terminalCodeGet().toLocal8Bit());
    }
    for( i=headInfo.constBegin(); i!=headInfo.constEnd(); ++i) {
        httpdList << networkHttpRequest::httpData(i.key().toLocal8Bit(), i.value().toLocal8Bit());
    }

    httpd.httpStatus = networkHttpRequest::get(saas_pay_detail::networkFilterHttpUrl(url), httpdList, strResponse, mesc, errstring);
    switch(httpd.httpStatus){
    case 200:
        retparam =strResponse;
        httpd.json=cJSON_Parse(retparam.toLocal8Bit().data());
        break;
    case 550:
        retparam =strResponse;
        httpd.json=cJSON_Parse(retparam.toLocal8Bit().data());
        httpd.message= cJSON_help::getcJSONvalue(httpd.json, "message").toString();
        httpd.code = cJSON_help::getcJSONvalue(httpd.json, "code").toString();

        cJSON_Delete(httpd.json);
        httpd.json = 0;

        if(errstring) *errstring = httpd.message;
        break;
    default:
        httpd.message=strResponse;
    }

    qDebug() << __FUNCTION__ << "httpStatus:" << httpd.httpStatus<< httpd.code << httpd.message;
    return httpd;
}

QNetworkReply *w_sys_manage_cloudsync_transfer::directGet(QNetworkAccessManager *networkAccessManager, const QUrl &url)
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
    QNetworkReply *reply = networkAccessManager->get(request);
    timer.start(mesc);
    eloop.exec();
    return reply;
}
