#include "w_scr_dish_weixinbill_import.h"
#include "ui_w_scr_dish_weixinwaimai_dialog.h"
#include "lds_messagebox.h"
#include "lds_dialog_input.h"
#include "backheader.h"
#include <QSqlRecord>
#include "w_scr_dish_weixinwaimai.h"
#include "lds_model_sqltablemodel.h"
#include "n_func.h"

w_scr_dish_weixinbill_import::w_scr_dish_weixinbill_import(QWidget *parent)
    :QDialog(parent),
      ui(new Ui_w_scr_dish_weixinwaimai_dialog)
{
    ui->setupUi(this);
    //init
    tablemodel=new lds_model_sqltablemodel(this);
    tablemodel->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    ui->tableView->setModel(tablemodel);
    tablemodel_2=new lds_model_sqltablemodel(this);
    tablemodel_2->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    ui->tableView_2->setModel(tablemodel_2);

    tablemodel->setTable("cey_web_order_info");
    tablemodel_2->setTable("cey_web_order_dish");

    tablemodel->save_set_header("sn",tr("订单编号"));
    tablemodel->save_set_header("tel",tr("手机号码"));
    ui->tableView->rePecifyHeader(tablemodel->get_save_set_header_saver_column_list());
    tablemodel_2->save_set_header("dishNo",tr("菜品编号"));
    tablemodel_2->save_set_header("dishName",tr("菜品名称"));
    tablemodel_2->save_set_header("number",tr("菜品数量"));
    tablemodel_2->save_set_header("price",tr("价格"));
    tablemodel_2->save_set_header("suitFlag",tr("套菜标志"));
    tablemodel_2->save_set_header("note",tr("备注"));
    ui->tableView_2->rePecifyHeader(tablemodel_2->get_save_set_header_saver_column_list());

    //    connect(ui->tableView,SIGNAL(selectChanged(int)),this,SLOT(torefresh()));
    //!after
    ui->comboBox->addItems1(QList<lds_ComboBox::PAIRLIST>()
                           << lds_ComboBox::PAIRLIST("0", tr("未受理"))
                           );
    ui->dateTimeEdit->setDateTime(QDateTime(n_func::f_get_sysdatetime().date(),QTime(0,0,0)));
    ui->dateTimeEdit_2->setDateTime(QDateTime(n_func::f_get_sysdatetime().date(),QTime(23,59,59)));
    ui->tableView->setEditTriggers(QTableView::NoEditTriggers);
    ui->tableView_2->setEditTriggers(QTableView::NoEditTriggers);
    ui->widget->setup(QStringList() <<tr("打印") << tr("下单") << tr("取消"), Qt::AlignRight);

    connect(ui->widget->index_widget(tr("打印")),SIGNAL(clicked()),this,SLOT(toprint()));
    connect(ui->widget->index_widget(tr("下单")),SIGNAL(clicked()),this,SLOT(totake_waimai()));
    connect(ui->widget->index_widget(tr("取消")),SIGNAL(clicked()),this,SLOT(toexit()));
    connect(ui->pushButton_search,SIGNAL(clicked()),this,SLOT(tosearch()));
    connect(ui->tableView,SIGNAL(clicked(QModelIndex)),this,SLOT(torefresh()));
    tosearch(0);
}

