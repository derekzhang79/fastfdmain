#include "w_sys_manage_outer_bill_preview.h"
#include "ui_w_sys_manage_outer_bill_preview.h"
#include "ftableview_delegate.h"
#include "ftableview_standmodel_sql_none.h"
#include "n_func.h"
#include "lds.h"
#include "getfilepath.h"
#include "public_printby_ch_billno.h"
#include "lds_menu.h"
#include "lds.h"
#include "lds_messagebox.h"

static QString map_filter = "bp_fmt";
static QString ifEmpty(const QString &text, const QString &def){
    if(text.trimmed().isEmpty()) {
        return def;
    }
    return text;
}

w_sys_manage_outer_bill_preview::w_sys_manage_outer_bill_preview(QWidget *parent) :
    lds_model_mapwidget_vertical(parent),
    ui(new Ui::w_sys_manage_outer_bill_preview)
{
    ui->setupUi(this);
    ui->bp_fmt_series_group->addItem(tr("不分组"), "0");
    ui->bp_fmt_series_group->addItem(tr("按大类"), "1");
    ui->bp_fmt_series_group->addItem(tr("按小类"), "2");

    ui->bp_fmt_dishname_row->addItem("1", 1);
    ui->bp_fmt_dishname_row->addItem("2", 2);

    ui->bp_fmt_amount_row->addItem("1", 1);
    ui->bp_fmt_amount_row->addItem("2", 2);

    ui->bp_fmt_price_row->addItem("1", 1);
    ui->bp_fmt_price_row->addItem("2", 2);

    ui->bp_fmt_unit_row->addItem("1", 1);
    ui->bp_fmt_unit_row->addItem("2", 2);

    ui->bp_fmt_disdetail_row->addItem("1", 1);
    ui->bp_fmt_disdetail_row->addItem("2", 2);

    ui->bp_fmt_total_row->addItem("1", 1);
    ui->bp_fmt_total_row->addItem("2", 2);

    ui->bp_fmt_num_row->addItem("1", 1);
    ui->bp_fmt_num_row->addItem("2", 2);

    ui->bp_fmt_table_fmts->addItem(tr("餐桌编码"), "no");
    ui->bp_fmt_table_fmts->addItem(tr("餐桌"), "name");
    ui->bp_fmt_table_fmts->addItem(tr("餐桌")+"("+tr("编码")+")", "name(no)");

    ui->bp_fmt_operid_fmts->addItem(tr("工号"), "no");
    ui->bp_fmt_operid_fmts->addItem(tr("名称"), "name");
    ui->bp_fmt_operid_fmts->addItem(tr("名称")+"("+tr("工号")+")", "name(no)");

    //1
    ftableview_standmodel_sql_none * backlist_model = new ftableview_standmodel_sql_none;
    backlist_model->set_row_column(3, 1);
    backlist_model->model_data_set(0, 0,tr("表头设置"), Qt::UserRole);
    backlist_model->model_data_set(0, 0,"anyvalue", Qt::UserRole+1);

    backlist_model->model_data_set(1, 0,tr("账单内容"), Qt::UserRole);
    backlist_model->model_data_set(1, 0,"anyvalue", Qt::UserRole+1);

    backlist_model->model_data_set(2, 0,tr("表尾内容"), Qt::UserRole);
    backlist_model->model_data_set(2, 0,"anyvalue", Qt::UserRole+1);
    ftableview_delegate *d = new ftableview_delegate(ui->tableView_list);
    d->keepConnect(ui->tableView_list);
    d->setMargin(2);
    d->indexisEmptystillDraw = true;
    ui->tableView_list->setProperty("outer_stylesheet", "tableView_type_pop");

    ui->tableView_list->setModel(backlist_model);
    ui->tableView_list->setItemDelegate(d);
    ui->tableView_list->setHeaderResizeMode(Qt::Horizontal, QHeaderView::Stretch);
    ui->tableView_list->verticalHeader()->setDefaultSectionSize(50);
    //3
    ui->tableView_list->setCurrentIndex(backlist_model->index(0, 0));
    toupdateSection(ui->tableView_list->currentIndex());

    tablemodel = new lds_model_sqltablemodel(this);
    tablemodel->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    tablemodel->setTable("cey_sys_parameter");

    mapper = new QDataWidgetMapper(this);
    mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
    mapper->setModel(tablemodel);
    mapper->setOrientation(Qt::Vertical);

    scan_all_objectname_add_map_setFilter("vch_parameter", map_filter);
    //
    mapper->setCurrentIndex(1);
    //
    connect(ui->tableView_list,SIGNAL(clicked(QModelIndex)),this,SLOT(toupdateSection(QModelIndex)));
    connect(ui->pushButton_ok, SIGNAL(clicked()), this, SLOT(took()));
    connect(ui->pushButton_cancel, SIGNAL(clicked()), this, SLOT(tocancel()));
    connect(ui->pushButton_getpath, SIGNAL(clicked()),this,SLOT(togetlogo()));
    connect(ui->pushButton_bill_export,SIGNAL(clicked()),this,SLOT(tobillexport()));
    connect(ui->pushButton_bill_import,SIGNAL(clicked()),this,SLOT(tobillimport()));
    connect(ui->toolButton_rb_input,SIGNAL(clicked()),this,SLOT(to_rb_input()));

    connect(ui->pushButton_hide_ExSet, SIGNAL(clicked()), this, SLOT(toHideExSet()));
    connect(ui->bp_fmt_serial_no, SIGNAL(timeout()), this, SLOT(toShowExSet()));
    connect(ui->bp_fmt_table, SIGNAL(timeout()), this, SLOT(toShowExSet()));
    connect(ui->bp_fmt_operid, SIGNAL(timeout()), this, SLOT(toShowExSet()));
    //长按可修改
    ui->bp_fmt_serial_no->enableLongPress(true);
    ui->bp_fmt_table->enableLongPress(true);
    ui->bp_fmt_operid->enableLongPress(true);
    //ini
    ui->bp_fmt_serial_no->setText(ui->bp_fmt_serial_no_desc->text());
    ui->bp_fmt_table->setText(ui->bp_fmt_table_fmts->currentText());
    ui->bp_fmt_operid->setText(ui->bp_fmt_operid_fmts->currentText());
    //bp_fmt_serial_no_desc作为备用
    ui->gridLayout->removeWidget(ui->bp_fmt_serial_no_desc);
    ui->gridLayout->removeWidget(ui->bp_fmt_table_fmts);
    ui->gridLayout->removeWidget(ui->bp_fmt_operid_fmts);
    ui->gridLayout->removeWidget(ui->pushButton_hide_ExSet);
    //备用不显示
    lds::hideWidget(ui->bp_fmt_serial_no_desc);
    lds::hideWidget(ui->bp_fmt_table_fmts);
    lds::hideWidget(ui->bp_fmt_operid_fmts);
    lds::hideWidget(ui->pushButton_hide_ExSet);
}

