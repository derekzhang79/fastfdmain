#ifndef FEXPANDMAIN_MODEL_SQLTABLEMODEL_H
#define FEXPANDMAIN_MODEL_SQLTABLEMODEL_H
#include "lds_model_sqltablemodel.h"
#include <QDateTime>
#include "lds.h"
#include <QElapsedTimer>
#include "lds_thread.h"
#include "lds_query.h"

class QTableView;
class  vch_membernoData{
public:
    enum chooseFlag{
        choose_switch = 0,
        choose_local,
        choose_yun,
    };
public:
    vch_membernoData();
    void init();
    void init0(const QString &vch_memberno, const QString &vch_member, const QString &ch_member_belong_state, const QString &ch_favourable, int int_discount, const QString &ch_pricekind, const QString &ch_attribute, const QString &vch_typename);
    bool init_by_memberno(QWidget *parent, const QString &memberno, const QString &ch_member_belong_state);
    QString desc();
    QString color();
    QString paymodeno();
    chooseFlag mutil_choose();
    void print();
    bool isyun() const;
    bool is_m_price() const;
    bool is_m_discount() const;
    double mprice(double org_price, double m_price) const;
    QString vch_memberno;
    QString vch_member;
    QString ch_member_belong_state;
    QString ch_favourable;//1会员价, 2固定折扣
    int int_discount;
    QString ch_pricekind;
    QString ch_attribute;//1：优惠卡、2充值卡
    QString vch_typename;
};

struct num_backData{
    qlonglong int_flowid;
    float num_back_add;

    QString vch_back_operid;
    QDateTime dt_back_operdate;
    QString vch_reason;

    QString vch_print_memo;
    QString vch_print_memo_remain;
    void print() const;
};
struct int_discountData {
    int int_discount;
    QString vch_dis_operid;
    QDateTime dt_dis_operdate;
};
class fexpandmain_keepData {
public:
    fexpandmain_keepData();
    void print() const;
    double dish_total() const;
    double dish_num() const;

    double consum;//                    (num_num-num_back)*num_price + num_price_add, num_price_add)
    double discount;//                  (num_num-num_back)*num_price*(1-int_discount*0.01)
    double present;//                   if(ch_presentflag = 'Y', (num_num-num_back)*num_price, 0)
    double num;//                  num_num-num_back
    double num_service;
    double num_rate;///这边会厨显1-1=-1.19209e-07的情况，以后注意
    double num_price_add;//num_price_add

public:
    fexpandmain_keepData operator-(const fexpandmain_keepData &other);
    fexpandmain_keepData operator+(const fexpandmain_keepData &other);
    inline void operator+=(const fexpandmain_keepData &other){
        *this = *this + other;
    }
    inline void operator-=(const fexpandmain_keepData &other){
        *this = *this - other;
    }
};


class fexpandmain_model_sqltablemodel : public lds_model_sqltablemodel{
    Q_OBJECT
public:
    fexpandmain_model_sqltablemodel(QObject *parent = 0);
    virtual ~fexpandmain_model_sqltablemodel();

    //num_x 是-1 是退单， 1是正常
    static QString get_value_vch_print_memo(const QMap<QString, int> &map);//010110002154
    static QMap<QString, int> get_map_vch_print_memo(const QString &vch_print_memo);//map<no, num>
    static QString get_desc_vch_print_memo(const QString &vch_print_memo, int num_x = 1);//map<no, num>
    static QString get_desc_vch_print_memo(const QString &vch_print_memo, int num_x, const lds_model_sqltablemodel *m_cache);//map<no, num>
    static QString get_desc_vch_print_memo(const QList<QPair<QString, int> > &cook_pair_list, float total, int num_x = 1);//QList<QPair<name, num> >
    static QString get_desc_vch_print_memo(const QMap<QString, int> &map, int num_x, const lds_model_sqltablemodel *m_cache);
    static float get_num_price_add_vch_print_memo(const QString &vch_print_memo) ;//12.6
    static float get_num_price_add_vch_print_memo(const QString &vch_print_memo, const lds_model_sqltablemodel *m_cache) ;//12.6
    static float get_num_price_add_vch_print_memo(const QMap<QString, int> &map, const lds_model_sqltablemodel *m_cache) ;//12.6

    static bool can_m_price(const fexpandmain_model_sqltablemodel *m, int row, QString *errstring = 0);
    //菜品描述
    QString get_dish_desc(int row) const;

    //cey_bt_cp_memo
    QString get_vch_cp_memo_by_map(const QString &ch_no) const;
    QString get_num_add_price_by_map(const QString &ch_no) const ;

