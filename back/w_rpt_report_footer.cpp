#include "w_rpt_report_footer.h"

w_rpt_report_footer::w_rpt_report_footer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui_w_rpt_report_footer)
{
    ui->setupUi(this);
}

w_rpt_report_footer::~w_rpt_report_footer()
{
    delete ui;
}
