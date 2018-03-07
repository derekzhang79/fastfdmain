#include "w_i_inventory_query.h"
//库存查询
#include "ui_w_i_inventory_query_dialog.h"
#include "backheader.h"
#include "n_func.h"
#include "lds_dialog_input.h"
#include <QSqlError>
#include "lds_messagebox.h"
#include "lds.h"
#include "w_rpt_report_dataexport.h"
#include "w_rpt_report_printformat.h"

w_i_inventory_query::w_i_inventory_query(QWidget *parent) :
    QDialog(parent),ui(new Ui_w_i_inventory_query_Dialog)
{
    ui->setupUi(this);
    ui->frame->setFixedWidth(w_rpt_report::frame_left_fixed_width);
    //菜品大类
    ui->comboBox_series->addItems(w_rpt_report::querymodelsetfilters("select * from cey_bt_dish_type"));
    querymodel=new report_querymodel(this);
    ui->tableView->setTransferHheader();
    ui->tableView->setModel(querymodel);
    ui->tableView->setEditTriggers(QTableView::AllEditTriggers);
    //
    report_standmodel *m_sum = new report_standmodel(this);
    m_sum->setHorizontalHeaderLabels(QStringList() << tr("项目") << tr("合计"));
    m_sum->setup(ui->tableView_sum);
    ui->tableView_sum->verticalHeader()->hide();
    ui->tableView_sum->setEditTriggers(QTableView::NoEditTriggers);
    ui->tableView_sum->setSelectionMode(QTableView::SingleSelection);
    ui->tableView_sum->setSelectionBehavior(QTableView::SelectRows);
    ui->tableView_sum->setShowGrid(false);
    ui->tableView_sum->setFocusPolicy(Qt::NoFocus);
    QFont f = ui->tableView_sum->font();
    f.setPointSize(14);
    ui->tableView_sum->setFont(f);
    ui->tableView_sum->resizeColumnsToContents();
    //! toolbar
    ui->widget->setup(QStringList() << tr("查询") /*<< tr("修改库存")*/ << tr("打印") << tr("导出") << tr("退出"), Qt::AlignTop);
    connect(ui->widget->index_widget(tr("查询")),SIGNAL(clicked()),this,SLOT(toselect()));
    connect(ui->widget->index_widget(tr("退出")),SIGNAL(clicked()),this,SLOT(toexit()));
    connect(ui->widget->index_widget(tr("打印")),SIGNAL(clicked()),this,SLOT(toprint()));
    connect(ui->widget->index_widget(tr("导出")),SIGNAL(clicked()),this,SLOT(toexport()));

    ui->checkBox->setChecked(lds::sysconf->value("w_i_inventory_query/isbucang", false).toBool());
    connect(ui->checkBox, SIGNAL(toggled(bool)),this,SLOT(tosave_bucang(bool)));
}

void w_i_inventory_query::tab1sqlinit()
{

}

void w_i_inventory_query::toselect()
{
    //查询时间
    ui->label_querytime->setText(n_func::f_get_sysdatetime().toString("yyyy-MM-dd hh:mm:ss"));
    //数据库sql
    QString from_sql
            =  " FROM cey_bt_dish a"

               " LEFT OUTER JOIN cey_st_material_num b"

               " ON a.ch_dishno = b.ch_materialno"

               " where IFNULL(a.item_flag, 'N') = 'Y'"

               " and (a.ch_sub_typeNo like '%1%' and a.ch_dishno like '%2%') "

            +QString(ui->checkBox->isChecked()? " and  b.num_num <= a.num_item_warn": "")+

            " ORDER BY num_store desc";
    ;
    from_sql = from_sql
            .arg(backheader::getinterdata(ui->comboBox_series->currentText()))
            .arg(ui->selectgoods->gettext())
            .arg(w_rpt_report::querymodelordersort(ui->tableView));

    querymodel->setQuery(
                "select ch_dishno ,"

                " vch_dishname ,"

                " (select vch_unitname from cey_bt_unit where cey_bt_unit.ch_unitno = t.ch_unitno) as vch_unitname,"

                " num_store ,"

                " num_cost as num_store_price ,"

                " num_store * num_cost as num_store_total,"

                " (select vch_dish_typename from cey_bt_dish_type where cey_bt_dish_type.ch_dish_typeno = t.ch_dish_typeno) as vch_dish_typename,"

                " (select vch_sub_typename from cey_bt_dish_subtype where cey_bt_dish_subtype.ch_sub_typeno = t.ch_sub_typeno) as vch_sub_typename"

                +QString(ui->checkBox->isChecked()? ", num_item_warn": "")+

                " from ("

                "SELECT a.ch_dishno, a.vch_dishname, a.int_unit_rate, a.ch_unitno,"

                " a.vch_pur_unitno,"

                " a.ch_dish_typeno,"

                " a.ch_sub_typeno,"

                " IFNULL(b.num_num, 0) as num_store,"

                " IFNULL(b.cost_price, 0) as num_cost,"

                " a.num_item_warn "

                + from_sql
                + ") t"
                );
    ui->tableView->resizeColumnsToContents();
    //
    lds_query query;
    query.execSelect("select sum(b.num_num) as num_store, sum(b.num_num * b.cost_price) as num_store_total "
                     + from_sql);
    query.next();
    QStandardItemModel *m = static_cast<QStandardItemModel *>(ui->tableView_sum->model());
    m->removeRows(0 , m->rowCount());
    m->appendRow(QList<QStandardItem *> ()
                 << new QStandardItem("num_store") << report_standmodel::newItem(query.recordValue("num_store")));
    m->appendRow(QList<QStandardItem *> ()
                 << new QStandardItem("num_store_total") << report_standmodel::newItem(query.recordValue("num_store_total")));
    ui->tableView_sum->resizeColumnsToContents();
}

void w_i_inventory_query::toexit()
{
    this->reject();
}

void w_i_inventory_query::toexport()
{
    QString title = this->windowTitle();
    if(ui->checkBox->isChecked())
        title = ui->checkBox->text();

    w_rpt_report_dataExport dialog(ui->tableView, 0, this);
    dialog.setWindowTitle(title);
    dialog.defFilepath(title);
    lds_roundeddialog_rect_align(&dialog).exec();
}

void w_i_inventory_query::toprint()
{
    QString title = this->windowTitle();
    if(ui->checkBox->isChecked())
        title = ui->checkBox->text();

    QPushButton *b = qobject_cast<QPushButton *>(this->sender());

    w_rpt_report_printFormat *dialog
            = new w_rpt_report_printFormat(w_rpt_report_printFormat::show_model_quick, this->windowTitle(), public_sql::gs_operid, n_func::f_get_sysdatetime_str(),
                                           this->metaObject()->className(),
                                           "", "",
                                           ui->tableView, ui->tableView_sum,
                                           0
                                           );
    dialog->pop(b, this);
}

void w_i_inventory_query::tosave_bucang(bool f)
{
    lds::sysconf->setValue("w_i_inventory_query/isbucang", f);
}
