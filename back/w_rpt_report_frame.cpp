#include "w_rpt_report_frame.h"
#include "ui_w_rpt_report_frame.h"
#include "lds_dialog_input.h"
#include <QStandardItemModel>
#include <QtDebug>
#include "w_rpt_report_dataexport.h"
#include "w_rpt_report_printformat.h"
#include "public_sql.h"
#include "n_func.h"
#include <QTableView>
#include "lds_model_sqltablemodel_delegate_com.h"
#include <QScrollBar>
#include "lds_tableview.h"
#include "w_rpt_report.h"
#include "lds.h"
#include "lds_model_sqltablemodel.h"
#include <QSqlRecord>
#include "lds_tableview_header.h"
#include "lds_menu.h"

QMap<QString, QString> w_rpt_report_frame::map;

static QSqlQueryModel *get_model_default_insert_one_row(QObject *parent) {
    QSqlQueryModel *m = new report_querymodel(parent);
    return m;
}

static lds_tableView *get_tableview(QSqlQueryModel *m){
    lds_tableView*tableview = new lds_tableView;
    tableview->setTransferHheader();
    tableview->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    tableview->setSelectionMode(QTableView::SingleSelection);
    tableview->setSelectionBehavior(QTableView::SelectRows);
    tableview->setEditTriggers(QTableView::NoEditTriggers);
    tableview->setModel(m);
    return tableview;
}

w_rpt_report_frame::w_rpt_report_frame(const QStringList &title_list, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_rpt_report_frame)
{
    ui->setupUi(this);
    //
    report_standmodel *m_sum = new report_standmodel(this);
    m_sum->setHorizontalHeaderLabels(QStringList() << tr("项目") << tr("合计"));
    ui->tableView_sum->verticalHeader()->hide();
    ui->tableView_sum->setEditTriggers(QTableView::NoEditTriggers);
    ui->tableView_sum->setModel(m_sum);
    ui->tableView_sum->setSelectionMode(QTableView::SingleSelection);
    ui->tableView_sum->setSelectionBehavior(QTableView::SelectRows);
    ui->tableView_sum->setShowGrid(false);
    ui->tableView_sum->setFocusPolicy(Qt::NoFocus);
    ui->tableView_sum->lds_tableview_delegate_virtual_enableFieldName2k0v1();
    QFont f = ui->tableView_sum->font();
    f.setPointSize(14);
    ui->tableView_sum->setFont(f);
    //dt
    QDateTime dt = n_func::f_get_sysdatetime();
    w_rpt_report::reportBeginDtSet(ui->dateTimeEdit);
    ui->dateTimeEdit_2->setDateTime(dt);
    //
    for(int k = 0; k < title_list.count(); k++) {
        ui->stackedWidget->insertWidget(0, get_tableview(get_model_default_insert_one_row(this)));
        ui->pushButton_step->insert_d(0);
    }
    ui->comboBox->addItems(title_list);
    ui->stackedWidget->setCurrentIndex(0);
    ui->pushButton_step->update_map_widget_visiable(0);
    //
    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)),this,SLOT(toupdate_limitbtn(int)));
    connect(ui->pushButton_step, SIGNAL(signal_step_change()),this,SLOT(toselect_page()));
    connect(ui->pushButton_up, SIGNAL(clicked()),this, SLOT(toup()));
    connect(ui->pushButton_down, SIGNAL(clicked()),this, SLOT(todown()));
    connect(ui->pushButton_step, SIGNAL(clicked()),this,SLOT(tochoosepage()));
    //
    sql_count_arg1 = "select count(0) from (select 0 %1 )t";
}

w_rpt_report_frame::~w_rpt_report_frame()
{
}

