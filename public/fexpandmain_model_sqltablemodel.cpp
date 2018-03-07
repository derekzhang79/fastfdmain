#include "fexpandmain_model_sqltablemodel.h"
#include "lds_query.h"
#include <QSqlError>
#include <QtDebug>
#include "lds.h"
#include "frontviewdialog_virtual.h"
#include "n_func.h"
#include "public_sql.h"
#include <QTableView>
#include <QHeaderView>
#include <QFile>
#include "fexpandmain_2.h"
#include "lds_expression2rpn.h"
#include "w_rpt_report_frame.h"

const QString fexpandmain_model_sqltablemodel_data::SENDD_DATA_FROM_DATABASE = "SENDD_DATA_FROM_DATABASE";

fexpandmain_model_sqltablemodel::fexpandmain_model_sqltablemodel(QObject *parent)
    : lds_model_sqltablemodel(parent)
{
}

fexpandmain_model_sqltablemodel::~fexpandmain_model_sqltablemodel()
{

}

QString fexpandmain_model_sqltablemodel::get_dish_desc(int row) const
{
    //    QString vch_print_memo_desc = get_desc_vch_print_memo_by_map(model_data(row, "vch_print_memo").toString());
    QString text = "";
    if("P"== model_data(row, "ch_suitflag")) text.append(tr("主")+"  ");
    if("Y"== model_data(row, "ch_suitflag")) text.append(tr("子")+"  ");
    //    if(!vch_print_memo_desc.isEmpty()) text += vch_print_memo_desc+"  ";
    if("Y"== model_data(row, "ch_presentflag").toString())text.append(tr("赠")+"  ");
    if("Y"== model_data(row, "ch_specialflag").toString())text.append(tr("特")+"  ");
    if("Y"== model_data(row, "ch_waidai_flag").toString())text.append(tr("外带")+"  ");
    if(100 > model_data(row, "int_discount").toInt()) {
        if("1" == public_sql::discount_off_mode(this)) {//折让
            text.append(tr("折扣") + "(" + QString::number(100 - model_data(row, "int_discount").toInt())+"%)  ");
        } else {
            text.append(tr("折扣") + "(" + QString::number(model_data(row, "int_discount").toInt())+"%)  ");
        }
    }

    return text;
}

QString fexpandmain_model_sqltablemodel::get_value_vch_print_memo(const QMap<QString, int> &map)
{
    QString value = "";// not null
    QMapIterator<QString, int>mapitr(map) ;
    while(mapitr.hasNext()) {
        mapitr.next();
        int count = mapitr.value();
        for(int k = 0; k < count; k ++) {
            value+=mapitr.key();
        }
    }
    return value;
}

QMap<QString, int> fexpandmain_model_sqltablemodel::get_map_vch_print_memo(const QString &vch_print_memo)
{
    QMap<QString, int> no_num_map;
    for(int index = 0, indexcount = vch_print_memo.count(); index < indexcount; index += 3) {
        QString ch_no = vch_print_memo.mid(index, 3);
        no_num_map.insert(ch_no, no_num_map.value(ch_no, 0) + 1);
    }
    return no_num_map;
}

QString fexpandmain_model_sqltablemodel::get_desc_vch_print_memo(const QString &vch_print_memo, int num_x)
{
    lds_model_sqltablemodel m;
    return get_desc_vch_print_memo(vch_print_memo, num_x, &m);
}

QString fexpandmain_model_sqltablemodel::get_desc_vch_print_memo(const QString &vch_print_memo, int num_x, const lds_model_sqltablemodel *m_cache)
{
    QMap<QString, int> map = get_map_vch_print_memo(vch_print_memo);

    return get_desc_vch_print_memo(map, num_x, m_cache);
}

QString fexpandmain_model_sqltablemodel::get_desc_vch_print_memo(const QList<QPair<QString, int> > &cook_pair_list, float total, int num_x)
{
    //
    // 牛排(10)x2, 盐x3,白菜(5)
    QString vch_print_memo_desc = "";// not null
    for(int k = 0; k < cook_pair_list.count(); k ++) {
        if(cook_pair_list[k].second == 1) {
            vch_print_memo_desc += QString("%1,").arg(cook_pair_list[k].first);
        } else {
            vch_print_memo_desc += QString("%1x%2,").arg(cook_pair_list[k].first).arg(cook_pair_list[k].second * num_x);
        }
    }

    if(!vch_print_memo_desc.isEmpty()) vch_print_memo_desc.chop(1);
    return vch_print_memo_desc;
}

QString fexpandmain_model_sqltablemodel::get_desc_vch_print_memo(const QMap<QString, int> &map, int num_x, const lds_model_sqltablemodel *m_cache)
{
    QList<QPair<QString, int> > cook_pair_list;

    float total = 0;
    double num_add_price = 0;
    QString vch_cp_memo;
    QMapIterator<QString, int>mapitr(map);
    while(mapitr.hasNext()) {
        mapitr.next();
        vch_cp_memo = m_cache->tablename_kvmap_value("cey_bt_cp_memo", mapitr.key(), "ch_no", "vch_cp_memo").toString();
        num_add_price = m_cache->tablename_kvmap_value("cey_bt_cp_memo", mapitr.key(), "ch_no", "num_add_price").toDouble();


        if(0 != num_add_price) {
            cook_pair_list.append(QPair<QString, int>(vch_cp_memo+"("+QString::number(num_add_price)+")"
                                                        , mapitr.value()));
        } else {
            cook_pair_list.append(QPair<QString, int>(vch_cp_memo
                                                        , mapitr.value()));
        }
        total += num_add_price * mapitr.value();
    }
    return get_desc_vch_print_memo(cook_pair_list, total, num_x);
}

float fexpandmain_model_sqltablemodel::get_num_price_add_vch_print_memo(const QString &vch_print_memo)
{
    lds_model_sqltablemodel m;
    return get_num_price_add_vch_print_memo(vch_print_memo, &m);
}

float fexpandmain_model_sqltablemodel::get_num_price_add_vch_print_memo(const QString &vch_print_memo, const lds_model_sqltablemodel *m_cache)
{
    QMap<QString, int>map =  get_map_vch_print_memo(vch_print_memo);
    return get_num_price_add_vch_print_memo(map, m_cache);
}

float fexpandmain_model_sqltablemodel::get_num_price_add_vch_print_memo(const QMap<QString, int> &map, const lds_model_sqltablemodel *m_cache)
{
    float total = 0;
    QMapIterator<QString, int>mapitr(map);
    while(mapitr.hasNext()) {
        mapitr.next();
        total += m_cache->tablename_kvmap_value("cey_bt_cp_memo", mapitr.key(), "ch_no", "num_add_price").toFloat() * mapitr.value();
    }
    return total;
}

bool fexpandmain_model_sqltablemodel::can_m_price(const fexpandmain_model_sqltablemodel *m, int row, QString *errstring)
{
    /// 会员价：～赠送、～子套菜,   ~ 特价
    if("Y" == m->model_data(row, "ch_presentflag")) {
        if(errstring) *errstring = "赠送，不能修改会员特价";
        return false;
    }
    if("Y" == m->model_data(row, "ch_suitflag")) {
        if(errstring) *errstring = "子套菜，不能修改会员特价";
        return false;
    }
    if("Y" == m->model_data(row, "ch_specialflag")) {
        if(errstring) *errstring = "特价菜，不能修改会员特价";
        return false;
    }
    return true;
}

QString fexpandmain_model_sqltablemodel::get_desc_vch_print_memo_by_map(const QMap<QString, int> &map) const
{
    return get_desc_vch_print_memo(map, 1, this);
}

QString fexpandmain_model_sqltablemodel::get_desc_vch_print_memo_by_map(const QString &vch_print_memo) const
{
    return get_desc_vch_print_memo(vch_print_memo, 1, this);
}

