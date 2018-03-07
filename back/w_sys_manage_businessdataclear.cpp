#include "w_sys_manage_businessdataclear.h"
//业务数据清除
#include "ui_w_sys_manage_businessdataclear_dialog.h"
#include "lds_query.h"

#include <QSqlError>
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#include <QtConcurrent>
#else
#include <QtCore>
#endif
#include "w_sys_manage.h"
#include "lds_messagebox.h"
#include "n_func.h"
#include "lds.h"
#include "lds_dialog_input.h"
#include "lds.h"

static bool SQL_TABLE_clear(w_sys_manage_businessdataclear *receiver, const QDateTime &adt_end, bool isclearkucun){

    bool isend=false;
    lds_query query;
    QString errtext;
    //! <--transaction begin-->
    lds_query::tran_saction();
    switch(0){//switch
    case 0:
        if(!query.execDelete("cey_u_checkout_detail", qrtEqual("dt_operdate", adt_end.toString(yyyyMMddhhmmss)))){
            errtext=query.recordError();
            break;
        }
        receiver->export_info_progress("", 0);
    case 1:
    case 2:
        //删除表1，表2
        if(!query.execDeleteSelectKey("cey_u_handover_total", "cey_u_handover_master",
                             QString("select a.int_id , b.int_id from cey_u_handover_total a , cey_u_handover_master b "
                                     "where a.int_id =b.int_id and b.dt_end <=  '%1'")
                             .arg(adt_end.toString("yyyy-MM-dd hh:mm:ss")))){
            errtext=query.recordError();
            break;
        }
        receiver->export_info_progress("", 2);
    case 3:
    case 4:
        //点菜信息
        if(!query.execDeleteSelectKey("cey_u_orderdish", "material_u_orderdish", "cey_u_orderdish_print_tr",
                             QString(" select a.int_flowid, b.int_flowid ,c.int_orderflow from "
                                     " cey_u_orderdish a "
                                     " left join  material_u_orderdish b on a.int_flowid = b.int_flowid "
                                     " left join  cey_u_orderdish_print_tr c on a.int_flowid = c.int_orderflow "
                                     " where a.dt_operdate <= '%1';")
                             .arg(adt_end.toString("yyyy-MM-dd hh:mm:ss"))
                             )){
            errtext=query.recordError();
        }
        //日志
        if(!query.execDelete("cey_u_orderdish_log", QString("dt_operdate <= '%1' ")
                             .arg(adt_end.toString("yyyy-MM-dd hh:mm:ss"))
                             )){
            errtext=query.recordError();
            break;
        }
        receiver->export_info_progress("", 4);
    case 5:
        if(!query.execDelete("cey_u_service_low",
                             QString("dt_operdate <= '%1' ")
                             .arg(adt_end.toString("yyyy-MM-dd hh:mm:ss"))
                             )){
            errtext=query.recordError();
            break;
        }
        receiver->export_info_progress("", 5);

    case 6:
        if(!query.execDelete("cey_u_table", QString("dt_operdate <= '%1' ")
                             .arg(adt_end.toString("yyyy-MM-dd hh:mm:ss"))
                             )){

            errtext=query.recordError();
            break;
        }
        receiver->export_info_progress("", 6);

    case 7:
        if(!query.execDelete("cey_u_togo", QString("dt_operdate <= '%1' ")
                             .arg(adt_end.toString("yyyy-MM-dd hh:mm:ss"))
                             )){
            errtext=query.recordError();
            break;
        }
        receiver->export_info_progress("", 7);

    case 8:
        if(!query.execDelete("cey_u_master", QString("dt_operdate <= '%1' ")
                             .arg(adt_end.toString("yyyy-MM-dd hh:mm:ss"))
                             )){
            errtext=query.recordError();
            break;
        }
        if(!query.execDelete("cey_u_master_help", QString("date <= '%1' or isnull(date)")
                             .arg(adt_end.toString("yyyy-MM-dd"))
                             )){
            errtext=query.recordError();
            break;
        }
        receiver->export_info_progress("", 8);
    case 9:
        if(!query.execDelete("cey_u_checkout_master",QString("dt_operdate <= '%1' ")
                             .arg(adt_end.toString("yyyy-MM-dd hh:mm:ss"))
                             )){
            errtext=query.recordError();
            break;
        }
        if(!query.execUpdate("cey_bt_table",  qrtVariantPairList()
                             << qrtVariantPair("ch_billno", ""),
                             QString(" ( ifnull(dt_datetime, '') = '' or  dt_datetime <= '%1')  ")
                             .arg(adt_end.toString("yyyy-MM-dd hh:mm:ss"))
                             )){
            errtext=query.recordError();
            break;
        }
        receiver->export_info_progress("", 9);
    }//switch


    if(isclearkucun){
        switch(0){
        case 0:
            if(!query.execDeleteSelectKey(
                        "cey_st_check_master",
                        QString("select b.ch_sheetno from cey_st_check_master b , cey_st_check_detail a where a.ch_sheetno =b.ch_sheetno and b.dt_operdate <= '%1' ")
                        .arg(adt_end.toString("yyyy-MM-dd hh:mm:ss"))
                        )){
                qDebug() << query.lastQuery();
                errtext=query.recordError();
                break;
            }
            receiver->export_info_progress("", 10);
        case 1:
            if(!query.execDelete("cey_st_check_master",
                                 QString(" dt_operdate <= '%1' ")
                                 .arg(adt_end.toString("yyyy-MM-dd hh:mm:ss"))
                                 )){
                errtext=query.recordError();
                break;
            }
            receiver->export_info_progress("", 11);
        case 2:
            if(!query.execDelete("cey_st_material_flow",
                                 QString(" dt_operdate <= '%1' ")
                                 .arg(adt_end.toString("yyyy-MM-dd hh:mm:ss"))
                                 )){
                errtext=query.recordError();
                break;
            }
            receiver->export_info_progress("", 12);
        case 3:
            if(!query.execDeleteSelectKey("cey_st_sheet_detail",
                                          QString(" select a.ch_sheetno from cey_st_sheet_detail a , cey_st_sheet_master b where a.ch_sheetno =b.ch_sheetno and(b.dt_audit <= '%1' or b.dt_operdate <= '%1') ")
                                          .arg(adt_end.toString("yyyy-MM-dd hh:mm:ss"))
                                          )){
                errtext=query.recordError();
                break;
            }
            receiver->export_info_progress("", 13);
        case 4:
            if(!query.execDelete("cey_st_sheet_master",
                                 QString("(dt_audit <= '%1' or dt_operdate <= '%1') ")
                                 .arg(adt_end.toString("yyyy-MM-dd hh:mm:ss"))
                                 )){
                errtext=query.recordError();
                break;
            }
            receiver->export_info_progress("", 14);
        }
    }
    receiver->export_info_progress(errtext.isEmpty()?"success":errtext);

    if(errtext.isEmpty()){
        lds_query::com_mit();
        return true;
    }
    lds_query::roll_back();
    return false;
    //! <--transaction end-->
}