const QMap<QString, QString>& w_rpt_report_frame::variant_transfer_map()
{
    if(map.isEmpty()) {
        ///流水
        map.insert("int_flowid", tr("序号"));
        map.insert("int_id", tr("序号"));
        map.insert("num_dish", tr("数量"));
        map.insert("num_num", tr("点菜数量"));
        map.insert("num_back", tr("退菜数量"));
        map.insert("num_material", tr("原料数量"));
        map.insert("num_num_change", tr("替换数量"));

        map.insert("ch_dishno", tr("品码"));
        map.insert("vch_dishname", tr("菜品"));
        map.insert("ch_materialno", tr("原料菜品"));
        map.insert("ch_discount", tr("折扣"));
        map.insert("ch_curprice", tr("时价菜"));
        map.insert("ch_groupno", tr("组号"));
        map.insert("num_price", tr("单价"));
        map.insert("num_sale_price", tr("进价"));
        map.insert("num_present", tr("赠送"));

        map.insert("ch_no", tr("编号"));
        map.insert("vch_cp_memo", tr("特殊做法"));
        map.insert("num_add_price", tr("加价"));//做法设置的加价


        map.insert("int_divno", tr("分单号"));
        map.insert("num_discount", tr("折扣"));
        map.insert("vch_print_memo", tr("做法"));
        map.insert("num_price_add", tr("加价"));//点菜流水的 加价

        map.insert("num_total", tr("小计"));
        map.insert("num_total2", tr("总计"));
        map.insert("num_total3", tr("总计(加价)"));

        map.insert("ch_payno", tr("付款号"));
        map.insert("ch_unitno", tr("单位"));
        map.insert("vch_unitname", tr("单位"));
        map.insert("vch_pur_unitno", tr("入库单位"));
        map.insert("ch_dish_typeno", tr("菜品大类"));
        map.insert("vch_dish_typename", tr("菜品大类"));
        map.insert("ch_sub_typeno", tr("菜品小类"));
        map.insert("vch_sub_typename", tr("菜品小类"));
        map.insert("ch_tableno", tr("餐桌"));
        map.insert("num_table", tr("餐桌数量"));
        map.insert("num_kaitai", tr("开台"));
        map.insert("num_kongxian", tr("空闲"));
        map.insert("vch_tablename", tr("餐桌名称"));
        map.insert("ch_typeno", tr("类型"));

        map.insert("ch_paymodeno", tr("收银代码"));
        map.insert("vch_paymodename", tr("收银方式"));
        map.insert("num_paymode_count", tr("收银次数"));
        map.insert("num_table_count", tr("台数"));
        map.insert("num_realamount", tr("实收金额"));
        map.insert("num_change", tr("找零"));
        map.insert("num_payamount", tr("付款金额"));
        map.insert("vch_voucherno", tr("付款凭证"));
        map.insert("int_deduct", tr("提成比率"));
        map.insert("num_deduct", tr("提成定额"));

        map.insert("num_realamount_income", tr("收入"));
        map.insert("num_realamount_unincome", tr("非收入"));
        map.insert("num_blotout", tr("抹零"));
        map.insert("num_get", tr("收取"));
        map.insert("num_free", tr("优惠"));
        map.insert("num_service", tr("服务费"));
        map.insert("num_lowcost", tr("低消"));
        map.insert("vch_back_operid", tr("退菜人"));
        map.insert("dt_back_operdate", tr("退菜时间"));
        map.insert("vch_reason", tr("原因"));
        map.insert("int_discount", tr("折扣"));
        map.insert("vch_dis_operid", tr("折扣人"));
        map.insert("dt_dis_operdate", tr("折扣时间"));
        map.insert("ch_presentflag", tr("是否赠送"));
        map.insert("vch_pre_operid", tr("赠送人"));
        map.insert("dt_pre_operdate", tr("赠送时间"));
        map.insert("ch_suitflag", tr("是否套菜"));
        map.insert("ch_suitno", tr("套菜"));
        map.insert("ch_specialflag", tr("是否特价"));
        map.insert("ch_billno", tr("单号"));
        map.insert("ch_sheetno", tr("单号"));
        map.insert("int_person", tr("人数"));
        map.insert("num_cost", tr("消费"));
        map.insert("num_rate", tr("税收"));
        map.insert("int_rate", tr("税收"));
        map.insert("num_cash", tr("应收"));
        map.insert("num_fdeduct", tr("定额提成"));
        map.insert("num_rdeduct", tr("比例提成"));

        ///会员
        map.insert("vch_member", tr("会员名称"));
        map.insert("vch_memberno", tr("会员编号"));
        map.insert("m_typename", tr("卡类型"));
        map.insert("ch_attribute", tr("卡种类"));
        map.insert("vch_handtel", tr("手机号"));
        map.insert("num_deposit", tr("充值金额"));
        map.insert("ch_type", tr("类型"));
        map.insert("ch_pay_mode", tr("支付方式"));

        map.insert("ch_state", tr("状态"));
        map.insert("ch_cardflag", tr("卡状态"));
        map.insert("int_basedata", tr("保本金额"));
        map.insert("m_cardcount", tr("卡总量"));
        map.insert("m_unsendcardcount", tr("正常未发卡量"));

        map.insert("m_sendcardcount", tr("正常已发卡量"));
        map.insert("m_stopcardcount", tr("止付卡量"));
        map.insert("m_hangcardcount", tr("挂失卡量"));
        map.insert("m_badcardcount", tr("作废卡量"));
        map.insert("num_remain", tr("剩余金额"));
        map.insert("num_remain_pay", tr("未还款金额"));


        map.insert("num_pay", tr("付款金额"));
        map.insert("vch_operid", tr("操作员"));
        map.insert("vch_operator", tr("名称"));
        map.insert("vch_waiter", tr("服务员"));
        map.insert("dt_operdate", tr("时间"));
        map.insert("date", tr("日期"));
        map.insert("vch_memo", tr("备注"));
        map.insert("ch_balanceno", tr("交易单号"));

        map.insert("m_deposit", tr("充值金额"));
        map.insert("m_deposit_real", tr("充值实收"));
        map.insert("m_deposit_discount", tr("充值优惠"));
        map.insert("num_point", tr("积分"));

        ///库存
        map.insert("num_store", tr("库存数量"));
        map.insert("num_store_total", tr("库存金额"));
        map.insert("num_store_price", tr("进价"));
        map.insert("num_item_warn", tr("预警下限"));
        map.insert("num_sys", tr("系统库存"));
        map.insert("num_check", tr("盘点数量"));
        map.insert("num_pro_loss", tr("盈亏数量"));
        map.insert("vch_audit", tr("审核人"));
        map.insert("dt_audit", tr("审核日期"));
        map.insert("vch_handle", tr("经手人"));
        map.insert("chg_flag", tr("调整库存"));
        //
        map.insert("do1", tr("操作"));
        map.insert("do2", tr("操作"));
        map.insert("do3", tr("操作"));
    }
    return map;
}

