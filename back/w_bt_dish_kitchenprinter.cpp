#include "w_bt_dish_kitchenprinter.h"
#include "ui_w_bt_dish_dialog.h"
#include "backheader.h"
#include "w_bt_dish.h"
#include "lds_tableview_delegate_check.h"
#include "lds_query.h"

#include "lds_messagebox.h"
#include <QSqlError>
#include "lds_combobox.h"
#include "n_func.h"
#include "public_logindialog.h"
#include "public_logindialog.h"
#include "public_debugdialog.h"
#include "printer_pos_net.h"
#include "lds_query.h"

#include "printer_0.h"
#include "printer_pos.h"
#include "printer_pos_net.h"
#include "printer_pos_vidpid.h"
#include <QHeaderView>
#include "w_bt_dish_kitchen_dish.h"

//===================
#include "ui_w_bt_dish_unitset_dialog.h"
#include "lds_tableview_delegate_check.h"
#include "backheader.h"
#include "lds_pushbuttonlock.h"
#include "lds_query.h"
#include "w_bt_dish_kitchenprinter_printerset.h"
#include "lds_model_sqltablemodel_delegate_com.h"
#include "printer_normal.h"

w_bt_dish_Kitchenprinter_printermanager::w_bt_dish_Kitchenprinter_printermanager(QWidget *parent) :
    QDialog(parent),
    ui(new Ui_w_bt_dish_unitset_Dialog)
{
    ui->setupUi(this);
    dchange = false;
    //! tableview
    tablemodel=new lds_model_sqltablemodel(this);
//    ui->tableView->setSortingEnabled(true);
    //    connect(ui->tableView->horizontalHeader(),SIGNAL(sortIndicatorChanged(int,Qt::SortOrder)),tablemodel,SLOT(toSort(int,Qt::SortOrder)));

    tablemodel->setTable("cey_bt_kitchen_print");
    tablemodel->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    tablemodel->save_set_header("int_id", tr("编码"));
    tablemodel->save_set_header("vch_printname", tr("打印机名称"));
    tablemodel->save_set_header("vch_printtype", tr("打印机型号"));
    tablemodel->save_set_header("vch_printip", tr("打印机端口"));
    tablemodel->save_set_header("ch_areaNo", tr("区域"));
    ui->tableView->setModel(tablemodel);
//    tablemodel->setSort(ui->tableView->horizontalHeader()->sortIndicatorSection(), ui->tableView->horizontalHeader()->sortIndicatorOrder());

    ui->tableView->rePecifyHeader(tablemodel->get_save_set_header_saver_column_list());
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("vch_printtype"), new lds_model_sqltablemodel_delegate_com(this,  &Printer_Normal::get_normal_print_map_ref()));
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("ch_areaNo"), new lds_model_sqltablemodel_delegate_com(this, "cey_bt_table_area", "ch_areaNo","vch_areaname"));


    //! toolbar
    ui->widget->setup(QStringList() << tr("厨显测试")<< tr("打印测试") <<tr("新增") <<tr("修改")<< tr("删除") << tr("退出"), Qt::AlignRight, lds_keytoolbutton::NUMKEY);
    connect(ui->widget->index_widget(tr("新增")), SIGNAL(clicked()),this,SLOT(tonew()));
    connect(ui->widget->index_widget(tr("修改")), SIGNAL(clicked()),this,SLOT(tochange()));
    connect(ui->widget->index_widget(tr("删除")), SIGNAL(clicked()),this,SLOT(todel()));
    connect(ui->widget->index_widget(tr("退出")), SIGNAL(clicked()),this,SLOT(toexit()));
    connect(ui->widget->index_widget(tr("打印测试")), SIGNAL(clicked()),this,SLOT(toprinttest()));
    connect(ui->widget->index_widget(tr("厨显测试")), SIGNAL(clicked()),this,SLOT(tokitchenviewtest()));
    connect(ui->tableView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(tochange()));
    connect(tablemodel,SIGNAL(dataChanged(QModelIndex,QModelIndex)),this,SLOT(datachanged(QModelIndex,QModelIndex)));
    ui->widget->index_widget(tr("厨显测试"))->hide();
    QTimer::singleShot(0, this, SLOT(torefresh()));
}

void w_bt_dish_Kitchenprinter_printermanager::tonew()
{
    int max_int_id_1 = tablemodel->max_column_add_1_dec("int_id");
    w_bt_dish_Kitchenprinter_printerSet dialog(this);
    dialog.setWindowTitle(tr("新增打印机"));
    dialog.initData(QString::number(max_int_id_1), "P"+QString::number(max_int_id_1));
    if(QDialog::Accepted == lds_roundeddialog_rect_align(&dialog).exec()){
        int rowcount=tablemodel->rowCount();
        tablemodel->insertRow(rowcount);
        tablemodel->model_data_set(rowcount, "int_id", dialog.getno());
        tablemodel->model_data_set(rowcount, "vch_printname", dialog.getname());
        tablemodel->model_data_set(rowcount, "vch_printtype", dialog.getprinttype());
        tablemodel->model_data_set(rowcount, "vch_instype", dialog.getinstype());
        tablemodel->model_data_set(rowcount, "vch_printip", dialog.getport());
        tablemodel->model_data_set(rowcount, "int_paperwidth", dialog.getpaperwidth());
        tablemodel->model_data_set(rowcount, "ch_areano", dialog.get_ch_areano());
        tablemodel->model_data_set(rowcount, "int_xinye_beep_count", dialog.get_xinye_beep());
        tablemodel->trySubmitAll();
        ui->tableView->resizeColumnsToContents();
    }
}

