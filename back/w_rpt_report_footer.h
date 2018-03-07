#ifndef W_RPT_REPORT_FOOTER_H
#define W_RPT_REPORT_FOOTER_H

#include <QWidget>
#include "ui_w_rpt_report_footer.h"

class w_rpt_report_footer : public QWidget
{
    Q_OBJECT

public:
    explicit w_rpt_report_footer(QWidget *parent = 0);
    ~w_rpt_report_footer();
    Ui_w_rpt_report_footer *ui;
};

#endif // W_RPT_REPORT_FOOTER_H
