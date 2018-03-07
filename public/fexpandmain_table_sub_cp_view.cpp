#include "fexpandmain_table_sub_cp_view.h"
#include "ui_fexpandmain_table_sub_cp_view.h"
#include "ftableview_standmodel_sql_none.h"
#include "ftableview_delegate.h"

fexpandmain_table_sub_cp_view::fexpandmain_table_sub_cp_view(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::fexpandmain_table_sub_cp_view)
{
    ui->setupUi(this);

    ftableview_delegate *d = new ftableview_delegate(ui->tableView);
    d->keepConnect(ui->tableView);
    d->setMargin(1);
    d->indexisEmptystillDraw = true;


    ftableview_standmodel_sql_none *standmodel = new ftableview_standmodel_sql_none(this);
    ui->tableView->setModel(standmodel);
    ui->tableView->setProperty("outer_stylesheet", "tableView_type");
    ui->tableView->setItemDelegate(d);
    ui->tableView->setSelectionModel(new QItemSelectionModel(standmodel));


    QList<ftableview_standmodel_sql_none::noneData> list_data;
    list_data << ftableview_standmodel_sql_none::noneData("sugar", "1");
    list_data << ftableview_standmodel_sql_none::noneData("sugar", "2");
    list_data << ftableview_standmodel_sql_none::noneData("sugar", "3");
    list_data << ftableview_standmodel_sql_none::noneData("sugar", "4");
    list_data << ftableview_standmodel_sql_none::noneData("sugar", "5");
    list_data << ftableview_standmodel_sql_none::noneData("sugar", "6");
    list_data << ftableview_standmodel_sql_none::noneData("sugar", "7");
    list_data << ftableview_standmodel_sql_none::noneData("sugar", "8");
    list_data << ftableview_standmodel_sql_none::noneData("sugar", "9");
    list_data << ftableview_standmodel_sql_none::noneData("sugar", "10");

    ui->tableView->updateFixed_fillData_addAnimation(3, 3, 80, 50);
    standmodel->add_list_data(list_data);
    standmodel->toinit();

    ui->stackedWidget->setCurrentIndex(0);
}

fexpandmain_table_sub_cp_view::~fexpandmain_table_sub_cp_view()
{
    delete ui;
}