float fexpandmain_model_sqltablemodel::get_num_price_add_vch_print_memo_by_map(const QMap<QString, int> &map) const
{
    return get_num_price_add_vch_print_memo(map, this);
}

float fexpandmain_model_sqltablemodel::get_num_price_add_vch_print_memo_by_memo(const QString &vch_print_memo) const
{
    return get_num_price_add_vch_print_memo_by_map(get_map_vch_print_memo(vch_print_memo));
}

QString fexpandmain_model_sqltablemodel::get_vch_dishname_by_map(const QString &ch_dishno) const
{
    return this->tablename_kvmap_value("cey_bt_dish", ch_dishno, "ch_dishno" ,"vch_dishname").toString();
}

QString fexpandmain_model_sqltablemodel::get_ch_curprice_by_map(const QString &ch_dishno) const
{
    return this->tablename_kvmap_value("cey_bt_dish", ch_dishno, "ch_dishno" ,"ch_curprice").toString();
}

QString fexpandmain_model_sqltablemodel::get_ch_discount_by_map(const QString &ch_dishno) const
{
    return this->tablename_kvmap_value("cey_bt_dish", ch_dishno, "ch_dishno" ,"ch_discount").toString();
}

double fexpandmain_model_sqltablemodel::get_num_m_price_by_map(const QString &ch_dishno) const
{
    return this->tablename_kvmap_value("cey_bt_dish", ch_dishno, "ch_dishno" ,"num_m_price").toDouble();
}

QString fexpandmain_model_sqltablemodel::get_ch_serviceflag_by_map(const QString &ch_dishno) const
{
    return this->tablename_kvmap_value("cey_bt_dish", ch_dishno, "ch_dishno" ,"ch_serviceflag").toString();
}

QString fexpandmain_model_sqltablemodel::get_item_flag_by_map(const QString &ch_dishno) const
{
    return this->tablename_kvmap_value("cey_bt_dish", ch_dishno, "ch_dishno" ,"item_flag").toString();
}

QVariant fexpandmain_model_sqltablemodel::get_cey_bt_dish_value(const QString &ch_dishno, const QString &valuename) const
{
    return this->tablename_kvmap_value("cey_bt_dish", ch_dishno, "ch_dishno", valuename);
}

QVariant fexpandmain_model_sqltablemodel::get_cey_bt_unit_value(const QString &ch_unitno, const QString &valuename) const
{
    return this->tablename_kvmap_value("cey_bt_unit", ch_unitno, "ch_unitno", valuename);
}

QString fexpandmain_model_sqltablemodel::get_sevice_mode_by_map(const QString &ch_tableno) const
{
    QString ch_typeno = get_table_ch_typeno_by_map(ch_tableno);
    return this->tablename_kvmap_value("cey_bt_table_type", ch_typeno, "ch_typeno" ,"ch_service_mode").toString();
}

double fexpandmain_model_sqltablemodel::get_sevice_int_rate_by_map(const QString &ch_tableno) const
{
    QString ch_typeno = get_table_ch_typeno_by_map(ch_tableno);
    return this->tablename_kvmap_value("cey_bt_table_type", ch_typeno, "ch_typeno" ,"int_rate").toDouble();
}

QString fexpandmain_model_sqltablemodel::get_cey_bt_table_type_value(const QString &ch_typeno, const QString &valuename) const
{
    return this->tablename_kvmap_value("cey_bt_table_type", ch_typeno, "ch_typeno" ,valuename).toString();
}

QString fexpandmain_model_sqltablemodel::get_cey_bt_table_value(const QString &ch_tableno, const QString &valuename) const
{
    return this->tablename_kvmap_value("cey_bt_table", ch_tableno, "ch_tableno" , valuename).toString();
}

QString fexpandmain_model_sqltablemodel::get_table_ch_typeno_by_map(const QString &ch_tableno) const
{
    return this->tablename_kvmap_value("cey_bt_table", ch_tableno, "ch_tableno" ,"ch_typeno").toString();
}

QString fexpandmain_model_sqltablemodel::get_cey_u_table_value(const QString &ch_billno, const QString &valuename) const
{
    return this->tablename_kvmap_value("cey_u_table", ch_billno, "ch_billno" ,valuename).toString();
}

QVariant fexpandmain_model_sqltablemodel::get_cey_bt_paymode_value(const QString &ch_paymodeno, const QString &valuename) const
{
    return this->tablename_kvmap_value("cey_bt_paymode", ch_paymodeno, "ch_paymodeno" ,valuename);
}

QVariant fexpandmain_model_sqltablemodel::get_cey_sys_parameter_value(const QString &vch_parameter, const QVariant &def_value) const
{
    return this->tablename_kvmap_value("cey_sys_parameter", vch_parameter, "vch_parameter" ,"vch_value", "", def_value);
}

QString fexpandmain_model_sqltablemodel::get_vch_cp_memo_by_map(const QString &ch_no) const
{
    return this->tablename_kvmap_value("cey_bt_cp_memo", ch_no, "ch_no" ,"vch_cp_memo").toString();
}

QString fexpandmain_model_sqltablemodel::get_num_add_price_by_map(const QString &ch_no) const
{
    return this->tablename_kvmap_value("cey_bt_cp_memo", ch_no, "ch_no" ,"num_add_price").toString();
}

fexpandmain_model_sqltablemodel_data::fexpandmain_model_sqltablemodel_data(QObject *parent)
    : fexpandmain_model_sqltablemodel(parent)
{
    kd_enable_row_begin = 0;
    kd_enable = true;
    n_m_filter_name_list
            << "num_num"
            << "num_back"
            << "num_price"
            << "num_price_add"
            << "int_discount"
            << "ch_presentflag"
            << "int_rate"
            << "ch_suitflag"
            << "ch_specialflag"

            << "vch_print_memo";
}

bool fexpandmain_model_sqltablemodel_data::insertRow(int arow, const QModelIndex &aparent)
{
    ///
    lds_model_sqltablemodel::insertRow(arow, aparent);
    ///

    //    //发送副屏数据
    //    if(kd_enable) {
    //        lds::DOUBLE_DATA ddata;
    //        ddata.udpKey = lds::udp_insert_request;
    //        ddata.row = arow;
    //        senddd(ddata);
    //    }
    return true;
}

bool fexpandmain_model_sqltablemodel_data::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(role != Qt::EditRole) {
        return lds_model_sqltablemodel::setData(index, value, role);
    }
    fexpandmain_keepData _keepdata0, _keepdata1;

    int row = index.row();
    bool f;
    lds_bool_default_false need2update;
    float dish_num_old = 0;
    ///
    if(n_m_filter_index_list.isEmpty()) {
        foreach(const QString &f, n_m_filter_name_list)
            n_m_filter_index_list.append(this->fieldIndex(f));
    }
    need2update = n_m_filter_index_list.contains(index.column())
            && kd_enable;

    ///
    if(need2update) {
        get_keep_data(row, _keepdata0);
        dish_num_old = model_data_NIBOLAN(row, "num_num-num_back");
    }


    ///
    f = true;
    if(this->fieldIndex("num_num") == index.column()) {
        f = lds_model_sqltablemodel::setData(
                    index, lds::f_get_decimal(value.toDouble(), get_cey_sys_parameter_value("bp_fmt_num_dec", 3).toInt()));
    } else if(this->fieldIndex("num_back") == index.column()) {
        f = lds_model_sqltablemodel::setData(
                    index, lds::f_get_decimal(value.toDouble(), get_cey_sys_parameter_value("bp_fmt_num_dec", 3).toInt()));
    } else if(this->fieldIndex("num_price") == index.column()) {
        f = lds_model_sqltablemodel::setData(
                    index, lds::f_get_decimal(value.toDouble(), get_cey_sys_parameter_value("bp_fmt_price_dec", 2).toInt()));
    } else if(this->fieldIndex("num_price_org") == index.column()) {
        f = lds_model_sqltablemodel::setData(
                    index, lds::f_get_decimal(value.toDouble(), get_cey_sys_parameter_value("bp_fmt_price_dec", 2).toInt()));
    } else if(this->fieldIndex("num_price_add") == index.column()) {
        f = lds_model_sqltablemodel::setData(
                    index, lds::f_get_decimal(value.toDouble(), get_cey_sys_parameter_value("bp_fmt_price_dec", 2).toInt()));
    } else {
        f = lds_model_sqltablemodel::setData(index, value, role);
    }
    ///
    if(need2update) {
        get_keep_data(row, _keepdata1);

        fexpandmain_keepData k = _keepdata1 - _keepdata0;
        _keepdata += k;
        emit_num_total_change_label(_keepdata.dish_num(), _keepdata.dish_total());
        senddd_update_insert(row, lds::udp_update_request, dish_num_old);
    }
    return f;
}

