#include "w_sys_manage_outer_pay_set_sql.h"
#include "ui_w_sys_manage_outer_pay_set_sql.h"
#include "lds_query.h"
#include <QSqlError>
#include <QElapsedTimer>

w_sys_manage_outer_pay_set_sql::w_sys_manage_outer_pay_set_sql(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_sys_manage_outer_pay_set_sql)
{
    ui->setupUi(this);
    connect(ui->pushButton_ok, SIGNAL(clicked()),this,SLOT(took()));
    connect(ui->pushButton_cancel, SIGNAL(clicked()),this,SLOT(tocancel()));
    ui->comboBox->addItem(tr("数据库重新建立"), "drop database hddpos;create database hddpos;");
    ui->comboBox->setCurrentIndex(-1);
    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this,SLOT(updateEdit(int)));
}

w_sys_manage_outer_pay_set_sql::~w_sys_manage_outer_pay_set_sql()
{
    delete ui;
}

void w_sys_manage_outer_pay_set_sql::took()
{
    QElapsedTimer etimer;
    etimer.start();
    lds_query query;
    if(query.execDirect(ui->textEdit->toPlainText())) {
        ui->textEdit_2->append("=================\n"+tr("操作成功") + "  elapsed:"+ QString::number(etimer.elapsed()) + "ms");
        while(query.next()) {
            int count = query.recordCount();
            QString str;
            for(int k = 0; k < count; k ++) {
                str += query.recordValue(k).toString()+"\t";
            }
            ui->textEdit_2->append(str);
        }
    } else {
        ui->textEdit_2->append("=================\n"+query.recordError());
    }
}

void w_sys_manage_outer_pay_set_sql::tocancel()
{
    this->reject();
}

void w_sys_manage_outer_pay_set_sql::updateEdit(int index)
{
    ui->textEdit->setText(ui->comboBox->itemData(index).toString());
}