void w_rpt_report_frame::clearCache()
{
    map.clear();
}

int w_rpt_report_frame::doubleValueDecimal(const QString &map_chinese_value)
{
    if(map_chinese_value == tr("数量")//
            || map_chinese_value == tr("点菜数量")//
            || map_chinese_value == tr("退菜数量")//
            || map_chinese_value == tr("原料数量")) {//

        return 3;
    }
    return 2;
}

QString w_rpt_report_frame::headdata_transfer(const QVariant &dd)
{
    QString hkey =  dd.toString();
    QString hvalue = w_rpt_report_frame::variant_transfer_map().value(hkey.toLower(), hkey);
    return hvalue;
}

void w_rpt_report_frame::sum_filldata(const QList<w_rpt_report_frame_btn_pageindex::total_desc_value_Data> &d_list)
{
    QStandardItemModel *m = static_cast<QStandardItemModel *>(ui->tableView_sum->model());
    m->removeRows(0 , m->rowCount());
    foreach(const w_rpt_report_frame_btn_pageindex::total_desc_value_Data &d, d_list) {
        QString fieldname = d.fieldname;
        m->appendRow(QList<QStandardItem *> ()
                     << new QStandardItem(fieldname.replace("`", "")) << report_standmodel::newItem(d.value));
        m->item(m->rowCount() - 1, 1)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    }
    ui->tableView_sum->resizeColumnsToContents();
    QTimer::singleShot(10, this, SLOT(sum_hscroll_set_value_1()));
}

