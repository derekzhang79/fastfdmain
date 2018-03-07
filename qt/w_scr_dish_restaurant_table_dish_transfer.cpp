#include "w_scr_dish_restaurant_table_dish_transfer.h"
#include "ui_w_scr_dish_restaurant_table_dish_transfer.h"
#include "restauranttableview.h"
#include "w_scr_dish_restaurant_table_select.h"
#include "lds_roundeddialog_rect_align.h"
#include <QtDebug>
#include "lds_model_sqltablemodel_delegate_com.h"
#include "lds.h"
#include "lds_messagebox.h"
#include "lds_query.h"
#include <QSqlError>
#include "public_sql.h"
#include "w_bt_dish.h"
#include "n_func.h"
#include <QTimer>
#include "w_rpt_report_frame.h"


w_scr_dish_restaurant_table_dish_transfer::w_scr_dish_restaurant_table_dish_transfer(const QString &ch_billno, const QString &ch_tableno, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_scr_dish_restaurant_table_dish_transfer)
{
    ui->setupUi(this);
    cur_ch_billno = ch_billno;
    des_ch_billno = "";
    ui->pushButton_cur->setText(tr("餐桌号") +":"+ch_tableno);
    ui->pushButton_des->setText(tr("目标餐桌")+tr("未设置"));
    ui->pushButton_cur->setIcon(restraurantDelegate::getPixmapByStateList(QList<tableState>() << tableState_KAITAI <<  tableState_YUDA));
    ui->pushButton_des->setIcon(restraurantDelegate::getPixmapByStateList(QList<tableState>() << tableState_KAITAI <<  tableState_YUDA));
    //
    tablemodel_cur = new lds_model_sqltablemodel(this);
    create_model(this, tablemodel_cur, ui->tableView_cur);
    tablemodel_des = new lds_model_sqltablemodel(this);
    create_model(this, tablemodel_des, ui->tableView_des);
    //

    connect(ui->pushButton_cur, SIGNAL(clicked()),this,SLOT(toselecttable()));
    connect(ui->pushButton_des, SIGNAL(clicked()),this,SLOT(toselecttable()));

    ui->checkBox_tranfer_print->setChecked(lds::sysconf->value("w_scr_dish_restaurant_table_dish_transfer/kitchen_turn_check", false).toBool());
    ui->checkBox_tranfer_out->setChecked(lds::sysconf->value("w_scr_dish_restaurant_table_dish_transfer/out_kitchen_turn_check", false).toBool());
    connect(ui->checkBox_tranfer_out,SIGNAL(toggled(bool)), this, SLOT(tosavekitchenout(bool)));
    connect(ui->checkBox_tranfer_print,SIGNAL(toggled(bool)), this, SLOT(tosavekitchenprint(bool)));
    connect(ui->pushButton_cur_selectall, SIGNAL(clicked()),  this,SLOT(tocurselectall()));
    connect(ui->pushButton_cur_unselectall, SIGNAL(clicked()),  this,SLOT(tocurunselectall()));
    connect(ui->pushButton_cancel,SIGNAL(clicked()),this,SLOT(tocancel()));
    connect(ui->pushButton_ok, SIGNAL(clicked()),this,SLOT(took()));

    QTimer::singleShot(10, this, SLOT(torefresh()));
}

w_scr_dish_restaurant_table_dish_transfer::~w_scr_dish_restaurant_table_dish_transfer()
{
    delete ui;
}

