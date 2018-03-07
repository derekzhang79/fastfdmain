#include "w_scr_dish_cook.h"
#include "ui_w_scr_dish_cook.h"
#include "ftableview_standmodel_sql_none.h"
#include "ftableview_delegate.h"
#include "lds_query.h"

#include "lds_dialog_input.h"
#include "fexpandmain.h"
#include <QtDebug>
#include "n_func.h"


w_scr_dish_cook::w_scr_dish_cook(QWidget *parent) :
    QDialog(parent)
{
    intData();
}

w_scr_dish_cook::w_scr_dish_cook(const QString &dishno, QWidget *parent) :
    QDialog(parent)
{
    intData();

    refresh(dishno);
}

w_scr_dish_cook::~w_scr_dish_cook()
{
    delete ui;
}

void w_scr_dish_cook::intData()
{
    ui = new Ui::w_scr_dish_cook;
    ui->setupUi(this);

    model = new ftableview_standmodel_sql_none(this);
    ftableview_delegate *d = new ftableview_delegate(ui->tableView);
    d->keepConnect(ui->tableView);
    d->setMargin(1);
    d->indexisEmptystillDraw = true;

    ui->tableView->setModel(model);
    ui->tableView->setProperty("outer_stylesheet", "tableView_type");
    ui->tableView->setItemDelegate(d);
    ui->tableView->setSelectionModel(new QItemSelectionModel(model));
    model->selectionmodel = ui->tableView->selectionModel();
    ui->tableView->updateFixed_fillData_addAnimation(4, 4, 100, 60);
    ui->tableView->setSelectionMode(QTableView::MultiSelection);
    ui->tableView->enable_mouse_press_after_select();

    connect(ui->pushButton_cancel, SIGNAL(clicked()),this,SLOT(tocancel()));
    connect(ui->pushButton_ok, SIGNAL(clicked()),this,SLOT(took()));
    connect(ui->tableView, SIGNAL(signal_enable_mouse_press_after_select(QModelIndex)), SLOT(tochangenum(QModelIndex)));
}

void w_scr_dish_cook::refresh(const QString &ch_dishno)
{
    vch_print_memo = "";
    QList<ftableview_standmodel_sql_none::noneData> list_data;
    lds_query query;
    query.execSelect(QString("select a.ch_no,a.vch_cp_memo,a.ch_typeno,a.num_add_price FROM cey_bt_cp_memo a,cey_bt_cp_dish b WHERE (a.ch_no = b.ch_no) and ( b.ch_dishno = '%1')")
                     .arg(ch_dishno));
    while(query.next()){
        list_data << ftableview_standmodel_sql_none::noneData(
                         QString("%1\n%2")
                         .arg(query.recordValue("vch_cp_memo").toString())
                         .arg(query.recordValue("num_add_price").toString()),
                         query.recordValue("ch_no").toString());
    }

    model->add_list_data(list_data);

    model->toinit();

    query.execSelect(QString(" select vch_dishname from cey_bt_dish where ch_dishno = '%1' ")
                     .arg(ch_dishno));
    query.next();
    this->setWindowTitle(query.recordValue("vch_dishname").toString() + "  " + tr("做法"));
}

void w_scr_dish_cook::took()
{
    QVariantList index2list = model->get_value_select_list(2);//做法编码
    QVariantList index3list = model->get_value_select_list(3);//做法数量

    for(int idx = 0; idx < index2list.count(); idx ++) {
        QString ch_no = index2list.value(idx).toString();
        int cook_num = index3list.value(idx).toInt();
        if(cook_num == 0) cook_num =1;
        for(int k = 0; k < cook_num; k++) {
            vch_print_memo += ch_no;
        }
    }

    this->accept();
}

void w_scr_dish_cook::tocancel()
{
    this->reject();
}

void w_scr_dish_cook::tochangenum(const QModelIndex &index)
{
    QString key = model->data(index, Qt::UserRole + 1).toString();
    if(key == "-1"
            || key == "-2"
            || key.isEmpty()
            ) {
        return;
    }

    int cook_num = model->data(index, Qt::UserRole + 3/*组数量*/).toInt();
    if(0 == cook_num) cook_num = 1;

    lds_dialog_input inputdialog(this);
    inputdialog.setDotEnabled(false);
    inputdialog.ui->label->setText(tr("数量"));
    inputdialog.setWindowTitle(tr("修改数量"));
    inputdialog.ui->lineEdit->setText(QString::number(cook_num));
    if(QDialog::Accepted == lds_roundeddialog_rect_align(&inputdialog).exec()){
        float num = inputdialog.ui->lineEdit->text().toInt();
        model->setData(index, num, Qt::UserRole+3/*组数量*/);
        if(num == 0) {
            ui->tableView->setCurrentIndex(index);
        }
        return;
    }
}