bool fexpandmain_model_sqltablemodel_data::removeRows(int row, int count, const QModelIndex &parent)
{
    fexpandmain_keepData _keepdata0;
    for(int k = 0; k < count; k ++) {
        int delRow = row + count - k - 1;

        fexpandmain_keepData d;
        get_keep_data(delRow, d);
        //
        _keepdata0 += d;
        //
        if(kd_enable)
        {
            ///
            lds::DOUBLE_DATA ddata;
            ddata.udpKey = lds::udp_delete_request;
            ddata.row = delRow;
            ddata.count = 1;
            ddata.items.ch_dishno = model_data(delRow, "ch_dishno").toString();
            ddata.items.dish_num = QString::number(model_data_NIBOLAN(delRow, "num_num-num_back"), 'f', get_cey_sys_parameter_value("bp_fmt_num_dec", 3).toInt());
            senddd(ddata);
        }
    }
    if(kd_enable)
    {
        _keepdata -= _keepdata0;
    }
    ///
    bool f = lds_model_sqltablemodel::removeRows(row, count, parent);
    ///
    if(kd_enable)
    {
        emit_num_total_change_label(_keepdata.dish_num(), _keepdata.dish_total());
    }
    return f;
}

void fexpandmain_model_sqltablemodel_data::keepdata_disabled_begin_rowcount(int rowbegin)
{
    if(rowbegin != -1) {
        kd_enable_row_begin =  rowbegin;
    } else {
        kd_enable_row_begin = this->rowCount();
    }
    kd_enable = false;
}

void fexpandmain_model_sqltablemodel_data::keepdata_enabled(bool f)
{
    kd_enable = f;
}

void fexpandmain_model_sqltablemodel_data::keepdata_submit(int rowcount)
{
    //    QElapsedTimer etimer;etimer.start();
    if(rowcount == -1) {
        rowcount = this->rowCount();
    }
    keepdata_enabled();

    for(int row = kd_enable_row_begin; row < rowcount; row++) {
        fexpandmain_keepData d;
        get_keep_data(row, d);
        _keepdata += d;
        //发送副屏数据
        senddd_update_insert(row, lds::udp_insert_request);
    }

    emit_num_total_change_label(_keepdata.dish_num(), _keepdata.dish_total());
}

void fexpandmain_model_sqltablemodel_data::keepdata_Y_dish_fold(QTableView *tableview, int rowBegin)
{
    QString ch_suitflag;
    for(;rowBegin < this->rowCount(); rowBegin ++) {
        ch_suitflag = this->model_data(rowBegin, "ch_suitflag").toString();
        if("P" == ch_suitflag) {
            this->row_fold_map.insert(rowBegin, true);
        }

        if("Y" == ch_suitflag) {
            tableview->hideRow(rowBegin);
        }
    }
}

void fexpandmain_model_sqltablemodel_data::update_ch_billno(const QString &ch_billno, const QList<int> rowListAsc)
{
    _ch_billno = ch_billno;

    _keepdata = fexpandmain_keepData();
    for(int row = 0, rowcount = this->rowCount(); row < rowcount; row++) {
        if(false == rowListAsc.contains(row)) continue;

        model_data_set(row, "ch_billno", ch_billno);
        fexpandmain_keepData k;//汇总信息
        get_keep_data(row, k);
        _keepdata += k;
    }
}

void fexpandmain_model_sqltablemodel_data::update_ch_billno(const QString &ch_billno)
{
    _ch_billno = ch_billno;
    for(int row = 0, rowcount = this->rowCount(); row < rowcount; row++) {
        model_data_set(row, "ch_billno", ch_billno);
    }
}

QString fexpandmain_model_sqltablemodel_data::cur_ch_billno() const
{
    return _ch_billno;
}

QString fexpandmain_model_sqltablemodel_data::cur_ch_tableno() const
{
    lds_query query;
    query.execSelect(QString("SELECT ch_tableno from cey_u_table where ch_billno = '%1' ")
                     .arg(_ch_billno));
    query.next();
    return query.recordValue(0).toString();
}

void fexpandmain_model_sqltablemodel_data::senddd_update_insert(int row, const QString &udp_request_type, float dish_num_old, const QString &SENDD_DATA_FROM_DATABASE)
{
}

void fexpandmain_model_sqltablemodel_data::senddd(const lds::DOUBLE_DATA &data, float dish_num_old, const QString &SENDD_DATA_FROM_DATABASE)
{
    if(SENDD_DATA_FROM_DATABASE == fexpandmain_model_sqltablemodel_dd::SENDD_DATA_FROM_DATABASE) {
        if(data.udpKey == lds::udp_delete_all_request) {//全部清空
            ///点菜记录初始化
            _keepdata = fexpandmain_keepData();
            ///菜品编码<可追加的set_row>
            ch_dishno_set_row_map_CLEAR();
            ///菜品编码<数量>
            ch_dishno_dish_num_map_CLEAR(SENDD_DATA_FROM_DATABASE);
        }
        return;
    }
}

double fexpandmain_model_sqltablemodel_data::model_expression_value(int row,  QString expression, bool num_price_is_m) const
{
    QString ch_dishno = model_data(row, "ch_dishno").toString();
    QString num_m_price = QString::number(get_num_m_price_by_map(ch_dishno));
    if(true == num_price_is_m) {
        expression = model_expression_replace(expression, "num_price", num_m_price);
    }
    return model_data_IF(row, expression);
}

void fexpandmain_model_sqltablemodel_data::get_keep_data(int row, fexpandmain_keepData &kdata)
{
    get_keep_data_pay_by_isMPrice_intDiscount(this, row, kdata);
}

