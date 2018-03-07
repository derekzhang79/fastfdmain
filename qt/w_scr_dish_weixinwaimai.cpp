#include "w_scr_dish_weixinwaimai.h"
#include "ui_w_scr_dish_weixinwaimai_dialog.h"
#include <QSqlRecord>
#include "takeout_cook.h"
#include "backheader.h"
#include "lds_messagebox.h"
#include "lds_dialog_input.h"
#include "n_func.h"
#include "lds_dialog_input_expand.h"
#include "w_scr_dish_restaurant_div_pop.h"
#include "w_bt_dish.h"

w_scr_dish_weixinwaimai::w_scr_dish_weixinwaimai(QWidget *parent):
    QDialog(parent),
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

    //!after
    ui->comboBox->addItems1(QList<lds_ComboBox::PAIRLIST>()
                           << lds_ComboBox::PAIRLIST("0", tr("未受理"))
                           << lds_ComboBox::PAIRLIST("1", tr("已受理"))
                           << lds_ComboBox::PAIRLIST("2", tr("交易结束"))

                           );
    ui->dateTimeEdit->setDateTime(QDateTime(n_func::f_get_sysdatetime().date(),QTime(0,0,0)));
    ui->dateTimeEdit_2->setDateTime(QDateTime(n_func::f_get_sysdatetime().date(),QTime(23,59,59)));
    ui->tableView->setEditTriggers(QTableView::NoEditTriggers);
    ui->tableView_2->setEditTriggers(QTableView::NoEditTriggers);
    ui->widget->setup(QStringList() << tr("打印") << tr("拒绝")<<tr("下单") << tr("取消"), Qt::AlignRight);

    connect(ui->widget->index_widget(tr("打印")),SIGNAL(clicked()),this,SLOT(toprintbill()));
    connect(ui->widget->index_widget(tr("拒绝")),SIGNAL(clicked()),this,SLOT(torefuse()));
    connect(ui->widget->index_widget(tr("下单")),SIGNAL(clicked()),this,SLOT(totake_waimai()));
    connect(ui->widget->index_widget(tr("取消")),SIGNAL(clicked()),this,SLOT(toexit()));
    connect(ui->pushButton_search,SIGNAL(clicked()),this,SLOT(tosearch()));
    connect(ui->tableView,SIGNAL(clicked(QModelIndex)),this,SLOT(torefresh()));
    tosearch(0);
}

QString w_scr_dish_weixinwaimai::getweixinorderStatename(const QString &orderState)
{

    switch(orderState.toInt()){
    case 1:return tr("未受理-订单");
    case 2:return tr("未受理-退单");
    case 11:return tr("已受理-订单");
    case 12:return tr("已受理-已制作派送");
    case 13:return tr("已受理-退订失败");
    case 21:return tr("交易结束-交易成功");
    case 22:return tr("交易结束-交易拒绝");
    case 24:return tr("交易结束-退订成功");
    case 25:return tr("交易结束-交易响应超时");
    }
    return "";
}

QString w_scr_dish_weixinwaimai::getweixinorderTypename(const QString &orderType)
{
    switch(orderType.toInt()){
    case 0: return tr("普通订单");
    case 1: return tr("外卖订单");
    case 2: return tr("订桌订单");
    }
    return "";
}

void w_scr_dish_weixinwaimai::torefuse()
{
    if(ui->tableView->currentIndex().row()<0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("没有选中行"));
        return;
    }

    lds_dialog_input_expand dialog(this);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    if(QDialog::Accepted==lds_roundeddialog_rect_align(&dialog).exec()){
        if(dialog.retreason.isEmpty()){
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("拒绝理由不能为空"));
            return;
        }
        if(1==lds_messagebox::question(this, MESSAGE_TITLE_VOID,tr("确认拒绝?"), tr("确认"), tr("'取消"))){
            return;
        }
        //    01->22
        if(w_sys_manage_cloudsync::up_weixin_change_takkeoutstate(this
                                                                  ,tablemodel->model_data(ui->tableView->currentIndex().row(), "sn").toString()
                                                                  ,"22"//交易结束-交易拒绝
                                                                  ,dialog.retreason
                                                                  )){
            tablemodel->removeRow(ui->tableView->currentIndex().row());
            tablemodel->trySubmitAll();
            lds_messagebox::information(this, MESSAGE_TITLE_VOID,tr("外卖拒绝成功"));
        }
    }
}

