#include "w_bt_dish_suit.h"
#include "ui_w_bt_dish_suit_dialog.h"
#include <QSqlRecord>
#include "lds_messagebox.h"
#include "lds_query.h"
#include <QSqlError>
#include "lds_tableview_repecify.h"
#include "backheader.h"
#include "w_sys_manage_cloudsync_with_time.h"
#include "lds_model_sqltablemodel_delegate_com.h"
#include "lds_model_sqlstandardmodel.h"
#include "w_st_good_input_select_dish.h"

w_bt_dish_suit::w_bt_dish_suit(QWidget *parent) : QDialog(parent),ui(new Ui_w_bt_dish_suit_Dialog)
{
    ui->setupUi(this);

    lds_tableview_delegate_virtual *d = 0;
    model_bt_dish = new lds_model_sqlstandardmodel(this);
    model_dish_suit =new  lds_model_sqlstandardmodel(this);
    model_suit_change = new lds_model_sqlstandardmodel(this);

    model_bt_dish->setQuery("select ch_dishno,vch_dishname, num_price from cey_bt_dish where ch_suitflag = 'Y' and ch_stopflag <> 'Y' ");
    model_bt_dish->save_set_header("ch_dishno");
    model_bt_dish->save_set_header("vch_dishname");
    model_bt_dish->save_set_header("num_price");
    ui->tableView->setModel(model_bt_dish);
    ui->tableView->setEditTriggers(QTableView::AllEditTriggers);
    ui->tableView->setTransferHheader();

    model_dish_suit->setQuery(
                "  select a.int_flowid,  a.ch_suitno,"
                "  a.ch_dishno,"
                "  b.vch_dishname ,"
                "  a.num_num,"
                "  (select vch_unitname from cey_bt_unit AA where AA.ch_unitno = b.ch_unitno)  as vch_unitname,"
                "  b.num_price"
                " from cey_bt_dish_suit a, cey_bt_dish b where a.ch_dishno = b.ch_dishno  and a.ch_suitno = '%1' ");
    model_dish_suit->save_set_header("ch_dishno");
    model_dish_suit->save_set_header("num_num", true);
    model_dish_suit->save_set_header("vch_dishname");
    model_dish_suit->save_set_header("vch_unitname");
    model_dish_suit->save_set_header("num_price");
    ui->tableView_2->setTransferHheader();
    ui->tableView_2->setModel(model_dish_suit);
    ui->tableView_2->setEditTriggers(QTableView::AllEditTriggers);
    ui->tableView_2->hideColumn(model_dish_suit->fieldIndex("int_flowid"));
    ui->tableView_2->hideColumn(model_dish_suit->fieldIndex("ch_suitno"));
    ui->tableView_2->setItemDelegate(d = new lds_tableview_delegate_virtual(this));
    connect(d, SIGNAL(signal_edit_commit(QModelIndex)), this,SLOT(toUpdateDishSuit(QModelIndex)));

    model_suit_change->setQuery(
                " select a.int_flowID, "
                " a.ch_dishno, "
                " b.vch_dishname,"
                " a.num_num_change,"
                " (select vch_unitname from cey_bt_unit AA where AA.ch_unitno = b.ch_unitno)  as vch_unitname,"
                " b.num_price"
                " from cey_bt_suit_change a, cey_bt_dish b where a.ch_dishno = b.ch_dishno and a.int_flowID = '%1' ");
    model_suit_change->save_set_header("ch_dishno");
    model_suit_change->save_set_header("num_num_change", true);
    model_suit_change->save_set_header("vch_dishname");
    model_suit_change->save_set_header("vch_unitname");
    model_suit_change->save_set_header("num_price");
    ui->tableView_3->setModel(model_suit_change);
    ui->tableView_3->setEditTriggers(QTableView::AllEditTriggers);
    ui->tableView_3->hideColumn(model_suit_change->fieldIndex("int_flowID"));
    ui->tableView_3->setItemDelegate(d = new lds_tableview_delegate_virtual);
    ui->tableView_3->setTransferHheader();
    connect(d, SIGNAL(signal_edit_commit(QModelIndex)), this,SLOT(toUpdateSuitChange(QModelIndex)));

    //! toolbar
    ui->widget->setup(QStringList()<< tr("增加菜品") << tr("删除菜品")<< tr("增加替换菜品") << tr("删除替换菜品") << tr("退出"));
    connect(ui->widget->index_widget(tr("增加菜品")), SIGNAL(clicked()),this,SLOT(toNewDishSuit()));
    connect(ui->widget->index_widget(tr("删除菜品")), SIGNAL(clicked()),this,SLOT(toDelDishSuit()));
    connect(ui->widget->index_widget(tr("增加替换菜品")), SIGNAL(clicked()),this,SLOT(toNewSuitChange()));
    connect(ui->widget->index_widget(tr("删除替换菜品")), SIGNAL(clicked()),this,SLOT(toDelSuitChange()));
    connect(ui->widget->index_widget(tr("退出")), SIGNAL(clicked()),this,SLOT(toExit()));

    connect(ui->tableView, SIGNAL(clicked(QModelIndex)), this, SLOT(toRefresh()));
    connect(ui->tableView_2, SIGNAL(clicked(QModelIndex)), this, SLOT(toRefresh()));

    QTimer::singleShot(10, this, SLOT(toRefresh()));
}