void fexpandmain_model_sqltablemodel_data::get_keep_data_pay_by_isMPrice_intDiscount(const fexpandmain_model_sqltablemodel_data *tablemodel, int row, fexpandmain_keepData &kdata, bool pay_is_m_price, int pay_int_discount)
{
    QString ch_dishno = tablemodel->model_data(row, "ch_dishno").toString();
    QString min_num_m_price;
    if(false == tablemodel->can_discount(row)) {
        pay_int_discount = -1;
    }

    if(false == tablemodel->can_m_price(row)) {
        pay_is_m_price = false;
    }
    if(pay_is_m_price) {
        min_num_m_price = QString::number(qMin(tablemodel->model_data(row, "num_price").toDouble(), tablemodel->get_num_m_price_by_map(ch_dishno)));
    }

    kdata.num_price_add = tablemodel->model_data_IF(row, expression_reslove(tablemodel, "num_price_add", min_num_m_price, pay_int_discount));
    kdata.consum = tablemodel->model_data_IF(row, expression_reslove(tablemodel, "(num_num-num_back)*num_price", min_num_m_price, pay_int_discount));
    kdata.discount = kdata.consum * tablemodel->model_data_IF(row, expression_reslove(tablemodel, "(1-int_discount*0.01)", min_num_m_price, pay_int_discount));
    kdata.present = kdata.consum * tablemodel->model_data_IF(row, expression_reslove(tablemodel, "if(ch_presentflag = 'Y', 1, 0)", min_num_m_price, pay_int_discount));
    kdata.num = tablemodel->model_data_IF(row, expression_reslove(tablemodel, "num_num-num_back", min_num_m_price, pay_int_discount));

    //P
    const double P =  kdata.consum - kdata.discount - kdata.present + kdata.num_price_add;//做法算到服务费和税收里
    kdata.consum  += kdata.num_price_add;
    //sc
    double sc = 0;
    QString ch_tableno =  tablemodel->get_cey_u_table_value(tablemodel->cur_ch_billno(), "ch_tableno");
    /// 折扣服务费: ch_service_mode = 4, ch_serviceflag = 'Y'
    if(tablemodel->get_sevice_mode_by_map(ch_tableno).toInt() == 4//菜品消费服务率
            && tablemodel->get_ch_serviceflag_by_map(ch_dishno) == "Y"//ch_serviceflag = 'Y'
            ) {
        sc = tablemodel->get_sevice_int_rate_by_map(ch_tableno) * 0.01;
    }
    //tax
    double tax = tablemodel->model_data(row, "int_rate").toDouble() * 0.01;
    //
    int tax_service_form_value = tablemodel->get_cey_sys_parameter_value("tax_service_form_value", 0).toInt();
    switch(tax_service_form_value) {
    case 1://含税,不含服务费
        kdata.num_rate = P * tax / (tax + 1);
        kdata.num_service = P *sc / (tax + 1);
        break;
    case 2://不含税,含服务费
        kdata.num_service = P * sc;
        kdata.num_rate = P * (sc + 1) * tax;
        break;
    case 3://含税,含服务费
        kdata.num_service = P * sc/ ((1 + sc ) *tax +1);
        kdata.num_rate =  P * tax * (1 + sc) / ((1 + sc ) *tax +1);
        break;
    default://不含税,不含服务费
        kdata.num_rate = P *tax;
        kdata.num_service = P * sc;
        break;
    }

    //    kdata.print();

    //    xx
    //            SCIN taxEx    100 10 11 121
    //            SCIN taxIn     111  10 11 121   100 * (sc + 1)*tax + 100 = 111,     qiu 100*sc、 100（sc + 1） *tax
    //            SCEx taxEx    100 10 10 120
    //            SCEx taxIn     110 10 10 120
}

QString fexpandmain_model_sqltablemodel_data::expression_reslove(const fexpandmain_model_sqltablemodel_data *tablemodel, QString expression, const QString &min_num_m_price, int pay_int_discount)
{
    if(min_num_m_price.length() > 0) {
        expression = tablemodel->model_expression_replace(expression, "num_price", min_num_m_price);
    }
    if(-1 != pay_int_discount) {
        expression = tablemodel->model_expression_replace(expression, "int_discount", QString::number(pay_int_discount));
    }
    return expression;
}

fexpandmain_keepData fexpandmain_model_sqltablemodel_data::get_keep_data_pay_by_isMPrice_intDiscount(const fexpandmain_model_sqltablemodel_data *tablemodel, bool pay_is_m_price, int pay_int_discount)
{
    fexpandmain_keepData kdata;
    for(int row = 0, rowcount = tablemodel->rowCount(); row < rowcount; row++) {
        if(tablemodel->model_data(row, "ch_billno").toString() != tablemodel->cur_ch_billno()) continue;

        fexpandmain_keepData d;
        get_keep_data_pay_by_isMPrice_intDiscount(tablemodel, row, d, pay_is_m_price, pay_int_discount);
        kdata += d;
    }
    return kdata;
}


bool fexpandmain_model_sqltablemodel::can_discount(int row, QString *errstring) const
{
    QString ch_dishno = model_data(row, "ch_dishno").toString();
    /// 折扣：折扣菜品、~赠送，～子套菜,   ~ 特价

    if("Y" != get_ch_discount_by_map(ch_dishno)) {
        if(errstring) *errstring = "非折扣，不能修改折扣";
        return false;
    }
    if("Y" == model_data(row, "ch_presentflag")) {
        if(errstring) *errstring = "赠送，不能修改折扣";
        return false;
    }
    if("Y" == model_data(row, "ch_suitflag")) {
        if(errstring) *errstring = "子套菜，不能修改折扣";
        return false;
    }
    if("Y" == model_data(row, "ch_specialflag")) {
        if(errstring) *errstring = "特价菜，不能修改折扣";
        return false;
    }
    return true;
}

bool fexpandmain_model_sqltablemodel::can_m_price(int row, QString *errstring) const
{
    return can_m_price(this, row, errstring);
}

bool fexpandmain_model_sqltablemodel::can_change_price(int row, QString *errstring) const
{
    /// 修改价格：~特价、~子套菜、~赠送、~时价菜
    ///
    if("Y" == model_data(row, "ch_specialflag").toString()) {
        if(errstring) *errstring = "特价，不能修改单价";
        return false;
    }
    if("Y" == model_data(row, "ch_suitflag").toString()) {
        if(errstring) *errstring = "子套菜，不能修改单价";
        return false;
    }
    if("Y" == model_data(row, "ch_presentflag").toString()) {
        if(errstring) *errstring = "已赠送，不能修改单价";
        return false;
    }
    if("Y" != get_ch_curprice_by_map(model_data(row, "ch_dishno").toString()) ) {
        if(errstring) *errstring = "非时价菜，不能修改单价";
        return false;
    }
    return true;
}

bool fexpandmain_model_sqltablemodel::can_change_num(int row, QString *errstring) const
{
    if(this->opera(row) != lds_model_sqltablemodel::OpInsert) {
        if(errstring) *errstring = "已落单，不能修数量";
        return false;
    }
    if(model_data(row, "ch_suitflag").toString() == "Y") {
        if(errstring) *errstring = "子套菜，不能修数量";
        return false;
    }
    if(model_data(row, "ch_specialflag").toString() == "Y") {
        if(errstring) *errstring = "特价，不能修数量";
        return false;
    }
    return true;
}

bool fexpandmain_model_sqltablemodel::can_change_dish(int row, QString *errstring) const
{
    /// 换菜：~落单、~非特价、~非主套菜、~赠送
    if(this->opera(row) != lds_model_sqltablemodel::OpInsert) {
        if(errstring) *errstring = tr("所点菜记录已保存,请进行退菜处理");
        return false;
    }
    if(model_data(row, "ch_specialflag").toString() == "Y") {
        if(errstring) *errstring = tr("选中点菜记录为特价,不能换菜");
        return false;
    }
    if(model_data(row, "ch_suitflag").toString() == "P") {
        if(errstring) *errstring = tr("选中点菜记录为主套菜,不能换菜");
        return false;
    }
    if(model_data(row, "ch_presentflag").toString() == "Y") {
        if(errstring) *errstring = tr("选中点菜记录已被赠送,不能换菜");
        return false;
    }
    return true;
}

