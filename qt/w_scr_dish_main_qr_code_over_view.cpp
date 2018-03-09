#include "w_scr_dish_main_qr_code_over_view.h"
#include "ui_w_scr_dish_main_qr_code_over_view.h"
#include "lds_query.h"
#include "w_scr_dish_main_qr_code.h"
#include <QTimer>
#include "lds_messagebox.h"
#include "lds_model_sqltablemodel_delegate_com.h"
#include "w_scr_dish_restaurant_dialog.h"
#include "public_sql.h"

w_scr_dish_main_qr_code_over_view::w_scr_dish_main_qr_code_over_view(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_scr_dish_main_qr_code_over_view)
{
    ui->setupUi(this);
    order_row = -1;
    standmodel = new lds_model_sqlstandardmodel(this);
    standmodel->setHorizontalHeaderLabels(QStringList() <<  "ch_billno" <<  "vch_memo" << "ch_tableno" << "num_total" << "ch_state" << "do");
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
    QTimer::singleShot(200, this, SLOT(refresh()));
}

w_scr_dish_main_qr_code_over_view::~w_scr_dish_main_qr_code_over_view()
{
    delete ui;
}

QString w_scr_dish_main_qr_code_over_view::order_ch_tableno()
{
    return standmodel->model_data(order_row, "ch_tableno").toString();
}

QString w_scr_dish_main_qr_code_over_view::order_sn()
{
    return standmodel->model_data(order_row, "ch_billno").toString();
}

void w_scr_dish_main_qr_code_over_view::refresh()
{
    //!等待
    lds_messagebox_loading_show loading(this, "WAITING...", "WAITING...");
    loading.show();
    //!~等待

    for(int row = standmodel->rowCount() - 1; row >= 0; row --) {
        delete ui->tableView->indexWidget(standmodel->index(row, standmodel->fieldIndex("ch_state")));
        standmodel->removeRow(row);
    }

    bool ok;
    QString errstring;
    QMap<QByteArray, QByteArray> map;
    QMap<QString, QString> ch_tableno_err_map;
    //3
    map.clear();
    map.insert("state", "3");//已支付
    w_scr_dish_main_qr_code::QrCodeMasterList  masters = w_scr_dish_main_qr_code::qr_code_master_get(map, &ok);
    if(!ok) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,  tr("二维码点餐") + "\n" + tr("无法获取相关数据"));
        return;
    }

    lds_query query;
    query.execSelect(QString(" select b.ch_tableno,  b.vch_tablename, a.vch_qr_code_sn, a.num_cost from cey_u_table a, cey_bt_table b where a.ch_billno = b.ch_billno and a.vch_qr_code_sn <> '' ; "));
    while(query.next()) {
        bool sn_has_paid = w_scr_dish_main_qr_code::indexof(masters, query.recordValue("vch_qr_code_sn").toString()) >= 0;
        QString sn = query.recordValue("vch_qr_code_sn").toString();
        QString ch_tableno = query.recordValue("ch_tableno").toString();
        QString state = "2";
        if(sn_has_paid) {
            state = "3";
        }

        //filter
        if(!sn.contains(ui->lineEdit_sn->text())) continue;
        if(ui->comboBox_state->curusrdata().toString().length() != 0 //所有订单
                && ui->comboBox_state->curusrdata().toString() != state ) {//指定订单
            continue;
        }
        if(!ch_tableno.contains(ui->lineEdit_tableNo->text())) continue;

        standmodel->appendRow(QList<QStandardItem*>()
                              << lds_model_sqlstandardmodel::newItem(sn)
                              << lds_model_sqlstandardmodel::newItem("")
                              << lds_model_sqlstandardmodel::newItem(ch_tableno)
                              << lds_model_sqlstandardmodel::newItem(query.recordValue("num_cost"))
                              << lds_model_sqlstandardmodel::newItem("")
                              << lds_model_sqlstandardmodel::newItem(tr("无操作"))
                              );
        int currow = standmodel->rowCount() - 1;
        if(sn_has_paid) {
            ui->tableView->setIndexWidget(standmodel->index(currow, standmodel->fieldIndex("ch_state")), billpayBtn(currow));
        } else  {
            standmodel->model_data_set(currow, "ch_state", tr("等待支付"));
        }
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

            standmodel->appendRow(QList<QStandardItem*>()
                                  << lds_model_sqlstandardmodel::newItem(sn)
                                  << lds_model_sqlstandardmodel::newItem("")
                                  << lds_model_sqlstandardmodel::newItem(masters[k].tableNo)
                                  << lds_model_sqlstandardmodel::newItem(masters[k].mount)
                                  << lds_model_sqlstandardmodel::newItem("")
                                  << lds_model_sqlstandardmodel::newItem("")
                                  );
            int currow = standmodel->rowCount() - 1;
            ui->tableView->setIndexWidget(standmodel->index(currow, standmodel->fieldIndex("ch_state")), billgetBtn(currow));
            ui->tableView->setIndexWidget(standmodel->index(currow, standmodel->fieldIndex("do")), billcancelBtn(currow));
            //errdesc
            if(ch_tableno_err_map.find(masters[k].tableNo) == ch_tableno_err_map.end()
                    && false == w_scr_dish_main_qr_code::qr_code_can_order(masters[k].tableNo, errstring)) {
                ch_tableno_err_map.insert(masters[k].tableNo, errstring);
            }
            standmodel->model_data_set(currow, "vch_memo", ch_tableno_err_map.value(masters[k].tableNo, tr("空闲")));
        }
    }
    ui->tableView->resizeColumnsToContents();
    ui->tableView->setColumnWidth(standmodel->fieldIndex("ch_state"), 100);
    ui->tableView->setColumnWidth(standmodel->fieldIndex("do"), 100);

    if(standmodel->rowCount() == 0 && ui->comboBox_state->curusrdata().toString() != "1") {
        ui->comboBox_state->setcomusrdata("1");
    }
}