void w_scr_dish_weixinbill_import::totake_waimai()
{
    //获取订单信息
    if(tablemodel_2->rowCount() <=0){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("没有菜品信息"));
        return;
    }
    //    01->11
    if(w_sys_manage_cloudsync::up_weixin_change_orderstate(this
                                                           ,tablemodel->model_data(ui->tableView->currentIndex().row(), "sn").toString()
                                                           ,"11"//交易结束-交易拒绝
                                                           )){
        //数据保存
        wdata.address = ui->lineEdit_address->text();
        wdata.eatPels = ui->lineEdit_eatPels->text();
        wdata.eatTableNo = ui->lineEdit_eatTableNo->text();
        wdata.eatTime = ui->lineEdit_eatTime->text();

        wdata.orderer = ui->lineEdit_orderer->text();
        wdata.orderState = ui->lineEdit_orderState->text();
        wdata.orderType = ui->lineEdit_orderType->text();
        wdata.payState = ui->lineEdit_payState->text();

        wdata.payType = ui->lineEdit_payType->text();
        wdata.sn = tablemodel->model_data(ui->tableView->currentIndex().row(), "sn").toString();
        wdata.tableAreaNo = tablemodel->model_data(ui->tableView->currentIndex().row(), "tableAreaNo").toString();
        wdata.tel = tablemodel->model_data(ui->tableView->currentIndex().row(), "tel").toString();
        lds_messagebox::information(this, MESSAGE_TITLE_VOID,tr("订单已受理"));
        this->accept();
    }
}

void w_scr_dish_weixinbill_import::toexit()
{
    this->reject();
}

void w_scr_dish_weixinbill_import::tosearch(int row)
{
    tablemodel->setFilter(
                "eatTime<='"+ui->dateTimeEdit_2->dateTime().toString("yyyy-MM-dd hh:mm:ss")+
                "'  and eatTime>='"+ui->dateTimeEdit->dateTime().toString("yyyy-MM-dd hh:mm:ss")+
                "'  and orderState like '"+ui->comboBox->comusrdata().toString()+"%' and "+
                "   orderType='0' ");
    if(row==-1) {
        ui->tableView->restore();
    } else{
        ui->tableView->selectRow(row);
        ui->tableView->resizeColumnsToContents();
    }
    torefresh();
}

void w_scr_dish_weixinbill_import::torefresh()
{
    tablemodel_2->setFilter("orderSn='"+tablemodel->model_data(ui->tableView->currentIndex().row(), "sn").toString()+"' ");
    ui->tableView_2->restore();

    ui->lineEdit_orderer->setText(tablemodel->model_data(ui->tableView->currentIndex().row(), "orderer").toString());
    ui->lineEdit_eatTime->setText(tablemodel->model_data(ui->tableView->currentIndex().row(), "eatTime").toDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    ui->lineEdit_tableAreaNo->setText(tablemodel->model_data(ui->tableView->currentIndex().row(), "tableAreaNo").toString());
    ui->lineEdit_eatTableNo->setText(tablemodel->model_data(ui->tableView->currentIndex().row(), "eatTableNo").toString());
    ui->lineEdit_eatPels->setText(tablemodel->model_data(ui->tableView->currentIndex().row(), "eatPels").toString());

    ui->lineEdit_payType->setText(tablemodel->model_data(ui->tableView->currentIndex().row(), "payType").toString()=="0"?tr("当面支付"):tr("在线支付"));
    ui->lineEdit_payState->setText(tablemodel->model_data(ui->tableView->currentIndex().row(), "payState").toString()=="0"?tr("未支付"):tr("已支付"));
    ui->lineEdit_orderType->setText(tablemodel->model_data(ui->tableView->currentIndex().row(), "orderType").toString());
    ui->lineEdit_orderType->setText(w_scr_dish_weixinwaimai::getweixinorderTypename(ui->lineEdit_payType->text()));
    ui->lineEdit_orderState->setText(tablemodel->model_data(ui->tableView->currentIndex().row(), "orderState").toString());

    ui->lineEdit_orderState->setText(
                w_scr_dish_weixinwaimai::getweixinorderStatename(ui->lineEdit_orderState->text()));

    ui->lineEdit_address->setText(tablemodel->model_data(ui->tableView->currentIndex().row(), "address").toString());

}

void w_scr_dish_weixinbill_import::toprint()
{
    int row = ui->tableView->currentIndex().row();
    if(row >= 0){
        QString sn = tablemodel->model_data(row, "sn").toString();
        w_scr_dish::static_weixinPrintbillBySn(this->windowTitle(), sn);
    }
}

