#include "bank_card.h"
#include "ui_bank_card.h"
#include "lds_messagebox.h"
#include <QDateTime>


#include "backheader.h"
#include "lds_dialog_input.h"
#include "n_func.h"
#include <QTimer>

bank_card::bank_card(const QString & num,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::bank_card)
{
    ui->setupUi(this);
    _num = num;
    init_control();
    btn_t_f = false;
    shishoujine = 0;
    ui->lineEdit_3->setValue(_num.toDouble());
    ui->lineEdit_8->setValue(_num.toDouble());

    QTimer::singleShot(0, this, SLOT(todelayfocus()));
}

bank_card::~bank_card()
{
    delete ui;
}

void bank_card::init_control()
{
    ui->lineEdit_3->setEnabled(false);
}

void bank_card::on_pushButton_4_clicked()//确定
{
    pingzhenghao = ui->lineEdit_2->text();
    shishoujine = ui->lineEdit_8->value();
    vch_memeo = ui->lineEdit_vch_memo->text();
    btn_t_f = true;
    time = n_func::f_get_sysdatetime().toString("yyyy-MM-dd hh:mm:ss");
    this->close();
}

void bank_card::on_pushButton_5_clicked()//取消
{
    btn_t_f = false;
    this->close();
}

void bank_card::todelayfocus()
{
    ui->lineEdit_8->setFocus();
    ui->lineEdit_8->selectAll();
}