w_sys_manage_outer_bill_preview::w_sys_manage_outer_bill_preview(QString quick_input)
{
    ui = 0;
    tablemodel = new lds_model_sqltablemodel(this);
    tablemodel->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    tablemodel->setTable("cey_sys_parameter");
    tablemodel->setFilter("vch_parameter like '%"+map_filter+"%' ");
}

w_sys_manage_outer_bill_preview::~w_sys_manage_outer_bill_preview()
{
    if(ui)delete ui;
}

void w_sys_manage_outer_bill_preview::if_bill_empty_import_58_def()
{
    QString filepath =  "userdata/settings/pint_format/"+lds::get_soft_curent_language_first()+"_58.txt";
    lds_query query;
    query.execSelect("select vch_parameter FROM cey_sys_parameter where vch_parameter = 'bp_fmt_title_second'; ");
    if(!query.next()) {
        import(filepath);
    }
}

bool w_sys_manage_outer_bill_preview::billexport(QWidget *parent)
{
    QString filepath = getFilePath::getSaveFileName(parent, tr("导出文件"),
                                                    "userdata/settings/pint_format/"
                                                    + n_func::f_get_sysdatetime().toString("yyMMddhhmmss")+"."+"txt",
                                                    QString(), 0, QFileDialog::ShowDirsOnly);
    if(!filepath.isEmpty()){
        QSettings fileconf(filepath, QSettings::IniFormat);
        for(int row = 0, rowcount = tablemodel->rowCount(); row < rowcount; row ++) {
            fileconf.setValue("w_sys_manage_outer_bill_preview/"+tablemodel->model_data(row, "vch_parameter").toString()  ,
                              tablemodel->model_data(row, "vch_value")
                              );
        }
        return true;
    }
    return false;
}