bool fexpandmain_model_sqltablemodel::can_org_dish_append(int row, QString *errstring) const
{
    ///可以追加：
    if(!model_data(row, "vch_print_memo").toString().isEmpty()) {
        if(errstring) *errstring = tr("有做法") + "，" + tr("不能追加");
        return false;
    }
    if(model_data(row, "ch_suitflag").toString() == "P") {
        for(int rowY = row + 1; rowY < this->rowCount(); rowY++) {
            if(model_data(rowY, "ch_suitflag").toString() == "Y") {
                if(!model_data(rowY, "vch_print_memo").toString().isEmpty()) {
                    if(errstring) *errstring = tr("子套菜")+ tr("有做法") + "，" + tr("不能追加");
                    return false;
                }
                continue;
            }
            break;
        }
    }
    if(model_data(row, "num_price_add").toDouble() != 0) {
        if(errstring) *errstring = tr("有做法加价") + "，" + tr("不能追加");
        return false;
    }

    if("Y" == model_data(row, "ch_presentflag").toString()) {
        if(errstring) *errstring = tr("赠送") + "，" + tr("不能追加");
        return false;
    }
    if("Y" == model_data(row, "ch_suitflag").toString()) {
        if(errstring) *errstring = tr("子套菜") + "，" + tr("不能追加");
        return false;
    }
    if("Y" == model_data(row, "ch_specialflag").toString()) {
        if(errstring) *errstring = tr("特价") + "，" + tr("不能追加");
        return false;
    }
    if(model_data(row, "num_price") != model_data(row, "num_price_org") ) {
        if(errstring) *errstring = tr("菜品现价不等于原价") + "，" + tr("不能追加");
        return false;
    }
    if(100 != model_data(row, "int_discount").toInt()) {
        if(errstring) *errstring = tr("折扣不等于100") + "，" + tr("不能追加");
        return false;
    }
    if(this->opera(row) != lds_model_sqltablemodel::OpInsert) {//已落单
        return false;
    }
    return true;
}

bool fexpandmain_model_sqltablemodel::can_present(int row, QString *errstring) const
{
    if("Y" == model_data(row, "ch_suitflag").toString()){
        if(errstring) *errstring = tr("套菜明细") + "" + tr("不能赠送");
        return false;
    }
    return true;
}

bool fexpandmain_model_sqltablemodel::can_cook(int row, QString *errstring) const
{
    if(this->opera(row) != lds_model_sqltablemodel::OpInsert) {//已落单
        if(errstring) *errstring= tr("已落单");
        return false;
    }
    if(model_data(row, "ch_suitflag").toString() == "P"){
        if(errstring) *errstring= tr("主套菜");
        return false;
    }
    return true;
}

bool fexpandmain_model_sqltablemodel::can_cook_pop(const QString &ch_dishno) const
{
    lds_query query;
    //非主套菜
    query.execSelect(QString("SELECT ch_dishno from cey_bt_dish where ch_dishno = '%1' and ifnull(ch_suitflag, '') <> 'Y' ").arg(ch_dishno));
    if(query.next()) {
        //有做法, 可能添加了做法，但是该做法在memo里没有找到
        query.execSelect(QString("select a.ch_no,a.vch_cp_memo,a.ch_typeno,a.num_add_price FROM cey_bt_cp_memo a,cey_bt_cp_dish b WHERE (a.ch_no = b.ch_no) and ( b.ch_dishno = '%1')")
                         .arg(ch_dishno));
        if(query.next()) {
            return true;
        }
    }
    return false;
}

bool fexpandmain_model_sqltablemodel::can_discount_limit(int int_discount, const QString &title, QString &discount_desc) const
{
    int ldec_opermindiscount;
    if(public_sql::gs_operid == "0000"){
        if(public_sql::discount_off_mode() == "1") {//折让
            ldec_opermindiscount = 100;
        } else {//折扣
            ldec_opermindiscount = 0;
        }
    } else {
        lds_query query;
        query.execSelect(QString("select int_discount from cey_sys_operator where vch_operID ='%1' and ch_flag = 'Y' ").arg(public_sql::gs_operid));
        query.next();
        ldec_opermindiscount = query.recordValue("int_discount").toInt();
    }

    if(public_sql::discount_off_mode() == "1") {//折让
        if(int_discount > ldec_opermindiscount) {
            discount_desc = title + "," + tr("超出营业员折扣上限")+QString(":%1%>%2%").arg(int_discount).arg(ldec_opermindiscount);
            return false;
        }
    } else {
        if(int_discount < ldec_opermindiscount) {
            discount_desc = title + "," + tr("超出营业员折扣下限")+QString(":%1%<%2%").arg(int_discount).arg(ldec_opermindiscount);
            return false;
        }
    }

    return true;
}

void fexpandmain_model_sqltablemodel::update_num_price_add_2_P(const int &row)
{
    int rowP = row;
    do {
        if(model_data(rowP, "ch_suitflag").toString() == "P")
            break;
         rowP--;
    } while(rowP >= 0);

    float num_price_add_P= 0;
    for(int k = rowP+1; k < rowCount(); k++) {
        if(model_data(k, "ch_suitflag").toString() == "Y") {
            num_price_add_P += fexpandmain_model_sqltablemodel::get_num_price_add_vch_print_memo(model_data(k, "vch_print_memo").toString());
            continue;
        }
        break;
    }
    model_data_set(rowP, "num_price_add", num_price_add_P);
}

void fexpandmain_model_sqltablemodel::update_num_num_Y(float num_num_P_old, const int &rowP)
{
    if(num_num_P_old == 0) {
        qDebug() << __LINE__ << "num_num_P_old is 0";
    }
    float num_num_P_new = model_data_NIBOLAN(rowP, "num_num - num_back");
    float rate_P_new_old = num_num_P_new / num_num_P_old;
    for(int row = rowP+1; row < rowCount(); row++) {
        if(model_data(row, "ch_suitflag").toString() == "Y") {
            model_data_set(row, "num_num", model_data_NIBOLAN(row, "num_num-num_back") * rate_P_new_old);
            continue;
        }
        break;
    }
}

void fexpandmain_model_sqltablemodel::update_vch_print_memo_Y(float num_num_old_P, const int &rowP)
{
    if(num_num_old_P == 0) {
        qDebug() << __LINE__ << "num_num_P_old is 0";
        return;
    }
    float num_num_P_new = model_data_NIBOLAN(rowP, "num_num - num_back");
    float rate_P_new_old = num_num_P_new / num_num_old_P;
    for(int row = rowP+1; row < rowCount(); row++) {
        if(model_data(row, "ch_suitflag").toString() == "Y") {
            QMap<QString, int> map = get_map_vch_print_memo(model_data(row, "vch_print_memo").toString());;
            for(QMap<QString, int>::iterator k = map.begin(); k != map.end(); k ++) {
                map.insert(k.key(), k.value() * rate_P_new_old);
            }
            model_data_set(row, "vch_print_memo", get_value_vch_print_memo(map));
            continue;
        }
        break;
    }
}

void fexpandmain_model_sqltablemodel::update_vch_print_memo_N(float num_num_old_N, const int &row_N)
{
    if(num_num_old_N == 0) {
        qDebug() << "num_num_old_N == 0";
        return;
    }
    float num_num_new_N = model_data_NIBOLAN(row_N, "num_num - num_back");
    float rate_new_old = num_num_new_N / num_num_old_N;
    QMap<QString, int> map = get_map_vch_print_memo(model_data(row_N, "vch_print_memo").toString());
    for(QMap<QString, int>::iterator k = map.begin(); k != map.end(); k ++) {
        map.insert(k.key(), k.value() * rate_new_old);
    }
    model_data_set(row_N, "vch_print_memo", get_value_vch_print_memo(map));
}

bool fexpandmain_model_sqltablemodel::select()
{
    return lds_model_sqltablemodel::select();
}

void fexpandmain_model_sqltablemodel_data::emit_num_total_change_DD(const QString &num_label, const QString &total_label) const
{
}

void fexpandmain_model_sqltablemodel_data::emit_num_total_change_label(float n, float t) const
{
    QString num_label = dish_num_str(n);
    QString total_label = dish_total_str(t);
    emit signals_num_total_change(num_label, total_label);

    emit_num_total_change_DD(num_label, total_label);
}

