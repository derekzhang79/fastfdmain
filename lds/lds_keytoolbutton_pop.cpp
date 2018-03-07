#include "lds_keytoolbutton_pop.h"
#include "ui_lds_keytoolbutton_pop.h"
#include <QtDebug>
#include <QPushButton>

lds_keytoolbutton_pop::lds_keytoolbutton_pop(QWidget *parent) :
    lds_roundedwidget(parent),
    ui(new Ui::lds_keytoolbutton_pop)
{
    ui->setupUi(this);
    //1
    connect(ui->frame,SIGNAL(signal_click(QString)),this,SIGNAL(signal_click(QString)));
}

lds_keytoolbutton_pop::~lds_keytoolbutton_pop()
{
    delete ui;
}