bool w_sys_manage_outer_bill_preview::billimport(QWidget *parent)
{
    //1
    QString filepath = getFilePath::getOpenFileName(parent, tr("导入文件"), "userdata/settings/pint_format");

    if(!filepath.isEmpty()){
        QSettings fileconf(filepath, QSettings::IniFormat);
        fileconf.beginGroup("w_sys_manage_outer_bill_preview");
        QStringList keys = fileconf.allKeys();
        for(int row = 0, rowcount = tablemodel->rowCount(); row < rowcount; row++) {
            QString subkey = tablemodel->model_data(row, "vch_parameter").toString();
            if(keys.contains(subkey)){
                tablemodel->model_data_set(row, "vch_value", fileconf.value(subkey));
            }
        }
        return true;
    }
    return false;
}

bool w_sys_manage_outer_bill_preview::import(const QString &filepath)
{
    lds_query query;
    if(!filepath.isEmpty()){
        QSettings fileconf(filepath, QSettings::IniFormat);
        fileconf.beginGroup("w_sys_manage_outer_bill_preview");
        QStringList keys = fileconf.allKeys();
        foreach(const QString &k, keys) {
            if(!query.execInsertDuplicateUpdate("cey_sys_parameter",
                                                QStringList() << "vch_parameter",
                                                qrtVariantPairList()
                                                << qrtVariantPair("vch_parameter", k)
                                                << qrtVariantPair("vch_value", fileconf.value(k)))) {
                return  false;
            }
        }
    }
    if(filepath.isEmpty()) {
        qDebug("filepath is empty");
    }
    return true;
}

void w_sys_manage_outer_bill_preview::took()
{
    if(saveData()) {
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, tr("保存成功"));
        this->accept();
    } else {
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, tr("操作失败"));
    }
}

void w_sys_manage_outer_bill_preview::tocancel()
{
    if(tablemodel->model_is_dirty()){
        switch(lds_messagebox::question(this, MESSAGE_TITLE_VOID,tr("有数据未保存,确认保存?"), tr("保存"),tr("不保存"),tr("取消"))){
        case 0:
            if(!saveData()){
                return;
            }
            break;
        case 1:
            break;
        case 2:
            return;
        }
    }
    this->reject();
}

void w_sys_manage_outer_bill_preview::toupdateSection(const QModelIndex &index)
{
    ui->stackedWidget->setCurrentIndex(index.row());
}

//
void w_sys_manage_outer_bill_preview::togetlogo()
{
    QString filepath = getFilePath::getOpenFileName(this, tr("导入文件"),ui->bp_fmt_title_logo->text());
    if(filepath.isEmpty())
        return;
    ui->bp_fmt_title_logo->setText(filepath);
    mapper->submit();
}

void w_sys_manage_outer_bill_preview::tobillexport()
{
    if(billexport(this)){
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, tr("导出成功"));
    }
}

void w_sys_manage_outer_bill_preview::tobillimport()
{
    if(billimport(this)){
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, tr("导入成功"));
        mapper->setCurrentIndex(1);
    }
}

void w_sys_manage_outer_bill_preview::to_rb_input()
{
    //获取路径
    QString filepath = getFilePath::getOpenFileName(this, tr("导入文件"));
    if(!filepath.isEmpty()){
        QFile file(filepath);
        if(file.open(QFile::ReadOnly)){
            ui->bp_fmt_end_rc->setText(file.readLine());
            ui->bp_fmt_end_rc->setFocus();
            file.close();
            return;
        }
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, file.errorString());
        return;
    }
}