void fexpandmain_model_sqltablemodel_data::emit_num_total_change_label_pay(float n, float t) const
{
    QString num_label = dish_num_str(n);
    QString total_label = dish_total_str(t, tr("应收"));
    emit signals_num_total_change(num_label, total_label);

    emit_num_total_change_DD(num_label, total_label);
}

QString fexpandmain_model_sqltablemodel_data::dish_total_str(float t) const
{
    return dish_total_str(t, tr("金额"));
}

QString fexpandmain_model_sqltablemodel_data::dish_total_str(float t, const QString &prefix) const
{
    return prefix+":"+ lds::septNumber(t, get_cey_sys_parameter_value("bp_fmt_amount_dec", 2).toInt());
}

QString fexpandmain_model_sqltablemodel_data::dish_num_str(float n) const
{
    return dish_num_str(n,  tr("数量"));
}

QString fexpandmain_model_sqltablemodel_data::dish_num_str(float n, const QString &prefix) const
{
    return prefix+":"+lds::septNumber(n, get_cey_sys_parameter_value("bp_fmt_num_dec", 3).toInt());
}

bool fexpandmain_model_sqltablemodel_data::select(const QString &ch_billno, QWidget *member_parent, bool skip_member)
{
    //1
    _ch_billno = ch_billno;
    setFilter(QString("ch_billno = '%1' ").arg(ch_billno));
    //5
    //会员卡
    QString vch_memberno;
    QString ch_member_belong_state;

    //发送副屏数据
    lds::DOUBLE_DATA d;
    d.udpKey = lds::udp_delete_all_request;
    senddd(d, 0, fexpandmain_model_sqltablemodel_data::SENDD_DATA_FROM_DATABASE);
    for(int row = 0, rowcount = this->rowCount(); row < rowcount; row++) {
        fexpandmain_keepData k;//汇总信息
        get_keep_data(row, k);
        _keepdata += k;
        senddd_update_insert(row, lds::udp_insert_request, 0, fexpandmain_model_sqltablemodel_data::SENDD_DATA_FROM_DATABASE);

        if(false == skip_member//没有会员
                &&vch_memberno.length() ==0//未读会员卡
                &&model_data(row, "vch_memberno").toString().length() > 0//有读卡记录
                ) {
            vch_memberno = model_data(row, "vch_memberno").toString();
            ch_member_belong_state = model_data(row, "ch_member_belong_state").toString();
        }
    }
    emit_num_total_change_label(_keepdata.dish_num(), _keepdata.dish_total());
    //
    vch_membernod.init_by_memberno(member_parent, vch_memberno, ch_member_belong_state);
    //
    //    this->tablename_kvmap_print();
    return true;
}

bool fexpandmain_model_sqltablemodel_data::try_commit_data(QString &errstring, bool kuaican_quit, bool only_update) const
{
    lds_query query;
    qrtVariantPairList key_value_list;
    QString cur_dt_str = n_func::f_get_sysdatetime_str();

    if(_ch_billno.isEmpty()) {
        errstring = "model ch_billno is empty";
        return false;
    }
    for(int row = 0; row < this->rowCount(); row ++) {

        //分单是,只提交分单的部分
        if(_ch_billno != model_data(row, "ch_billno").toString()) continue;
        //非新增和修改的,不进行本操作
        if(opera(row) != OpInsert
                && opera(row) != OpUpdate) continue;
        //若只是更新
        if(only_update)
            if(opera(row) != OpUpdate) continue;

        //qrtVariantPair
        if(opera(row) == OpInsert) {
            //c == 0,int_flowid skip
            key_value_list.clear();
            for(int c = 1; c < this->columnCount(); c ++) {
                qrtVariantPair pair = qrtVariantPair(headerData(c, Qt::Horizontal).toString(), model_data(row, c, Qt::EditRole));
                //kuaican_quit
                if(kuaican_quit) {
                    if("num_num" == pair.first) {
                        pair.second = 0;
                    }
                    if("num_back" == pair.first) {
                        pair.second = model_data_NIBOLAN(row, "num_num - num_back");
                    }
                    if("vch_back_operID" == pair.first) {
                        pair.second = public_sql::gs_operid;
                    }
                    if("dt_back_operdate" == pair.first) {
                        pair.second = cur_dt_str;
                    }
                }
                //~kuaican_quit
                key_value_list << pair;
            }
        }
        if(opera(row) == OpUpdate) {
            key_value_list.clear();
            for(int column = 0; column < this->columnCount(); column ++) {
                if(isDirty(index(row, column))) {
                    key_value_list << qrtVariantPair(record(row).fieldName(column), model_data(row, column, Qt::EditRole));
                }
            }
        }
        //exec
        if(opera(row) == OpInsert) {
            if(!query.execInsert(tableName(), key_value_list, &errstring)) {
                return false;
            }
        }
        if(opera(row) == OpUpdate) {
            if(!query.execUpdate(tableName(), key_value_list,
                                 qrtEqual("int_flowid",  model_data(row, "int_flowid").toString()), &errstring)) {
                return false;
            }
        }
        //material
        if(!fexpandmain_2::material_insert(query, _ch_billno, model_data(row, "int_id").toInt())) {
            errstring = query.recordError();
            return false;
        }
    }//for

    //判断负库存不允许消费
    if(get_cey_sys_parameter_value("inventory_negative_cannot_pay", "0").toString() == "1") {
        QString msg;
        QString record;
        QString ch_dishno;
        QString ch_dishno_sub;
        QString int_id;
        lds_query query2;
        //销售数量大于0
        query.execSelect(QString("select c.ch_dishno , a.num_num, b.int_id, b.int_flowid "
                                 "from cey_st_material_num a, cey_u_orderdish b, cey_bt_dish c  where b.ch_billno = '%1' and (b.num_num-b.num_back) > 0 and a.ch_materialno = b.ch_dishno and b.ch_dishno = c.ch_dishno  order by b.int_flowid asc; ").arg(_ch_billno));
        while(query.next()) {
            ch_dishno = query.recordValue("ch_dishno").toString();
            int_id = query.recordValue("int_id").toString();
            if(get_item_flag_by_map(ch_dishno) == "Y" && query.recordValue("num_num").toDouble() < 0) {//库存管理
                record = int_id.rightJustified(2, ' ') + "." + tr("菜品") + ":\t"+ get_vch_dishname_by_map(ch_dishno);
                msg += record + "\n";
                ch_dishno = "";
            }

            query2.execSelect(QString("select b.ch_dishno, a.num_num "
                                      "from cey_st_material_num a, material_u_orderdish b  where b.int_order_flowID = %1 and a.ch_materialno = b.ch_dishno  and a.num_num < 0 ; ")
                              .arg(query.recordValue("int_flowid").toString()));
            while(query2.next()) {
                ch_dishno_sub = query2.recordValue("ch_dishno").toString();
                if(get_item_flag_by_map(ch_dishno_sub) == "Y") {//库存管理
                    record = "---"+tr("原料")+":\t"+ get_vch_dishname_by_map(ch_dishno_sub);
                    if(!ch_dishno.isEmpty()) {
                        record += "\t"+ tr("所属主菜") + ":" +int_id.rightJustified(2, ' ') + "." + get_vch_dishname_by_map(ch_dishno);
                    }
                    msg += record + "\n";
                }
            }
        }
        if(!msg.isEmpty()) {
            errstring = tr("负库存不允许消费")+"\n"+msg;
            return false;
        }
    }
    return true;
}

bool fexpandmain_model_sqltablemodel_data::try_update_data(QString &errstring) const
{
    return try_commit_data(errstring, false, true);
}