void w_bt_dish_suit::toNewDishSuit()
{
    int row = ui->tableView->currentIndex().row();
    if(row < 0) {
        lds_messagebox::warning(this ,MESSAGE_TITLE_VOID, tr("没有数据被选中"));
        return;
    }
    QString ch_suitno = model_bt_dish->model_data(row, "ch_dishno").toString();
    w_st_good_input_select_dish dialog(this);
    dialog.filter_by_no_dish_suit();
    if(QDialog::Accepted ==lds_roundeddialog_rect_align(&dialog).exec()) {
        //query
        lds_query::tran_saction();
        lds_query query;
        QString errstring;
        foreach(int drow, dialog.ret_row_list) {
            //dishexisted
            bool dishexisted = false;
            for(int row = 0; row < model_dish_suit->rowCount(); row ++) {
                if(model_dish_suit->model_data(row, "ch_dishno").toString() == dialog.get_tablemodel_value(drow, "ch_dishno").toString()) {
                    dishexisted = true;
                    break;
                }
            }
            if(dishexisted) {
                continue;
            }
            //
            if(!query.execInsert("cey_bt_dish_suit",qrtVariantPairList()
                                 << qrtVariantPair("ch_suitno",  ch_suitno)
                                 << qrtVariantPair("ch_dishno", dialog.get_tablemodel_value(drow, "ch_dishno"))
                                 << qrtVariantPair("num_num", 1), &errstring)) {
                lds_query::roll_back();
                lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
                return;
            }
        }
        lds_query::com_mit();
        model_dish_suit->execQuery(model_bt_dish->model_data(ui->tableView->currentIndex().row(), "ch_dishno").toString());
        ui->tableView_2->restore(0);
        ui->tableView_2->resizeColumnsToContents();
    }
}

void w_bt_dish_suit::toDelDishSuit()
{
    int row=ui->tableView_2->currentIndex().row();
    if(row < 0) {
        lds_messagebox::warning(this ,MESSAGE_TITLE_VOID, tr("没有数据被选中"));
        return;
    }
    lds_query query;
    query.execSelect(QString(" select 1 from cey_bt_suit_change where int_flowID = %1 ")
                     .arg(model_dish_suit->model_data(row, "int_flowID").toString()));
    if(query.next()) {
        if(1==lds_messagebox::question(this,MESSAGE_TITLE_VOID,tr("该套菜明细下有可替换菜品，是否继续删除"),tr("继续"),tr("取消"))){
            return;
        }
    } else {
        if(1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, tr("确定删除"), tr("确定"), tr("取消"))) {
            return;
        }
    }
    lds_query::tran_saction();
    QString errstring;
    if(query.execDelete("cey_bt_dish_suit", qrtEqual("int_flowID", model_dish_suit->model_data(row, "int_flowID").toString()), &errstring)) {
        if(query.execDelete("cey_bt_suit_change", qrtEqual("int_flowID", model_dish_suit->model_data(row, "int_flowID").toString()), &errstring)) {
            model_dish_suit->execQuery(model_bt_dish->model_data(ui->tableView->currentIndex().row(), "ch_dishno").toString());
            ui->tableView_2->restore(0);

            model_suit_change->execQuery(model_dish_suit->model_data(ui->tableView_2->currentIndex().row(), "int_flowID").toString());
            ui->tableView_2->restore(0);
            lds_query::com_mit();
            return;
        }
    }
    lds_query::roll_back();
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
    return;
}

