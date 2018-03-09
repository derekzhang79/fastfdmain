#ifndef PUBLIC_SQL_H
#define PUBLIC_SQL_H

#include <QObject>
#include <QSettings>
#include <QDialog>
#include "lds_thread.h"
#include "lds_messagebox.h"
#include <QMap>
#include <QElapsedTimer>
#include <QStandardItemModel>
#include "fexpandmain.h"
#include "lds_query.h"
#include <QFile>
#include "clientdisplay.h"
#include "saas_pay_detail.h"
#ifdef QT_NO_DEBUG
#include <QNoDebug>
#endif
#include "reader_tel.h"
#include "lds_model_sqltablemodel.h"

#define errorStringExec(object, cmd, error) (error = object.cmd?"":object.errorString()).isEmpty()
#define lastErrorExec(object, cmd, error) (error = object.cmd?"":object.lastError().text()).isEmpty()

class fexpandmain_2;
class public_loginDialog;
class fexpandmain;
class fastfd_update;
class public_sql : public QObject
{
    Q_OBJECT

public:
    enum showprinterrID{
        error_no = -1,
        error_kitchen
    };
    enum afterExecType{
        Rejected =                  QDialog::Rejected,
        Accepted =                  QDialog::Accepted,
        Reboot =                    QDialog::Accepted + 1,
        GoFast =                    QDialog::Accepted + 2,
        GoRestaurant =          QDialog::Accepted + 3,
        GoRestaurantOrder =          QDialog::Accepted + 4,
    };
public:
    explicit public_sql(QObject *parent = 0);
    struct MySQL_SQLVERSION_DATA{
        inline MySQL_SQLVERSION_DATA(){}
        inline MySQL_SQLVERSION_DATA(const QStringList &p0, const QString &p1="")
            :normal_sql(p0), desc(p1){}
        QStringList normal_sql;
        QString desc;
    };

    static QList<MySQL_SQLVERSION_DATA> MySQL_sqlversion_data_content_list;
    static QString get_MySQL_sqlversion();
    static QString MySQL_sqlversion_update(QWidget *parent, const QString &fromversion, lds_messagebox_loading_show *loading = 0);
    static bool Other_sql_update();
    static bool Pre_sql_update();
    static bool Pre_sql_update_one(lds_query &query, const QString &tablename, int count);

    static void PUBLIC_SQL_INIT();
    static void PUBLIC_SQL_INIT_DEL();

    static bool fastfd_init_UNZIP();
    static void MySQL_PARAM_INIT();                                 //初始化数据库相关参数
    static void MySQL_PARAM_SAVE();                                 //保存数据库相关参数
    static bool CONNECT_MySQL(lds_messagebox_loading_show *loading);                                    //return 错误信息
    static int MySQL_PARAME_SET(QWidget *parent, const QString &title = "");//DialogCode { Rejected, Accepted };
    static QString MySQL_info;                                      //任何关于数据库操作的信息都会保存到合格变量里

    static QString dbHostName;
    static QString dbDatabaseName;
    static QString dbUserName;
    static QString dbPassword;

    static bool islocal;
    static QString localdbHostName;
    static QString localdbDatabaseName;
    static QString localdbUserName;
    static QString localdbPassword;

    static QString getdbHostName();
    static QString getdbDatabaseName();
    static QString getdbUserName();
    static QString getdbPassword();
    static void setdbHostName(const QString &db_host_name);
    static void setdbDatabaseName(const QString &db_database_name);
    static void setdbUserName(const QString &db_usr_name);
    static void setdbPassword(const QString &db_pwd);

    static QString sql_danwei;                                      //数据库备份文件的拓展名

    static QString show_empty_sql_file_dir;                       //数据库备份文件路径
    static QString show_empty_sql_file;                           //数据库备份文件路径

    static QString show_chinese_sql_file_dir;                       //数据库备份文件路径
    static QString show_chinese_sql_file;                           //数据库备份文件路径

    static QString show_english_sql_file_dir;                       //数据库备份文件路径
    static QString show_english_sql_file;                           //数据库备份文件路径

    static QString sql_backup_dir;                                  //数据库备份默认路径
    static QString sql_table_export_dir;                            //数据库基础数据默认导出路径

    static QString image_dish_dir;                                  //外部图片dish路径
    static QString image_double_screen_dir;                         //外部图片double_screen路径

    static QString settings_txt_soft_limit_dir_file;                //配置文件的软件限时ini，filepath