void w_scr_dish_restaurant_table_dish_transfer::toselecttable()
{
    QPushButton *b = qobject_cast<QPushButton *> (sender());

    if(b == ui->pushButton_cur) {
        w_scr_dish_restaurant_table_select dialog(getTableNo(des_ch_billno), this);
        if(QDialog::Accepted == lds_roundeddialog_rect_align(&dialog, true).exec()) {
            if(dialog.get_ch_billno().isEmpty()) {
                lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("餐桌未开台"));
                return;
            }
            cur_ch_billno = dialog.get_ch_billno();
            b->setText(dialog.tabledesc());
            tablemodel_cur->setFilter(QString("ch_billno = '%1' ").arg(cur_ch_billno));
            ui->tableView_cur->resizeColumnsToContents(true);
        }
    }

    if(b == ui->pushButton_des) {
        w_scr_dish_restaurant_table_select dialog(getTableNo(cur_ch_billno), this);
        if(QDialog::Accepted == lds_roundeddialog_rect_align(&dialog, true).exec()) {
            if(dialog.get_ch_billno().isEmpty()) {
                lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("餐桌未开台"));
                return;
            }
            des_ch_billno = dialog.get_ch_billno();
            b->setText(dialog.tabledesc());
            tablemodel_des->setFilter(QString("ch_billno = '%1' ").arg(des_ch_billno));
            ui->tableView_des->resizeColumnsToContents(true);
        }
    }
}

void w_scr_dish_restaurant_table_dish_transfer::tocurselectall()
{
    ui->tableView_cur->selectAll();
}

void w_scr_dish_restaurant_table_dish_transfer::tocurunselectall()
{
    ui->tableView_cur->selectionModel()->clear();
}

void w_scr_dish_restaurant_table_dish_transfer::tosavekitchenout(bool f)
{
    lds::sysconf->setValue("w_scr_dish_restaurant_table_dish_transfer/out_kitchen_turn_check", f);
}

void w_scr_dish_restaurant_table_dish_transfer::tosavekitchenprint(bool f)
{
    lds::sysconf->setValue("w_scr_dish_restaurant_table_dish_transfer/kitchen_turn_check", f);
}

void w_scr_dish_restaurant_table_dish_transfer::took()
{
    //ch_billno
    //ch_tableono
    //int_id
    //vch_memberno
    //ch_member_belong_state
    if(cur_ch_billno.isEmpty() ) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID , tr("当前餐桌")+tr("未设置"));
        return;
    }
    if(des_ch_billno.isEmpty() ) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID , tr("目标餐桌")+tr("未设置"));
        return;
    }

    //

    QModelIndexList indexlist = ui->tableView_cur->selectionModel()->selectedIndexes();
    QSet<int> rowlist;
    lds_query query;
    foreach(const QModelIndex &index, indexlist) {
        rowlist.insert(index.row());
    }
    if(rowlist.isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID , ui->pushButton_cur->text()+"\n"+tr("没有行被选中"));
        return;
    }

    //
    QString transferinfo =tr("当前餐桌") +":" + ui->pushButton_cur->text() +"\n " + tr("目标餐桌")+":"+ui->pushButton_des->text()
            ;
    if(0 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, transferinfo, tr("确定"), tr("取消"))) {
        lds_query::tran_saction();
        QDateTime dt = n_func::f_get_sysdatetime();
        QString state;
        if(lds::sysconf->value("w_scr_dish_restaurant_table_dish_transfer/kitchen_turn_check", false).toBool()){
            state += QString(3, w_bt_dish::k2o3_state2hex_char(w_bt_dish::KITCHEN_ZHUANTAI_CAIPIN));
        } else {
            state += QString(3, w_bt_dish::k2o3_state2hex_char(w_bt_dish::KITCHEN_FINISH));
        }
        if(lds::sysconf->value("w_scr_dish_restaurant_table_dish_transfer/out_kitchen_turn_check", false).toBool()){
            state += QString(3, w_bt_dish::k2o3_state2hex_char(w_bt_dish::KITCHEN_ZHUANTAI_CAIPIN));
        } else {
            state += QString(3, w_bt_dish::k2o3_state2hex_char(w_bt_dish::KITCHEN_FINISH));
        }
        query.execSelect(QString("select max(int_id) from cey_u_orderdish where ch_billno = '%1' ").arg(des_ch_billno));
        query.next();
        int des_max_int_id = query.recordValue(0).toInt();
        qlonglong int_flowid;
        foreach(int row, rowlist) {
            des_max_int_id++;
            int_flowid = tablemodel_cur->model_data(row, "int_flowid").toLongLong();
            if(query.execUpdate("cey_u_orderdish",qrtVariantPairList()
                                << qrtVariantPair("ch_billno", des_ch_billno)
                                << qrtVariantPair("ch_tableno", getTableNo(des_ch_billno))
                                << qrtVariantPair("int_id", des_max_int_id)
                                << qrtVariantPair("vch_memberno", "")
                                << qrtVariantPair("ch_member_belong_state", "")

                                << qrtVariantPair("dt_operdate", dt)
                                << qrtVariantPair("vch_plan_desc", transferinfo) ,
                                qrtEqual("int_flowid",  int_flowid))) {
                if(query.execUpdate("cey_u_orderdish_print_tr",  "vch_plan_k3o3_state", state ,qrtEqual("int_orderflow", int_flowid))) {
                    continue;
                }
            }
            goto rollback;
        }

        if(query.execUpdate("cey_u_table","num_cost", public_sql::getTotalBy_ch_billno(cur_ch_billno) ,qrtEqual("ch_billno", (cur_ch_billno)))) {
            if(query.execUpdate("cey_u_table","num_cost", public_sql::getTotalBy_ch_billno(des_ch_billno) ,qrtEqual("ch_billno", (des_ch_billno)))) {
                lds_query::com_mit();
                torefresh();
                lds_messagebox::information(this, MESSAGE_TITLE_VOID, tr("操作成功"));
                this->accept();
                return;
            }
        }
        goto rollback;
    }