void w_bt_dish_suit::toNewSuitChange()
{
    int row2 = ui->tableView_2->currentIndex().row();
    if(row2 < 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("没有数据被选中"));
        return;
    }

    w_st_good_input_select_dish dialog(this);
    dialog.filter_by_no_ch_dishno(model_dish_suit->model_data(row2, "ch_dishno").toString());
    dialog.filter_by_no_dish_suit();
    if(QDialog::Accepted == lds_roundeddialog_rect_align(&dialog).exec()) {
        lds_query::tran_saction();
        lds_query query;
        QString errstring;
        foreach(int drow, dialog.ret_row_list) {
            bool dishexisted = false;
            for(int row = 0; row < model_suit_change->rowCount(); row ++) {
                if(model_suit_change->model_data(row, "ch_dishno").toString() == dialog.get_tablemodel_value(drow, "ch_dishno").toString()) {
                    dishexisted = true;
                    break;
                }
            }
            if(dishexisted) {
                continue;
            }
            //int_flowID, ch_dishno, num_num_change
            if(!query.execInsert("cey_bt_suit_change", qrtVariantPairList()
                                 << qrtVariantPair("int_flowID", model_dish_suit->model_data(row2, "int_flowID"))
                                 << qrtVariantPair("ch_dishno", dialog.get_tablemodel_value(drow, "ch_dishno"))
                                 << qrtVariantPair("num_num_change", 1),
                                 &errstring
                                 )) {
                lds_query::roll_back();
                lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
                return;
            }
        }
        lds_query::com_mit();
        model_suit_change->execQuery(model_dish_suit->model_data(ui->tableView_2->currentIndex().row(), "int_flowID").toString());
        ui->tableView_3->restore(0);
        ui->tableView_3->resizeColumnsToContents();
    }
}

void w_bt_dish_suit::toDelSuitChange()
{
    int row=ui->tableView_3->currentIndex().row();
    if(row < 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("没有数据被选中"));
        return;
    }
    if(1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, tr("确定删除"), tr("确定"), tr("取消"))) {
        return;
    }
    lds_query::tran_saction();
    lds_query query;
    QString errstring;
    if(query.execDelete("cey_bt_suit_change",
                        qrtEqual("int_flowID", model_suit_change->model_data(row, "int_flowID").toString()) + " and " +
                        qrtEqual("ch_dishno", model_suit_change->model_data(row, "ch_dishno").toString()),
                        &errstring)) {
        lds_query::com_mit();
        model_suit_change->execQuery(model_dish_suit->model_data(ui->tableView_2->currentIndex().row(), "int_flowID").toString());
        ui->tableView_3->restore(0);
        return;
    }
    lds_query::roll_back();
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
}

void w_bt_dish_suit::toExit()
{
    w_sys_manage_cloudsync_with_time_running::exec_keepdata_byconf(this);
    this->reject();
}

void w_bt_dish_suit::toRefresh()
{
    //yun
    w_sys_manage_cloudsync_with_time_running::exec_no_up_then_down_byconf(this);
    ui->widget->index_widget(tr("退出"))->show_flag_top_right(w_sys_manage_cloudsync_with_time_running::q_bt_get_ready_total_byconf());
    //~yun

    model_bt_dish->execQuery();
    ui->tableView->restore(0);
    ui->tableView->resizeColumnsToContents();

    model_dish_suit->execQuery(model_bt_dish->model_data(ui->tableView->currentIndex().row(), "ch_dishno").toString());
    ui->tableView_2->restore(0);
    ui->tableView_2->resizeColumnsToContents();

    model_suit_change->execQuery(model_dish_suit->model_data(ui->tableView_2->currentIndex().row(), "int_flowID").toString());
    ui->tableView_3->restore(0);
    ui->tableView_3->resizeColumnsToContents();
}

void w_bt_dish_suit::toUpdateDishSuit(const QModelIndex &index)
{
    if(model_dish_suit->fieldIndex("num_num") == index.column()) {
        lds_query query;
        QString errstring;
        lds_query::tran_saction();
        if(query.execUpdate("cey_bt_dish_suit",
                            "num_num", model_dish_suit->model_data(index.row(), "num_num"),
                            qrtEqual("int_flowID", model_dish_suit->model_data(index.row(), "int_flowID").toString()), &errstring)) {
            lds_query::com_mit();
        } else {
            lds_query::roll_back();
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
        }
    }
}

void w_bt_dish_suit::toUpdateSuitChange(const QModelIndex &index)
{
    if(model_suit_change->fieldIndex("num_num_change") == index.column()) {
        lds_query query;
        QString errstring;
        lds_query::tran_saction();
        if(query.execUpdate("cey_bt_suit_change",
                            "num_num_change", model_suit_change->model_data(index.row(), "num_num_change"),
                            qrtEqual("int_flowID", model_suit_change->model_data(index.row(), "int_flowID").toString()) + " and " +
                            qrtEqual("ch_dishno", model_suit_change->model_data(index.row(), "ch_dishno").toString()), &errstring)) {
            lds_query::com_mit();
        } else {
            lds_query::roll_back();
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
        }
    }
}