w_sys_manage_businessdataclear::w_sys_manage_businessdataclear(QWidget *parent) :
    QDialog(parent),
    ui(new Ui_w_sys_manage_businessdataclear_Dialog)
{
    ui->setupUi(this);
    //! toolbar
    //
    ui->widget->setup(QStringList() << /*tr("清除退菜厨打记录") <<*/ tr("清除库存数量")<< tr("执行")<< tr("取消"), Qt::AlignRight);
    connect(ui->widget->index_widget(tr("执行")),SIGNAL(clicked()),this,SLOT(tosave()));
    connect(ui->widget->index_widget(tr("取消")),SIGNAL(clicked()),this,SLOT(toexit()));
    connect(ui->widget->index_widget(tr("清除库存数量")),SIGNAL(clicked()),this,SLOT(toclearinventorynum()));
    //    connect(ui->widget->index_widget(tr("清除退菜厨打记录")),SIGNAL(clicked()),this,SLOT(tocleardataquit()));
    //! after
    ui->checkBox->setChecked(true);
    ui->checkBox->setEnabled(false);
    isend=new bool;
    *isend=true;
    ui->dateTimeEdit->setDisplayFormat("yyyy-MM-dd");
    ui->dateTimeEdit->setDate(n_func::f_get_sysdatetime().date());
    //    ui->dateTimeEdit->setDateTime(QDateTime::fromString("2000-01-02", "yyyy-MM-dd"));
}

void w_sys_manage_businessdataclear::closeEvent(QCloseEvent *e)
{

    if(*isend){
        QDialog::closeEvent(e);
    }
}

void w_sys_manage_businessdataclear::togetfilepath()
{

}