    //vch_print_memo
    QString get_desc_vch_print_memo_by_map(const QMap<QString, int> &map) const;//加辣X1 = 15
    QString get_desc_vch_print_memo_by_map(const QString &vch_print_memo) const;//加辣X1 = 15
    float get_num_price_add_vch_print_memo_by_map(const QMap<QString, int> &map) const;//12.6
    float get_num_price_add_vch_print_memo_by_memo(const QString &vch_print_memo) const;//12.6
    //cey_bt_dish
    QString get_vch_dishname_by_map(const QString &ch_dishno) const;
    QString get_ch_curprice_by_map(const QString &ch_dishno) const;
    QString get_ch_discount_by_map(const QString &ch_dishno) const;
    double get_num_m_price_by_map(const QString &ch_dishno) const;
    QString get_ch_serviceflag_by_map(const QString &ch_dishno) const;
    QString get_item_flag_by_map(const QString &ch_dishno) const;
    QVariant get_cey_bt_dish_value(const QString &ch_dishno, const QString &valuename) const;

    //cey_bt_unit
    QVariant get_cey_bt_unit_value(const QString &ch_unitno, const QString &valuename) const;
    //cey_bt_table_type
    QString get_sevice_mode_by_map(const QString &ch_tableno) const;
    double get_sevice_int_rate_by_map(const QString &ch_tableno) const;
    QString get_cey_bt_table_type_value(const QString &ch_typeno, const QString &valuename) const;
    //cey_bt_table
    QString get_table_ch_typeno_by_map(const QString &ch_tableno) const;
    QString get_cey_bt_table_value(const QString &ch_tableno, const QString &valuename) const;
    //cey_u_table
    QString get_cey_u_table_value(const QString &ch_billno, const QString &valuename) const;
    //cey_bt_paymode
    QVariant get_cey_bt_paymode_value(const QString &ch_paymodeno, const QString &valuename) const;
    //cey_sys_parameter
    QVariant get_cey_sys_parameter_value(const QString &vch_parameter, const QVariant &def_value = QVariant()) const;


    //判断
    bool can_discount(int row, QString *errstring = 0) const;
    bool can_m_price(int row, QString *errstring = 0) const;
    bool can_change_price(int row, QString *errstring = 0) const;
    bool can_change_num(int row, QString *errstring = 0) const;
    bool can_change_dish(int row, QString *errstring = 0) const;//落单前
    bool can_org_dish_append(int row, QString *errstring = 0) const;
    bool can_present(int row, QString *errstring = 0) const;
    bool can_cook(int row, QString *errstring = 0) const;
    bool can_cook_pop(const QString &ch_dishno)const ;
    bool can_discount_limit(int int_discount, const QString &title, QString &discount_desc) const;
    //
    void update_num_price_add_2_P(const int &row);
    void update_num_num_Y(float num_num_P_old, const int &rowP);
    void update_vch_print_memo_Y(float num_num_old_P, const int &rowP);
    void update_vch_print_memo_N(float num_num_old_N, const int &row_N);

    //
protected slots:
     bool select();
};

///双屏同步数据
///select时会先清空所有的keepdata,并且在from db的同时重新生成keepdata
///remove keepdata +- 一行的数据
///
/// model当cur_billno的值和数据库里的ch_billno不同时,只计算cur_billno对应的keepdata
///
/// 数据库里的ch_payno不为空的时候,不计算对应的keepdata
class fexpandmain_model_sqltablemodel_data : public fexpandmain_model_sqltablemodel{
    Q_OBJECT
public:
    fexpandmain_model_sqltablemodel_data(QObject *parent = 0);

    ///数量和金额的实时修改-------------------------------------------begin

    void keepdata_disabled_begin_rowcount(int rowbegin = -1);
    void keepdata_enabled(bool f = true);
    void keepdata_submit(int  rowcount = -1);//会更具dd_lock的值来同步副屏数据，插入
    void keepdata_Y_dish_fold(QTableView *tableview, int rowBegin);