    static QString ftp_businessdataupload_dir;                      //营业数据上传文件夹

    static bool MySQL_backup(const QString &dbpath, const QString &filename, lds_messagebox_loading_show *loading, const QString &desc);//数据库备份
    static bool MySQL_restore(const QString &dbpath, const QString &filename, lds_messagebox_loading_show *loading, const QString &desc);//数据库还原
    static void WX_POLLING();                                      //点菜包轮询判断状态

    static QString makedir(const QString &dir);
    static QString copyfile(const QString &filefrom, const QString &fileto);
    static QString remove_copyfile(const QString &filefrom, const QString &fileto);
    static bool removefile(const QString &filename);
    static QString err_path_arg;

    static public_loginDialog *save_login;                          //保存前台的指针
    static QObject *save_login_object();                          //保存前台的指针
    static ClientDisplay display_led;
    static QString get_logo_path();

    //【登录信息】
    static const QString gs_operid;
    static const QString gs_opername;
    static const QString gs_password;
    static int gi_productid;
    static QString _u;
    static QString _p;
    static QString _d;


    // 【打印参数】
    static QString get_print_line();
    // 【其他】
    static void systranslate();                                     //系统翻译
    static void syswindowicon();                                       //系统icon
    static void sysstylesheet();                                //
    static bool winDogCheck(QWidget *parent);
    static bool syslimitcheck(QWidget *parent);                                    //软件到期自检
    static bool sysstart(public_loginDialog *login, lds_messagebox_loading_show *loading);                //软件启动
    static void syssetcode();                                       //软件编码
    static void sysdebugmode(int argc, char *argv[]);                                     //软件debug模式
    static void genrateShPid(QStringList &pids, const QString &psFilter, const QString &exactFilter);
#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
    static void customMessageHandler(QtMsgType type, const char *msg);
#else
    static void customMessageHandler(QtMsgType type, const QMessageLogContext &msg, const QString &text);
#endif

    static fexpandmain_2 *widgetlist;
    //配置文件

    //报错信息
    static QString msg_sql_error;
    static QElapsedTimer etimer;
    static QString getTablenoByChbillno(const QString &ch_billno);
    static float getTotalBy_ch_billno(const QString &ch_billno);
    static QString getTableName(const QString &ch_tableno);
    //中餐下的预打信息
    static QMap<QString/*ch_tableno*/, bool> preprinttablemap;
    static QString mysqlinstallpath;
    static QMap<QString, QString> sqlkeeptracemap;

    //云同步sql语句
    static void action_sql_debug();
    static void action_sql_debug_piece(const QString &tablename);
    //    static QString get_sqlerror(qsqlta);
    //报错信息
    static const QString alipay_sys_service_provider_id;
    //快捷键
    static QStandardItemModel OPER_KEY_MODEL;
    static void OPER_KEY_MODEL_INIT();

    static int get_god_password(QWidget *parent, const QString &title,  QString *errstring = 0);
    static bool get_password(QWidget *parent, const QString &title, QString &password);

    //
    static void softUpdate();

    //
    static QString discount_off_mode();
    static QString discount_off_mode(const lds_model_sqltablemodel *tablemodel);

    static bool localContainIp(const QString &ip);
};

//数据库设置
class Ui_public_sqlset_Dialog;
class public_loginDialog;
class public_sqlSet:public QDialog{
    Q_OBJECT
public:
    explicit public_sqlSet(QWidget *parent = 0);

    static bool privileges(QString *errstring = 0);
    static bool privileges(lds_query_db *query);
private slots:
    void took();
    void toexit();
    void switch_server_loacl(bool flag);
    void tohddpos_yaoqianshu();
    void updatelocalpath();
    void enable_remote_MYSQL_connect();
private:
    Ui_public_sqlset_Dialog *ui;
};

//============================
class WX_POLLING_thread:public lds_thread
{
    Q_OBJECT
public:
    WX_POLLING_thread(QObject *parent = 0);
    ~WX_POLLING_thread();
protected:
    virtual void run();
};
//=====================================
class TelBox_thread_event : public lds_thread_event
{
    Q_OBJECT
public:
    TelBox_thread_event(QObject *parent = 0);
    static info_tel readTel();
public slots:
    virtual void toExec() ;
};

class TelBox_thread : public lds_thread
{
    Q_OBJECT
public:
    TelBox_thread(QObject *parent = 0);
    //from QThread
protected:
    virtual void run();
};

#endif // PUBLIC_SQL_H
