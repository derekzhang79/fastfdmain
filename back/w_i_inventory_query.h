#ifndef W_I_INVENTORY_QUERY_H
#define W_I_INVENTORY_QUERY_H

#include <QDialog>
#include "w_rpt_report.h"
#include "w_rpt_report_frame.h"

class QSqlQueryModel;
class Ui_w_i_inventory_query_Dialog;
class w_i_inventory_query : public QDialog
{
    Q_OBJECT
public:
    explicit w_i_inventory_query(QWidget *parent = 0);
    
private:
    void tab1sqlinit();
private slots:
    void toselect();
    void toexit();

    void toexport();
    void toprint();

    void tosave_bucang(bool f);
private :
    Ui_w_i_inventory_query_Dialog *ui;
    report_querymodel *querymodel;
};

#endif // W_I_INVENTORY_QUERY_H
