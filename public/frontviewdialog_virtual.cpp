#include "frontviewdialog_virtual.h"
#include <QtDebug>
#include <QItemSelectionModel>
#include <QtDebug>
#include "lds_query.h"
#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include "lds_messagebox.h"
#include <QProcess>
#include "n_func.h"
#include "backheader.h"
#include "lds_dialog_input.h"
#include "public_logindialog.h"
#include "w_bt_dish_kitchenprinter_printerset.h"

frontViewDialog_virtual::frontViewDialog_virtual(QObject *parent) :
    QObject(parent)
{
}

frontViewDialog_virtual::~frontViewDialog_virtual()
{
}

void frontViewDialog_virtual::static_display_price(const QString &value, const QString &vch_dishname)
{
    public_sql::save_login->qt_display_print_polling->emitsignalTrigger(1, value, vch_dishname);
    //    static_display(1, value,vch_dishname);
}

void frontViewDialog_virtual::static_display_total(const QString &value)
{
    public_sql::save_login->qt_display_print_polling->emitsignalTrigger(2, value);
    //    static_display(2, value);
}

void frontViewDialog_virtual::static_display_cash(const QString &value)
{
    public_sql::save_login->qt_display_print_polling->emitsignalTrigger(3, value);
    //    static_display(3, value);
}

void frontViewDialog_virtual::static_display_change(const QString &value)
{
    public_sql::save_login->qt_display_print_polling->emitsignalTrigger(4, value);
    //    static_display(4, value);
}

void frontViewDialog_virtual::static_display_price(double value, const QString &vch_dishname)
{
    static_display_price(QString::number(value, 'f', 2), vch_dishname);
}

void frontViewDialog_virtual::static_display_total(double value)
{
    static_display_total(QString::number(value, 'f', 2));
}

void frontViewDialog_virtual::static_display_cash(double value)
{
    static_display_cash(QString::number(value, 'f', 2));
}

void frontViewDialog_virtual::static_display_change(double value)
{
    static_display_change(QString::number(value, 'f', 2));
}

void frontViewDialog_virtual::static_display_clear()
{
    if(public_sql::display_led._type!=lds::sysconf->value("system_setting/kexianleixing").toString()){
        public_sql::display_led._type=lds::sysconf->value("system_setting/kexianleixing").toString();
    }
    public_sql::display_led.startShowinfo(lds::sysconf->value("system_setting/kexian_vfd_start").toString(),
                                          lds::sysconf->value("system_setting/kexianduankou").toString());
}

void frontViewDialog_virtual::static_display(int sqr, const QString &value, const QString &vch_dishname)
{
    QString led_path = lds::sysconf->value("system_setting/kexianduankou").toString();
    if(led_path.isEmpty()) return;

    static int static_sqr;
    static QString static_value;
    static QString static_vch_dishname;

    if(static_sqr != sqr || static_value != value || static_vch_dishname != vch_dishname)
    {
        if(public_sql::display_led._type!=lds::sysconf->value("system_setting/kexianleixing").toString()){
            public_sql::display_led._type=lds::sysconf->value("system_setting/kexianleixing").toString();
        }

        //        qDebug() << "static_display:"<<sqr << value << led_path << vch_dishname;
        public_sql::display_led.writeCom(sqr, value,
                                         led_path,
                                         vch_dishname);
        static_sqr = sqr;
        static_value = value;
        static_vch_dishname = vch_dishname;
    }
}

void frontViewDialog_virtual::doublescreensenddataByconf(lds::DOUBLE_DATA ddata)
{
    //不启用副屏、
    if(lds::sysconf->value("system_setting/fuping_mode", -1).toInt() == -1) {
        return;
    }
    //非关闭 && 不显示菜品信息
    if(ddata.udpKey != lds::udp_close_request
            &&false == lds::sysconf->value("system_setting/dd_show_dish_list").toBool()) {
        return;
    }
    lds::doublescreensenddata(ddata);
}
