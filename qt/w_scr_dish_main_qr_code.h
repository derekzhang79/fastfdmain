#ifndef W_SCR_DISH_MAIN_QR_CODE_H
#define W_SCR_DISH_MAIN_QR_CODE_H

#include <QDialog>
#include <QStandardItemModel>
#include <QDateTime>
#include "lds_model_sqlstandardmodel.h"

namespace Ui {
class w_scr_dish_main_qr_code;
}

class w_scr_dish_main_qr_code : public QDialog
{
    Q_OBJECT
public:
    enum billType{
        billTypeNone = 0,
        billTypeAVailable = 1,
        billTypeRCV = 2,
        billTypePaid = 3
    };
    //
    struct QrCodeMaster{
        QString sn;
        QString orgName;
        double mount;
        QString tableNo;
        billType state;
        QDateTime date;
    };
    typedef QList<QrCodeMaster> QrCodeMasterList;
    static int indexof(const QrCodeMasterList &masters, const QString &sn);
    //
    struct QrCodeDetail{
        QString nickname;
        QString dishNo;
        double number;
        double price;
        QString zf;
        double mount;
        QString tc;
    };
    typedef QList<QrCodeDetail> QrCodeDetailList;

    struct QrCode_cey_u_orderdish {
        QString ch_dishno;
        double num_num;
        double num_price;
        QString vch_print_memo;
        double num_total;
        QString ch_suitflag;
        QString vch_operid;
    };
    typedef QList<QrCode_cey_u_orderdish> QrCode_cey_u_orderdishList;

    struct QrCodePay{//05微信， 06支付宝
        double mount;
        double alipay;
        double wechatpay;
    };

    static QrCodeMasterList qr_code_master_get(const QString &ch_tableno, billType state, bool *ok = 0);
    static QrCodeMasterList qr_code_master_get(const QMap<QByteArray, QByteArray> &headmap, bool *ok = 0);

    static void qr_code_list_print(const QrCodeMasterList &list);

    static QrCodeDetailList qr_code_detail_get(const QString &sn);
    static void qr_code_list_print(const QList<QrCodeDetail> &list);


    static bool qr_code_bill_do_CANCEL(const QString &sn, QString &errstring);
    static bool qr_code_bill_do_RCV(const QString &sn, QString &errstring);
    static bool qr_code_has_paid(const QString &sn, QrCodePay &paydetail);

    static bool qr_code_bill_do_pay_transaction(const QString &ch_tableno, const QString &ch_billno_R, const QrCodePay &paydetail,
                                                QString &ret_errstring);

    static bool qr_code_can_order(const QString &ch_tableno, QString &errstring);

    static void qr_code_generate_cey_u_orderdish(const QString &sn, QrCode_cey_u_orderdishList & retvalue);
    static bool qr_code_ch_dishno_is_valid(const QrCode_cey_u_orderdishList &value_list, QString &null_ch_dishno);

    static bool load_u_orderdish_transaction(const QrCodeMaster &master, const QrCode_cey_u_orderdishList &value_list, QString &errstring);
public:
    w_scr_dish_main_qr_code(const QString &ch_tableno, const QrCodeMasterList &master_list, QWidget *parent = 0);
    w_scr_dish_main_qr_code(const QString &ch_billno_R, QWidget *parent = 0);
    ~w_scr_dish_main_qr_code();

private slots:
    void toupdatedetail(int index);

    void took();
    void tocancel();
    void tobillcancel();
private:
    void comMasterAddItem(const w_scr_dish_main_qr_code::QrCodeMasterList &master_list);
    void updatemaster(const QrCodeMaster &d);
    void updatemaster();
    void init();
    void updatedetail(const QString &ch_billno_R);
    QrCodeMaster cur_qr_code_master(int index = -1);

private:
    Ui::w_scr_dish_main_qr_code *ui;
    lds_model_sqlstandardmodel *standmodel_detail;
    QString ch_tableno;
};

#endif // W_SCR_DISH_MAIN_QR_CODE_H
