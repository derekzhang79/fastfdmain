#include "w_scr_dish_main_qr_code_over_view.h"
#include "ui_w_scr_dish_main_qr_code_over_view.h"
#include "lds_query.h"
#include "w_scr_dish_main_qr_code.h"
#include <QTimer>
#include "lds_messagebox.h"
#include "lds_model_sqltablemodel_delegate_com.h"
#include "w_scr_dish_restaurant_dialog.h"
#include "public_sql.h"
#include "lds_roundeddialog_rect_pop.h"

w_scr_dish_main_qr_code_over_view::w_scr_dish_main_qr_code_over_view(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_scr_dish_main_qr_code_over_view)
{
    ui->setupUi(this);
    standmodel = new lds_model_sqlstandardmodel(this);
    standmodel->setHorizontalHeaderLabels(QStringList() <<  "ch_billno"  << "ch_tableno" << "num_total" << "do1" << "do2"<< "do3");
    ui->tableView->setTransferHheader();
    ui->tableView->setModel(standmodel);
    ui->tableView->verticalHeader()->setDefaultSectionSize(50);


    ui->comboBox_state->addItem(tr("全部订单"), "");
    ui->comboBox_state->addItem(tr("新的订单"), "1");
    ui->comboBox_state->addItem(tr("已经确定"), "2");
    ui->comboBox_state->addItem(tr("已经支付"), "3");
    ui->comboBox_state->setcomusrdata("3");//

    this->setWindowTitle(MESSAGE_TITLE_VOID);

    connect(ui->pushButton_exit, SIGNAL(clicked()), this, SLOT(toexit()));
    connect(ui->lineEdit_sn, SIGNAL(textChanged(QString)), this, SLOT(refresh()));
    connect(ui->lineEdit_tableNo, SIGNAL(textChanged(QString)), this, SLOT(refresh()));
    connect(ui->pushButton_down, SIGNAL(clicked()), ui->tableView, SLOT(toPageDown()));
    connect(ui->pushButton_up, SIGNAL(clicked()), ui->tableView, SLOT(toPageUp()));
    connect(ui->comboBox_state, SIGNAL(currentIndexChanged(int)), this, SLOT(refresh()));
    connect(ui->pushButton_refresh, SIGNAL(clicked()), this, SLOT(refresh()));
    //
    QTimer::singleShot(200, this, SLOT(refresh0()));
}

w_scr_dish_main_qr_code_over_view::~w_scr_dish_main_qr_code_over_view()
{
    delete ui;
}

void w_scr_dish_main_qr_code_over_view::refresh0()
{
    refresh();
    if(standmodel->rowCount() == 0 && ui->comboBox_state->curusrdata().toString() != "1") {
        ui->comboBox_state->setcomusrdata("1");
    }
}