end:
    return;
rollback:
    lds_query::roll_back();
    lds_messagebox::information(this, MESSAGE_TITLE_VOID, tr("数据库错误"));
    goto end;
}

void w_scr_dish_restaurant_table_dish_transfer::tocancel()
{
    this->reject();
}

void w_scr_dish_restaurant_table_dish_transfer::torefresh()
{
    tablemodel_cur->setFilter(QString("ch_billno = '%1' ").arg(cur_ch_billno));
    ui->tableView_cur->resizeColumnsToContents();

    tablemodel_des->setFilter(QString("ch_billno = '%1' ").arg(des_ch_billno));
    ui->tableView_des->resizeColumnsToContents();
}

QString w_scr_dish_restaurant_table_dish_transfer::getTableNo(const QString &ch_billno)
{
    lds_query query;
    query.execSelect(QString(" select ch_tableno from cey_u_table where ch_billno = '%1' ").arg(ch_billno));
    query.next();
    return query.recordValue("ch_tableno").toString();
}

void w_scr_dish_restaurant_table_dish_transfer::create_model(QWidget *parent, lds_model_sqltablemodel *tablemodel, lds_tableView *tableview)
{
    tablemodel->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    tablemodel->setTable("cey_u_orderdish");
    tablemodel->tablename_kvmap_insert("suittype", "P", tr("主套菜"));
    tablemodel->tablename_kvmap_insert("suittype", "Y", tr("子套菜"));
    //强制执行select
    tableview->setModel(tablemodel);
    tableview->setEditTriggers(QTableView::NoEditTriggers);
    tableview->rePecifyHeader(QStringList() << "ch_dishno" << "num_num" << "num_back" << "ch_suitflag");
    tableview->setItemDelegateForColumn(tablemodel->fieldIndex("ch_dishno"), new lds_model_sqltablemodel_delegate_com(
                                            parent, tablemodel, "cey_bt_dish","ch_dishno","vch_dishname"
                                            ));
    tableview->setItemDelegateForColumn(tablemodel->fieldIndex("ch_suitflag"), new lds_model_sqltablemodel_delegate_com(
                                            parent, tablemodel->tablename_kvmap_d("suittype")
                                            ));
    tableview->horizontalHeader()->setStretchLastSection(true);
    tableview->setSelectionMode(QTableView::MultiSelection);
    tableview->setTransferHheader();
}
