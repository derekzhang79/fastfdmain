#include "saas_guide_activateterminal.h"
#include "ui_saas_guide_activateterminal.h"
#include "cjson_help.h"
#include "w_sys_manage_cloudsync_transfer.h"
#include <QtDebug>
#include "w_sys_manage_cloudsync.h"
#include "lds.h"
#include "saas_pay_detail.h"
#include "lds_messagebox.h"
#include "saas_guide.h"


saas_guide_activateTerminal::saas_guide_activateTerminal(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::saas_guide_activateTerminal)
{
    ui->setupUi(this);

    ui->lineEdit_terminalCode->setText(saas_pay_detail::terminalCodeGet());
    ui->lineEdit_terminalCode->setEnabled(false);

    ui->lineEdit_storeName->setText(saas_pay_detail::hostNameGet());
    ui->lineEdit_storeName->setEnabled(false);
}

saas_guide_activateTerminal::~saas_guide_activateTerminal()
{
    delete ui;
}

bool saas_guide_activateTerminal::toTerminalActive()
{
    QString saasType = saas_pay_detail::terminalTypeGet();//AA    FA
    QString saasSystem = saas_pay_detail::terminalSytemGet();

    cJSONHttpData root = cJSON_CreateObject();

    cJSON_AddItemToObject(root.json, "terminalCode", cJSON_CreateString(ui->lineEdit_terminalCode->text().toLocal8Bit().data()));
    cJSON_AddItemToObject(root.json, "storeName", cJSON_CreateString(ui->lineEdit_storeName->text().toLocal8Bit().data()));
    cJSON_AddItemToObject(root.json, "contacts", cJSON_CreateString(ui->lineEdit_contacts->text().toLocal8Bit().data()));
    cJSON_AddItemToObject(root.json, "telephone", cJSON_CreateString(ui->lineEdit_telephone->text().toLocal8Bit().data()));
    cJSON_AddItemToObject(root.json, "province", cJSON_CreateString(ui->lineEdit_province->text().toLocal8Bit().data()));

    cJSON_AddItemToObject(root.json, "city", cJSON_CreateString(ui->lineEdit_city->text().toLocal8Bit().data()));
    cJSON_AddItemToObject(root.json, "area", cJSON_CreateString(ui->lineEdit_area->text().toLocal8Bit().data()));
    cJSON_AddItemToObject(root.json, "street", cJSON_CreateString(ui->lineEdit_street->text().toLocal8Bit().data()));
    cJSON_AddItemToObject(root.json, "address", cJSON_CreateString(ui->lineEdit_address->text().toLocal8Bit().data()));
    cJSON_AddItemToObject(root.json, "saasType", cJSON_CreateString(saasType.toLocal8Bit().data()));//1餐饮\2超市

    cJSON_AddItemToObject(root.json, "saasSystem", cJSON_CreateString(saasSystem.toLocal8Bit().data()));

    QString param  = root.jsonToString();

    QMap<QString, QString> headinfo;
    cJSONHttpData httpd = w_sys_manage_cloudsync::get_yun_httppos(this,
                                                    "http://"+saas_pay_detail::saas_Host()+"/cloudTerminal/activateTerminal.terminal",
                                                    param,
                                                    headinfo,
                                                    tr("提示"),
                                                    true,
                                                    false
                                                    );
    qDebug() << cJSON_help::getcJSONprint(httpd.json) << httpd.code << httpd.message;
    if(200 == httpd.httpStatus) {

        httpd.initRef() = saas_guide::getTerminal(this);
        if(200 == httpd.httpStatus) {
            lds_messagebox::information(this,
                                        MESSAGE_TITLE_VOID,
                                        tr("激活成功") +"\n" +
                                        tr("终端号") + ":" + saas_pay_detail::terminalCodeGet() + "\n" +
                                        tr("截止日期")+":"+cJSON_help::getcJSONvalue(httpd.json, "dueDate").toString() +"\n"+
                                        "saas"+tr("版本")+":"+saas_pay_detail::codeSaaSMapGet().value(cJSON_help::getcJSONvalue(httpd.json, "type").toString()) +"\n"+
                                        tr("局域网机器个数")+":"+cJSON_help::getcJSONvalue(httpd.json, "num").toString() +"\n"
                                        );
            return true;
        }
    }
    return false;
}