void w_bt_dish_Kitchenprinter_printermanager::todel()
{
    QString ipport;
    int row = ui->tableView->currentIndex().row();
    if(row >= 0){
        ipport = tablemodel->model_data(row,"vch_printip").toString().trimmed();
        tablemodel->removeRow(ui->tableView->currentIndex().row());
        tablemodel->trySubmitAll();
        ui->tableView->resizeColumnsToContents();
    }
}

void w_bt_dish_Kitchenprinter_printermanager::tochange()
{
    int row =ui->tableView->currentIndex().row();
    if(row < 0){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("没有行被选中"));
        return;
    }
    w_bt_dish_Kitchenprinter_printerSet dialog(this);
    dialog.setWindowTitle(tr("修改打印机"));
    dialog.initData(tablemodel->model_data(row,"int_id").toString(),
                    tablemodel->model_data(row,"vch_printname").toString(),
                    tablemodel->model_data(row,"vch_printtype").toString(),
                    tablemodel->model_data(row,"vch_instype").toInt(),
                    tablemodel->model_data(row,"vch_printip").toString(),
                    tablemodel->model_data(row,"int_paperwidth").toInt(),
                    tablemodel->model_data(row,"ch_areano").toString(),
                    tablemodel->model_data(row,"int_xinye_beep_count").toInt()
                    );
    if(QDialog::Accepted == lds_roundeddialog_rect_align(&dialog).exec()){
        tablemodel->model_data_set(row, "int_id", dialog.getno());
        tablemodel->model_data_set(row, "vch_printname", dialog.getname());
        tablemodel->model_data_set(row, "vch_printtype", dialog.getprinttype());
        tablemodel->model_data_set(row, "vch_instype", dialog.getinstype());
        tablemodel->model_data_set(row, "vch_printip", dialog.getport());
        tablemodel->model_data_set(row, "int_paperwidth", dialog.getpaperwidth());
        tablemodel->model_data_set(row, "ch_areano", dialog.get_ch_areano());
        tablemodel->model_data_set(row, "int_xinye_beep_count", dialog.get_xinye_beep());
        tablemodel->trySubmitAll();
        ui->tableView->restore();
    }
}

void w_bt_dish_Kitchenprinter_printermanager::torefresh()
{
    tablemodel->select();
    ui->tableView->resizeColumnsToContents();
}

void w_bt_dish_Kitchenprinter_printermanager::toexit()
{
    if(dchange){
        lds_messagebox::information(this, MESSAGE_TITLE_VOID,tr("有数据修改,请检测:厨打打印机、出品打印机 设置情况是否正确"));
    }
    this->reject();
}

void w_bt_dish_Kitchenprinter_printermanager::toprinttest()
{
    int row = ui->tableView->currentIndex().row();
    if(row < 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("没有行被选中"));
        return;
    }
    QString path=tablemodel->model_data(row,"vch_printip").toString();
    if(path.isEmpty()){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("打印路径为空"));
        return;
    }
    lds_pushbuttonlock b(qobject_cast<QWidget *>(this->sender()));
    b.toLock();

    backheader::syn_dataprin_test_byresolveself(
                tablemodel->model_data(row, "vch_printip").toString(),
                tablemodel->model_data(row, "vch_instype").toInt(),
                tablemodel->model_data(row, "vch_printtype").toString(),
                tablemodel->model_data(row, "int_paperwidth").toInt(),
                tablemodel->model_data(row, "int_xinye_beep_count").toInt()
                );
}

void w_bt_dish_Kitchenprinter_printermanager::tokitchenviewtest()
{
    int row = ui->tableView->currentIndex().row();
    if(row < 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("没有行被选中"));
        return;
    }
    QString path=tablemodel->model_data(row,"vch_printip").toString();
    if(path.isEmpty()){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("打印路径为空"));
        return;
    }
    lds_pushbuttonlock b(qobject_cast<QWidget *>(this->sender()));
    b.toLock();
    QString ls_err;
    QString senddata = QString("1206,包子,,Q000,%1,2\n"
                               "1207,豆浆,加糖+加糖,1002,%1,1\n"
                               )
            .arg(n_func::f_get_sysdatetime().toString("yyyy-MM-dd hh:mm:ss"));
    QStringList ipport = path.split(":");
    QTcpSocket tcp;
    tcp.connectToHost(ipport.value(0), ipport.value(1).toInt());
    ls_err = tr("服务器连接失败");
    if(tcp.waitForConnected(5*1000)){
        tcp.write(senddata.toLocal8Bit());
        ls_err = tr("等待回读超时");
        if(tcp.waitForReadyRead(1000)){
            ls_err = tr("测试失败");
            if(tcp.readAll().startsWith(0x12)){
                lds_messagebox::information(this, MESSAGE_TITLE_VOID, tr("厨显测试成功"));
                goto end;
            }
        }
    }
    lds_messagebox::information(this, MESSAGE_TITLE_VOID,  tr("厨显测试") + "\n"+  ls_err);
end:
    return;
}

void w_bt_dish_Kitchenprinter_printermanager::datachanged(const QModelIndex &/*topLeft*/, const QModelIndex &/*bottomRight*/)
{
    if(dchange) return;
    dchange = true;
}