QString fexpandmain_model_sqltablemodel_data::msg_mid_by_N(const QString &msg, int pos, int length, bool show_dot_when_not_complete)
{
    QStringList m = msg.split("\n", QString::SkipEmptyParts);
    QString app;
    if(show_dot_when_not_complete) {
        if(m.length() > pos + length){
            app = "\n...";
        }
    }
    m = m.mid(pos, length);
    return m.join("\n")+app;
}

const fexpandmain_keepData &fexpandmain_model_sqltablemodel_data::getKeepdata() const
{
    return _keepdata;
}

fexpandmain_keepData fexpandmain_model_sqltablemodel_data::getKeepdata(int row) const
{
    fexpandmain_keepData kdata;
    get_keep_data_pay_by_isMPrice_intDiscount(this, row, kdata);

    return kdata;
}

bool fexpandmain_model_sqltablemodel_data::vch_membernod_m_update(QString &errstring)
{
    //会员价
    if(vch_membernod.ch_favourable == "1"){//会员价
        if(vch_membernod.ch_pricekind.isEmpty()) {
            errstring = tr("没有设置会员价")+","+tr("请到会员类型里修改");
            return false;
        }
    }

    for(int row = 0, rowcount = rowCount(); row < rowcount; row ++) {
        if(vch_membernod.is_m_price()) {
            if(can_m_price(row)) {
                model_data_set(row, "num_price", get_num_m_price_by_map(model_data(row, "ch_dishno").toString()));
            }
        }
        if(vch_membernod.is_m_discount()) {
            if(can_discount(row)) {
                model_data_set(row, "int_discount", vch_membernod.int_discount);
            }
        }
    }

    for(int row = 0, rowcount = rowCount(); row < rowcount; row ++) {
        model_data_set(row, "vch_memberno", vch_membernod.vch_memberno);
        model_data_set(row, "ch_member_belong_state", vch_membernod.ch_member_belong_state);
    }
    return true;
}

QSet<int> fexpandmain_model_sqltablemodel_data::ch_dishno_set_row_map_GET(const QString &ch_dishno) const
{
    return ch_dishno_set_row_map_get.value(ch_dishno);
}

QSet<int> fexpandmain_model_sqltablemodel_data::ch_dishno_set_row_map_GET(int row) const
{
    return ch_dishno_set_row_map_GET(model_data(row, "ch_dishno").toString());
}

const QMap<QString, QSet<int> > &fexpandmain_model_sqltablemodel_data::ch_dishno_set_row_map_GET() const
{
    return ch_dishno_set_row_map_get;
}

//bool fexpandmain_model_sqltablemodel_data::can_change_num(int row) const
//{
//    int count = ch_dishno_set_row_map_GET(row).count();
//    //没有记录在案，但是允许追加
//    if(count == 0
//            && can_org_dish_append(row)
//            ) return true;

//    return count;
//}

void fexpandmain_model_sqltablemodel_data::ch_dishno_set_row_map_PRINT()
{
    return;
    QMapIterator<QString, QSet<int> > mapitr(ch_dishno_set_row_map_get);
    int count = 0;
    qDebug() << "==================";
    while(mapitr.hasNext()) {
        mapitr.next();
        if(!mapitr.value().isEmpty()) {
            qDebug() << "---------------------------------";
            qDebug() << get_vch_dishname_by_map(mapitr.key()) << mapitr.value();
            foreach(int d, mapitr.value()) {
                qDebug() << "行:" << d << get_vch_dishname_by_map(model_data(d, "ch_dishno").toString());
            }
            count += mapitr.value().count();
        }
    }
    qDebug() << "总行数" << count << ch_dishno_set_row_map_get;
}

void fexpandmain_model_sqltablemodel_data::ch_dishno_set_row_map_INSERT( int row)
{
    QString ch_dishno = model_data(row, "ch_dishno").toString();
    if(ch_dishno.isEmpty()) return;

    if(ch_dishno_set_row_map_get.find(ch_dishno) == ch_dishno_set_row_map_get.end()) {
        ch_dishno_set_row_map_get.insert(ch_dishno, QSet<int>());
    }
    ch_dishno_set_row_map_get[ch_dishno].insert(row);
}

void fexpandmain_model_sqltablemodel_data::ch_dishno_set_row_map_UPDATE(int row)
{
    QString errstring;
    if(can_org_dish_append(row, &errstring)) {
        ch_dishno_set_row_map_INSERT(row);
    } else {
        ch_dishno_set_row_map_DEL(row, 1);
    }
}

void fexpandmain_model_sqltablemodel_data::ch_dishno_set_row_map_DEL(int row, int count)
{
    QMap<QString, QString> ch_dishno_op_map;
    QString ch_dishno;
    //10002,  x4,x5,7-5,8-6
    for(int rowbegin = row, rowcount = rowCount(); rowbegin < rowcount; rowbegin ++) {
        ch_dishno = model_data(rowbegin, "ch_dishno").toString();
        if(rowbegin - row < count) {
            ch_dishno_op_map.insert(ch_dishno, ch_dishno_op_map.value(ch_dishno)+","+"x"+QString::number(rowbegin));
        } else {
            ch_dishno_op_map.insert(ch_dishno, ch_dishno_op_map.value(ch_dishno)+","+QString("%1=%2").arg(rowbegin).arg(rowbegin-count));
        }
    }
    //
    QMap<QString, QSet<int> >::iterator k;
    QMapIterator<QString, QString> mapitr(ch_dishno_op_map);
    while(mapitr.hasNext()) {
        mapitr.next();
        if((k=ch_dishno_set_row_map_get.find(mapitr.key())) != ch_dishno_set_row_map_get.end()) {
            QStringList op_list = mapitr.value().split(",", QString::SkipEmptyParts);
            foreach(QString op, op_list) {
                if(op.contains("x")) {//delete
                    op.remove(0, 1);
                    k.value().remove(op.toInt());
                }
                if(op.contains("=")){//replace
                    QStringList ops = op.split("=" ,QString::SkipEmptyParts);
                    if(k.value().remove(ops[0].toInt())) {
                        k.value().insert(ops[1].toInt());
                    }
                }
            }
        }
    }

}

void fexpandmain_model_sqltablemodel_data::ch_dishno_set_row_map_CLEAR()
{
    ch_dishno_set_row_map_get.clear();
}

void fexpandmain_model_sqltablemodel_data::row_fold_map_RELEASE(QTableView *tableView)
{
    for(int k = 0; k < tableView->verticalHeader()->count(); k++){
        tableView->verticalHeader()->showSection(k);
    }
    row_fold_map.clear();
}

void fexpandmain_model_sqltablemodel_data::row_fold_map_FOLD(QTableView *tableView, const QModelIndex &index)
{
    int row = index.row();
    bool fold_state = row_fold_map.value(row, get_cey_sys_parameter_value("qt_Y_dish_fold") == "1");
    for(int row = index.row() + 1; row < rowCount(); row ++) {
        if("Y" == model_data(row, "ch_suitflag").toString()){
            if(fold_state) {
                tableView->showRow(row);
            } else {
                tableView->hideRow(row);
            }
            continue;
        }
        break;
    }
    row_fold_map.insert(index.row(), !fold_state);
    tableView->update(index);
}

void fexpandmain_model_sqltablemodel_data::ch_dishno_dish_num_map_UPDATE(const lds::DOUBLE_DATA &data, double off, const QString &SENDD_DATA_FROM_DATABASE)
{
    if(SENDD_DATA_FROM_DATABASE == fexpandmain_model_sqltablemodel_data::SENDD_DATA_FROM_DATABASE) {

    } else {
        fexpandmain_2::guqing_update(data.items.ch_dishno, off);
    }
    if(data.items.ch_suitflag != "Y")
        ch_dishno_dish_num_map.insert(data.items.ch_dishno, ch_dishno_dish_num_map.value(data.items.ch_dishno, 0)+off);
}