void w_scr_dish_main_qr_code_over_view::tobillget()
{
    QPushButton *b = qobject_cast<QPushButton *>(this->sender());
    if(b) {
        int row = b->property("row").toInt();
        if(row >= 0) {
            order_row = row;
            this->done(public_sql::GoRestaurantOrder);
        }
    }
}

void w_scr_dish_main_qr_code_over_view::tobillpay()
{
    QPushButton *b = qobject_cast<QPushButton *>(this->sender());
    if(b) {
        int row = b->property("row").toInt();
        if(row >= 0) {
            order_row = row;
            this->done(public_sql::GoRestaurantOrder);
        }
    }
}

void w_scr_dish_main_qr_code_over_view::tobillcancel()
{
    QPushButton *b = qobject_cast<QPushButton *>(this->sender());
    if(b) {
        int row = b->property("row").toInt();
        if(row >= 0) {
            if(1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, b->text(), tr("确定"), tr("取消"))) {
                return;
            }
            //!等待
            lds_messagebox_loading_show loading(this, "WAITING...", "WAITING...");
            loading.show();
            //!~等待

            QString errstring;
            if(w_scr_dish_main_qr_code::qr_code_bill_do_CANCEL(standmodel->model_data(row, "ch_billno").toString(), errstring)) {
                hideButton(row, standmodel->fieldIndex("ch_state"));
                hideButton(row, standmodel->fieldIndex("do"));
                standmodel->model_data_set(row, "ch_state", "");
                standmodel->model_data_set(row, "do", QColor("green"), Qt::TextColorRole);
                standmodel->model_data_set(row, "do", tr("取消成功"));
                lds_messagebox::information(this, MESSAGE_TITLE_VOID, tr("操作成功"));
                ui->tableView->selectRow(row);
                return;
            }
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
        }
    }
}

void w_scr_dish_main_qr_code_over_view::toexit()
{
    this->reject();
}

QPushButton *w_scr_dish_main_qr_code_over_view::billpayBtn(int row)
{
    QPushButton *btn = new QPushButton;
    btn->setText(tr("确认支付"));
    btn->setProperty("row", row);
    connect(btn, SIGNAL(clicked()), this, SLOT(tobillpay()));
    return btn;
}

QPushButton *w_scr_dish_main_qr_code_over_view::billgetBtn(int row)
{
    QPushButton *btn = new QPushButton;
    btn->setText(tr("确认订单"));
    btn->setProperty("row", row);
    connect(btn, SIGNAL(clicked()), this, SLOT(tobillget()));
    return btn;
}

QPushButton *w_scr_dish_main_qr_code_over_view::billcancelBtn(int row)
{
    QPushButton *btn = new QPushButton;
    btn->setText(tr("取消订单"));
    btn->setProperty("row", row);
    connect(btn, SIGNAL(clicked()), this, SLOT(tobillcancel()));
    return btn;
}

void w_scr_dish_main_qr_code_over_view::hideButton(int row, int column)
{
    QPushButton *b = qobject_cast<QPushButton *>(ui->tableView->indexWidget(standmodel->index(row, column)));
    if(b) {
        b->hide();
    }
}
