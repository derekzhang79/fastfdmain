//#include "mainwindow.h"
#include <QApplication>
#include <QLabel>
#include <QtDebug>
#include "public_logindialog.h"
#include "backheader.h"
#include "w_i_inventory.h"
#include "w_sys_manage.h"
#include "w_bt_dish.h"
#include "w_bt_dish_kitchenprinter.h"
#include "lds.h"
#include "alipay_help.h"
#include "mainwindow.h"
#include "s610pos.h"
#include "lds_calendarwidget.h"
#include "w_sys_manage_wirelessordermonitoring.h"
#include "lds.h"
#include "lds_messagebox.h"
#include <QProcess>
#include "barscale_main_dialog.h"
#include "ui_w_m_member_sendcard_dialog.h"
#include "printer_pos_serial.h"
#include "w_bt_dish_base.h"
#include "lds_roundeddialog_rect_align.h"
#include "public_pushbutton_numkey.h"
#include "lds_key3x4.h"
#include "electronicscale_rongta.h"
#include "system_setting_mobile_max_connect.h"
#include "printer_pos_net.h"
#include "fexpandmain_2.h"
#include "lds_roundeddialog_rect_align.h"
#include "back_view_dialog.h"
#include "ui_w_scr_dish_restaurant_dialog.h"
#include "w_scr_dish_restaurant_dialog.h"
#include "back_view_dialog.h"
#include "ui_back_view_dialog.h"
#include <QStringList>
#include "w_scr_dish_pay_widget.h"
#include "w_sys_manage_cloudsync_with_time.h"
#include "lds_toolbar_button.h"
#include "w_scr_dish_restaurant_table_select.h"
#include "w_rpt_report_frame_dishsale.h"
#include "w_rpt_report_frame_cashflow.h"
#include "w_rpt_report_frame_cash.h"
#include "w_bt_dish_kitchen_dish_state.h"
#include "lds_int.h"
#include "w_sys_manage_outer_settings.h"
#include "w_sys_manage_cloudsync_rb_order.h"
#include "w_rpt_report_frame_dishsalerank.h"
#include "colordialog.h"
#include "saas_pay_detail.h"
#include "w_sys_manage_cloudsync_cloud2.h"
#include "colordialog.h"
#include "saas_guide.h"
#include "w_scr_dish_pay_widget.h"
#include "w_scr_dish_pay_dialog.h"
#include "saas_guide_hard_test.h"
#include "w_inventory_input.h"
#include "saas_guide_hard_test.h"
#include "w_sys_manage_cloudsync_transfer.h"
#include "saas_guide_direct.h"
#include "saas_guide2.h"
#include "saas_guide_pay.h"
#include "saas_guide_num_key.h"
#include "printer_pos_vidpid.h"
#include "saas_guide_init_soft.h"
#include "fastfd_update.h"
#include "public_showimages.h"
#include "w_scr_dish_main_bar_cancel_win.h"
#ifdef QT_NEW_SYSTEM
#include <QDBusInterface>
#include "frminputnew.h"
#endif
#include "w_inventory_draw.h"
#include "saas_guide_pay_rbcode.h"
#include "commonheader.h"
#include "w_bt_dish_cookset.h"
#include "w_scr_dish_main_table_800x600.h"
#include "w_scr_dish_phone_pay.h"
#include "w_sys_manage_cloudsync_rb_order.h"
#include "w_scr_dish_main_qr_code.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    //新系统 且 unix 且 发布
#if defined(QT_NEW_SYSTEM) && defined(Q_OS_UNIX) && defined(QT_NO_DEBUG)
    frmInputNew::Instance();
#endif
    //配置文件
    QSettings conf(lds::localdataPath+"/fastfd.ini", QSettings::IniFormat);
    lds::sysconf = &conf;
    //~配置文件
    public_sql::    syssetcode();                           //设置编码
    public_sql::    syswindowicon();
    public_sql::    sysstylesheet();                    //icon设置



    lds::               LDS_INIT();
    CommonHeader::com_widget_dir = lds::SYS_LAYOUT_DIRECTION;
    CommonHeader::com_ini_filepath=lds::sysconf->fileName();
    CommonHeader::com_root_dirpath= lds::localdataPath;
    Printer_POS::com_upsiseffect = lds::sysconf->value("system_setting/upsiseffect", false).toBool();
    //判断是否插入加密狗
    if(false == public_sql::winDogCheck(0)) {
        return 0;
    }
    //获取机器号
    QString hardID = lds::getHardWareID_unique();
    if(hardID.length() == 0) {
        lds_messagebox::warning(0, MESSAGE_TITLE_VOID, "LOCAL MACHINE IS EMPTY!");
        return 0;
    }
    lds::setTterminalCode(hardID);

    public_sql::    systranslate();                         //翻译
    //! 0-----------------判断软件是否正常运行
