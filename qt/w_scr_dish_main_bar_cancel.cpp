#include "w_scr_dish_main_bar_cancel.h"
#include "ui_w_scr_dish_main_bar_cancel.h"
#include "public_printby_ch_billno.h"
#include "lds_messagebox.h"
#include "lds_model_sqltablemodel_delegate_com.h"
#include <QtDebug>
#include "n_func.h"
#include "w_rpt_report_frame.h"
#include "w_scr_dish_restaurant_div_pop.h"
#include <QTimer>


bool w_scr_dish_main_bar_cancel::cey_bt_table_bar_dish_existed(const QString &ch_tableno, int int_div_id)
{
    lds_query query;
    query.execSelect(QString("SELECT ch_billno from cey_bt_table_bar where ch_tableno = '%1'  and int_div_id = %2 ")
               .arg(ch_tableno)
               .arg(int_div_id));
    while(query.next()) {
        if(w_scr_dish_restaurant_div_pop::cey_u_orderdish_dish_existed(query.recordValue("ch_billno").toString())) {
            return true;
        }
    }
    return false;
}

bool w_scr_dish_main_bar_cancel::cey_bt_table_bar_clear_transaction(const QString &ch_tableno, int int_div_id, QString *errstring)
{
    lds_query query;
    lds_query::tran_saction();
    if(query.execDelete("cey_bt_table_bar", QString(" ch_tableno = '%1' and int_div_id = %2 ")
                  .arg(ch_tableno)
                  .arg(int_div_id))) {
        lds_query::com_mit();
        return true;
    }
    lds_query::roll_back();
    if(errstring) *errstring = query.recordError();
    return false;
}

void w_scr_dish_main_bar_cancel::cey_bt_table_bar_print(QWidget *parent, const QString &ch_tableno, int int_div_id)
{
    QStringList ch_billno_list;
    lds_query query;
    query.execSelect(QString("SELECT ch_billno from cey_bt_table_bar where ch_tableno = '%1' and int_div_id = %2 ")
               .arg(ch_tableno)
               .arg(int_div_id));
    while(query.next()) {
        if(w_scr_dish_restaurant_div_pop::cey_u_orderdish_dish_existed(query.recordValue("ch_billno").toString())) {
            ch_billno_list.append(query.recordValue("ch_billno").toString());
        }
    }
    //
    if(ch_billno_list.count() == 0) {
        lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, tr("没有需要打印的数据"));
        return;
    }
    //
    if(0 == lds_messagebox::question(parent, MESSAGE_TITLE_VOID, tr("打印消费单"), tr("是"), tr("否"))){
        QString ch_billno = ch_billno_list.first();
        public_printby_ch_billno printer(ch_billno);//所有单号的前12位都一样，所以对应的桌号也一致
        printer.toprint_preprint_div_title(printer.defpath(), tr("消费单"));
        double subtotal = 0;
        int idvcount = 0;
        foreach(ch_billno, ch_billno_list) {
            public_printby_ch_billno printer(ch_billno);
            subtotal += printer.toprint_preprint_div(printer.defpath());
            idvcount ++;
        }
        printer.clear_ins();
        printer.toprint_preprint_div_tail(printer.defpath(), subtotal, idvcount);
    }
}

w_scr_dish_main_bar_cancel::w_scr_dish_main_bar_cancel(const QString &ch_tableno, int int_div_id, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_scr_dish_main_bar_cancel)
{
    ui->setupUi(this);
    this->ch_tableno = ch_tableno;
    this->int_div_id = int_div_id;

    map.insert( "P", tr("主套菜"));
    map.insert( "Y", tr("子套菜"));

    tablemodel = new lds_model_sqltablemodel(this);
    tablemodel->setTable("cey_u_orderdish");
    tablemodel->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);


    ui->tableView->setModel(tablemodel);
    ui->tableView->setTransferHheader();
    ui->tableView->setEditTriggers(QTableView::NoEditTriggers);
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("ch_suitflag"), new lds_model_sqltablemodel_delegate_com(
                                                this, &map
                                                ));
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("ch_dishno"), new lds_model_sqltablemodel_delegate_com(
                                                this, tablemodel, "cey_bt_dish","ch_dishno","vch_dishname"
                                                ));
    ui->tableView->rePecifyHeader(QStringList() << "ch_tableno" << "ch_billno"<< "ch_dishno"<< "num_num"<< "num_back"<< "ch_suitflag");

    //connect
    connect(ui->pushButton_cancel, SIGNAL(clicked()), this,SLOT(tocancel()));
    connect(ui->pushButton_ok, SIGNAL(clicked()), this,SLOT(took()));
    connect(ui->pushButton_print, SIGNAL(clicked()), this,SLOT(toprint()));

    this->setWindowTitle(tr("清台"));
    QTimer::singleShot(0, this, SLOT(torefresh()));
}

w_scr_dish_main_bar_cancel::~w_scr_dish_main_bar_cancel()
{
    delete ui;
}

void w_scr_dish_main_bar_cancel::tocancel()
{
    this->reject();
}

void w_scr_dish_main_bar_cancel::took()
{
    if(1 == lds_messagebox::question(this ,MESSAGE_TITLE_VOID, tr("确认清台"), tr("确定"), tr("取消"))) {
        return;
    }
    QString errstring;
    if(cey_bt_table_bar_clear_transaction(ch_tableno, int_div_id, &errstring)) {
        this->accept();
        return;
    }
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
}

void w_scr_dish_main_bar_cancel::toprint()
{
    cey_bt_table_bar_print(this, ch_tableno, int_div_id);
}

void w_scr_dish_main_bar_cancel::torefresh()
{
    lds_query query;
    QStringList inch_billnos;
    query.execSelect(QString("select ch_billno from cey_bt_table_bar where ch_tableno = '%1' and int_div_id = %2 ")
                .arg(ch_tableno)
                .arg(int_div_id));
    while(query.next()) {
        inch_billnos << query.recordValue("ch_billno").toString();
    }

    //select
    tablemodel->setFilter(QString("ch_billno in ('%1')")
                          .arg(inch_billnos.join("','")));
    ui->tableView->resizeColumnsToContents();
    lds_tableview_virtual::tableviewSetpan(ui->tableView, tablemodel->fieldIndex("ch_tableno"),
                                        QList<int>() << tablemodel->fieldIndex("ch_tableno"));
    lds_tableview_virtual::tableviewSetpan(ui->tableView, tablemodel->fieldIndex("ch_billno"),
                                        QList<int>() << tablemodel->fieldIndex("ch_billno"), false);
}

w_scr_dish_main_bar_cancel_print::w_scr_dish_main_bar_cancel_print(const QString &ch_tableno, int int_div_id, QWidget *parent)
    : w_scr_dish_main_bar_cancel(ch_tableno, int_div_id, parent)
{
    ui->pushButton_ok->hide();
}

w_scr_dish_main_bar_cancel_print::~w_scr_dish_main_bar_cancel_print()
{

}
