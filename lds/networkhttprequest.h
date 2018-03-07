#ifndef NETWORKHTTPREQUEST_H
#define NETWORKHTTPREQUEST_H

#include <QObject>
#include <QNetworkRequest>


class networkHttpRequest : public QObject
{
    Q_OBJECT
public:
    struct httpData{
        inline httpData(){}
        inline httpData(const QByteArray &k, const QByteArray &v){
            this->key = k;
            this->value = v;
        }
        QByteArray key;
        QByteArray value;
    };

public:
    explicit networkHttpRequest(QObject *parent = 0);

public :
    static int post(const QString &url, const QList<httpData> &httpd_list, const QString &data, QByteArray &retdata,  int mesc = 20*60*1000, QString *errstring = 0);
    static int post1(const QString &url, const QList<httpData> &httpd_list, const QString &data, QByteArray &retdata,  int mesc = 20*60*1000, QString *errstring = 0);

    static int get(const QString &url, const QList<httpData> &httpd_list, QByteArray &retdata,  int mesc = 20*60*1000, QString *errstring = 0);
private:
};

#endif // NETWORKHTTPREQUEST_H
