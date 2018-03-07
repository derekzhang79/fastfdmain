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
    standmodel->setHorizontalHeaderLabels(QStringList() << "ch_tableno" << "vch_tablename" << "ch_billno" << "ch_state" << "do");
    ui->tableView->setTransferHheader();
    ui->tableView->setModel(standmodel);
    ui->tableView->verticalHeader()->setDefaultSectionSize(50);

    this->setWindowTitle(MESSAGE_TITLE_VOID);

    connect(ui->pushButton_exit, SIGNAL(clicked()), this, SLOT(toexit()));
    connect(ui->lineEdit_sn, SIGNAL(textChanged(QString)), this, SLOT(refresh()));
    connect(ui->pushButton_down, SIGNAL(clicked()), ui->tableView, SLOT(toPageDown()));
    connect(ui->pushButton_up, SIGNAL(clicked()), ui->tableView, SLOT(toPageUp()));
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
    //3
    QMap<QByteArray, QByteArray> map;
    map.insert("state", "3");//已付款
    w_scr_dish_main_qr_code::QrCodeMasterList  masters = w_scr_dish_main_qr_code::qr_code_master_get(map);

    lds_query query;
    query.execSelect(QString(" select b.ch_tableno,  b.vch_tablename, a.vch_qr_code_sn from cey_u_table a, cey_bt_table b where a.ch_billno = b.ch_billno and ; "));
    while(query.next()) {
        bool sn_has_paid = w_scr_dish_main_qr_code::indexof(masters, query.recordValue("vch_qr_code_sn").toString()) >= 0;
        QString sn = query.recordValue("vch_qr_code_sn").toString();

        //filter
        if(!sn.contains(ui->lineEdit_sn->text()))  continue;

        standmodel->appendRow(QList<QStandardItem*>()
                              << lds_model_sqlstandardmodel::newItem(sn)
                              << lds_model_sqlstandardmodel::newItem(query.recordValue("ch_tableno"))
                              << lds_model_sqlstandardmodel::newItem(query.recordValue("vch_tablename"))
                              << lds_model_sqlstandardmodel::newItem("")
                              << lds_model_sqlstandardmodel::newItem(tr("无操作"))
                              );
        int currow = standmodel->rowCount() - 1;
        ui->tableView->setIndexWidget(standmodel->index(currow, standmodel->fieldIndex("ch_state")), sn_has_paid ? billpayBtn(currow) : billwaitpayBtn(currow));
    }

    //1
    map.clear();
    map.insert("state", "1");//新订单
    masters = w_scr_dish_main_qr_code::qr_code_master_get(map);
    for(int k = 0; k < masters.count(); k++) {
        if(standmodel->findItems(masters[k].sn).count() == 0 ) {
            QString sn = masters[k].sn;

            //filter
            if(!sn.contains(ui->lineEdit_sn->text()))  continue;

            standmodel->appendRow(QList<QStandardItem*>()
                                  << lds_model_sqlstandardmodel::newItem(sn)
                                  << lds_model_sqlstandardmodel::newItem(masters[k].tableNo)
                                  << lds_model_sqlstandardmodel::newItem(lds_query::selectValue(QString(" select vch_tablename from cey_bt_table where ch_tableno = '%1' ").arg(masters[k].tableNo)))
                                  << lds_model_sqlstandardmodel::newItem("")
                                  << lds_model_sqlstandardmodel::newItem("")
                                  );
            int currow = standmodel->rowCount() - 1;
            ui->tableView->setIndexWidget(standmodel->index(currow, standmodel->fieldIndex("ch_state")), billgetBtn(currow));
            ui->tableView->setIndexWidget(standmodel->index(currow, standmodel->fieldIndex("do")), billcancelBtn(currow));
        }
    }
    ui->tableView->resizeColumnToContents(standmodel->fieldIndex("ch_billno"));
    ui->tableView->resizeColumnToContents(standmodel->fieldIndex("ch_tableno"));
    ui->tableView->resizeColumnToContents(standmodel->fieldIndex("vch_tablename"));
    ui->tableView->setColumnWidth(standmodel->fieldIndex("ch_state"), 100);
    ui->tableView->setColumnWidth(standmodel->fieldIndex("do"), 100);
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
                set_model_btext_hide_button(row, 3);
                set_model_btext_hide_button(row, 4);
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
    btn->setText(tr("已支付"));
    btn->setProperty("row", row);
    connect(btn, SIGNAL(clicked()), this, SLOT(tobillpay()));
    return btn;
}

QPushButton *w_scr_dish_main_qr_code_over_view::billwaitpayBtn(int row)
{
    QPushButton *btn = new QPushButton;
    btn->setText(tr("待支付"));
    btn->setProperty("row", row);
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

void w_scr_dish_main_qr_code_over_view::set_model_btext_hide_button(int row, int column)
{
    QPushButton *b = qobject_cast<QPushButton *>(ui->tableView->indexWidget(standmodel->index(row, column)));
    if(b) {
        standmodel->item(row, column)->setText(b->text());
        b->hide();
    }
}