void w_sys_manage_outer_bill_preview::toHideExSet()
{
    //餐桌
    if(lds::widgetIsShow(ui->pushButton_hide_ExSet)
            && ui->pushButton_hide_ExSet->pos() == ui->bp_fmt_table_len->pos()) {
        lds::hideWidget(ui->bp_fmt_table_fmts);
        lds::hideWidget(ui->pushButton_hide_ExSet);
        ui->bp_fmt_table->setText(ifEmpty(ui->bp_fmt_table_fmts->currentText(), ui->bp_fmt_table_fmts->itemText(0)));
    }
    //流失号
    if(lds::widgetIsShow(ui->pushButton_hide_ExSet)
            && ui->pushButton_hide_ExSet->pos() == ui->bp_fmt_serial_no_len->pos()) {
        lds::hideWidget(ui->bp_fmt_serial_no_desc);
        lds::hideWidget(ui->pushButton_hide_ExSet);
        ui->bp_fmt_serial_no->setText(ifEmpty(ui->bp_fmt_serial_no_desc->text(), tr("流水号")));
    }
    //操作员
    if(lds::widgetIsShow(ui->pushButton_hide_ExSet)
            && ui->pushButton_hide_ExSet->pos() == ui->bp_fmt_operid_len->pos()) {
        lds::hideWidget(ui->bp_fmt_operid_fmts);
        lds::hideWidget(ui->pushButton_hide_ExSet);
        ui->bp_fmt_operid->setText(ifEmpty(ui->bp_fmt_operid_fmts->currentText(), ui->bp_fmt_operid_fmts->itemText(0)));
    }
}

void w_sys_manage_outer_bill_preview::toShowExSet()
{
    //已经有设置中的空间会报存并关闭
    toHideExSet();

    ui->pushButton_hide_ExSet->raise();
    //餐桌
    if(this->sender() == ui->bp_fmt_table) {
        ui->bp_fmt_table_fmts->raise();
        lds::showWidget(ui->bp_fmt_table_fmts);
        lds::showWidget(ui->pushButton_hide_ExSet);
        ui->bp_fmt_table_fmts->setGeometry(ui->bp_fmt_table->geometry());
        ui->pushButton_hide_ExSet->setGeometry(ui->bp_fmt_table_len->geometry());
    }
    //流失号
    if(this->sender() == ui->bp_fmt_serial_no) {
        ui->bp_fmt_serial_no_desc->raise();
        lds::showWidget(ui->bp_fmt_serial_no_desc);
        lds::showWidget(ui->pushButton_hide_ExSet);
        ui->bp_fmt_serial_no_desc->setGeometry(ui->bp_fmt_serial_no->geometry());
        ui->pushButton_hide_ExSet->setGeometry(ui->bp_fmt_serial_no_len->geometry());
    }
    //操作员
    if(this->sender() == ui->bp_fmt_operid) {
        ui->bp_fmt_operid_fmts->raise();
        lds::showWidget(ui->bp_fmt_operid_fmts);
        lds::showWidget(ui->pushButton_hide_ExSet);
        ui->bp_fmt_operid_fmts->setGeometry(ui->bp_fmt_operid->geometry());
        ui->pushButton_hide_ExSet->setGeometry(ui->bp_fmt_operid_len->geometry());
    }
}

bool w_sys_manage_outer_bill_preview::insert_sql_objectname(const QString &objectname)
{
    lds_query query;
    query.execSelect(QString("SELECT vch_parameter from cey_sys_parameter where vch_parameter = '%1' ").arg(objectname));
    if(!query.next()) {
        QString vch_value;
        if(objectname == ui->bp_fmt_serial_no_desc->objectName()) {
            vch_value = ui->bp_fmt_serial_no->text();//默认
        }
        if(objectname == ui->bp_fmt_operid_fmts->objectName()) {
            vch_value = ui->bp_fmt_operid_fmts->itemData(0).toString();//默认
        }
        if(objectname == ui->bp_fmt_table_fmts->objectName()) {
            vch_value = ui->bp_fmt_table_fmts->itemData(0).toString();//默认
        }
        query.execInsert("cey_sys_parameter",qrtVariantPairList()
                         << qrtVariantPair("vch_parameter", objectname)
                         << qrtVariantPair("vch_value", vch_value));
    }
    return true;
}

bool w_sys_manage_outer_bill_preview::saveData()
{
    lds_query::tran_saction();
    if(mapper_save_data()) {
        lds_query::com_mit();
        return true;
    }
    lds_query::roll_back();
    return false;
}
