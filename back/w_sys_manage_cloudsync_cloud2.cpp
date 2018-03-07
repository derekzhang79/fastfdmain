#include "w_sys_manage_cloudsync_cloud2.h"
#include "ui_w_sys_manage_cloudsync_cloud2.h"
#include "lds.h"
#include "w_sys_manage_cloudsync.h"
#include "lds_messagebox.h"
#include "w_sys_manage_cloudsync_with_time.h"
#include "saas_pay_detail.h"
#include <QFile>
#include "public_sql.h"
#include "saas_guide.h"
#include "saas_guide2.h"
#include <QtDebug>


w_sys_manage_cloudsync_cloud2::w_sys_manage_cloudsync_cloud2(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_sys_manage_cloudsync_cloud2)
{
    ui->setupUi(this);

    ui->pushButton_check_data_updownload->setCheckedByIniN("w_sys_manage_cloudsync_with_time/cloud_yun_is_run", false);
    ui->pushButton_check_yun_member->setCheckedByIniN("w_sys_manage_cloudsync_with_time/cloud_yun_member", false);
    ui->pushButton_check_local_member->setCheckedByIniN("w_sys_manage_cloudsync_with_time/cloud_no_use_local_member", false);

    ui->pushButton_saas_type->setTitleKey(tr("SaaS版本"),  saas_pay_detail::saasDesc());
    ui->pushButton_machine_id->setTitleKey(tr("机器号"),  saas_pay_detail::terminalCodeGet());//机器号\n4567498746479
    ui->pushButton_machine_id->disableClicked();

    connect(ui->pushButton_data_upload, SIGNAL(clicked()),this,SLOT(to_data_upload()));
    connect(ui->pushButton_data_download, SIGNAL(clicked()),this,SLOT(to_data_download()));
    connect(ui->pushButton_data_downupload, SIGNAL(clicked()),this,SLOT(to_data_downupload()));
    connect(ui->pushButton_flow_upload, SIGNAL(clicked()),this,SLOT(to_flow_upload()));
    connect(ui->pushButton_timecheck, SIGNAL(clicked()),this,SLOT(to_timecheck()));
    connect(ui->pushButton_exit, SIGNAL(clicked()),this,SLOT(to_exit()));
    connect(ui->pushButton_saas_type, SIGNAL(clicked()),this,SLOT(to_saas_update()));
}

w_sys_manage_cloudsync_cloud2::~w_sys_manage_cloudsync_cloud2()
{
    delete ui;
}

void w_sys_manage_cloudsync_cloud2::to_data_upload()
{
    w_sys_manage_cloudsync_with_time_running dialog(w_sys_manage_cloudsync_with_time_running::cloud_up);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    lds_roundeddialog_rect_align(&dialog).exec();
}

void w_sys_manage_cloudsync_cloud2::to_data_download()
{
    if(0 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, tr("本操作将会清空本地所有基础资料"), tr("继续"), tr("取消"))) {
        w_sys_manage_cloudsync_with_time_running dialog(w_sys_manage_cloudsync_with_time_running::cloud_down);
        dialog.setWindowTitle(_TEXT_SLOT(this));
        lds_roundeddialog_rect_align(&dialog).exec();
    }
}

void w_sys_manage_cloudsync_cloud2::to_data_downupload()
{
    w_sys_manage_cloudsync_with_time_running dialog(w_sys_manage_cloudsync_with_time_running::cloud_downup);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    lds_roundeddialog_rect_align(&dialog).exec();
}

void w_sys_manage_cloudsync_cloud2::to_flow_upload()
{
    switch(lds_messagebox::question(this,MESSAGE_TITLE_VOID, tr("流水上传"), tr("全部流水上传"), tr("未上传流水上传"), tr("取消"))) {
    case 0:
    {
        if(1 == lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("全部流水将置为未上传状态，流水越多花时间越多"), tr("继续"), tr("取消"))) {
            return;
        }
        lds_messagebox_loading_show loading(this, "");
        loading.show_delay();
        if(false == w_sys_manage_cloudsync_with_time_running::flow_data_clearAll_Y_transaction(this, &loading)) {
            return;
        }
    }
        break;
    case 1:
        break;
    default:
        return;
    }

    w_sys_manage_cloudsync_with_time_running dialog(w_sys_manage_cloudsync_with_time_running::cloud_flow_up);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    lds_roundeddialog_rect_align(&dialog).exec();
}

void w_sys_manage_cloudsync_cloud2::to_timecheck()
{
    //-1云端失败 0本地校对失败 1校对成功 2无需校对//
    switch(w_sys_manage_cloudsync::match_local_time(this)){
    case 1:
        lds_messagebox::information(this, MESSAGE_TITLE_VOID,tr("校对成功"));
        break;
    case 2:
        lds_messagebox::information(this, MESSAGE_TITLE_VOID,tr("时间同步,无需校对"));
        break;
    }
}

void w_sys_manage_cloudsync_cloud2::to_exit()
{
    //不包括验证信息
    lds::sysconf->setValue("w_sys_manage_cloudsync_with_time/cloud_wx_polling", false);
    this->reject();
}

void w_sys_manage_cloudsync_cloud2::to_saas_update()
{
    if(saas_pay_detail::isSaaSFA20()) {
        //saas_guide_pay
        saas_guide2 d(new saas_guide_pay_update, this);
        if(QDialog::Rejected == lds_roundeddialog_rect_align(&d, false).exec()) {
            return ;
        }
        ///从云端读取终端权限、确认升级成功
        cJSONHttpData httpd = saas_guide::getTerminal(this);
        if(200 == httpd.httpStatus
                && "21" == saas_pay_detail::saas_type())
        {
            lds_messagebox::information(this, MESSAGE_TITLE_VOID, tr("操作成功"));
            return ;
        }
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, tr("操作失败"));
        return;
    }

    if(saas_pay_detail::isSaaSFA21()
            || saas_pay_detail::isSaaSAA21()) {
        //saas_guide_pay
        saas_guide2 d(new saas_guide_pay, this);
        if(QDialog::Rejected == lds_roundeddialog_rect_align(&d, false).exec()) {
            return ;
        }
        ///从云端读取终端权限、确认升级成功
        cJSONHttpData httpd = saas_guide::getTerminal(this);
        if(200 == httpd.httpStatus
                && "21" == saas_pay_detail::saas_type())
        {
            lds_messagebox::information(this, MESSAGE_TITLE_VOID, tr("操作成功"));
            return ;
        }
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, tr("操作失败"));
        return;
    }
}