#ifdef Q_OS_UNIX

    //fastfddlist fastfdlist
    QStringList fastfddlist;
    QStringList fastfdlist;
    //相比SoftPos少一个s，以免后面匹配Pos时，多一个
    public_sql::genrateShPid(fastfdlist, "pos/SoftPo", "pos/SoftPos");
    //相比fastfdd少一个d，以免后面匹配Pos时，多一个
    public_sql::genrateShPid(fastfddlist, "pos/fastfd", "pos/fastfdd");

    if(fastfdlist.count() > 1 || fastfddlist.count() >= 1) {//软件软已经启动，并且本次启动fastfdlist>1；副屏启动时
        if(1 == QMessageBox::critical(0, MESSAGE_TITLE_VOID, QObject::tr("软件已启动") +":"+"self"+QString::number(app.applicationPid())+"fastfdlist:"+fastfdlist.join("|")+"fastfddlist:"+fastfddlist.join("|"), QObject::tr("关闭软件"),QObject::tr("退出"))){
            return 0;
        }

        QProcess p;
        //fastfdd异常的话，全部杀死
        foreach(const QString &pid, fastfddlist){
            p.start("kill -9 "+pid);
            p.waitForReadyRead(5*1000);
            p.close();
        }
        //fastfd异常的话，全部杀死
        foreach(const QString &pid, fastfdlist){
            if(pid == QString::number(app.applicationPid())) {//本身最后杀死
                continue;
            }
            p.start("kill -9 "+pid);
            p.waitForReadyRead(5*1000);
            p.close();
        }
        //杀死本身
        p.start("kill -9 "+QString::number(app.applicationPid()));
        p.waitForReadyRead(5*1000);
        p.close();
        return 0;
    }
#endif

    //! 设置字体
    w_sys_manage_outer_settings::SYS_FONT_APP_byconf();

    //! 2-----------------icon设置

    public_sql::    sysdebugmode(argc, argv);               //debug模式，获取usr password debug等信息
    //! 4-----------------软件 初始化
    public_sql::    PUBLIC_SQL_INIT();                      //数据库的初始化
    public_sql::    MySQL_PARAM_INIT();                     //mysql的一些初始化
    //! 4-----------------软件到期自检

    public_loginDialog login;
    //! 5-----------------登录界面
    public_sql::save_login = &login;
    {
        lds_messagebox_loading_show loading(0, "");
        loading.show_delay();
#ifdef QT_DEBUG
#else
        if(false == saas_pay_detail::isSaaS()) {//非saas
            loading.setText("软件到期自检");
            if(false == public_sql::syslimitcheck(0)) return 0;               //软件到期自检
        }
#endif

        //新系统
#if defined(QT_NEW_SYSTEM)
        //! 5 软件版本号
        fastfd_update::setSoftVersiontCyPath(
                    "V" + public_sql::get_MySQL_sqlversion()/*数据库版本*/+"19"/*软件版本*/+"."+"180227"/*修改日期*/
                    );
#else //旧系统
        fastfd_update::setSoftVersiontCyPath(
                    "V" + public_sql::get_MySQL_sqlversion()/*数据库版本*/+"C1"/*软件版本*/+"."+"171201"/*修改日期*/
                    );
#endif
        qDebug() << "gs_soft_version:" << fastfd_update::gs_soft_version;

        //! 6 SaaS
        if(saas_pay_detail::isSaaS())
        {
#ifdef QT_DEBUG
#else
            //saaslimitcheck
            loading.setText(QObject::tr("软件到期自检"), 100);
            if(false == saas_pay_detail::saaslimitcheck(0))
            {
                saas_guide2 d(new saas_guide_hard_test);
                lds_roundeddialog_rect_align da(&d, false);
                if(QDialog::Rejected == da.exec()) {
                    return 0;
                }
                return 0;               //软件到期自检
            }
#endif
        }
        //    软件激活
        if(saas_pay_detail::isSaaS()) {//第二次判断是否为saas
            if(saas_pay_detail::saas_type() == "")
            {
                saas_guide2 d(new saas_guide_direct(0, 0));
                lds_roundeddialog_rect_align da(&d, false);
                if(QDialog::Rejected == da.exec()) {
                    return 0;
                }
            }
            if(saas_pay_detail::saas_type() == "") {
                lds_messagebox::warning(0, MESSAGE_TITLE_VOID, QObject::tr("软件未激活"));
                return 0;
            }
            //广告
#ifdef QT_NO_DEBUG
            loading.setText(QObject::tr("云端广告下载"), 100);
            saas_pay_detail::saas_update_adv(lds::localdataPath+"/image_double_screen_dir" , &loading);
#endif
            //检测软件版本号
            //        loading.setText(QObject::tr("检测软件更新"), 100);
            //        QString version;
            //        bool isdownload;
            //        if(fastfd_update::getNewVersion(version, &isdownload, &errstring)) {
            //            loading.setText("检测软件更新");
            //            if(version <= fastfd_update::gs_soft_version) {
            //                loading.setText("已经是最新版本");
            //            } else {
            //                public_sql::softUpdate();
            //                return 0;
            //            }
            //        }
        }
        //登陆界面ddd
        //! 6-----------------启动
        if(false == public_sql::    sysstart(&login, &loading)) {                       //软件启动
            return 0;
        }
    }

    //! 7-----------------若是启动双屏
    if(lds::sysconf->value("system_setting/fuping_mode", -1).toInt() != -1) {
        //开始
        QString cmd =app.applicationDirPath()+"/fastfdd";
#if defined (Q_OS_UNIX)
#ifdef QT_NO_DEBUG
        QString param = QString("-display Transformed:Rot%1:LinuxFb:/dev/fb1:1 -nomouse -nokeyboard -qws")
                .arg(lds::sysconf->value("system_setting/bofangg_rote", 0).toInt());//为逆时针旋转
        cmd += " "+param;
#endif
        cmd += " &";
        system(cmd.toLocal8Bit().data());
#else
        QProcess *p = new QProcess(public_sql::save_login);
        p->start("\"" + cmd + "\"");
#endif
    }
    return public_sql::save_login->exec();
    bool ret = app.exec();
    return ret;
}
