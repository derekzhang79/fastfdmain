#include "w_bt_dish_base_material.h"
#include "ui_w_bt_dish_base_material.h"
#include "ui_w_bt_dish_new_dialog_2.h"
#include "lds_model_sqltablemodel_delegate_com.h"
#include "lds_tableview_delegate_check.h"
#include "lds_query.h"
#include "w_sys_manage_cloudsync_with_time.h"
#include "w_bt_dish_base.h"

w_bt_dish_base_material::w_bt_dish_base_material(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_bt_dish_base_material)
{
    ui->setupUi(this);
    Ui_w_bt_dish_new_dialog_2 ui_dish;
    QDialog dialog;
    ui_dish.setupUi(&dialog);

    tablemodel=new lds_model_sqltablemodel(this);
    tablemodel->setTable("cey_bt_dish");
    tablemodel->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);


    ui->frame->setup(QStringList() << tr("新增") << tr("修改") << tr("删除") << tr("退出"));
    tablemodel->save_set_header("ch_dishno",tr("菜品编码"));
    tablemodel->save_set_header("vch_dishname",tr("菜品名称"));
    tablemodel->save_set_header("ch_unitno",tr("单位"));
    tablemodel->save_set_header("num_price",tr("价格"));
    tablemodel->save_set_header("ch_discount",tr("折扣"));
    tablemodel->save_set_header("item_flag",tr("库存管理"));
    tablemodel->save_set_header("vch_barcode", tr("条码"));

    ui->tableView->setEditTriggers(QTableView::AllEditTriggers);
    ui->tableView->setModel(tablemodel);
    ui->tableView->rePecifyHeader(tablemodel->get_save_set_header_saver_column_list());

    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("ch_unitno"), new lds_model_sqltablemodel_delegate_com(
                                                this, tablemodel, "cey_bt_unit","ch_unitno","vch_unitname"
                                                ));
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("item_flag"), new lds_tableview_delegate_check(QVector<int>() << tablemodel->fieldIndex("item_flag"), this, true));
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("ch_discount"), new lds_tableview_delegate_check(QVector<int>() << tablemodel->fieldIndex("ch_discount"), this, true));

    connect(ui->frame->index_widget(tr("新增")), SIGNAL(clicked()),this,SLOT(tonew()));
    connect(ui->frame->index_widget(tr("修改")), SIGNAL(clicked()),this,SLOT(tochange()));
    connect(ui->frame->index_widget(tr("删除")), SIGNAL(clicked()),this,SLOT(todel()));
    connect(ui->frame->index_widget(tr("退出")), SIGNAL(clicked()),this,SLOT(toexit()));

    tablemodel->setFilter("ifnull(ch_material_flag, '') = 'Y' and ifnull(ch_stopflag, '') <> 'Y' ");

}

w_bt_dish_base_material::~w_bt_dish_base_material()
{
    delete ui;
}

void w_bt_dish_base_material::tonew()
{
    w_bt_dish_base_new dialog(this, "", "", "", "");
    connect(&dialog,SIGNAL(signalsave()),this,SLOT(torefresh()));
    dialog.setWindowTitle(_TEXT_SLOT(this));
    lds_roundeddialog_rect_align(&dialog).exec();
}

void w_bt_dish_base_material::tochange()
{
    w_bt_dish_base_change dialog(this, ui->tableView, tablemodel);
    dialog.refresh();
    dialog.setObjectName("w_bt_dish_base_change");
    dialog.setWindowTitle(_TEXT_SLOT(this));
    connect(&dialog, SIGNAL(signal_parent_to_refresh()),this,SLOT(torefresh()));
    lds_roundeddialog_rect_align(&dialog).exec();
    torefresh();
}

void w_bt_dish_base_material::todel()
{
    int row=ui->tableView->currentIndex().row();
    if(row<0){
        lds_messagebox::information(this, MESSAGE_TITLE_VOID,tr("请选择将要删除的菜品记录"));
        return;
    }

    QString ls_dishno=tablemodel->index(row, 0).data().toString();
    lds_query query;
    query.execSelect(QString("SELECT ch_dishno from cey_u_orderdish where ch_dishno = '%1' limit 1 ").arg(ls_dishno));
    if(query.next()){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("指定菜品记录已经营业操作,不能删除"));
        return;
    }
    query.execSelect(QString("select count(0) from cey_st_material_flow where ch_materialno='%1'").arg(ls_dishno));
    if(query.next()){
        if(query.recordValue(0).toInt()>0){
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("指定菜品/或商品记录已经营业操作,不能删除"));
            return;
        }
    }
    if(0==lds_messagebox::question(this, MESSAGE_TITLE_VOID,tr("确认删除"),tr("确认"),tr("取消"))){
        tablemodel->removeRow(row);
        torefresh();
    }
}

void w_bt_dish_base_material::toexit()
{
    lds_query::tran_saction();
     tablemodel->trySubmitAll();
     lds_query::com_mit();
     this->reject();
}

void w_bt_dish_base_material::torefresh()
{
    tablemodel->select();
    ui->tableView->restore();
}