void w_scr_dish_main_qr_code_over_view::refresh()
{
    //!等待
    lds_messagebox_loading_show loading(this, "WAITING...", "WAITING...");
    loading.show();
    //!~等待

    for(int row = standmodel->rowCount() - 1; row >= 0; row --) {
        delete ui->tableView->indexWidget(standmodel->index(row, standmodel->fieldIndex("do1")));
        standmodel->removeRow(row);
    }

    bool ok;
    QMap<QByteArray, QByteArray> map;
    //3
    map.clear();
    map.insert("state", "3");//已支付
    w_scr_dish_main_qr_code::QrCodeMasterList  masters = w_scr_dish_main_qr_code::qr_code_master_get(map, &ok);
    if(!ok) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,  tr("二维码点餐") + "\n" + tr("无法获取相关数据"));
        return;
    }

    lds_query query;
    query.execSelect(QString(" select b.ch_tableno,  b.vch_tablename, a.vch_qr_code_sn, a.num_cost, a.dt_operdate from cey_u_table a, cey_bt_table b where a.ch_billno = b.ch_billno and a.vch_qr_code_sn <> '' ; "));
    while(query.next()) {
        bool sn_has_paid_k = w_scr_dish_main_qr_code::indexof(masters, query.recordValue("vch_qr_code_sn").toString());
        QString sn = query.recordValue("vch_qr_code_sn").toString();
        QString ch_tableno = query.recordValue("ch_tableno").toString();
        QString state = "2";
        if(sn_has_paid_k >= 0) {
            state = "3";
        }

        //filter
        if(!sn.contains(ui->lineEdit_sn->text())) continue;
        if(ui->comboBox_state->curusrdata().toString().length() != 0 //所有订单
                && ui->comboBox_state->curusrdata().toString() != state ) {//指定订单
            continue;
        }
        if(!ch_tableno.contains(ui->lineEdit_tableNo->text())) continue;

        int row = standmodel->rowCount();
        standmodel->insertRow(row);
        standmodel->model_data_set(row, "ch_billno", sn);
        standmodel->model_data_set(row, "ch_tableno", ch_tableno);
        standmodel->model_data_set(row, "num_total", query.recordValue("num_cost"));
        standmodel->model_data_set(row, "do1", sn);
        standmodel->model_data_set(row, "do2", "");
        //        standmodel->model_data_set(row, "dt_operdate", query.recordValue("dt_operdate"));

        if(sn_has_paid_k >= 0) {
            ui->tableView->setIndexWidget(standmodel->index(row, standmodel->fieldIndex("do1")), billpayBtn(row, masters[sn_has_paid_k]));
        } else  {
            standmodel->model_data_set(row, "do1", tr("等待支付"));
        }
        ui->tableView->setIndexWidget(standmodel->index(row, standmodel->fieldIndex("do3")), billdetailBtn(row, masters[k]));
    }

    //1
    map.clear();
    map.insert("state", "1");//新订单
    masters = w_scr_dish_main_qr_code::qr_code_master_get(map, &ok);
    if(!ok) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,  tr("二维码点餐") + "\n" + tr("无法获取相关数据"));
        return;
    }

    //校对
    for(int k = 0; k < masters.count(); k++) {
        if(lds_query::selectValue(QString(" select ch_tableno from cey_bt_table where ch_tableno = '%1' " ).arg(masters[k].tableNo)).isNull()) {
            if(1 == lds_messagebox::warning(this, tr("餐桌") + ":" + MESSAGE_TITLE_VOID, masters[k].tableNo +  tr("不存在"), tr("数据同步"), tr("取消"))) {
                return;
            } else {
                /*同步*/
                w_sys_manage_cloudsync_with_time_running::exec_keepdata(this);
                k = 0;
                continue;
            }
        }
    }

    for(int k = 0; k < masters.count(); k++) {
        if(standmodel->findItems(masters[k].sn).count() == 0 ) {
            QString sn = masters[k].sn;
            QString state = "1";
            //filter
            if(!sn.contains(ui->lineEdit_sn->text())) continue;
            if(ui->comboBox_state->curusrdata().toString().length() != 0 //所有订单
                    && ui->comboBox_state->curusrdata().toString() != state ) {//指定订单
                continue;
            }
            if(!masters[k].tableNo.contains(ui->lineEdit_tableNo->text())) continue;


            int row = standmodel->rowCount();
            standmodel->insertRow(row);
            standmodel->model_data_set(row, "ch_billno", sn);
            standmodel->model_data_set(row, "ch_tableno", masters[k].tableNo);
            standmodel->model_data_set(row, "num_total", masters[k].mount);
            standmodel->model_data_set(row, "do1", "");
            standmodel->model_data_set(row, "do2", "");

            ui->tableView->setIndexWidget(standmodel->index(row, standmodel->fieldIndex("do1")), billgetBtn(row, masters[k]));
            ui->tableView->setIndexWidget(standmodel->index(row, standmodel->fieldIndex("do2")), billcancelBtn(row, masters[k]));
            ui->tableView->setIndexWidget(standmodel->index(row, standmodel->fieldIndex("do3")), billdetailBtn(row, masters[k]));
        }
    }
    ui->tableView->resizeColumnsToContents();
    ui->tableView->setColumnWidth(standmodel->fieldIndex("do1"), 100);
    ui->tableView->setColumnWidth(standmodel->fieldIndex("do2"), 100);
}

