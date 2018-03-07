#include "w_inventory_input.h"
#include "ui_w_inventory_input.h"
#include "backheader.h"
#include "w_i_inventory.h"
#include "n_func.h"
#include <QTimer>
#include "lds_model_sqltablemodel_delegate_com.h"
#include "w_inventory_input_new.h"

bool w_inventory_input::detail_control_transaction(const QString &ch_state, int master_row, const lds_model_sqltablemodel *tablemodel_master, const lds_model_sqltablemodel *tablemodel_detail, QString *errstring)
{
    int row = master_row;
    if(row < 0) {
        if(errstring) *errstring =  tr("没有数据被选中");
        return false;
    }
    if(tablemodel_master->model_data(row, "ch_sheetNo").toString().isEmpty()) {
        if(errstring) *errstring = tr("单据号码不能为空");
        return false;
    }
    if(tablemodel_detail->rowCount() == 0) {
        if(errstring) *errstring = tr("空单据不允许审核");
        return false;
    }
    if(tablemodel_master->model_data(row, "vch_audit").toString().isEmpty()) {
        if(errstring) *errstring = tr("审核人不能为空");
        return false;
    }
    if(tablemodel_master->model_data(row, "ch_state").toString() == "Y") {//审核
        if(errstring) *errstring = tr("单据")+
                backheader::VIPFINDVALUE(w_i_inventory::get_MODEL_INVENTORY_STATE_TYPE(), "Y");
        return false;
    }
    if(tablemodel_master->model_data(row, "ch_state").toString() == "O") {//作废
        if(errstring) *errstring = tr("单据")+
                backheader::VIPFINDVALUE(w_i_inventory::get_MODEL_INVENTORY_STATE_TYPE(), "O");
        return false;
    }

    QString ls_sheetno = tablemodel_master->model_data(row, "ch_sheetno").toString();
    lds_query::tran_saction();
    lds_query query;
    if(false == query.execUpdate("cey_st_sheet_master",qrtVariantPairList()
                                 << qrtVariantPair("ch_state", ch_state)
                                 << qrtVariantPair("dt_audit", n_func::f_get_sysdatetime()),
                                 qrtEqual("ch_sheetNo", ls_sheetno),
                                 errstring)) {
        lds_query::roll_back();
        return false;
    }
    lds_query::com_mit();
    return true;
}

