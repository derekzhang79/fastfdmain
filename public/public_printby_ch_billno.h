#ifndef PUBLIC_PRINTBY_CH_BILLNO_H
#define PUBLIC_PRINTBY_CH_BILLNO_H

#include <QObject>
#include "commonheader.h"
#include "public_mainprinter.h"
#include "lds_query.h"
#include "public_ins_maker.h"
#include <QDateTime>
#include "lds_model_sqltablemodel.h"
#include "fexpandmain_model_sqltablemodel.h"

class public_printby_ch_billno : public QObject
{
    Q_OBJECT
public:
    enum payPrintMode{
        payNone = 0,
        payExit = 1, //退单
        payNormal = 2,//结算单
        payDrawer = 4,//钱箱
        payLabel = 8,//标签
    };
    Q_DECLARE_FLAGS(payPrintModes, payPrintMode)
public:
    static QString getfixedlen(const QString &value, int len, const QString &isCut);
public:
    explicit public_printby_ch_billno(const QString &_ch_billno, const QString &languageKey = "",  QObject *parent = 0);
    ~public_printby_ch_billno();

    //payexit pay drawer label
    void toPayPrint(const QString &bp_path, int bp_count, int label_print_int_flowid_max, payPrintModes m);
    void toprint(const QString &path, int manual_bp_count = -1, bool payexit = false);
    void toprint_dish_return(const QString &path, const QDateTime &dt);

    //外卖
    void toprintwaimai(const QString &path, const QDateTime &dt = QDateTime());//打印外卖
    void toprintwaimai_pay(const QString &path, int manual_bp_count);//打印外卖

    //没有表头，没有表尾，不切刀,包含分单号
    void toprint_preprint(const QString &path, const QString &title, const QDateTime &dt = QDateTime(), const QString & out_sql/*例如 int_flowid in (1232,213,213,213)*/ = "");//打印简单的消费信息，用于预打单
    double toprint_preprint_div(const QString &path, int sqr = -1, const QDateTime &dt = QDateTime());//预打单
    void toprint_preprint_div_title(const QString &path, const QString &title);//预打单
    void toprint_preprint_div_tail(const QString &path, double subtotal, int idvcount);//预打单
    double toprint_preprint_summary();

    void clear_ins();
    QString defpath();
//    static void print_label(QWidget *parent, const QString &ch_billno, int print_int_flowid_max = -1);
    static void print_label(const QString &ch_billno, int print_int_flowid_max = -1);
    static void print_label_test(QWidget *parent);

    blockIns getPintIns() const;
private:
    void print_content(const QDateTime &dt = QDateTime(), bool payexit = false, const QString & out_sql/*例如 int_flowid in (1232,213,213,213)*/ = "");
    void print_content_dishreturn(const QDateTime &dt = QDateTime());//中餐退菜总单
    void print_tail();
    void print_finish();
    void checkpaperwidth();
    void print_pay_detail();
private:
    QString get_dish_typename(const QString &ch_dish_typeno);
    QString get_sub_typename(const QString &ch_sub_typeno);
    QString get_dish_typeno_sum(const QString &ch_dish_typeno);
    QString get_sub_typeno_sum(const QString &ch_sub_typeno);
    QString getfixedlen(const QString &value, int len);
    void suitdishprint(int P_int_flowid, const QByteArray &pre, int print_memo_num_x = 1);
    void piecedishprint(lds_query &query, const QByteArray &pre, bool isheader = false, int print_memo_num_x = 1);

    int format_Negative(const QString &field_name);
    QString format_amount(float value, const QString &field_name);
    QString format_num(float value, const QString &field_name);
    QString format_price(float value, const QString &field_name);

    QString name_of_operid(const QString &operid);
    QString key_value_com(const QString &k, const QString &v, int len);
    int scale_h_by(const QByteArray &s00);
    QString get_bp_fmt_table(const QString &vch_tablename, const QString &ch_tableno);
    QString get_bp_fmt_table();

    QString get_bp_fmt_operid(const QString &vch_operator, const QString &vch_operid);
    QString get_bp_fmt_operid();

    QVariant get_cey_u_table(const QString &sql_value_name);

    QString group_sql_samedishNotadd_qtprint();
    QString f_get_sysparm(const QString &as_parameter, const QVariant &as_default = QVariant());

    QString pay_fixed_service_suffix();
    QString pay_fixed_tax_suffix();
private:
    QString intersql;
    QString sumsql;
    QString litterblank;
    QString ch_billno;
    int paperwidth;
    int column_count;
    int bp_count;
    int bp_offcols;
    int bp_begincols;
    QString bp_vch_dishname_No;

    QByteArray bp_content_len;
//    QString ch_payno;
//    QString int_id;
//    QByteArray vch_print_memo;
//    QString vch_reason;
//    QString ch_suitflag;
//    QString bp_series_group;//按大类分组
//    QString ch_dish_typeno;
//    QString ch_sub_typeno;
//    QString ch_tableno;
//    QString vch_tablename;
//    int int_person;

    QString bp_rowid;
    QString bp_dishname;
    QString bp_unit;
    QString bp_num;
    QString bp_price;
    QString bp_fmt_price_with_org;
    QString bp_amount;
    QString bp_disdetail;
    QString bp_total;
    QString bp_printmemo;
    QString bp_presentflag;
    QString bp_fmt_specialflag;
    QString bp_fmt_overstep_cut;
    QString bp_fmt_table_fmts;
    QString bp_fmt_serial_no_desc;
    QString bp_fmt_operid_fmts;

    int bp_amount_left;
    int bp_fmt_disdetail_left ;
    int bp_fmt_total_left ;
    int bp_dishname_left ;
    int bp_num_left ;
    int bp_price_left ;
    int bp_unit_left;
    int bp_num_dec;
    int bp_price_dec;
    int bp_amount_dec;

    int bp_dishname_row;
    int bp_unit_row;
    int bp_num_row;
    int bp_price_row;
    int bp_amount_row;
    int bp_disdetail_row;
    int bp_total_row;

    int bp_dishname_len;
    int bp_unit_len;
    int bp_num_len;
    int bp_price_len;
    int bp_amount_len;
    int bp_disdetail_len;
    int bp_total_len;

    public_ins_maker print_format;
    int rowno;

    fexpandmain_model_sqltablemodel model_cache;
};

#endif // PUBLIC_PRINTBY_CH_BILLNO_H