void w_sys_manage_businessdataclear::tosave()
{
    //    QDate ldt_day;
    QDateTime ldt_date;

    ldt_date=ui->dateTimeEdit->dateTime();
    ldt_date.setTime(QTime(23, 59, 59));
    //    ldt_day=ldt_date.date();

    //    lds_query query;
    //    query.execSelect(QString("select count(0) from cey_u_day_sumup where dt_date = '%1' ")
    //               .arg(ldt_day.toString("yyyy-MM-dd"))
    //               );
    //    query.next();
    //    if(query.recordValue(0).toLongLong() == 0){
    //        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("不存在此营业日，请先进行日结"));
    //        return;
    //    }
    //    query.execSelect(QString("select dt_end from cey_u_day_sumup where dt_date = '%1' ")
    //               .arg(ldt_day.toString("yyyy-MM-dd"))
    //               );
    //    query.next();
    //    ldt_date=query.recordValue(0).toDateTime();

    qlonglong ll_pos = 10;
    if(ui->checkBox_2->isChecked()){
        ll_pos+=5;
    }
    ui->progressBar->setRange(0, ll_pos);
    ui->progressBar->setValue(0);

    QString errstring;
    if(0==lds_messagebox::critical(this, MESSAGE_TITLE_VOID,tr("该操作会重置数据库一些表的内容，是否继续"), tr("确认"),tr("取消"))){
        lds_dialog_input inputdialog(this);
        inputdialog.ui->label->setText(tr("密码") + ":");
        inputdialog.ui->lineEdit->setEchoMode(QLineEdit::Password);
#ifdef QT_DEBUG
        inputdialog.ui->lineEdit->setText(n_func::f_get_godpassword());
#endif
        inputdialog.setWindowTitle(tr("请输入密钥"));
        if(QDialog::Accepted==lds_roundeddialog_rect_align(&inputdialog).exec()){
            errstring = tr("密码错误");
            if(inputdialog.ui->lineEdit->text()==n_func::f_get_godpassword()){
                SQL_TABLE_clear(this, ldt_date, ui->checkBox_2->isChecked());

                //                QFuture<void> future = QtConcurrent::run(SQL_TABLE_clear,
                //                                                         this,
                //                                                         ldt_date,
                //                                                         isend,
                //                                                         ui->checkBox_2->isChecked()
                //                                                         );
                //        future.waitForFinished();
                return;
            }
        }
    }
    if(!errstring.isEmpty())
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
}

void w_sys_manage_businessdataclear::toexit()
{
    this->reject();
}

void w_sys_manage_businessdataclear::export_info_progress(const QString &info, int step)
{
    if((step)!=-1){
        ui->progressBar->setValue(step+1);
    }
    if(info.isEmpty()){

    }else {
        ui->textBrowser_2->setText(info);
    }
}

void w_sys_manage_businessdataclear::toclearinventorynum()
{
    if(1 == lds_messagebox::critical(this, tr("清除所有库存"), tr("本操作有风险，是否继续？"), tr("继续"), tr("取消"))){
        return;
    }

    QString errstring;
    lds_dialog_input inputdialog(this);
    inputdialog.ui->label->setText(tr("密码") + ":");
    inputdialog.ui->lineEdit->setEchoMode(QLineEdit::Password);
#ifdef QT_DEBUG
    inputdialog.ui->lineEdit->setText(n_func::f_get_godpassword());
#endif
    inputdialog.setWindowTitle(tr("请输入密钥"));
    if(QDialog::Accepted==lds_roundeddialog_rect_align(&inputdialog).exec()){
        errstring = tr("密码错误");
        if(inputdialog.ui->lineEdit->text()==n_func::f_get_godpassword()){
            //清除库存数据
            errstring = tr("数据库错误");
            lds_query query;
            if(query.execDelete("cey_st_check_detail")){
                if(query.execDelete("cey_st_check_master")){
                    if(query.execDelete("cey_st_material_flow")){
                        if(query.execDelete("cey_st_material_num")){
                            if(query.execDelete("cey_st_sheet_detail")){
                                if(query.execDelete("cey_st_sheet_master")){
                                    lds_messagebox::warning(this,tr("清除库存"), tr("清除所有库存成功"));
                                    return;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    if(!errstring.isEmpty())
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
    return;
}

void w_sys_manage_businessdataclear::tocleardataquit()
{
    lds_query query;
    if(0 == lds_messagebox::question(this, tr("清除退菜厨打记录"), tr("确认"), tr("取消"))) {
        lds_query::tran_saction();
        if(query.execDelete("cey_u_orderdish_log", "num_back_cur > 0")){
            lds_query::com_mit();
            lds_messagebox::information(this, MESSAGE_TITLE_VOID, tr("操作成功"));
            return;
        }
        lds_query::roll_back();
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, tr("操作失败"));
    }
}