w_inventory_input::w_inventory_input(const QString &ch_sheetType, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_inventory_input)
{
    ui->setupUi(this);
    this->ch_sheetType = "RK";
    this->ch_sheetType = ch_sheetType;

    backheader::fillData(ui->comboBox_ch_state, &w_i_inventory::get_MODEL_INVENTORY_STATE_TYPE(), true, tr("全部"));
    ui->comboBox_ch_state->setcomusrdata("N");

    tablemodel_master = new lds_model_sqltablemodel(this);
    tablemodel_master->setTable("cey_st_sheet_master");
    tablemodel_master->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    tablemodel_master->save_set_header(tablemodel_master->fieldIndex("ch_state"), tr("状态"));
    tablemodel_master->save_set_header(tablemodel_master->fieldIndex("ch_sheetno"), tr("单据号码"));
    tablemodel_master->save_set_header(tablemodel_master->fieldIndex("ch_operation"), tr("入库类型"));
    tablemodel_master->save_set_header(tablemodel_master->fieldIndex("vch_handle"), tr("经手人"));
    tablemodel_master->save_set_header(tablemodel_master->fieldIndex("vch_operId"), tr("制单人"));
    tablemodel_master->save_set_header(tablemodel_master->fieldIndex("dt_operdate"), tr("制单日期"));
    tablemodel_master->save_set_header(tablemodel_master->fieldIndex("vch_audit"), tr("审核人"));
    tablemodel_master->save_set_header(tablemodel_master->fieldIndex("dt_audit"), tr("审核日期"));
    tablemodel_master->save_set_header(tablemodel_master->fieldIndex("vch_memo"), tr("备注"));
    
    tablemodel_master->tablename_kvmap_insert("cey_sys_operator",  backheader::SELECT_OPERID_NAME, false, lds_model_sqltablemodel::SELECT_AUTO_REFRESH_KEY);
    ui->tableView_master->setModel(tablemodel_master);
    ui->tableView_master->setEditTriggers(QTableView::NoEditTriggers);
    ui->tableView_master->rePecifyHeader(tablemodel_master->get_save_set_header_saver_column_list());
    ui->tableView_master->setItemDelegateForColumn(tablemodel_master->fieldIndex("ch_operation"), new lds_model_sqltablemodel_delegate_com(this, &w_i_inventory::get_MODEL_INVENTORY_TYPE()));
    ui->tableView_master->setItemDelegateForColumn(tablemodel_master->fieldIndex("vch_handle"), new lds_model_sqltablemodel_delegate_com(this, tablemodel_master->tablename_kvmap_d("cey_sys_operator")));
    ui->tableView_master->setItemDelegateForColumn(tablemodel_master->fieldIndex("vch_audit"), new lds_model_sqltablemodel_delegate_com(this, tablemodel_master->tablename_kvmap_d("cey_sys_operator")));
    ui->tableView_master->setItemDelegateForColumn(tablemodel_master->fieldIndex("vch_operId"), new lds_model_sqltablemodel_delegate_com(this, tablemodel_master->tablename_kvmap_d("cey_sys_operator")));
    ui->tableView_master->setItemDelegateForColumn(tablemodel_master->fieldIndex("ch_state"), new lds_model_sqltablemodel_delegate_com(this, ui->comboBox_ch_state));

    tablemodel_detail = new lds_model_sqltablemodel(this);
    tablemodel_detail->setTable("cey_st_sheet_detail");
    tablemodel_detail->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    
    tablemodel_detail->save_set_header("int_id", tr("序号"));
    tablemodel_detail->save_set_header("ch_materialno", tr("菜品名称"));
    tablemodel_detail->save_set_header("num_wrapping", tr("小包装"));
    tablemodel_detail->save_set_header("num_num", tr("大包装"));
    tablemodel_detail->save_set_header("num_present", tr("赠送数量"));
    tablemodel_detail->save_set_header("num_price", tr("进价"));
    tablemodel_detail->save_set_header("cost_price", tr("金额"));
    tablemodel_detail->save_set_header("vch_memo", tr("备注"));
    ui->tableView_detail->setModel(tablemodel_detail);
    ui->tableView_detail->rePecifyHeader(tablemodel_detail->get_save_set_header_saver_column_list());
    ui->tableView_detail->setEditTriggers(QTableView::AllEditTriggers);
    ui->tableView_detail->setSelectionBehavior(QTableView::SelectItems);
    ui->tableView_detail->setSelectionMode(QTableView::SingleSelection);
    ui->tableView_detail->setItemDelegateForColumn(tablemodel_detail->fieldIndex("ch_materialno"), new lds_model_sqltablemodel_delegate_com(this, "cey_bt_dish", "ch_dishno", "vch_dishname"));


    //datetime
    QDateTime dt = n_func::f_get_sysdatetime();
    ui->dateTimeEdit_to->setDateTime(QDateTime(dt.date(), QTime(23, 59, 59)));
    ui->dateTimeEdit_from->setDateTime(QDateTime(dt.date(), QTime(0,0,0)));
#ifdef QT_DEBUG
    ui->dateTimeEdit_from->setDateTime(ui->dateTimeEdit_from->dateTime().addYears(-1));
#endif
    //connect
    connect(ui->tableView_master, SIGNAL(selectChanged(int)), this, SLOT(to_update_tableview_detail(int)));
    connect(ui->pushButton_exit, SIGNAL(clicked()), this,   SLOT(toexit()));
    connect(ui->pushButton_select, SIGNAL(clicked()), this , SLOT(torefresh()));
    connect(ui->pushButton_audit, SIGNAL(clicked()), this, SLOT(toaudit()));
    connect(ui->pushButton_bad, SIGNAL(clicked()), this, SLOT(tobad()));
    connect(ui->pushButton_change, SIGNAL(clicked()), this, SLOT(tochange()));
    connect(ui->tableView_master, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(tochange()));
    connect(ui->pushButton_del, SIGNAL(clicked()), this, SLOT(todel()));
    connect(ui->pushButton_new, SIGNAL(clicked()), this, SLOT(tonew()));
    connect(ui->comboBox_ch_state, SIGNAL(currentIndexChanged(int)), this,SLOT(torefresh()));

    QTimer::singleShot(0, this, SLOT(torefresh()));
}

