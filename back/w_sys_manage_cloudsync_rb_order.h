#ifndef W_SYS_MANAGE_CLOUDSYNC_RB_ORDER_H
#define W_SYS_MANAGE_CLOUDSYNC_RB_ORDER_H

#include <QDialog>
#include "lds_query.h"
#include <QtNetwork>

namespace Ui {
class w_sys_manage_cloudsync_rb_order;
}

class w_sys_manage_cloudsync_rb_order : public QDialog
{
    Q_OBJECT
public:
    static QSqlDatabase create_database();
public:
    explicit w_sys_manage_cloudsync_rb_order(QWidget *parent = 0);
    ~w_sys_manage_cloudsync_rb_order();

    void to_get_kichen_date_from_rb_server();
    int toget(QNetworkReply * x, QString *errstring);//数据库报错才会，返回-1


private slots:
    void on_pushButton_clicked();

private:
    Ui::w_sys_manage_cloudsync_rb_order *ui;
private:
    static bool create_table();
};

class rb_order_data{
public:
    rb_order_data();

    void get_kichen_date_from_rb_server();
    bool mk_sn_file(const QString &sn, const QString &orgName, const QString &date);
private:
    bool isrunning;
    class auto_false {
    public:
        auto_false(bool *value) {_value = value;}
        ~auto_false(){*_value = false;}
        bool *_value;
    };
};

#endif // W_SYS_MANAGE_CLOUDSYNC_RB_ORDER_H