    //model 允许存在多个ch_billn在commit的时候,会按照主ch_billno来提交
    QString cur_ch_billno() const;
    QString cur_ch_tableno() const;
    /// 副屏
public:
    bool insertRow(int arow, const QModelIndex &aparent=QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
protected:
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    double model_expression_value(int row, QString expression, bool num_price_is_m = false) const;

    virtual void senddd_update_insert(int row,const QString &udp_request_type, float dish_num_old = 0, const QString &SENDD_DATA_FROM_DATABASE = "");
    virtual void senddd(const lds::DOUBLE_DATA  &data, float dish_num_old = 0, const QString &SENDD_DATA_FROM_DATABASE = "");
    virtual void emit_num_total_change_DD(const QString &num_label , const QString &total_label) const;
public:
    void emit_num_total_change_label(float n, float t) const;
    void emit_num_total_change_label_pay(float n, float t) const;

    QString dish_total_str(float t) const;
    QString dish_total_str(float t, const QString &prefix) const;
    QString dish_num_str(float n) const;
    QString dish_num_str(float n, const QString &prefix) const;


protected:
    bool kd_enable;//双屏锁
    int kd_enable_row_begin;
    void get_keep_data(int row, fexpandmain_keepData &kdata);
    static void get_keep_data_pay_by_isMPrice_intDiscount(const fexpandmain_model_sqltablemodel_data *tablemodel, int row, fexpandmain_keepData &kdata, bool pay_is_m_price = false, int pay_int_discount = -1/*-1 not run*/);
    static QString expression_reslove(const fexpandmain_model_sqltablemodel_data *tablemodel, QString expression, const QString &min_num_m_price/*empty not run*/, int pay_int_discount = -1/*-1 not run*/);
    static const QString SENDD_DATA_FROM_DATABASE;
public:
    static fexpandmain_keepData get_keep_data_pay_by_isMPrice_intDiscount(const fexpandmain_model_sqltablemodel_data *tablemodel, bool pay_is_m_price = false, int pay_int_discount = -1/*-1 not run*/);

    //副屏
public slots:
    bool select(const QString &ch_billno, QWidget *member_parent, bool skip_member = false);
    void update_ch_billno(const QString &ch_billno, const QList<int> rowListAsc);
    void update_ch_billno(const QString &ch_billno);
public:
    ///提交数据库
    bool try_commit_data(QString &errstring, bool kuaican_quit = false, bool only_update = false) const;
    bool try_update_data(QString &errstring) const;
    QString msg_mid_by_N(const QString &msg, int pos, int length, bool show_dot_when_not_complete = true);
signals:
    void signals_num_total_change(QString, QString) const;
public:
    const fexpandmain_keepData& getKeepdata() const;//汇总信息
    fexpandmain_keepData getKeepdata(int row) const;//汇总信息
protected:
    fexpandmain_keepData _keepdata;//汇总信息
protected:
    QString _ch_billno;
    //
public:
    vch_membernoData  vch_membernod;
    bool vch_membernod_m_update(QString &errstring);
protected:
    //
    QList<int> n_m_filter_index_list;
    QStringList n_m_filter_name_list;

public:
    ///数量和金额的实时修改-------------------------------------------end
    ///ch_dishno_set_row_map
    /// 底部
public:
    QSet<int> ch_dishno_set_row_map_GET(const QString &ch_dishno) const;
    QSet<int> ch_dishno_set_row_map_GET(int row) const;
    const QMap<QString, QSet<int> > &ch_dishno_set_row_map_GET() const;
    //    bool can_change_num(int row) const ;
    void ch_dishno_set_row_map_DEL(int row, int count);
protected:
    void ch_dishno_set_row_map_PRINT();
    void ch_dishno_set_row_map_INSERT(int row);
    void ch_dishno_set_row_map_UPDATE(int row);
    void ch_dishno_set_row_map_CLEAR();
    QMap<QString, QSet<int> >  ch_dishno_set_row_map_get;
    //
public:
    void row_fold_map_RELEASE(QTableView *tableView);
    void row_fold_map_FOLD(QTableView *tableView, const QModelIndex &index);
    QMap<int/*row*/, bool /*isfold*/> row_fold_map;
    ///数量
protected:
    void ch_dishno_dish_num_map_UPDATE(const lds::DOUBLE_DATA &data, double off/*可以为负数*/ , const QString &SENDD_DATA_FROM_DATABASE);
    void ch_dishno_dish_num_map_CLEAR(const QString &SENDD_DATA_FROM_DATABASE);//简单的清空
    void ch_dishno_dish_num_map_PRINT();
    QMap<QString, double> ch_dishno_dish_num_map;
public:
    const QMap<QString, double> &ch_dishno_dish_num_map_GET() const;
    void print_dish_info();
};

class fexpandmain_model_sqltablemodel_dd : public fexpandmain_model_sqltablemodel_data{
    Q_OBJECT
public:
    fexpandmain_model_sqltablemodel_dd(QObject *parent = 0);
protected:
    void senddd(const lds::DOUBLE_DATA  &data, float dish_num_old = 0, const QString &SENDD_DATA_FROM_DATABASE = "");
    void senddd_update_insert(int row,const QString &udp_request_type, float dish_num_old = 0, const QString &SENDD_DATA_FROM_DATABASE = "");
    void emit_num_total_change_DD(const QString &num_label , const QString &total_label) const;
};

#endif // FEXPANDMAIN_MODEL_SQLTABLEMODEL_H
