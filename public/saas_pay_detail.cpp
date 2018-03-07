#include "saas_pay_detail.h"
#include "ui_saas_pay_detail.h"
#include <QStandardItemModel>
#include <QtDebug>
#include <QTimer>
#include "lds_messagebox.h"
#include "lds_query.h"
#include "lds.h"
#include "lds.h"
#include "saas_guide.h"
#include "cjson_help.h"
#include "saas_guide_activateterminal.h"
#include <QtDebug>
#include <QHostInfo>
#include "saas_guide2.h"
#include "saas_guide_direct.h"
#include <QPainter>
#include "n_func.h"
#include "public_sql.h"
#include <QDir>
QWidgetList saas_pay_detail::signWidgetList;
int saas_pay_detail::signWidgetLight = 0;

saas_pay_detail::saas_pay_detail(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::saas_pay_detail)
{
    ui->setupUi(this);
    ui->tableView->setModel(saasModel());

    resize(700, 400);

    QTimer::singleShot(0, ui->tableView, SLOT(resizeColumnsToContents()));

    connect(ui->pushButton_exit, SIGNAL(clicked()),this,SLOT(toexit()));
    connect(ui->pushButton_ok, SIGNAL(clicked()) ,this,SLOT(took()));
}

saas_pay_detail::~saas_pay_detail()
{
    delete ui;
}

bool saas_pay_detail::isSaaSAA()
{
    return terminalCodeGet().startsWith("AA")
            && "" == saas_type();
}

bool saas_pay_detail::isSaaSAA21()
{
    return terminalCodeGet().startsWith("AA")
            && "21" == saas_type();
}

bool saas_pay_detail::isSaaSFA20()
{
    return terminalCodeGet().startsWith("FA")
            && ("" == saas_type()/*未激活*/ || "20" == saas_type()/*已激活*/);
}

bool saas_pay_detail::isSaaSFA21()
{
    return terminalCodeGet().startsWith("FA")
            && "21" == saas_type();
}

bool saas_pay_detail::isSaaS()
{
    //    return saas_type() != ""
    //            || terminalCodeGet().startsWith("FA");
    return isSaaSAA21()
            || isSaaSFA20()
            || isSaaSFA21();
}

bool saas_pay_detail::btnlanguageswicthCan()
{
    if(isSaaSFA20())//标准版
        return false;
    return true;//专业版或普通版
}

void saas_pay_detail::mobile_connect_max_value_set_zero()//开机强制
{
    if(isSaaSFA20()){
        lds_query query;
        query.execSelect("select  vch_value from cey_sys_parameter  where vch_parameter = 'mobile_connect_max_value' ");
        if(!query.next()) {
            n_func::f_set_sysparm("mobile_connect_max_value", 0);
        }
        qDebug() << "若是标准版,则强制连接数为0";
    }
}

