#include "w_scr_dish_cuicaipiece_choose.h"
#include "ui_w_bt_dish_unitset_dialog.h"
#include "public_sql.h"
#include "lds_tableview_delegate_check.h"
#include "lds_messagebox.h"
#include "fexpandmain_2.h"
#include <QPainter>
#include "lds_model_sqltablemodel.h"
#include <QSqlError>
#include "w_rpt_report_frame.h"
#include "lds_model_sqltablemodel_delegate_com.h"



w_scr_dish_cuicaipiece_choose::w_scr_dish_cuicaipiece_choose(const QString &ch_billno, w_bt_dish::kitchenState kstate, QWidget *parent) :
    QDialog(parent),
    ui(new Ui_w_bt_dish_unitset_Dialog)
{
    ui->setupUi(this);
    //
    _ch_billno = ch_billno;
    _kstate = kstate;
    tablemodel = new lds_model_sqltablemodel(this);
    tablemodel->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    tablemodel->setTable("cey_u_orderdish");
    tablemodel->tablename_kvmap_insert("suittype", "P", tr("主套菜"));
    tablemodel->tablename_kvmap_insert("suittype", "Y", tr("子套菜"));
    //强制执行select
    ui->tableView->setModel(tablemodel);
    ui->tableView->setEditTriggers(QTableView::NoEditTriggers);
    ui->tableView->rePecifyHeader(QStringList() << "ch_dishno" << "num_num" << "num_back" << "ch_suitflag");
    ui->tableView->setTransferHheader();
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("ch_dishno"), new lds_model_sqltablemodel_delegate_com(
                                            parent, tablemodel, "cey_bt_dish","ch_dishno","vch_dishname"
                                            ));
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("ch_suitflag"), new lds_model_sqltablemodel_delegate_com(
                                            parent, tablemodel->tablename_kvmap_d("suittype")
                                            ));
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->setSelectionMode(QTableView::MultiSelection);
    //    //3
    ui->widget->setup(QStringList()<<tr("全选") << tr("全不选") <<tr("确定") << tr("取消"), Qt::AlignRight);
    connect(ui->widget->index_widget(tr("全选")),SIGNAL(clicked()),this,SLOT(toall()));
    connect(ui->widget->index_widget(tr("全不选")),SIGNAL(clicked()),this,SLOT(tonoall()));
    connect(ui->widget->index_widget(tr("确定")),SIGNAL(clicked()),this,SLOT(took()));
    connect(ui->widget->index_widget(tr("取消")),SIGNAL(clicked()),this,SLOT(tocanecel()));

    ui->label_desc->setText(tr("数量为0、未落单的菜，将不会显示"));
    ui->label_desc->setMaximumHeight(100);

    QTimer::singleShot(10, this, SLOT(torefresh()));
}

w_scr_dish_cuicaipiece_choose::~w_scr_dish_cuicaipiece_choose()
{
    delete ui;
}

void w_scr_dish_cuicaipiece_choose::toall()
{
    for(int row = 0, rowcount = ui->tableView->verticalHeader()->count(); row < rowcount; row++){
        ui->tableView->selectRow(row);
    }
    ui->tableView->setFocus();
}

void w_scr_dish_cuicaipiece_choose::tonoall()
{
    ui->tableView->clearSelection();
    ui->tableView->setFocus();
}

void w_scr_dish_cuicaipiece_choose::took()
{
    //
    QSet<int> row_set;
    lds_query query;
    QModelIndexList indexlist = ui->tableView->selectionModel()->selectedIndexes();
    foreach(const QModelIndex &index, indexlist) {
        row_set.insert(index.row());
    }
    if(row_set.isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("没有菜品被选中"));
        return;
    }
    //
    QString state =   QString(6,w_bt_dish::k2o3_state2hex_char(_kstate));

    //
    lds_query::tran_saction();
    foreach(int row, row_set) {
        if(!query.execUpdate("cey_u_orderdish_print_tr",
                             "vch_plan_k3o3_state", state ,
                             QString("int_orderflow = %1 ").arg(tablemodel->model_data(row, "int_flowid").toInt())
                             )){
            lds_query::roll_back();
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("数据库错误") + query.recordError());
            return;
        }
    }

    lds_query::com_mit();
    lds_messagebox::information(this,MESSAGE_TITLE_VOID, tr("操作成功"));
    this->accept();
}

void w_scr_dish_cuicaipiece_choose::tocanecel()
{
    this->reject();
}

void w_scr_dish_cuicaipiece_choose::torefresh()
{
    tablemodel->setFilter(QString(" ch_billno = '%1' and (num_num-num_back) <> 0 ").arg(_ch_billno));
    ui->tableView->resizeColumnsToContents();
}