void w_scr_dish_main_qr_code_over_view::tobillget()
{
    //!等待
    lds_messagebox_loading_show loading(this, "WAITING...", "WAITING...");
    loading.show();
    //!~等待


    PushButton *b = static_cast<PushButton *>(this->sender());
    if(b) {
        int row = b->row;
        if(row >= 0) {
            QString errstring;
            QString null_ch_dishno;
            w_scr_dish_main_qr_code::QrCode_cey_u_orderdishList detail_list;

            if(false == w_scr_dish_main_qr_code::qr_code_can_order(b->master.tableNo, errstring)) {
                lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
                return;
            }
            //
            w_scr_dish_main_qr_code::qr_code_generate_cey_u_orderdish(b->master.sn, detail_list);
            //
            while(false == w_scr_dish_main_qr_code::qr_code_ch_dishno_is_valid(detail_list, null_ch_dishno)) {
                if(1 == lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("菜品") + null_ch_dishno + tr("不存在"), tr("数据同步"), tr("取消"))) {
                    return;
                }
                /*同步*/
                w_sys_manage_cloudsync_with_time_running::exec_keepdata(this);
            }
            //
            if(w_scr_dish_main_qr_code::load_u_orderdish_transaction(b->master, detail_list, errstring)) {
                lds_messagebox::information(this, MESSAGE_TITLE_VOID,  tr("操作成功"));
                refresh();
                return;
            }
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
        }
    }
}

void w_scr_dish_main_qr_code_over_view::tobillpay()
{
    //!等待
    lds_messagebox_loading_show loading(this, "WAITING...", "WAITING...");
    loading.show();
    //!~等待

    PushButton *b = static_cast<PushButton *>(this->sender());
    if(b) {
        int row = b->row;
        if(row >= 0) {
            QString errstring;
            w_scr_dish_main_qr_code::QrCodePay qr_code_pay_data;
            QString sn = standmodel->model_data(row, "ch_billno").toString();
            QString ch_tableno = standmodel->model_data(row, "ch_tableno").toString();
            QString ch_billno = lds_query::selectValue(QString(" select ch_billno from cey_bt_table where ch_tableno = '%1' ").arg(ch_tableno)).toString();

            //not_paid
            if(false == w_scr_dish_main_qr_code::qr_code_has_paid(
                        sn,
                        qr_code_pay_data)) {
                lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("操作失败") + "not_paid");
                return;
            }
            //
            if(1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, tr("二维码点单") + "\n" +
                                             tr("本单已支付") + "\n" +
                                             tr("餐桌") + ":" +ch_tableno+ "\n"  +
                                             tr("总计") + ":" + QString().sprintf("%.02f", qr_code_pay_data.mount) + "\n"  +
                                             tr("支付宝") + ":" + QString().sprintf("%.02f", qr_code_pay_data.alipay) + "\n"  +
                                             tr("微信") + ":" + QString().sprintf("%.02f", qr_code_pay_data.wechatpay),

                                             tr("确定"), tr("取消"))) {
                return;
            }
            if(false == w_scr_dish_main_qr_code::qr_code_bill_do_pay_transaction(ch_tableno, ch_billno, qr_code_pay_data, errstring)) {
                lds_messagebox::information(this, MESSAGE_TITLE_VOID, errstring);
                return;
            }
            lds_messagebox::information(this, MESSAGE_TITLE_VOID, tr("操作成功"));
            refresh();
        }
    }
}

void w_scr_dish_main_qr_code_over_view::tobillcancel()
{
    //!等待
    lds_messagebox_loading_show loading(this, "WAITING...", "WAITING...");
    loading.show();
    //!~等待

    PushButton *b = static_cast<PushButton *>(this->sender());
    if(b) {
        int row = b->row;
        if(row >= 0) {
            if(1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, b->text(), tr("确定"), tr("取消"))) {
                return;
            }
            QString errstring;
            if(w_scr_dish_main_qr_code::qr_code_bill_do_CANCEL(b->master.sn, errstring)) {
                lds_messagebox::information(this, MESSAGE_TITLE_VOID, tr("操作成功"));
                refresh();
                return;
            }
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
        }
    }
}

