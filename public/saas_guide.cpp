#include "saas_guide.h"
#include <QStateMachine>
#include <QPropertyAnimation>
#include <QSignalTransition>
#include <QLineEdit>
#include "lds_messagebox.h"
#include "cJSON.h"
#include <QComboBox>
#include "cjson_help.h"
#include <QtDebug>
#include "w_sys_manage_cloudsync.h"
#include <QPainter>
#include <QStandardItemModel>
#include <QScrollBar>
#include "public_sql.h"
#include "saas_guide_hard_test.h"
#include "lds.h"
#include "saas_guide_activateterminal.h"
#include "saas_guide_init_soft.h"

saas_guide::saas_guide(QWidget *parent) :
    QDialog(parent)
{
    ///赋值
}

saas_guide::~saas_guide()
{
}

cJSONHttpData saas_guide::getTerminal(QWidget *parent)
{
    QString terminalCode = saas_pay_detail::terminalCodeGet();
    cJSONHttpData root = cJSON_CreateObject();
    cJSON_AddItemToObject(root.json, "terminalCode", cJSON_CreateString(terminalCode.toLocal8Bit().data()));
    QString param  = root.jsonToString();

    QMap<QString, QString> headinfo;
    cJSONHttpData httpd = w_sys_manage_cloudsync::get_yun_httppos(parent,
                                                    "http://"+saas_pay_detail::saas_Host()+"/cloudTerminal/getTerminal.terminal",
                                                    param,
                                                    headinfo,
                                                    tr("终端")+":"+saas_pay_detail::terminalCodeGet(),
                                                    false,
                                                    false
                                                    );
    if(200 == httpd.httpStatus) {
        //dueDate
        QDateTime dueDate = QDateTime::fromString(cJSON_help::getcJSONvalue(httpd.json, "dueDate").toString(), "yyyy-MM-dd hh:mm:ss");
        if(saas_pay_detail::saas_dueDate() != dueDate) {
            saas_pay_detail::saasSetValue("saas/saas_dueDate", dueDate);
        }

        //saas_online_last_run
        QDateTime curDate = QDateTime::currentDateTime();
        if(saas_pay_detail::saas_online_last_run() != curDate) {
            saas_pay_detail::saasSetValue("saas/saas_online_last_run", curDate);
        }

        //saas_type
        QString saas_type = cJSON_help::getcJSONvalue(httpd.json, "type").toString();
        if(saas_pay_detail::saas_type() != saas_type){
            saas_pay_detail::saasSetValue("saas/saas_type", saas_type);
        }
        //saas_lan_num
        int saas_lan_num = cJSON_help::getcJSONvalue(httpd.json, "num").toInt();
        if(saas_pay_detail::saas_lan_num() != saas_lan_num){
            saas_pay_detail::saasSetValue("saas/saas_lan_num", saas_lan_num);
        }
    } else if(550 == httpd.httpStatus) {
        if("-101" == httpd.code//终端未激活
                || "-100" == httpd.code//终端不存在
                || "-1" == httpd.code//终端号异常
                ) {
            saas_pay_detail::saasSetValue("saas/saas_dueDate", "");
            saas_pay_detail::saasSetValue("saas/saas_online_last_run", "");
            saas_pay_detail::saasSetValue("saas/saas_type", "");
        }
    }
    return httpd;
}
