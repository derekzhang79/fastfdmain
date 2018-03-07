#include "w_scr_dish_dish_quit.h"
#include "ui_w_scr_dish_dish_quit.h"
#include <QStandardItemModel>
#include "lds_query.h"
#include "lds_tableview.h"
#include "fexpandmain_2.h"
#include "lds_messagebox.h"
#include "public_sql.h"
#include "n_func.h"
#include <QSqlError>
#include "w_bt_dish.h"
#include "public_printby_ch_billno.h"
#include "fexpandmain_model_sqltablemodel.h"
#include "ftableview_delegate.h"
#include "lds_model_sqltablemodel_delegate_com.h"

w_scr_dish_dish_quit::w_scr_dish_dish_quit(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_scr_dish_dish_quit)
{
    ui->setupUi(this);
    {
        QStandardItemModel *com_member_model=new QStandardItemModel;
        lds_query query;
        query.execSelect(QString("select vch_reason FROM hddpos.cey_bt_reason;"));
        lds_tableView *com_tab;
        while(query.next()){
            com_member_model->appendRow(QList<QStandardItem*>()
                                        << new QStandardItem(query.recordValue("vch_reason").toString())
                                        );
        }
        com_member_model->setHorizontalHeaderLabels(QStringList() << tr("退菜原因:"));
        ui->comboBox_vch_reason->setView(com_tab=new lds_tableView);
        com_tab->horizontalHeader()->setStretchLastSection(true);
        ui->comboBox_vch_reason->setModel(com_member_model);
        //        ui->comboBox_vch_reason->setCurrentIndex(-1);
    }

    ui->lineEdit_vch_dishname->setEnabled(false);
    ui->lineEdit_num2back->setEnabled(false);
    ui->lineEdit_num_num->setEnabled(false);
    ui->lineEdit_num_back->setEnabled(false);
    //
    connect(ui->pushButton_ok,SIGNAL(clicked()),this,SLOT(took()));
    connect(ui->pushButton_cancel,SIGNAL(clicked()),this,SLOT(tocancel()));
    connect(ui->pushButton_all_quit,SIGNAL(clicked()),this,SLOT(toallquit()));
    //做法
    ftableview_delegate *d = new ftableview_delegate(ui->tableView);
    d->keepConnect(ui->tableView);
    d->setMargin(1);
    d->indexisEmptystillDraw = true;

    cookmodel = new ftableview_standmodel_sql_none(this);
    ui->tableView->setModel(cookmodel);
    ui->tableView->setProperty("outer_stylesheet", "tableView_type");
    ui->tableView->setItemDelegate(d);
    ui->tableView->setSelectionModel(new QItemSelectionModel(cookmodel));
    cookmodel->selectionmodel = ui->tableView->selectionModel();
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
#else
    ui->tableView->verticalHeader()->setResizeMode(QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
#endif

    ui->tableView->updateFixed_fillData_addAnimation(3, 3, 80, 50);
    ui->tableView->setSelectionMode(QTableView::MultiSelection);
}

w_scr_dish_dish_quit::~w_scr_dish_dish_quit()
{
    delete ui;
}

void w_scr_dish_dish_quit::initData(int row, const fexpandmain_model_sqltablemodel_data *frommodel, const QString &ch_billno)
{
    _ch_billno = ch_billno;
    tablemodel = frommodel;
    const_row = row;

    ui->lineEdit_num2back->setValue(tablemodel->model_data_NIBOLAN(row, "num_num-num_back"));
    ui->lineEdit_inputnum->setValue(ui->lineEdit_num2back->value());

    ui->lineEdit_num_back->setValue(tablemodel->model_data_NIBOLAN(row, "num_back"));
    ui->lineEdit_num_num->setValue(tablemodel->model_data_NIBOLAN(row, "num_num"));
    ui->lineEdit_vch_dishname->setText(tablemodel->get_vch_dishname_by_map(tablemodel->model_data(row, "ch_dishno").toString()));


    QList<ftableview_standmodel_sql_none::noneData> list_data;
    lds_query query;
    QMap<QString, int>map = tablemodel->get_map_vch_print_memo(tablemodel->model_data(row, "vch_print_memo").toString());//map<no, num>
    for(QMap<QString, int>::iterator k = map.begin(); k != map.end(); k++) {
        query.execSelect(QString("select vch_cp_memo FROM hddpos.cey_bt_cp_memo where ch_no = '%1'; ").arg(k.key()));
        query.next();
        int count = k.value();
        while(count >= 0 && count--) {
            list_data << ftableview_standmodel_sql_none::noneData(
                             QString("%1").arg(query.recordValue("vch_cp_memo").toString()),
                             k.key()
                             );
        }
    }
    ui->tableView->setVisible(!map.isEmpty());
    cookmodel->add_list_data(list_data);
    cookmodel->toinit();

    //退菜
    if(row < 0) {
        ui->lineEdit_inputnum->setEnabled(false);
        ui->pushButton_ok->setEnabled(false);
        //        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("没有菜品被选中"));
        return;
    }
    if(tablemodel->model_data_NIBOLAN(row, "num_num-num_back") <= 0) {
        //        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("指定点菜数量小等于0,不能退菜"));
        ui->lineEdit_inputnum->setEnabled(false);
        ui->pushButton_ok->setEnabled(false);
        return;
    }
    if(tablemodel->model_data(row, "ch_suitflag").toString() == "Y") {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("套菜明细不能单独退菜"));
        ui->lineEdit_inputnum->setEnabled(false);
        ui->pushButton_ok->setEnabled(false);
        return;
    }
}