bool saas_pay_detail::SaaSFuncCheck(QWidget *parent, const QString & code)
{
    int saas_ret = 0;
    QString SaaS_title;
    //普通版
    if(isSaaSAA()) {
        return true;
    }
    //专业版
    if(isSaaSAA21()){
        return true;
    }
    //标准版
    if(isSaaSFA20()){
        QAbstractItemModel *model = saasModel();
        saas_guide::getTerminal(parent);

        for(int row = 0; row < model->rowCount(); row ++) {
            if(model->data(model->index(row, 0)).toString() == code) {
                if("0201" == code) {//餐桌数量
                    lds_query query;
                    query.execSelect(QString("SELECT count(0) from cey_bt_table "));
                    query.next();
                    if(query.recordValue(0).toInt() >= 10) {
                        SaaS_title = model->data(model->index(row, 2)).toString()+":"+model->data(model->index(row, 3)).toString();
                        goto saasUpdate;
                    }
                }  else if("0508" == code) {
                    lds_query query;
                    query.execSelect(QString("SELECT count(0) FROM cey_bt_kitchen_plan; "));
                    query.next();
                    if(query.recordValue(0).toInt() > 0) {
                        SaaS_title = model->data(model->index(row, 2)).toString()+":"+model->data(model->index(row, 3)).toString();
                        goto saasUpdate;
                    }
                } else {//其他
                    SaaS_title = model->data(model->index(row, 2)).toString()+":"+model->data(model->index(row, 3)).toString();
                    goto saasUpdate;
                }
            }
            //
        }//for
    }
    //saas专业版
    if(isSaaSFA21()){
        return true;
    }

    return true;
saasUpdate:
    while(1 ==  (saas_ret = lds_messagebox::information(parent
                                                        , SaaS_title
                                                        , tr("终端号")+":"+saas_pay_detail::terminalCodeGet() + "\n" + tr("您目前使用的是标准版本，无法使用更高级的功能，85%的用户已经升级了专业版；享受更强大的专业版软件，支持多语言系统自如切换，支持区域厨打及手机、平板点菜、结算、支付，各类促销导流功能及其它更激动人心的功能，请抓紧升级专业版。")
                                                        , tr("升级"), tr("了解专业版"), tr("取消")))) {
        //saas_guide_direct
        saas_guide2 d(new saas_guide_direct(3, 0), parent);
        d.only_show_exit();
        lds_roundeddialog_rect_align(&d, false).exec();
    }
    if(0 == saas_ret) {
        //saas_guide_pay
        saas_guide2 d(new saas_guide_pay_update, parent);
        if(QDialog::Rejected == lds_roundeddialog_rect_align(&d, false).exec()) {
            return false;
        }
        ///从云端读取终端权限、确认升级成功
        cJSONHttpData httpd = saas_guide::getTerminal(parent);
        if(200 == httpd.httpStatus
                && "21" == saas_type())
        {
            lds_messagebox::information(parent, MESSAGE_TITLE_VOID, tr("操作成功"));
            return true;
        }
        lds_messagebox::information(parent, MESSAGE_TITLE_VOID, tr("操作失败"));
    }

    return false;
}

bool saas_pay_detail::networkSkipHttpData(const networkHttpRequest::httpData &d, const QString &url)
{
    bool ret_skip =
            d.key == "terminalCode";
    if(isSaaSAA()) {
        return ret_skip;
    }

    bool SaaS_ret_skip =
            d.key == "orgCode"
            || d.key == "userName"
            || d.key == "password"
            ;/*
    if(url.endsWith("arm/dish-base/add-rest") ) {
        return SaaS_ret_skip;
    }
    if(url.endsWith("arm/dish-base/update-rest") ) {
        return SaaS_ret_skip;
    }
    if(url.endsWith("arm/dish-base/get-rest") ) {
        return SaaS_ret_skip;
    }
    if(url.endsWith("arm/dish-flow/add-rest") ) {
        return SaaS_ret_skip;
    }*/

    return SaaS_ret_skip;
}

QString saas_pay_detail::networkFilterHttpUrl(const QString &url)
{
    if(isSaaSAA()) {
        return url;
    }
    //scheme + "://"+host+path;
    QUrl u(url);
    QString scheme = u.scheme();
    QString host = u.host();
    QString path = u.path();
    QString old_yun_prefix = "/CateSystem";

    host = saas_Host();

    if(path.startsWith(old_yun_prefix)) {
        path.remove(0, old_yun_prefix.length());
    }
    return scheme + "://"+host + path;
}

QString saas_pay_detail::terminalCodeGet()
{
    return lds::terminalCode;
}

QString saas_pay_detail::hostNameGet()
{
    return QHostInfo::localHostName();
}

QString saas_pay_detail::terminalTypeGet()
{
    if(terminalCodeGet().startsWith("FA")) {
        return "FA";//saas专业版
    } else {
        return "AA";//专业版
    }
    return "";
}

QString saas_pay_detail::terminalSytemGet()
{
    //00餐饮linux   01餐饮PC   02餐饮android
#ifdef Q_OS_UNIX
    return "00";
#endif

#ifdef Q_OS_WIN
    return "01";
#endif

#ifdef Q_OS_ANDROID
    return "02";
#endif

    return "";
}

const QMap<QString, QString> &saas_pay_detail::codeSaaSMapGet()
{
    static QMap<QString, QString> map;
    if(map.isEmpty()) {
        map.insert("20", tr("标准版"));
        map.insert("21", tr("专业版"));
    }
    return map;
}

QString saas_pay_detail::saasDesc()
{
    if(isSaaSAA()) return tr("普通版");
    if(isSaaSAA21()) return tr("经典") + tr("专业版");
    if(isSaaSFA20()) return "SaaS"+tr("标准版");
    if(isSaaSFA21()) return "SaaS"+tr("专业版");

    return "";
}