w_inventory_input::~w_inventory_input()
{
    delete ui;
}

void w_inventory_input::torefresh()
{
    tablemodel_master->setFilter(QString(" ch_sheetType = '%1' and dt_operdate >= '%2' and dt_operdate <= '%3' and ch_state like '%4%' ")
                                 .arg(ch_sheetType)
                                 .arg(ui->dateTimeEdit_from->dateTimeStr())
                                 .arg(ui->dateTimeEdit_to->dateTimeStr())
                                 .arg(ui->comboBox_ch_state->curusrdata().toString())
                                 );
    ui->tableView_master->restore();
    if(ui->tableView_master->currentIndex().row() < 0) {
        if(tablemodel_master->rowCount() > 0) {
            ui->tableView_master->selectRow(0);
        } else {
            to_update_tableview_detail(-1);
        }
    }
    ui->tableView_master->resizeColumnsToContents();
}

void w_inventory_input::to_update_tableview_detail(int row)
{
    tablemodel_detail->setFilter(QString("ch_sheetno = '%1' ")
                                 .arg(tablemodel_master->model_data(row, "ch_sheetno").toString()));
    ui->tableView_detail->resizeColumnsToContents();
}

void w_inventory_input::toexit()
{
    this->reject();
}

void w_inventory_input::toaudit()
{
    detail_control("Y");
}

void w_inventory_input::tobad()
{
    detail_control("O");
}

void w_inventory_input::tochange()
{
    int row = ui->tableView_master->currentIndex().row();
    if(row < 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("没有数据被选中"));
        return;
    }
    w_inventory_input_new dialog(ch_sheetType, tablemodel_master->model_data(row, "ch_sheetNo").toString() , this);
    dialog.setWindowTitle(tr("查看"));
    lds_roundeddialog_rect_align(&dialog).exec();
    torefresh();

}

void w_inventory_input::todel()
{
    int row = ui->tableView_master->currentIndex().row();
    if(row < 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("没有数据被选中"));
        return;
    }
    if(tablemodel_master->model_data(row, "ch_state").toString() == "Y") {//审核
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("单据")+ ui->comboBox_ch_state->comText("Y"));
        return;
    }
    if(tablemodel_master->model_data(row, "ch_state").toString() == "O") {//作废
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("单据")+ ui->comboBox_ch_state->comText("O"));
        return;
    }
    if( 1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, tr("确认删除"), tr("确认"), tr("取消"))) {
        return;
    }
    //未审核
    QString ls_sheetno = tablemodel_master->model_data(row, "ch_sheetno").toString();
    if(tablemodel_master->model_data(row, "ch_state").toString() == "N") {
        lds_query::tran_saction();
        lds_query query;
        if(query.execDelete("cey_st_sheet_master", QString(" ch_sheetNo = '%1' ")
                            .arg(ls_sheetno))) {
            if(query.execDelete("cey_st_sheet_detail", QString("ch_sheetNo = '%1' ")
                                .arg(ls_sheetno))) {
                lds_query::com_mit();
                lds_messagebox::information(this, MESSAGE_TITLE_VOID, tr("操作成功"));
                torefresh();
                return;
            }
        }
        lds_query::roll_back();
    }
    lds_messagebox::information(this, MESSAGE_TITLE_VOID, tr("操作失败"));
    torefresh();
}

void w_inventory_input::tonew()
{
    w_inventory_input_new dialog(ch_sheetType, "", this);
    dialog.setWindowTitle(tr("新增"));
    lds_roundeddialog_rect_align(&dialog).exec();
    torefresh();
}

void w_inventory_input::detail_control(const QString &ch_state)
{
    QString errstring;
    if(0==lds_messagebox::question(this, MESSAGE_TITLE_VOID,tr("确认")+ (_TEXT_SLOT(this)), tr("确认"),tr("取消"))) {
        if(detail_control_transaction(ch_state, ui->tableView_master->currentIndex().row(), tablemodel_master, tablemodel_detail, &errstring)) {
            lds_messagebox::information(this, MESSAGE_TITLE_VOID, tr("操作成功"));
            torefresh();
            return;
        }
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,errstring);
    }
}