void w_scr_dish_weixinwaimai::totake_waimai()
{
    //获取订单信息
    if(tablemodel_2->rowCount() <=0){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("没有菜品信息"));
        return;
    }
    if(1==lds_messagebox::question(this, MESSAGE_TITLE_VOID,tr("确认下单?"), tr("确认"), tr("'取消"))){
        return;
    }

    QString sn = tablemodel->model_data(ui->tableView->currentIndex().row(), "sn").toString();

    //
    QDateTime dt = n_func::f_get_sysdatetime();
    lds_query query;
    QString vch_plan_k3o3_state;
    QString vch_plan_k3o3_id;
    QString ch_dishno;
    QString ch_billno = w_scr_dish_restaurant_div_pop::getMaxChbillnoAdd1_cey_u_master("Q");
    fexpandmain_model_sqltablemodel_data tablemodel_dd;
    tablemodel_dd.select(ch_billno, this);
    query.execSelect(QString("select a.sn, a.orderSn, a.dishNo, a.dishName, a.number, a.price, a.suitFlag, a.note, a.date, b.ch_unitno, b.num_price as num_price_org FROM cey_web_order_dish a, cey_bt_dish b where  a.dishNo = b.ch_dishno and a.orderSn = '%1'  ")
               .arg(sn));
    int int_id_max = 0;
    while(query.next()) {
        int_id_max ++;
        ch_dishno = query.recordValue("dishNo").toString();
        w_bt_dish::get_vch_plan_k3o3(vch_plan_k3o3_state, vch_plan_k3o3_id, ch_dishno);

        int row = tablemodel_dd.rowCount();
        tablemodel_dd.insertRow(row);

        tablemodel_dd.model_data_set(row, "ch_billno", tablemodel_dd.cur_ch_billno());
        tablemodel_dd.model_data_set(row, "ch_tableno", "Q000");
        tablemodel_dd.model_data_set(row, "int_id", int_id_max);
        tablemodel_dd.model_data_set(row, "ch_dishno", ch_dishno);
        tablemodel_dd.model_data_set(row, "num_price", query.recordValue("price"));

        tablemodel_dd.model_data_set(row, "num_price_org", query.recordValue("num_price_org"));
        tablemodel_dd.model_data_set(row, "num_price_add", 0);
        tablemodel_dd.model_data_set(row, "num_num", query.recordValue("number"));
        tablemodel_dd.model_data_set(row, "num_back", 0);
        tablemodel_dd.model_data_set(row, "int_discount",100);

        tablemodel_dd.model_data_set(row, "ch_suitflag",query.recordValue("suitFlag"));
        tablemodel_dd.model_data_set(row, "vch_operID", public_sql::gs_operid);
        tablemodel_dd.model_data_set(row, "dt_operdate", dt);
        tablemodel_dd.model_data_set(row, "vch_print_memo", "");
        tablemodel_dd.model_data_set(row, "vch_plan_k3o3_id", vch_plan_k3o3_id);

        tablemodel_dd.model_data_set(row, "vch_plan_k3o3_state", vch_plan_k3o3_state);
        tablemodel_dd.model_data_set(row, "ch_presentflag", "N");
        tablemodel_dd.model_data_set(row, "ch_specialflag",  "N");
    }

    //成功配送后，下次自动轮询会自动更新orderstate的信息
    //弹出对应的窗口
    takeout_cook dialog(&tablemodel_dd, "Q000", this);
    dialog.ui->lineEdit_tel->setText(tablemodel->model_data(ui->tableView->currentIndex().row(), "tel").toString());
    dialog.ui->lineEdit_orderman->setText(tablemodel->model_data(ui->tableView->currentIndex().row(), "orderer").toString());
    dialog.ui->lineEdit_sendaddress->setText(tablemodel->model_data(ui->tableView->currentIndex().row(), "address").toString());
    QTimer::singleShot(100, &dialog, SLOT(tosendman()));//送餐员
    dialog.setWindowTitle(_TEXT_SLOT(this));
    dialog.ui->label_desc->setText(this->windowTitle());
    dialog.ui->label_sn->setText(tablemodel->model_data(ui->tableView->currentIndex().row(), "sn").toString());
    if(QDialog::Accepted==lds_roundeddialog_rect_align(&dialog).exec()){

    }
}

void w_scr_dish_weixinwaimai::toexit()
{
    this->reject();
}

void w_scr_dish_weixinwaimai::tosearch(int row)
{
    tablemodel->setFilter(
                "eatTime<='"+ui->dateTimeEdit_2->dateTime().toString("yyyy-MM-dd hh:mm:ss")+
                "'  and eatTime>='"+ui->dateTimeEdit->dateTime().toString("yyyy-MM-dd hh:mm:ss")+
                "'  and orderState like '"+ui->comboBox->comusrdata().toString()+"%' and "+
                "   orderType='1' ");
    if(row==-1) {
        ui->tableView->restore();
    } else{
        ui->tableView->selectRow(row);
        ui->tableView->resizeColumnsToContents();
    }
    torefresh();
}

void w_scr_dish_weixinwaimai::torefresh()
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

    ui->widget->index_widget(tr("下单"))->setEnabled(false);
    ui->widget->index_widget(tr("拒绝"))->setEnabled(false);
    if(ui->lineEdit_orderState->text().toInt() == 1){
        ui->widget->index_widget("下单")->setEnabled(true);
        ui->widget->index_widget("拒绝")->setEnabled(true);
    }
    ui->lineEdit_orderState->setText(
                w_scr_dish_weixinwaimai::getweixinorderStatename(ui->lineEdit_orderState->text()));

    ui->lineEdit_address->setText(tablemodel->model_data(ui->tableView->currentIndex().row(), "address").toString());
}

void w_scr_dish_weixinwaimai::toprintbill()
{
    int row = ui->tableView->currentIndex().row();
    if(row >= 0){
        QString sn = tablemodel->model_data(row, "sn").toString();
        w_scr_dish::static_weixinPrintbillBySn(this->windowTitle(), sn);
    }
}