void w_scr_dish_main_qr_code_over_view::tobilldetail()
{
    //!等待
    lds_messagebox_loading_show loading(this, "WAITING...", "WAITING...");
    loading.show();
    //!~等待

    PushButton *b = static_cast<PushButton *>(this->sender());
    if(b) {
        int row = b->row;
        if(row >= 0) {
            const w_scr_dish_main_qr_code::QrCodeMaster& m = b->master;
            w_scr_dish_main_qr_code::QrCode_cey_u_orderdishList detail_list;
            w_scr_dish_main_qr_code::qr_code_generate_cey_u_orderdish(m.sn, detail_list);
            QString dish_info;
            dish_info += m.orgName + "\n";
            QDialog dialog(this);
            QTableView table(&dialog);
            dialog.resize(600, 400);
            table.setGeometry(10, 10, 580, 380);

            QStandardItemModel model;
            table.setModel(&model);
            table.setEditTriggers(QTableView::NoEditTriggers);
            model.appendRow(QList<QStandardItem*>()
                            << lds_model_sqlstandardmodel::newItem(m.sn)
                            << lds_model_sqlstandardmodel::newItem(m.orgName)
                            << lds_model_sqlstandardmodel::newItem(m.date)
                            << lds_model_sqlstandardmodel::newItem(m.tableNo)
                            << lds_model_sqlstandardmodel::newItem(m.state)
                            << lds_model_sqlstandardmodel::newItem(m.mount)
                            );
            for(int k = 0; k < detail_list.count(); k++) {
                model.appendRow(QList<QStandardItem*>()
                                << lds_model_sqlstandardmodel::newItem(detail_list[k].ch_dishno)
                                << lds_model_sqlstandardmodel::newItem(detail_list[k].num_num)
                                << lds_model_sqlstandardmodel::newItem(detail_list[k].num_price)
                                << lds_model_sqlstandardmodel::newItem(detail_list[k].vch_print_memo)
                                << lds_model_sqlstandardmodel::newItem(detail_list[k].num_total)
                                << lds_model_sqlstandardmodel::newItem(detail_list[k].ch_suitflag)
                                << lds_model_sqlstandardmodel::newItem(detail_list[k].vch_operid)
                                );
            }

            lds_roundeddialog_rect_pop(&dialog, this).exec();
        }
    }
}

void w_scr_dish_main_qr_code_over_view::toexit()
{
    this->reject();
}

QPushButton *w_scr_dish_main_qr_code_over_view::billpayBtn(int row, const w_scr_dish_main_qr_code::QrCodeMaster &master)
{
    PushButton *btn = new PushButton;
    btn->setText(tr("确认支付"));
    btn->row = row;
    btn->master = master;
    connect(btn, SIGNAL(clicked()), this, SLOT(tobillpay()));
    return btn;
}

QPushButton *w_scr_dish_main_qr_code_over_view::billgetBtn(int row, const w_scr_dish_main_qr_code::QrCodeMaster &master)
{
    PushButton *btn = new PushButton;
    btn->setText(tr("确认订单"));
    btn->row = row;
    btn->master = master;
    connect(btn, SIGNAL(clicked()), this, SLOT(tobillget()));
    return btn;
}

QPushButton *w_scr_dish_main_qr_code_over_view::billcancelBtn(int row, const w_scr_dish_main_qr_code::QrCodeMaster &master)
{
    PushButton *btn = new PushButton;
    btn->setText(tr("取消订单"));
    btn->row = row;
    btn->master = master;
    connect(btn, SIGNAL(clicked()), this, SLOT(tobillcancel()));
    return btn;
}

QPushButton *w_scr_dish_main_qr_code_over_view::billdetailBtn(int row, const w_scr_dish_main_qr_code::QrCodeMaster &master)
{
    PushButton *btn = new PushButton;
    btn->setText(tr("详细"));
    btn->row = row;
    btn->master = master;
    connect(btn, SIGNAL(clicked()), this, SLOT(tobilldetail()));
    return btn;
}

void w_scr_dish_main_qr_code_over_view::hideButton(int row, int column)
{
    QPushButton *b = static_cast<QPushButton *>(ui->tableView->indexWidget(standmodel->index(row, column)));
    if(b) {
        b->hide();
    }
}