bool saas_pay_detail::saaslimitcheck(QWidget *parent)
{
    //判断有效期
    cJSONHttpData httpd = saas_guide::getTerminal(parent);

    if(200 == httpd.httpStatus)
    {
        QDateTime curDate = QDateTime::currentDateTime();
        if(saas_pay_detail::saas_dueDate().isValid() == false) {
            lds_messagebox::warning(parent, MESSAGE_TITLE_VOID,
                                    tr("终端号") + ":" + saas_pay_detail::terminalCodeGet() + "\n" +
                                    tr("已连接云端")+"\n"+
                                    tr("截止日期格式错误") + "\n" +
                                    tr("请登录云端核实"));
            return false;
        }
        int remainDay = curDate.daysTo(saas_pay_detail::saas_dueDate()) ;
        if(remainDay >= 0 && remainDay < 30) {
            if(0 == lds_messagebox::question(parent, MESSAGE_TITLE_VOID,
                                             tr("终端号") + ":" + saas_pay_detail::terminalCodeGet() + "\n" +
                                             tr("已连接云端")+"\n"+
                                             tr("使用时间不足30天")+"\n"+
                                             tr("当前日期") + ":" + curDate.toString("yyyy-MM-dd hh:mm:ss")+"\n"+
                                             tr("截止日期") + ":" + saas_pay_detail::saas_dueDate_str(),
                                             tr("立刻购买"), tr("继续使用"))) {
                saas_guide2 d(new saas_guide_pay, 0);
                if(QDialog::Rejected == lds_roundeddialog_rect_align(&d, false).exec()) {
                    return false;
                }
            }
        }
        if(remainDay < 0) {
            if(0 == lds_messagebox::warning(parent, MESSAGE_TITLE_VOID,
                                            tr("终端号") + ":" + saas_pay_detail::terminalCodeGet() + "\n" +
                                            tr("已连接云端")+"\n"+
                                            tr("使用时间不足")+"\n"+
                                            tr("当前日期") + ":" + curDate.toString("yyyy-MM-dd hh:mm:ss")+"\n"+
                                            tr("截止日期") + ":" + saas_pay_detail::saas_dueDate_str(),
                                            tr("立刻购买"), tr("退出软件"))) {
                saas_guide2 d(new saas_guide_pay, 0);
                if(QDialog::Rejected == lds_roundeddialog_rect_align(&d, false).exec()) {
                    return false;
                }
            } else {
                return false;
            }
        }
    } else if(550 == httpd.httpStatus) {
        if("-103" == httpd.code) {//终端已过有效期
            lds_messagebox::warning(parent, MESSAGE_TITLE_VOID,
                                    tr("终端号") + ":" + saas_pay_detail::terminalCodeGet() + "\n" +
                                    httpd.message);
            saas_guide2 d(new saas_guide_pay, 0);
            if(QDialog::Rejected == lds_roundeddialog_rect_align(&d, false).exec()) {
                return false;
            }
        }
        if("-105" == httpd.code) {//终端已挂起
            lds_messagebox::warning(parent, MESSAGE_TITLE_VOID,
                                    tr("终端号") + ":" + saas_pay_detail::terminalCodeGet() + "\n" +
                                    httpd.message + "\n" +
                                    tr("请登录云端核实"));
            return false;
        }
    } else {//离线
        QDateTime curDate = QDateTime::currentDateTime();
        if(saas_pay_detail::saas_dueDate().isNull()) {
            lds_messagebox::information(parent, MESSAGE_TITLE_VOID,
                                        tr("终端号") + ":" + saas_pay_detail::terminalCodeGet() + "\n" +
                                        tr("未连接云端")
                                        );
            return false;
        }
        int remainDay = curDate.daysTo(saas_pay_detail::saas_dueDate()) ;
        if(remainDay >= 0 && remainDay < 30) {
            lds_messagebox::information(parent, MESSAGE_TITLE_VOID,
                                        tr("终端号") + ":" + saas_pay_detail::terminalCodeGet() + "\n" +
                                        tr("未连接云端")+"\n"+
                                        tr("使用时间不足30天")+"\n"+
                                        tr("当前日期") + ":" + curDate.toString("yyyy-MM-dd hh:mm:ss")+"\n"+
                                        tr("截止日期") + ":" + saas_pay_detail::saas_dueDate_str() +"\n"+
                                        tr("请及时续期"));
        }
        if(remainDay < 0) {
            lds_messagebox::warning(parent, MESSAGE_TITLE_VOID,
                                    tr("终端号") + ":" + saas_pay_detail::terminalCodeGet() + "\n" +
                                    tr("未连接云端")+"\n"+
                                    tr("使用时间不足")+"\n"+
                                    tr("当前日期") + ":" + curDate.toString("yyyy-MM-dd hh:mm:ss")+"\n"+
                                    tr("截止日期") + ":" + saas_pay_detail::saas_dueDate_str() +"\n"+
                                    tr("请及时续期"));
            return false;
        }
    }
    return true;
}