void w_scr_dish_dish_quit::took()
{
    QDateTime dt = n_func::f_get_sysdatetime();
    lds_query query;
    if(ui->lineEdit_inputnum->value() > ui->lineEdit_num2back->value()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("退菜数量不能大于可退数量"));
        ui->lineEdit_inputnum->selectAll();
        return;
    }

    if(n_func::f_get_sysparm_q(query,"orderdish_back") == "1"
            && ui->comboBox_vch_reason->currentText().trimmed().isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("请输入退菜原因"));
        return;
    }
    if(0==lds_messagebox::question(this, MESSAGE_TITLE_VOID,tr("是否按指定数量退菜？"),tr("确认"),tr("取消"))) {
        //更新
        QString errstring;
        double dishnum_back_total = ui->lineEdit_inputnum->value();
        lds_query::tran_saction();

        bool f = false;
        if(tablemodel->model_data(const_row, "ch_suitflag").toString() == "P") {
            f = fexpandmain_2::static_toquit_1_suit(errstring, tablemodel->model_data(const_row, "int_flowid").toLongLong(), dishnum_back_total,
                                                    ui->comboBox_vch_reason->currentText(), public_sql::gs_operid, dt, &back_data_list);
        } else {
            num_backData d;
            d.int_flowid = tablemodel->model_data(const_row, "int_flowid").toLongLong();
            d.num_back_add =  ui->lineEdit_inputnum->value();
            d.dt_back_operdate = dt;
            d.vch_back_operid = public_sql::gs_operid;
            d.vch_reason = ui->comboBox_vch_reason->currentText();

            d.vch_print_memo = cookmodel->get_value_select();
            d.vch_print_memo_remain = cookmodel->get_value_unselect();

            if(ui->lineEdit_inputnum->value() == ui->lineEdit_num2back->value()) {
                d.vch_print_memo = tablemodel->model_data(const_row, "vch_print_memo").toString();
                d.vch_print_memo_remain = "";

                if(!d.vch_print_memo.isEmpty()) {
                    if(1 == lds_messagebox::information(this, MESSAGE_TITLE_VOID, tr("做法将会全部退掉"), tr("继续"), tr("取消"))) {
                        lds_query::roll_back();
                        return;
                    }
                }
            }
            f = toquitrow(errstring, d);

            back_data_list.append(d);
        }

        if(f) {
            lds_query::com_mit();
            lds_messagebox::information(this, MESSAGE_TITLE_VOID, tr("退菜成功"));

            public_printby_ch_billno  printer(_ch_billno);
            printer.toprint_dish_return(printer.defpath(), dt);

            this->done(QDialogButtonBox::AcceptRole);
            return;
        }
        lds_query::roll_back();
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, tr("退菜失败"));
        return;
    }//reject
}

void w_scr_dish_dish_quit::tocancel()
{
    this->done(QDialogButtonBox::RejectRole);
}

void w_scr_dish_dish_quit::toallquit()
{
    QDateTime dt = n_func::f_get_sysdatetime();
    lds_query query;
    if(n_func::f_get_sysparm_q(query,"orderdish_back") == "1"
            && ui->comboBox_vch_reason->currentText().trimmed().isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("请输入退菜原因"));
        return;
    }
    if(tablemodel->rowCountOp(lds_model_sqltablemodel::OpInsert) > 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("有菜品未落单"));
        return;
    }
    if(0==lds_messagebox::question(this, MESSAGE_TITLE_VOID,tr("是否整单退菜？"),tr("确认"),tr("取消"))) {
        QString errstring;
        lds_query::tran_saction();
        for(int row = 0, rowcount = tablemodel->rowCount(); row < rowcount; row++) {
            num_backData d;
            d.int_flowid = tablemodel->model_data(row, "int_flowid").toLongLong();
            d.num_back_add = tablemodel->model_data_NIBOLAN(row, "num_num-num_back");
            d.dt_back_operdate = dt;
            d.vch_back_operid = public_sql::gs_operid;
            d.vch_reason = ui->comboBox_vch_reason->currentText();
            d.vch_print_memo = tablemodel->model_data(row, "vch_print_memo").toString();

            if(toquitrow(errstring, d)) continue;
            goto rollback;
        }
        if(query.execUpdate("cey_u_table","num_cost", 0, qrtEqual("ch_billno", tablemodel->cur_ch_billno()))) {
            lds_query::com_mit();
            lds_messagebox::information(this, MESSAGE_TITLE_VOID, tr("退菜成功"));

            public_printby_ch_billno  printer(_ch_billno);
            printer.toprint_dish_return(printer.defpath(), dt);
            this->done(QDialogButtonBox::ApplyRole);
            return;
        }
        goto rollback;
    }
    return;
rollback:
    lds_query::roll_back();
    lds_messagebox::information(this, MESSAGE_TITLE_VOID, tr("退菜失败"));
}

bool w_scr_dish_dish_quit::toquitrow(QString &errstring, const num_backData &d)
{
    return fexpandmain_2::static_toquit_1_only(errstring, d);
}