void fexpandmain_model_sqltablemodel_data::ch_dishno_dish_num_map_CLEAR(const QString &SENDD_DATA_FROM_DATABASE)
{
    QMapIterator<QString, double> mapitr(ch_dishno_dish_num_map);
    while(mapitr.hasNext()) {
        mapitr.next();
        lds::DOUBLE_DATA d;
        d.items.ch_dishno = mapitr.key();
        ch_dishno_dish_num_map_UPDATE(d, -mapitr.value(), SENDD_DATA_FROM_DATABASE);
    }

    ch_dishno_dish_num_map.clear();
}

void fexpandmain_model_sqltablemodel_data::ch_dishno_dish_num_map_PRINT()
{
#ifdef QT_DEBUG
    QMapIterator<QString, double> mapitr(ch_dishno_dish_num_map);
    while(mapitr.hasNext()) {
        mapitr.next();

        qDebug() <<  mapitr.key() + ":"+QString::number(mapitr.value()) + "\t";
    }
#endif
}

const QMap<QString, double> &fexpandmain_model_sqltablemodel_data::ch_dishno_dish_num_map_GET() const
{
    return ch_dishno_dish_num_map;
}

void fexpandmain_model_sqltablemodel_data::print_dish_info()
{
    //    ch_dishno_dish_num_map_PRINT();
    //    ch_dishno_set_row_map_PRINT();
}

void num_backData::print() const
{
    return;
    qDebug() << "int_flowid" << int_flowid;
    qDebug() << "num_back_add" << num_back_add;

    qDebug() << "vch_back_operid" << vch_back_operid;
    qDebug() << "dt_back_operdate" << dt_back_operdate;
    qDebug() << "vch_reason" << vch_reason;

    qDebug() << "vch_reason" << vch_print_memo;
    qDebug() << "vch_reason" << vch_print_memo_remain;
}

fexpandmain_keepData::fexpandmain_keepData()
{
    // 消费 可折扣金额 折扣额 赠送 税率
    // 服务费
    consum = 0;
    discount = 0;
    present = 0;
    num_rate = 0;
    num = 0;
    num_service = 0;
    num_price_add = 0;
}

void fexpandmain_keepData::print() const
{
    qDebug() << "consum"<<consum;
    qDebug() << "discount"<<discount;
    qDebug() << "present"<<present;
    qDebug() << "num_rate"<<num_rate;
    qDebug() << "num"<<num;
    qDebug() << "num_service"<<num_service;
    qDebug() << "num_price_add"<<num_price_add;
}

double fexpandmain_keepData::dish_total() const
{
    return consum - discount - present;
}

double fexpandmain_keepData::dish_num() const
{
    return num;
}

fexpandmain_keepData fexpandmain_keepData::operator-(const fexpandmain_keepData &other)
{
    consum -= other.consum;
    discount -= other.discount;
    present -= other.present;
    num_rate -= other.num_rate;
    num -= other.num;
    num_service -= other.num_service;
    num_price_add -= other.num_price_add;

    return *this;
}

fexpandmain_keepData fexpandmain_keepData::operator+(const fexpandmain_keepData &other)
{
    consum += other.consum;
    discount += other.discount;
    present += other.present;
    num_rate += other.num_rate;
    num += other.num;
    num_service += other.num_service;
    num_price_add += other.num_price_add;

    return *this;
}

/////////////////////
fexpandmain_model_sqltablemodel_dd::fexpandmain_model_sqltablemodel_dd(QObject *parent)
    : fexpandmain_model_sqltablemodel_data(parent)
{
}

void fexpandmain_model_sqltablemodel_dd::senddd(const lds::DOUBLE_DATA &data, float dish_num_old, const QString &SENDD_DATA_FROM_DATABASE)
{
    frontViewDialog_virtual::doublescreensenddataByconf(data);
    if(data.udpKey == lds::udp_delete_all_request) {//全部清空
        if(SENDD_DATA_FROM_DATABASE == fexpandmain_model_sqltablemodel_dd::SENDD_DATA_FROM_DATABASE) {
            ///点菜记录初始化
            _keepdata = fexpandmain_keepData();
            ///菜品编码<可追加的set_row>
            ch_dishno_set_row_map_CLEAR();
            ///菜品编码<数量>
            ch_dishno_dish_num_map_CLEAR(SENDD_DATA_FROM_DATABASE);
        }
        return;
    }

    if(data.udpKey == lds::udp_insert_request) {//插入新的数量，只有在commitdata 时，有效
        ///菜品编码<可追加的set_row>
        ch_dishno_set_row_map_UPDATE(data.row);
        ///菜品编码<数量>
        ch_dishno_dish_num_map_UPDATE(data, data.items.dish_num.toFloat(), SENDD_DATA_FROM_DATABASE);//插入新的数量
    }
    if(data.udpKey == lds::udp_update_request) {
        ///菜品编码<可追加的set_row>
        ch_dishno_set_row_map_UPDATE(data.row);
        ///菜品编码<数量>
        ch_dishno_dish_num_map_UPDATE(data, data.items.dish_num.toFloat() - dish_num_old, SENDD_DATA_FROM_DATABASE);//更新时，新旧互减
    }
    if(data.udpKey == lds::udp_delete_request) {
        ///菜品编码<可追加的set_row>
        ch_dishno_set_row_map_DEL(data.row, data.count);
        ///菜品编码<数量>
        ch_dishno_dish_num_map_UPDATE(data, -data.items.dish_num.toFloat(), SENDD_DATA_FROM_DATABASE);//删除当时的菜品数量
    }
    if(data.udpKey == lds::udp_delete_all_request) {//全部清空
        ///菜品编码<可追加的set_row>
        ch_dishno_set_row_map_CLEAR();
        ///菜品编码<数量>
        ch_dishno_dish_num_map_CLEAR(SENDD_DATA_FROM_DATABASE);
    }
}

void fexpandmain_model_sqltablemodel_dd::senddd_update_insert(int row, const QString &udp_request_type, float dish_num_old, const QString &SENDD_DATA_FROM_DATABASE)
{
    fexpandmain_keepData kdata;
    get_keep_data(row, kdata);

    lds::DOUBLE_DATA ddata;
    ddata.udpKey = udp_request_type;
    ddata.row = row;
    ddata.items.vch_dishname = get_vch_dishname_by_map(model_data(row, "ch_dishno").toString());
    ddata.items.ch_dishno = model_data(row, "ch_dishno").toString();
    ddata.items.dish_price = get_cey_sys_parameter_value("pay_unit_prefix").toString()+ QString::number(model_data_NIBOLAN(row, "num_price"), 'f', get_cey_sys_parameter_value("bp_fmt_price_dec", 2).toInt());
    ddata.items.dish_num = QString::number(kdata.dish_num(), 'f', get_cey_sys_parameter_value("bp_fmt_num_dec", 3).toInt());
    ddata.items.dish_total = QString::number(kdata.dish_total(), 'f', get_cey_sys_parameter_value("bp_fmt_amount_dec", 2).toInt());
    ddata.items.vch_other = get_dish_desc(row);
    ddata.items.ch_suitflag = model_data(row, "ch_suitflag").toString();

    senddd(ddata, dish_num_old, SENDD_DATA_FROM_DATABASE);
}

void fexpandmain_model_sqltablemodel_dd::emit_num_total_change_DD(const QString &num_label, const QString &total_label) const
{
    lds::DOUBLE_DATA ddata;
    ddata.udpKey = lds::udp_title_request;
    ddata.sum_num_desc = num_label;
    ddata.sum_total_desc = total_label;
    frontViewDialog_virtual::doublescreensenddataByconf(ddata);
}