QStandardItemModel *saas_pay_detail::saasModel()
{
    static QStandardItemModel m;
    if(m.rowCount() == 0) {
        QStandardItemModel *model = &m;
        model->setHorizontalHeaderLabels(QStringList() <<  tr("代码") << tr("分类") << tr("项目") << tr("标准版") << tr("专业版"));

        QList<QStandardItem*> items;
        /*OK*/  items.clear(); items<< new QStandardItem("0101") << new QStandardItem("前台点单")<< new QStandardItem(tr("手牌号"))<< new QStandardItem(tr("不支持"))<< new QStandardItem(tr("支持"));
        model->appendRow(items);
        /**/  items.clear(); items<< new QStandardItem("0102") << new QStandardItem("前台点单")<< new QStandardItem(tr("点单叫号"))<< new QStandardItem(tr("不支持"))<< new QStandardItem(tr("支持"));
        model->appendRow(items);
        /*OK*/  items.clear(); items<< new QStandardItem("0201") << new QStandardItem("中餐模式")<< new QStandardItem(tr("餐桌数量"))<< new QStandardItem(tr("10台"))<< new QStandardItem(tr("不限制"));
        model->appendRow(items);
        /*OK*/  items.clear(); items<< new QStandardItem("0202") << new QStandardItem("中餐模式")<< new QStandardItem(tr("餐桌地图模式"))<< new QStandardItem(tr("不支持"))<< new QStandardItem(tr("支持"));
        model->appendRow(items);
        /*OK*/  items.clear(); items<< new QStandardItem("0203") << new QStandardItem("中餐模式")<< new QStandardItem(tr("本地餐桌预定"))<< new QStandardItem(tr("不支持"))<< new QStandardItem(tr("支持"));
        model->appendRow(items);
        /*OK*/  items.clear(); items<< new QStandardItem("0301") << new QStandardItem("会员设置")<< new QStandardItem(tr("云、本地会员积分"))<< new QStandardItem(tr("不支持"))<< new QStandardItem(tr("支持"));
        model->appendRow(items);
        /*OK*/  items.clear(); items<< new QStandardItem("0302") << new QStandardItem("会员设置")<< new QStandardItem(tr("云、本地会员积分兑换"))<< new QStandardItem(tr("不支持"))<< new QStandardItem(tr("支持"));
        model->appendRow(items);
        /*OK*/  items.clear(); items<< new QStandardItem("0303") << new QStandardItem("会员设置")<< new QStandardItem(tr("云、本地会员充值"))<< new QStandardItem(tr("不支持"))<< new QStandardItem(tr("支持"));
        model->appendRow(items);
        /**/  items.clear(); items<< new QStandardItem("0304") << new QStandardItem("会员设置")<< new QStandardItem(tr("微会员？")) << new QStandardItem(tr("不支持"))<< new QStandardItem(tr("支持"));
        model->appendRow(items);
        /*OK*/  items.clear(); items<< new QStandardItem("0401") << new QStandardItem("基础资料")<< new QStandardItem(tr("套菜"))<< new QStandardItem(tr("不支持"))<< new QStandardItem(tr("支持"));
        model->appendRow(items);
        /*OK*/  items.clear(); items<< new QStandardItem("0402") << new QStandardItem("基础资料")<< new QStandardItem(tr("库存管理"))<< new QStandardItem(tr("不支持"))<< new QStandardItem(tr("支持"));
        model->appendRow(items);
        /*OK*/  items.clear(); items<< new QStandardItem("0403") << new QStandardItem("基础资料")<< new QStandardItem(tr("菜品原料"))<< new QStandardItem(tr("不支持"))<< new QStandardItem(tr("支持"));
        model->appendRow(items);
        /*OK*/  items.clear(); items<< new QStandardItem("0404") << new QStandardItem("基础资料")<< new QStandardItem(tr("菜品时价菜"))<< new QStandardItem(tr("不支持"))<< new QStandardItem(tr("支持"));
        model->appendRow(items);
        /*OK*/  items.clear(); items<< new QStandardItem("0405") << new QStandardItem("基础资料")<< new QStandardItem(tr("菜品临时菜"))<< new QStandardItem(tr("不支持"))<< new QStandardItem(tr("支持"));
        model->appendRow(items);
        /*OK*/  items.clear(); items<< new QStandardItem("0406") << new QStandardItem("基础资料")<< new QStandardItem(tr("菜品沽清"))<< new QStandardItem(tr("不支持"))<< new QStandardItem(tr("支持"));
        model->appendRow(items);
        /*OK*/  items.clear(); items<< new QStandardItem("0407") << new QStandardItem("基础资料")<< new QStandardItem(tr("菜品服务费"))<< new QStandardItem(tr("不支持"))<< new QStandardItem(tr("支持"));
        model->appendRow(items);
        /*OK*/  items.clear(); items<< new QStandardItem("0408") << new QStandardItem("基础资料")<< new QStandardItem(tr("菜品营业员提成"))<< new QStandardItem(tr("不支持"))<< new QStandardItem(tr("支持"));
        model->appendRow(items);
        /**/  items.clear(); items<< new QStandardItem("0501") << new QStandardItem("系统设置")<< new QStandardItem(tr("优惠券？"))<< new QStandardItem(tr("不支持"))<< new QStandardItem(tr("支持"));
        model->appendRow(items);
        /*OK*/  items.clear(); items<< new QStandardItem("0502") << new QStandardItem("系统设置")<< new QStandardItem(tr("点菜宝"))<< new QStandardItem(tr("不支持"))<< new QStandardItem(tr("支持"));
        model->appendRow(items);
        /*OK*/  items.clear(); items<< new QStandardItem("0503") << new QStandardItem("系统设置")<< new QStandardItem(tr("手机最大连接数"))<< new QStandardItem(tr("固定0"))<< new QStandardItem(tr("可以修改"));
        model->appendRow(items);
        /**/  items.clear(); items<< new QStandardItem("0504") << new QStandardItem("系统设置")<< new QStandardItem(tr("双屏"))<< new QStandardItem(tr("广告只能读取云端"))<< new QStandardItem(tr("广告可以自定义"));
        model->appendRow(items);
        /*OK*/  items.clear(); items<< new QStandardItem("0505") << new QStandardItem("系统设置")<< new QStandardItem(tr("促销"))<< new QStandardItem(tr("不支持"))<< new QStandardItem(tr("支持"));
        model->appendRow(items);
        /*OK*/  items.clear(); items<< new QStandardItem("0506") << new QStandardItem("系统设置")<< new QStandardItem(tr("结账打印格式"))<< new QStandardItem(tr("没法修改"))<< new QStandardItem(tr("可以修改"));
        model->appendRow(items);
        /*OK*/  items.clear(); items<< new QStandardItem("0507") << new QStandardItem("系统设置")<< new QStandardItem(tr("标签格式"))<< new QStandardItem(tr("不能修改"))<< new QStandardItem(tr("可以修改"));
        model->appendRow(items);
        /*OK*/  items.clear(); items<< new QStandardItem("0508") << new QStandardItem("系统设置")<< new QStandardItem(tr("厨房打印机数量"))<< new QStandardItem(tr("只能一台厨打"))<< new QStandardItem(tr("不限制"));
        model->appendRow(items);
        /*OK*/  items.clear(); items<< new QStandardItem("0509") << new QStandardItem("系统设置")<< new QStandardItem(tr("区域厨打"))<< new QStandardItem(tr("不支持"))<< new QStandardItem(tr("支持"));
        model->appendRow(items);
        /**/  items.clear(); items<< new QStandardItem("0510") << new QStandardItem("系统设置")<< new QStandardItem(tr("厨显"))<< new QStandardItem(tr("不支持"))<< new QStandardItem(tr("支持"));
        model->appendRow(items);
        /**/  items.clear(); items<< new QStandardItem("0511") << new QStandardItem("系统设置")<< new QStandardItem(tr("来电弹屏"))<< new QStandardItem(tr("不支持"))<< new QStandardItem(tr("支持"));
        model->appendRow(items);
        /**/  items.clear(); items<< new QStandardItem("0512") << new QStandardItem("系统设置")<< new QStandardItem(tr("多语言"))<< new QStandardItem(tr("不支持"))<< new QStandardItem(tr("支持"));
        model->appendRow(items);


        //        标准版：移动最大连接数为0，
        //        多语言： 厨打语言、结算单语言、点单语言
        //        沽清：菜品修改，和沽清报表的批量修改


        //避免重复
#ifdef QT_DEBUG
        QSet<QString> code_set;
        for(int row = 0; row < model->rowCount(); row ++) {
            code_set.insert(model->item(row, 0)->text());
        }
        if(code_set.size() != model->rowCount()) {
            qDebug("model rowcount must equal code_set count");
            QObject *o=0;o->children();
        }
#endif
    }

    //    //debug
    //    QString line;
    //    for(int c = 0; c < m.columnCount(); c++) {
    //        line +=  m.horizontalHeaderItem(c)->text() + "\t";
    //    }
    //    qDebug() << line;
    //    for(int r = 0; r < m.rowCount(); r++) {
    //        QString line;
    //        for(int c = 0; c < m.columnCount(); c++) {
    //            if(m.item(r,c ))
    //                line +=  m.item(r, c)->text()+"\t";
    //        }
    //        qDebug() << line;
    //    }
    //    //~debug
    return &m;
}