void w_rpt_report_frame::sum_hscroll_set_value_1()
{
    ui->tableView_sum->horizontalScrollBar()->setValue(1);
}

void w_rpt_report_frame::toup()
{
    if(false == ui->pushButton_step->up(ui->stackedWidget->currentIndex())) {
        toPageUp();
    }
}

void w_rpt_report_frame::todown()
{
    if(false == ui->pushButton_step->down(ui->stackedWidget->currentIndex())){
        toPageDown();
    }
}

void w_rpt_report_frame::tochoosepage()
{
    lds_dialog_input inputdialog(this);
    inputdialog.ui->label->setText(tr("页数"));
    inputdialog.setWindowTitle(tr("选择页数"));
    inputdialog.setDotEnabled(false);
    inputdialog.ui->lineEdit->setText(QString::number(ui->pushButton_step->get_cur_page(ui->stackedWidget->currentIndex())));
    if(QDialog::Accepted==lds_roundeddialog_rect_align(&inputdialog).exec()){
        ui->pushButton_step->setpage(ui->stackedWidget->currentIndex(), inputdialog.ui->lineEdit->text().toInt() - 1);
    }
}

void w_rpt_report_frame::toupdate_limitbtn(int index)
{
    ui->stackedWidget->setCurrentIndex(index);
    ui->pushButton_step->setText(ui->pushButton_step->get_desc(index));
    ui->pushButton_step->update_map_widget_visiable(index);
    sum_filldata(ui->pushButton_step->get_sum_list_total_data(index));
}

void w_rpt_report_frame::toselect_page()
{
    int index = ui->stackedWidget->currentIndex();
    lds_tableView *tableview = static_cast<lds_tableView *>(ui->stackedWidget->widget(index));
    report_querymodel *model = static_cast<report_querymodel *>(tableview->model());
    model->setQuery(ui->pushButton_step->get_limit_sql_update_desc(index));
    select_page_after();
    tableview->resizeColumnsToContents2();
}

void report_querymodel::setQuery(const QString &query, const QSqlDatabase &db)
{
    qDebug() << this << ++querycount;
    return QSqlQueryModel::setQuery(query, db);
}

QVariant report_querymodel::model_data(int row, const QString &field_name) const
{
#ifdef QT_DEBUG
    if(this->fieldIndex(field_name) < 0) {
        qDebug() << __FUNCTION__<<row << field_name << this->fieldIndex(field_name);
        QObject *x = 0;
        x->children();
    }
#endif
    return data(this->index(row, this->fieldIndex(field_name)));
}

int report_querymodel::fieldIndex(const QString &fieldname) const
{
    for(int column = 0, columncount =  this->query().record().count(); column < columncount; column ++) {
        if(0 == this->query().record().fieldName(column).compare(fieldname, Qt::CaseInsensitive)) {
            return column;
        }
    }
    qWarning("fieldName not found") ;
    return -1;
}

void w_rpt_report_frame::toexport()
{
    exportSql();
}

void w_rpt_report_frame::toprint()
{
    printSql();
}

void w_rpt_report_frame::toprintmore()
{

}

void w_rpt_report_frame::toprint58()
{

}

void w_rpt_report_frame::toprint80()
{

}


void w_rpt_report_frame::toexit()
{
    this->reject();
}

void w_rpt_report_frame::toPageUp()
{
    lds_tableView *tableview = static_cast<lds_tableView *>(ui->stackedWidget->currentWidget());
    tableview->toPageUp();
}

void w_rpt_report_frame::toPageDown()
{
    lds_tableView *tableview = static_cast<lds_tableView *>(ui->stackedWidget->currentWidget());
    tableview->toPageDown();
}

void w_rpt_report_frame::exportSql()
{
    int index  = ui->stackedWidget->currentIndex();
    lds_tableView *tableview = static_cast<lds_tableView *>(ui->stackedWidget->widget(index));

    w_rpt_report_dataExport dialog(tableview, ui->pushButton_step->get_limit_sql(index), ui->tableView_sum, this);
    dialog.setWindowTitle(ui->comboBox->currentText());
    dialog.defFilepath(ui->comboBox->currentText());
    lds_roundeddialog_rect_align(&dialog).exec();
}

