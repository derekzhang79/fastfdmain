#ifndef W_SCR_DISH_MAIN_QR_CODE_OVER_VIEW_H
#define W_SCR_DISH_MAIN_QR_CODE_OVER_VIEW_H

#include <QDialog>
#include "lds_model_sqlstandardmodel.h"

namespace Ui {
class w_scr_dish_main_qr_code_over_view;
}

class w_scr_dish_main_qr_code_over_view : public QDialog
{
    Q_OBJECT

public:
    explicit w_scr_dish_main_qr_code_over_view(QWidget *parent = 0);
    ~w_scr_dish_main_qr_code_over_view();

    QString order_ch_tableno();
    QString order_sn();
public slots:
    void refresh();
    void tobillget();
    void tobillpay();
    void tobillcancel();
    void toexit();
private:
    QPushButton * billpayBtn(int row);
    QPushButton * billwaitpayBtn(int row);
    QPushButton * billgetBtn(int row);
    QPushButton * billcancelBtn(int row);
    void set_model_btext_hide_button(int row, int column);
private:
    Ui::w_scr_dish_main_qr_code_over_view *ui;
    lds_model_sqlstandardmodel *standmodel;
    int order_row;
};

#endif // W_SCR_DISH_MAIN_QR_CODE_OVER_VIEW_H