QVariant saas_pay_detail::saasValue(const QString &key, const QVariant &defaultValue)
{
#if defined(QT_DEBUG)
    QSettings settings("saas_type",QSettings::IniFormat);
    return settings.value(key, defaultValue);
#elif defined(Q_OS_UNIX)
    QSettings settings("/usr/desk/pos/saas_type", QSettings::IniFormat);
    return settings.value(key, defaultValue);
#elif defined(Q_OS_WIN)
    QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\fastfd", QSettings::NativeFormat);
    return settings.value(key, defaultValue);
#endif
    return QVariant();
}

void saas_pay_detail::saasSetValue(const QString &key, const QVariant &value)
{
#if defined(QT_DEBUG)
    QSettings settings("saas_type",QSettings::IniFormat);
    settings.setValue(key, value);
#elif defined(Q_OS_UNIX)
    QSettings settings("/usr/desk/pos/saas_type", QSettings::IniFormat);
    settings.setValue(key, value);
#elif defined(Q_OS_WIN)
    QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\fastfd", QSettings::NativeFormat);
    settings.setValue(key, value);
#endif
}
//AA78937493                       FA12321423
//"", "AA"                               "20", "21",
//"", "20", "21", "AA"
int saas_pay_detail::saas_lan_num()
{
    return saas_pay_detail::saasValue("saas/saas_lan_num").toInt();
}

