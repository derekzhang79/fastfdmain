#include "w_scr_dish_language_switch_set.h"
#include "ui_w_scr_dish_language_switch_set.h"
#include "ftableview_standmodel_sql_none.h"
#include "ftableview_delegate.h"
#include "w_sys_manage_outer_pay_set.h"
#include "lds_messagebox.h"
#include "lds.h"

w_scr_dish_language_switch_set::w_scr_dish_language_switch_set(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_scr_dish_language_switch_set)
{
    ui->setupUi(this);

    QList<ftableview_standmodel_sql_none::noneData> list_data;

    QList<QPair<QString, QString> >& list = w_sys_manage_outer_pay_set::getLanguagelist();
    for(int k = 0; k < list.count();k ++) {
        if(list[k].second.isEmpty()) {
            continue;
        }
        list_data << ftableview_standmodel_sql_none::noneData(
                         list[k].first,
                         list[k].second
                         );
    }

    ftableview_standmodel_sql_none *model = new ftableview_standmodel_sql_none(this);
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



    model->toinit();

    ui->pushButton_language->enabledSwitchKey(false);
    ui->pushButton_language->initKeys_conf();
    connect(ui->pushButton_cancel, SIGNAL(clicked()),this,SLOT(tocancel()));
    connect(ui->pushButton_ok, SIGNAL(clicked()),this,SLOT(took()));
    connect(ui->pushButton_language_clear, SIGNAL(clicked()),this,SLOT(to_language_clear()));
    connect(ui->tableView, SIGNAL(clicked(QModelIndex)), this, SLOT(to_language_set(QModelIndex)));
}

w_scr_dish_language_switch_set::~w_scr_dish_language_switch_set()
{
    delete ui;
}

void w_scr_dish_language_switch_set::took()
{
    lds::sysconf->setValue("system_setting/SYS_LANGUAGE", ui->pushButton_language->firstKey());
    lds::sysconf->setValue("system_setting/SYS_LANGUAGE_SECOND", ui->pushButton_language->secondKey());
    this->accept();
}

void w_scr_dish_language_switch_set::tocancel()
{
    this->reject();
}

void w_scr_dish_language_switch_set::to_language_clear()
{
    ui->pushButton_language->initKeys("", "");
    ui->pushButton_language->update();
}

void w_scr_dish_language_switch_set::to_language_set(const QModelIndex &index)
{
    QString k = index.data(Qt::UserRole + 2).toString();
    if(k == "-1") return;
    if(k == "-2") return;

    if(ui->pushButton_language->firstKey().isEmpty()) {
        if(ui->pushButton_language->secondKey() == k) return;

        ui->pushButton_language->initKeys(k, ui->pushButton_language->secondKey());
        ui->pushButton_language->update();
        return;
    }
    if(ui->pushButton_language->secondKey().isEmpty()) {
        if(ui->pushButton_language->firstKey() == k) return;

        ui->pushButton_language->initKeys(ui->pushButton_language->firstKey(), k);
        ui->pushButton_language->update();
        return;
    }

    ui->pushButton_language->initKeys(k, "");
    ui->pushButton_language->update();
    return;
}
