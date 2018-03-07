#ifndef W_SCR_DISH_WEIXINPAY_INPUT_H
#define W_SCR_DISH_WEIXINPAY_INPUT_H

#include <QDialog>
#include "ui_w_scr_dish_weixinpay_input.h"

namespace Ui {
class w_scr_dish_weixinpay_input;
}

class w_scr_dish_weixinpay_input : public QDialog
{
    Q_OBJECT
public:
//    enum DialogCode { Rejected, Accepted };
    enum finishCode {
        qr_ALIED =  Accepted +1,
        qr_WXED,
    };
public slots:
    void to_qr_ali();
    void to_qr_wx();

public:
    explicit w_scr_dish_weixinpay_input(QWidget *parent, const QString &ch_paymodeno);
    ~w_scr_dish_weixinpay_input();
    Ui::w_scr_dish_weixinpay_input *ui;
public slots:
    void updatetype(int index, const QString &title);
    void toalipaytype(int index);
private:
    QString ch_paymodeno;
};

#endif // W_SCR_DISH_WEIXINPAY_INPUT_H