QString saas_pay_detail::saas_type()
{
    return saas_pay_detail::saasValue("saas/saas_type").toString();
}

QDateTime saas_pay_detail::saas_dueDate()
{
    return saas_pay_detail::saasValue("saas/saas_dueDate").toDateTime();
}

QString saas_pay_detail::saas_dueDate_str()
{
    return saas_dueDate().toString("yyyy-MM-dd hh:mm:ss");
}

QDateTime saas_pay_detail::saas_online_last_run()
{
    return saas_pay_detail::saasValue("saas/saas_online_last_run").toDateTime();
}

QString saas_pay_detail::appName()
{
    return lds::appName();
}

QString saas_pay_detail::saas_Host()
{
    QString host;
    host = "www.yqspos.com";
    //    host = "115.28.155.165";
#ifdef QT_DEBUG
    //    host = "www.yqspos.com/CateSystem";
    //    host = "192.168.1.103";
//        host = "192.168.1.81";
#endif
    return host;
}

bool saas_pay_detail::saas_update_adv(const QString &dir, lds_messagebox_loading_show *loading)
{
    if(false == QDir().exists(dir)) {
        QDir().mkpath(dir);
    }
    QString errstring;
    QSettings conf(dir+"/conf.ini", QSettings::IniFormat);
    QString version = QString::number(conf.value("advertiisement/version", "0").toInt() + 1);//最大值,版本号为1

    if(QDir(dir).entryList().count() <= 3/*.\..*/) {//若是空的文件夹,则版本号重置
        version = "1";
    }
    QString url = "http://"+saas_pay_detail::saas_Host()+"/arm/store/advertiisement-rest";
    QMap<QString, QString> map;
    map.insert("version", version);
    w_sys_manage_cloudsync_transfer cloud_transfer;
    int mesc = 30*1000;
    QString retparam;
    cJSONHttpData httpd = cloud_transfer.sendGet(url, map, retparam, mesc, &errstring);
    if(200 == httpd.httpStatus) {
        cJSON *array = cJSON_GetObjectItem(httpd.json, "advertiisement");
        version = cJSON_help::getcJSONvalue(httpd.json, "version").toString();
        for(int k = 0, count = cJSON_GetArraySize(array); k < count; k++) {
            cJSON *item = cJSON_GetArrayItem(array, k);
            QString imag_url = cJSON_help::getcJSONvalue(item, "img").toString();
            QByteArray strResponse;
            QList<networkHttpRequest::httpData> httpd_list;
            if(200 == networkHttpRequest::get(imag_url, httpd_list, strResponse, mesc, &errstring) ){
                QFile file(dir + "/" + QFileInfo(imag_url).fileName());
                if(errorStringExec(file, open(QFile::WriteOnly), errstring)) {
                    file.write(strResponse);
                    file.close();
                    continue;
                }
            }
            loading->setText(errstring, 3000);
        }
        conf.setValue("advertiisement/version", version);
        return true;
    }
    return false;
}

