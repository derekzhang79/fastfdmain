#include "w_scr_dish_pay_select_language.h"
#include "ui_w_scr_dish_pay_select_language.h"
#include "w_sys_manage_outer_pay_set.h"
#include "ftableview_standmodel_sql_none.h"
#include "ftableview_delegate.h"
#include "lds.h"
#include "lds_messagebox.h"

w_scr_dish_pay_select_language::w_scr_dish_pay_select_language(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_scr_dish_pay_select_language)
{
    ui->setupUi(this);

    QList<ftableview_standmodel_sql_none::noneData> list_data;
    QList<QPair<QString, QString> > & language_list = w_sys_manage_outer_pay_set::getLanguagelist();
    for(int k = 0, count = language_list.count(); k < count; k++) {
        list_data << ftableview_standmodel_sql_none::noneData(
                         language_list[k].first,
                         language_list[k].second);
    }

    model = new ftableview_standmodel_sql_none(this);
    model->add_list_data(list_data);
    ftableview_delegate *d = new ftableview_delegate(ui->tableView);
    d->keepConnect(ui->tableView);
    d->setMargin(1);
    d->indexisEmptystillDraw = true;

    ui->tableView->setModel(model);
    ui->tableView->setProperty("outer_stylesheet", "tableView_type");
    ui->tableView->setItemDelegate(d);
    ui->tableView->updateFixed_fillData_addAnimation(3, 3, width()/3, 60);
    ui->tableView->setSelectionModel(model->selectionmodel = new QItemSelectionModel(model));

    model->selectCurUData2(lds::sysconf->value("system_setting/maidan_language").toString());
    ui->checkBox_pay_select_language->setChecked(lds::sysconf->value("system_setting/pay_select_language", false).toBool());

    connect(ui->pushButton_cancel, SIGNAL(clicked()),this,SLOT(tocancel()));
    connect(ui->pushButton_ok, SIGNAL(clicked()),this,SLOT(took()));
    connect(ui->checkBox_pay_select_language, SIGNAL(clicked(bool)), this,SLOT(to_save_pay_select_language(bool)));
}

w_scr_dish_pay_select_language::~w_scr_dish_pay_select_language()
{
    delete ui;
}

void w_scr_dish_pay_select_language::took()
{
    QVariantList key_list = model->get_value_select_list(2);//key

    if(key_list.isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("没有数据被选中"));
        return;
    }
    QString key = key_list[0].toString();
    lds::sysconf->setValue("system_setting/maidan_language", key);
    this->accept();
}

void w_scr_dish_pay_select_language::tocancel()
{
    this->reject();
}

void w_scr_dish_pay_select_language::to_save_pay_select_language(bool f)
{
    lds::sysconf->setValue("system_setting/pay_select_language", f);
}
