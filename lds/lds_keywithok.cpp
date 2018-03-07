#include "lds_keywithok.h"
#include "ui_lds_keywithok.h"

lds_keywithok::lds_keywithok(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::lds_keywithok)
{
    ui->setupUi(this);

    connect(ui->pushButton, SIGNAL(clicked()),this,SLOT(tocenter()));
    connect(ui->pushButton_2, SIGNAL(clicked()),this,SLOT(tocenter()));
    connect(ui->pushButton_3, SIGNAL(clicked()),this,SLOT(tocenter()));
    connect(ui->pushButton_4, SIGNAL(clicked()),this,SLOT(tocenter()));
    connect(ui->pushButton_5, SIGNAL(clicked()),this,SLOT(tocenter()));
    connect(ui->pushButton_6, SIGNAL(clicked()),this,SLOT(tocenter()));
    connect(ui->pushButton_7, SIGNAL(clicked()),this,SLOT(tocenter()));
    connect(ui->pushButton_8, SIGNAL(clicked()),this,SLOT(tocenter()));
    connect(ui->pushButton_9, SIGNAL(clicked()),this,SLOT(tocenter()));
    connect(ui->pushButton_10, SIGNAL(clicked()),this,SLOT(tocenter()));
    connect(ui->pushButton_11, SIGNAL(clicked()),this,SLOT(tocenter()));
    connect(ui->pushButton_12, SIGNAL(clicked()),this,SLOT(tocenter()));
    connect(ui->pushButton_13, SIGNAL(clicked()),this,SLOT(tocenter()));
    connect(ui->pushButton_14, SIGNAL(clicked()),this,SLOT(tocenter()));
    connect(ui->pushButton_16, SIGNAL(clicked()),this,SLOT(tocenter()));
    connect(ui->pushButton_today, SIGNAL(clicked()),this,SLOT(tocenter()));

}

lds_keywithok::~lds_keywithok()
{
    delete ui;
}

void lds_keywithok::tocenter()
{
    QPushButton *p=qobject_cast<QPushButton*>(this->sender());
    if(p){
        emit signal_click(p->text());
    }
}
