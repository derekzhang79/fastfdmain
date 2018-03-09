#ifndef W_SCR_DISH_MAIN_QR_CODE_OVER_VIEW_H
#define W_SCR_DISH_MAIN_QR_CODE_OVER_VIEW_H

#include <QDialog>
#include "lds_model_sqlstandardmodel.h"
#include "w_scr_dish_main_qr_code.h"
#include <QPushButton>

namespace Ui {
class w_scr_dish_main_qr_code_over_view;
}

class w_scr_dish_main_qr_code_over_view : public QDialog
{
    Q_OBJECT
public:
    class PushButton : public QPushButton {
    public:
        PushButton(QWidget *parent = 0 ) : QPushButton(parent){}
        int row;
        w_scr_dish_main_qr_code::QrCodeMaster master;
    };
public:
    explicit w_scr_dish_main_qr_code_over_view(QWidget *parent = 0);
    ~w_scr_dish_main_qr_code_over_view();

public slots:
    void refresh0();
    void refresh();
    void tobillget();
    void tobillpay();
    void tobillcancel();
    void tobilldetail();
    void toexit();
private:
    QPushButton * billpayBtn(int row, const w_scr_dish_main_qr_code::QrCodeMaster &master);
    QPushButton * billgetBtn(int row, const w_scr_dish_main_qr_code::QrCodeMaster &master);
    QPushButton * billcancelBtn(int row, const w_scr_dish_main_qr_code::QrCodeMaster &master);
    QPushButton * billdetailBtn(int row, const w_scr_dish_main_qr_code::QrCodeMaster &master);
    void hideButton(int row, int column);
private:
    Ui::w_scr_dish_main_qr_code_over_view *ui;
    lds_model_sqlstandardmodel *standmodel;

};

#endif // W_SCR_DISH_MAIN_QR_CODE_OVER_VIEW_H
