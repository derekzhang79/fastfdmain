#include "lds_timeedit_pop.h"
#include "ui_lds_timeedit_pop.h"
#include <QtDebug>

lds_timeEdit_pop::lds_timeEdit_pop(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::lds_timeEdit_pop)
{
    ui->setupUi(this);

    //2
    connect(ui->pushButton_num0,SIGNAL(clicked()),this,SLOT(set_num()));
    connect(ui->pushButton_num1,SIGNAL(clicked()),this,SLOT(set_num()));
    connect(ui->pushButton_num2,SIGNAL(clicked()),this,SLOT(set_num()));
    connect(ui->pushButton_num3,SIGNAL(clicked()),this,SLOT(set_num()));
    connect(ui->pushButton_num4,SIGNAL(clicked()),this,SLOT(set_num()));

    connect(ui->pushButton_num5,SIGNAL(clicked()),this,SLOT(set_num()));
    connect(ui->pushButton_num6,SIGNAL(clicked()),this,SLOT(set_num()));
    connect(ui->pushButton_num7,SIGNAL(clicked()),this,SLOT(set_num()));
    connect(ui->pushButton_num8,SIGNAL(clicked()),this,SLOT(set_num()));
    connect(ui->pushButton_num9,SIGNAL(clicked()),this,SLOT(set_num()));

    connect(ui->pushButton_clear,SIGNAL(clicked()),this,SLOT(set_num()));
    connect(ui->pushButton_ok,SIGNAL(clicked()),this,SLOT(set_num()));

}

lds_timeEdit_pop::~lds_timeEdit_pop()
{
    delete ui;
}

void lds_timeEdit_pop::setTime(const QTime &time)
{
    ui->lineEdit_hour->setText(QString::number(time.hour()));
    ui->lineEdit_minute->setText(QString::number(time.minute()));
    ui->lineEdit_second->setText(QString::number(time.second()));
}

QTime lds_timeEdit_pop::getTime()
{
    return rettime;
}

void lds_timeEdit_pop::set_num()
{
    QPushButton *btn=qobject_cast<QPushButton *>(sender());
    QLineEdit *line=qobject_cast<QLineEdit *>(this->focusWidget());
    if(btn == ui->pushButton_ok){
        rettime = QTime::fromString(ui->lineEdit_hour->text()+":"+ui->lineEdit_minute->text()+":"+ui->lineEdit_second->text(), "h:m:s");
        this->accept();
        return;
    }
    if(line) {
        if(btn==ui->pushButton_clear){
            line->clear();
        }else {
            line->insert(btn->text());
        }
    }
}
