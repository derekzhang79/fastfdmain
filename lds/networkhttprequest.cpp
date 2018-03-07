#include "networkhttprequest.h"
#include <QtNetwork>
#ifdef Q_OS_UNIX
#include "curl/curl.h"
#endif
#include <string>
#include<qdebug.h>
#include "saas_pay_detail.h"

static size_t OnWriteData(void* buffer, size_t size, size_t nmemb, void* lpVoid)
{
    std::string* str = dynamic_cast<std::string*>((std::string *)lpVoid);
    if( NULL == str || NULL == buffer )
    {
        return -1;
    }

    char* pData = (char*)buffer;
    str->append(pData, size * nmemb);
    return nmemb;
}

networkHttpRequest::networkHttpRequest(QObject *parent) :
    QObject(parent)
{
}

int networkHttpRequest::post(const QString &url, const QList<networkHttpRequest::httpData> &httpd_list, const QString &data, QByteArray &retdata, int mesc, QString *errstring)
{
    QNetworkRequest request;
    foreach (const networkHttpRequest::httpData &d, httpd_list) {
        if(saas_pay_detail::networkSkipHttpData(d, url))
            continue;

        if(d.key == "Content-Type") {
            request.setHeader(QNetworkRequest::ContentTypeHeader, d.value);
        } else if(d.key == "ContentLength") {
            request.setHeader(QNetworkRequest::ContentLengthHeader, d.value.toInt());
        } else {
            request.setRawHeader(d.key, d.value);
        }
        qDebug() << __FUNCTION__<<url << d.key << d.value;
    }
    request.setUrl(url);
    QEventLoop eloop;
    QNetworkAccessManager networkAccessManager;
    QTimer timer;
    timer.setSingleShot(true);
    connect(&timer,SIGNAL(timeout()),&eloop,SLOT(quit()));
    connect(&networkAccessManager,SIGNAL(finished(QNetworkReply*)),&eloop,SLOT(quit()));
    QNetworkReply *reply = networkAccessManager.post(request,data.toLocal8Bit());
    timer.start(mesc);
    eloop.exec();

    int httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    switch(httpStatus) {
    case 200:
        retdata = reply->readAll();
        break;
    case 550:
        retdata = reply->readAll();
        break;
    default:
        if(false == timer.isActive())retdata = "HTTP :" "timeOut";
        else  retdata = "HTTP :" + QByteArray::number(httpStatus);
        if(errstring) *errstring = retdata;
    }
    return httpStatus;
}

int networkHttpRequest::post1(const QString &url, const QList<networkHttpRequest::httpData> &httpd_list, const QString &data, QByteArray &retdata, int mesc, QString *errstring)
{
 #ifdef Q_OS_UNIX
    std::string strPost = QString(data).toStdString();
    std::string strUrl = url.toStdString();
    std::string strResponse;
    CURLcode ret;
    CURL* curl = curl_easy_init();
    if(NULL == curl)
    {
        if(errstring) *errstring = "curl is null";
        return 0;//200 = success
    }

//    {
//        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
//        curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
//    }
    //http请求头
    struct curl_slist *headers = NULL;
    foreach (const networkHttpRequest::httpData &d, httpd_list) {
        if(saas_pay_detail::networkSkipHttpData(d, url))
            continue;

        if(d.key == "Content-Type") {
            headers = curl_slist_append(headers,d.key+":"+d.value);
        } else if(d.key == "ContentLength") {
            headers = curl_slist_append(headers,d.key+":"+d.value);
        } else {
            headers = curl_slist_append(headers,d.key+":"+d.value);
        }

        qDebug() << __FUNCTION__<<url << d.key << d.value;
    }
    qDebug() << __FUNCTION__ << "data" << data;
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 1);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, strPost.c_str());
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strResponse);
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, mesc);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, mesc); //设置cURL允许执行的最长毫秒数。
    ret = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    long httpStatus = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpStatus);
    switch(httpStatus) {
    case 200:
        retdata = QString::fromStdString(strResponse).toLocal8Bit();
        break;
    case 550:
        retdata = QString::fromStdString(strResponse).toLocal8Bit();
        break;
    default:
        retdata = curl_easy_strerror(ret);
        if(retdata == "No error") {
            retdata = "Server no response";
        }
        if(errstring) *errstring = retdata;
    }
    return httpStatus;
#else
    return post(url, httpd_list, data, retdata, mesc);
#endif
}

int networkHttpRequest::get(const QString &url, const QList<networkHttpRequest::httpData> &httpd_list, QByteArray &retdata, int mesc, QString *errstring)
{
    QNetworkRequest request;
    foreach (const networkHttpRequest::httpData &d, httpd_list) {
        if(saas_pay_detail::networkSkipHttpData(d, url))
            continue;
        if(d.key == "Content-Type") {
            request.setHeader(QNetworkRequest::ContentTypeHeader, d.value);
        } else if(d.key == "ContentLength") {
            request.setHeader(QNetworkRequest::ContentLengthHeader, d.value.toInt());
        } else {
            request.setRawHeader(d.key, d.value);
        }
        qDebug() << __FUNCTION__<<url << d.key << d.value;
    }

    request.setUrl(url);
    QEventLoop eloop;
    QNetworkAccessManager networkAccessManager;
    QTimer timer;
    timer.setSingleShot(true);
    connect(&timer,SIGNAL(timeout()),&eloop,SLOT(quit()));
    connect(&networkAccessManager,SIGNAL(finished(QNetworkReply*)),&eloop,SLOT(quit()));
    QNetworkReply *reply = networkAccessManager.get(request);
    timer.start(mesc);
    eloop.exec();

    int httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    switch(httpStatus) {
    case 200:
        retdata = reply->readAll();
        break;
    case 550:
        retdata = reply->readAll();
        break;
    default:
        if(false == timer.isActive()) retdata = "HTTP :""time out";
        else  retdata = "HTTP :" + QByteArray::number(httpStatus)+"    "+reply->errorString().toLocal8Bit();
        if(errstring) *errstring = retdata;
    }

    return httpStatus;
}