void w_rpt_report_frame::printSql()
{
    int index = ui->stackedWidget->currentIndex();
    lds_tableView *tableview = static_cast<lds_tableView *>(ui->stackedWidget->widget(index));

    w_rpt_report_printFormat dialog(w_rpt_report_printFormat::show_model_more, ui->comboBox->currentText(), public_sql::gs_operid, n_func::f_get_sysdatetime_str(),
                                    QString("%1_%2").arg(this->metaObject()->className()).arg(index),
                                    ui->dateTimeEdit->dateTimeStr(), ui->dateTimeEdit_2->dateTimeStr(),
                                    tableview, ui->pushButton_step->get_limit_sql(index), ui->tableView_sum,
                                    this);
    dialog.setWindowTitle(MESSAGE_TITLE_VOID);
    lds_roundeddialog_rect_align d(&dialog);
    d.exec();
}

void w_rpt_report_frame::exportModel()
{
    lds_tableView *tableview = static_cast<lds_tableView *>(ui->stackedWidget->currentWidget());
    w_rpt_report_dataExport dialog(tableview, ui->tableView_sum, this);
    dialog.setWindowTitle(ui->comboBox->currentText());
    dialog.defFilepath(ui->comboBox->currentText());
    lds_roundeddialog_rect_align(&dialog).exec();
}

void w_rpt_report_frame::printModel()
{
    int index = ui->stackedWidget->currentIndex();
    lds_tableView *tableview = static_cast<lds_tableView *>(ui->stackedWidget->widget(index));

    w_rpt_report_printFormat dialog(w_rpt_report_printFormat::show_model_more,  ui->comboBox->currentText(),
                public_sql::gs_operid,
                n_func::f_get_sysdatetime_str(),
                QString("%1_%2").arg(this->metaObject()->className()).arg(index),
                ui->dateTimeEdit->dateTimeStr(),
                ui->dateTimeEdit_2->dateTimeStr(),
                tableview,
                ui->tableView_sum,
                this );
    dialog.setWindowTitle(MESSAGE_TITLE_VOID);
    lds_roundeddialog_rect_align(&dialog).exec();
}

void w_rpt_report_frame::resize_column_to_content_t()
{
    QTimer::singleShot(0, this, SLOT(to_resize_column_to_content()));
}

void w_rpt_report_frame::select_page_after()
{

}

void w_rpt_report_frame::tableview_setpan(const QString &cmp_column, const QStringList &span_column_list)
{
    int index = ui->stackedWidget->currentIndex();
    lds_tableView *tableview = static_cast<lds_tableView *>(ui->stackedWidget->widget(index));
    report_querymodel *model = static_cast<report_querymodel *>(tableview->model());

    QList<int> column_list;
    foreach(const QString &c, span_column_list) {
        column_list << model->fieldIndex(c);
    }

    lds_tableview_virtual::tableviewSetpan(tableview, model->fieldIndex(cmp_column), column_list);
}

void w_rpt_report_frame::to_resize_column_to_content()
{
    lds_tableView *tableview = static_cast<lds_tableView *>(ui->stackedWidget->currentWidget());
    tableview->resizeColumnsToContents();
}

QVariant report_standmodel::data(const QModelIndex &index, int role) const
{
    QVariant d = QStandardItemModel::data(index, role);

    if(b_data_transfer && role == Qt::TextColorRole) {
        if(index.column() % 2 == 1) {
            return QColor("#ff650b");
        }
    }

    return d;
}

QStandardItem *report_standmodel::newItem(const QVariant &value)
{
    QStandardItem *item = new QStandardItem;
    item->setData(value, Qt::EditRole);
    return item;
}

void report_standmodel::setup(lds_tableView *tableview)
{
    tableview->lds_tableview_delegate_virtual_enableFieldName2k0v1();
    tableview->setModel(this);
}

void report_standmodel::disable_data_transfer()
{
    b_data_transfer = false;
}