//saas 专业版和标准版的区分
void saas_pay_detail::paintLeftTop(QWidget *widget, const QRect &rect0, QPainter *painter)
{
    signWidgetList << widget;

    QPolygon poly;
    int off =  15;
    int gap = 5;
    poly.append(QPoint(rect0.x() + off + gap, rect0.y()));//C
    poly.append(QPoint(rect0.x() , rect0.y() + off + gap));//F
    poly.append(QPoint(rect0.x() , rect0.y() + off + gap + off));//E
    poly.append(QPoint(rect0.x() + off + gap + off, rect0.y()));//D
    QPainterPath path;
    path.addPolygon(poly);
    if(2 == lds::sysconf->value("system_setting/yingyemoshi", 0).toInt()) {//酒吧模式

        QPolygon py;
        py.append(QPoint(rect0.x() + off, rect0.y()));//B
        py.append(QPoint(rect0.x() , rect0.y() + off));
        py.append(QPoint(rect0.x() , rect0.y() ));
        QPainterPath ph;
        ph.addPolygon(py);

        painter->fillPath(ph, QColor("#ffff00"));
    }
    if(saas_pay_detail::isSaaSFA20()) {
        painter->fillPath(path, QColor("#ff650b"));
        return;
    }
    if(saas_pay_detail::isSaaSFA21()) {
        painter->fillPath(path, QColor("gold"));
        return;
    }
    if(saas_pay_detail::isSaaSAA21()) {
        painter->fillPath(path, QColor("cyan"));
        return;
    }
    if(saas_pay_detail::isSaaSAA()) {
        return;
    }
    painter->fillPath(path, QColor("red"));
    painter->setPen(QColor("red"));
    painter->drawText(rect0, (Qt::AlignTop | Qt::AlignRight), "ERROR:""MACHINE-ID:" + terminalCodeGet() + "  SYS-TYPE:" + saas_type() + "  ");
}

void saas_pay_detail::paintLeftTopAA(QWidget *widget, const QRect &rect0, QPainter *painter)
{
    signWidgetList << widget;

    QPolygon poly;
    int rect0h_2 =  qMin(30, rect0.height()/2);
    poly.append(QPoint(rect0.x() + rect0h_2  - 5, rect0.y()));
    poly.append(QPoint(rect0.x() + rect0h_2 + 5, rect0.y()));
    poly.append(QPoint(rect0.x() , rect0.y() + rect0h_2 + 5));
    poly.append(QPoint(rect0.x() , rect0.y() + rect0h_2 - 5));
    QPainterPath path;
    path.addPolygon(poly);

    painter->fillPath(path, QColor("blue"));
}

void saas_pay_detail::took()
{
    this->accept();
}

void saas_pay_detail::toexit()
{
    this->reject();
}
