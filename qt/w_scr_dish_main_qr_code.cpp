#include "w_scr_dish_main_qr_code.h"
#include "ui_w_scr_dish_main_qr_code.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include "lds.h"
#include "cjson_help.h"
#include "cJSON.h"
#include <QtDebug>
#include "lds_messagebox.h"
#include "lds_model_sqltablemodel_delegate_com.h"
#include "fexpandmain_model_sqltablemodel.h"
#include "w_scr_dish_restaurant_div_pop.h"
#include "public_sql.h"
#include "w_scr_dish_restaurant_dialog.h"
#include "fexpandmain_2.h"
#include "w_bt_dish.h"
int w_scr_dish_main_qr_code::indexof(const w_scr_dish_main_qr_code::QrCodeMasterList &masters, const QString &sn)
{
    for(int k = 0; k < masters.count(); k++) {
        if(masters[k].sn == sn) {
            return k;
        }
    }
    return -1;
}

w_scr_dish_main_qr_code::QrCodeMasterList w_scr_dish_main_qr_code::qr_code_master_get(const QString &ch_tableno, w_scr_dish_main_qr_code::billType state)
{
    QMap<QByteArray, QByteArray> map;
    map.insert("tableNo", ch_tableno.toLocal8Bit());
    map.insert("state", QByteArray::number(state));

    return qr_code_master_get(map);
}

w_scr_dish_main_qr_code::QrCodeMasterList w_scr_dish_main_qr_code::qr_code_master_get(const QMap<QByteArray, QByteArray> &headmap)
{
    w_scr_dish_main_qr_code::QrCodeMasterList qr_data_list;
    QNetworkAccessManager m;
    QEventLoop loop;
    QObject::connect(&m, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
    QNetworkReply *reply;
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader,  "application/json;charset=utf-8");
#ifdef QT_DEBUG
    request.setRawHeader("terminalCode",  "FA1101");
#else
    request.setRawHeader("terminalCode",  lds::terminalCode.toLocal8Bit());
#endif
    for(QMap<QByteArray, QByteArray>::const_iterator k = headmap.begin(); k != headmap.end(); k ++) {
        request.setRawHeader(k.key(), k.value());
    }
    request.setUrl(QUrl("http://115.28.155.165/qryqs/dishREST/queryUserOrders-rest"));
    reply = m.get(request);
    loop.exec();

    int httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if(200 == httpStatus) {
        cJSONHttpData root = cJSON_Parse(reply->readAll().data());
        w_scr_dish_main_qr_code::QrCodeMaster qr_data;
        qr_data.orgName = cJSON_help::getcJSONvalue(root.json, "orgName").toString();
        cJSON *array = cJSON_help::getcJSON(root.json, "data");
        for(int k = 0; k < cJSON_help::array_count(array); k ++) {
            cJSON *item = cJSON_GetArrayItem(array, k);
            qr_data.date = cJSON_help::getcJSONvalue(item, "date").toDateTime();
            qr_data.mount = cJSON_help::getcJSONvalue(item, "mount").toDouble();
            qr_data.sn = cJSON_help::getcJSONvalue(item, "sn").toString();
            qr_data.tableNo = cJSON_help::getcJSONvalue(item, "tableNo").toString();
            qr_data.state = billType(cJSON_help::getcJSONvalue(item, "state").toInt());

            qr_data_list << qr_data;
        }
    }
    return qr_data_list;
}

void w_scr_dish_main_qr_code::qr_code_list_print(const QList<w_scr_dish_main_qr_code::QrCodeMaster> &list)
{
    for(int k = 0; k < list.count(); k ++) {
        qDebug() << "sn:" << list[k].sn;
        qDebug() << "date:" << list[k].date;
        qDebug() << "orgName:" << list[k].orgName;
        qDebug() << "mount:" << list[k].mount;
        qDebug() << "state:" << list[k].state;
    }
}

w_scr_dish_main_qr_code::QrCodeDetailList w_scr_dish_main_qr_code::qr_code_detail_get(const QString &sn)
{

    w_scr_dish_main_qr_code::QrCodeDetailList qr_data_list;
    QNetworkAccessManager m;
    QEventLoop loop;
    QObject::connect(&m, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
    QNetworkReply *reply;
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader,  "application/json;charset=utf-8");
#ifdef QT_DEBUG
    request.setRawHeader("terminalCode",  "FA1101");
#else
    request.setRawHeader("terminalCode",  lds::terminalCode.toLocal8Bit());
#endif
    request.setRawHeader("orderSn",  sn.toLocal8Bit());
    request.setUrl(QUrl("http://115.28.155.165/qryqs/dishREST/getOrderDishInfo-rest"));
    reply = m.get(request);
    loop.exec();

    int httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if(200 == httpStatus) {
        cJSONHttpData root = cJSON_Parse(reply->readAll().data());
        w_scr_dish_main_qr_code::QrCodeDetail qr_data;
        cJSON *array = cJSON_help::getcJSON(root.json, "data");
        for(int k = 0; k < cJSON_help::array_count(array); k ++) {
            cJSON *item = cJSON_GetArrayItem(array, k);
            qr_data.dishNo = cJSON_help::getcJSONvalue(item, "dishNo").toString();
            qr_data.mount = cJSON_help::getcJSONvalue(item, "mount").toDouble();
            qr_data.nickname = cJSON_help::getcJSONvalue(item, "nickname").toString();
            qr_data.number = cJSON_help::getcJSONvalue(item, "number").toDouble();
            qr_data.price = cJSON_help::getcJSONvalue(item, "price").toDouble();

            qr_data.tc = cJSON_help::getcJSONvalue(item, "tc").toString();
            qr_data.zf = cJSON_help::getcJSONvalue(item, "zf").toString();

            qr_data_list << qr_data;
        }
    }
    return qr_data_list;
}

void w_scr_dish_main_qr_code::qr_code_list_print(const QList<w_scr_dish_main_qr_code::QrCodeDetail> &list)
{
    for(int k = 0; k < list.count(); k ++) {
        qDebug() << "dishNo:" << list[k].dishNo;
        qDebug() << "mount:" << list[k].mount;
        qDebug() << "nickname:" << list[k].nickname;
        qDebug() << "mount:" << list[k].number;
        qDebug() << "price:" << list[k].price;

        qDebug() << "tc:" << list[k].tc;
        qDebug() << "zf:" << list[k].zf;
    }
}

bool w_scr_dish_main_qr_code::qr_code_bill_do_CANCEL(const QString &sn, QString &errstring)
{
    QNetworkAccessManager m;
    QEventLoop loop;
    QObject::connect(&m, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
    QNetworkReply *reply;
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader,  "application/json;charset=utf-8");
#ifdef QT_DEBUG
    request.setRawHeader("terminalCode",  "FA1101");
#else
    request.setRawHeader("terminalCode",  lds::terminalCode.toLocal8Bit());
#endif
    request.setRawHeader("orderSn",  sn.toLocal8Bit());//已下单
    request.setUrl(QUrl("http://115.28.155.165/qryqs/dishREST/cannelOrderDishInfoByOrderNo-rest"));
    reply = m.get(request);
    loop.exec();

    int httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if(200 == httpStatus) {
        return true;
    }
    errstring = cJSON_help::getcJSONvalue(reply->readAll(), "message").toString();
    return false;
}

bool w_scr_dish_main_qr_code::qr_code_bill_do_RCV(const QString &sn, QString &errstring)
{
    QNetworkAccessManager m;
    QEventLoop loop;
    QObject::connect(&m, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
    QNetworkReply *reply;
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader,  "application/json;charset=utf-8");
#ifdef QT_DEBUG
    request.setRawHeader("terminalCode",  "FA1101");
#else
    request.setRawHeader("terminalCode",  lds::terminalCode.toLocal8Bit());
#endif
    request.setRawHeader("orderSn",  sn.toLocal8Bit());//已下单
    request.setUrl(QUrl("http://115.28.155.165/qryqs/dishREST/commitOrderDishInfoByOrderNo-rest"));
    reply = m.get(request);
    loop.exec();

    int httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    QByteArray readall = reply->readAll();
    qDebug() << "httpStatus:" << httpStatus << "readall:" << readall;
    if(200 == httpStatus) {
        return true;
    }
    errstring = cJSON_help::getcJSONvalue(readall, "message").toString();
    return false;
}

bool w_scr_dish_main_qr_code::qr_code_has_paid(const QString &sn, w_scr_dish_main_qr_code::QrCodePay &paydetail)
{
    QNetworkAccessManager m;
    QEventLoop loop;
    QObject::connect(&m, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
    QNetworkReply *reply;
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader,  "application/json;charset=utf-8");
#ifdef QT_DEBUG
    request.setRawHeader("terminalCode",  "FA1101");
#else
    request.setRawHeader("terminalCode",  lds::terminalCode.toLocal8Bit());
#endif
    request.setRawHeader("orderSn",  sn.toLocal8Bit());//已下单
    request.setUrl(QUrl("http://115.28.155.165/qryqs/dishREST/getOrderDishInfoByOrderNo-rest"));
    reply = m.get(request);
    loop.exec();

    cJSONHttpData root = cJSON_Parse(reply->readAll().data());
    int httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if(200 == httpStatus) {
        cJSON *d = cJSON_help::getcJSON(root.json, "data");
        qDebug() << __LINE__ << root.jsonToString();
        if(billTypePaid == cJSON_help::getcJSONvalue(d, "state").toInt()) {
            paydetail.alipay = cJSON_help::getcJSONvalue(cJSON_help::getcJSON(d, "paydetail"), "alipay").toDouble();
            paydetail.wechatpay = cJSON_help::getcJSONvalue(cJSON_help::getcJSON(d, "paydetail"), "wechatpay").toDouble();
            paydetail.mount = cJSON_help::getcJSONvalue(d, "mount").toDouble();
            return true;
        }
    }
    return false;
}

bool w_scr_dish_main_qr_code::qr_code_bill_do_pay_transaction(const QString &ch_tableno, const QString &ch_billno_R, const QrCodePay &paydetail, QString &ret_errstring)
{
    QDateTime curdt = n_func::f_get_sysdatetime();
    lds_query query;
    lds_query::tran_saction();
    QString ch_payno = w_scr_dish_restaurant_div_pop::getMaxChpaynoAdd1_cey_u_checkout_master();
    int sqr = 0;
    //cancel table
    if(w_scr_dish_restaurant_div_pop::cey_bt_table_update(ch_tableno, "", sqr, &ret_errstring)) {
        //checkout_master
        if(query.execInsert("cey_u_checkout_master",qrtVariantPairList()
                            << qrtVariantPair("ch_billno", ch_billno_R)
                            << qrtVariantPair("ch_tableno", ch_tableno)
                            << qrtVariantPair("num_cost", paydetail.mount)
                            << qrtVariantPair("num_discount", 0)
                            << qrtVariantPair("num_present", 0)

                            << qrtVariantPair("num_service", 0)
                            //                                        << qrtVariantPair("vch_service_desc", "")
                            << qrtVariantPair("num_lowcost", 0)
                            << qrtVariantPair("num_blotout", 0)
                            << qrtVariantPair("num_run", 0)

                            << qrtVariantPair("vch_run_operID", "")
                            << qrtVariantPair("ch_state", "Y")
                            << qrtVariantPair("vch_operID", public_sql::gs_operid)
                            << qrtVariantPair("dt_operdate", curdt)
                            << qrtVariantPair("vch_operID2", "")

                            << qrtVariantPair("num_rate", 0)
                            << qrtVariantPair("ch_payno", ch_payno)
                            << qrtVariantPair("vch_memberno", "")
                            << qrtVariantPair("ch_member_belong_state", "")
                            << qrtVariantPair("ch_yun_payno", "")

                            << qrtVariantPair("num_cash", paydetail.mount) , &ret_errstring)) {
            //check detail alipay
            if(paydetail.alipay == 0 ||query.execInsert("cey_u_checkout_detail",qrtVariantPairList()
                                                        << qrtVariantPair("ch_payno", ch_payno)
                                                        << qrtVariantPair("ch_paymodeno", "06")//支付宝
                                                        << qrtVariantPair("num_payamount", paydetail.alipay)
                                                        << qrtVariantPair("num_change", 0)
                                                        << qrtVariantPair("num_realamount", paydetail.alipay)

                                                        << qrtVariantPair("num_face", 0)
                                                        << qrtVariantPair("vch_operID", public_sql::gs_operid)
                                                        << qrtVariantPair("dt_operdate", curdt)
                                                        << qrtVariantPair("num_card_remain", 0)
                                                        << qrtVariantPair("vch_voucherno", "")

                                                        << qrtVariantPair("num_face_Num", 0), &ret_errstring))  {
                //check detail wechatpay
                if(paydetail.wechatpay == 0 ||query.execInsert("cey_u_checkout_detail",qrtVariantPairList()
                                                               << qrtVariantPair("ch_payno", ch_payno)
                                                               << qrtVariantPair("ch_paymodeno", "05")//微信
                                                               << qrtVariantPair("num_payamount", paydetail.wechatpay)
                                                               << qrtVariantPair("num_change", 0)
                                                               << qrtVariantPair("num_realamount", paydetail.wechatpay)

                                                               << qrtVariantPair("num_face", 0)
                                                               << qrtVariantPair("vch_operID", public_sql::gs_operid)
                                                               << qrtVariantPair("dt_operdate", curdt)
                                                               << qrtVariantPair("num_card_remain", 0)
                                                               << qrtVariantPair("vch_voucherno", "")

                                                               << qrtVariantPair("num_face_Num", 0), &ret_errstring))  {
                    //u_table
                    if(query.execUpdate("cey_u_table","ch_payno", ch_payno, qrtEqual("ch_billno", ch_billno_R),  &ret_errstring)){
                        //u_orderdish
                        if(query.execUpdate("cey_u_orderdish","ch_payno", ch_payno, qrtEqual("ch_billno", ch_billno_R) , &ret_errstring)){
                            lds_query::com_mit();
                            return true;
                        }
                    }
                }
            }
        }
    }
    lds_query::roll_back();
    return false;
}


QString w_scr_dish_main_qr_code::getBillTypeDesc(billType type) {
    return billTypeDescMap().value(QString::number(type), tr("状态异常")).toString();
}

const QMap<QString, QVariant> &w_scr_dish_main_qr_code::billTypeDescMap()
{
    static QMap<QString , QVariant>map;
    if(map.isEmpty()) {
        map.insert(QString::number(billTypeAVailable), tr("新订单"));
        map.insert(QString::number(billTypePaid), tr("已支付"));
        map.insert(QString::number(billTypeRCV), tr("待结算"));
        map.insert(QString::number(billTypeNone), tr("状态异常"));
    }
    return map;
}

bool w_scr_dish_main_qr_code::qr_code_can_order(const QString &ch_tableno, QString &errstring)
{
    lds_query query;
    query.execSelect(QString(" select " + restraurantDelegate::sql_ch_billnos + ", ch_billno from cey_bt_table where ch_tableno = '%1' ").arg(ch_tableno));
    query.next();
    QString ch_billnos = query.recordValue(0).toString();
    QString ch_billno = query.recordValue(1).toString();

    //已经分单
    if(restraurantDelegate::ch_billno_is_div(ch_billnos)) {
        errstring = tr("餐桌已经分单");
        return false;
    }
    //点菜记录
    if(lds_query::selectValue(QString(" select count(0) from cey_u_orderdish where ch_billno = '%1' ")
                              .arg(ch_billno)).toInt() > 0) {
        errstring = tr("已有点菜记录");
        return false;
    }
    return true;
}

w_scr_dish_main_qr_code::w_scr_dish_main_qr_code(const QString &ch_tableno, const w_scr_dish_main_qr_code::QrCodeMasterList &master_list, const QString &currentOrderSn, QWidget *parent) :  QDialog(parent),
    ui(new Ui::w_scr_dish_main_qr_code)
{
    ui->setupUi(this);
    this->ch_tableno =ch_tableno;
#ifdef QT_DEBUG
    this->ch_tableno = "1002";
#endif
    init();

    updatemaster(master_list, currentOrderSn);
    toupdatedetail(0);

    connect(ui->comboBox_master, SIGNAL(currentIndexChanged(int)), this, SLOT(toupdatedetail(int)));
    connect(ui->pushButton_bill_cancel, SIGNAL(clicked()), this, SLOT(tobillcancel()));
}

w_scr_dish_main_qr_code::w_scr_dish_main_qr_code(const QString &ch_billno_R, QWidget *parent):  QDialog(parent),
    ui(new Ui::w_scr_dish_main_qr_code)
{
    ui->setupUi(this);
    ch_tableno = lds_query::selectValue(QString(" select ch_tableno from cey_u_table where ch_billno = '%1' ").arg(ch_billno_R)).toString();
    init();

    ui->comboBox_master->setEnabled(false);
    ui->pushButton_ok->hide();
    ui->pushButton_bill_cancel->hide();

    QString currentOrderSn = lds_query::selectValue(QString(" select vch_qr_code_sn from cey_u_table where ch_billno = '%1' ").arg(ch_billno_R)).toString();
    updatemaster(qr_code_master_get(ch_tableno, billTypeRCV), currentOrderSn);
    updatedetail(ch_billno_R);

    connect(ui->comboBox_master, SIGNAL(currentIndexChanged(int)), this, SLOT(toupdatedetail(int)));

}

w_scr_dish_main_qr_code::~w_scr_dish_main_qr_code()
{
    delete ui;
}

bool w_scr_dish_main_qr_code::load_u_orderdish_transaction(QString &ret_ch_billno)
{
    lds_query query;
    fexpandmain_model_sqltablemodel_data tablemodel;
    int int_id;
    QString ch_dishno;
    QString vch_plan_k3o3_state;
    QString vch_plan_k3o3_id;
    double num_price_add;
    double num_total;
    double num_num;
    double num_price;
    w_bt_dish::kitchenState  s0 = w_bt_dish::KITCHEN_VOID;
    int row0;
    QDateTime curdt = n_func::f_get_sysdatetime();
    QString info;
    QrCodeMaster qr_code_master = cur_qr_code_master();

    lds_query::tran_saction();
    ret_ch_billno = w_scr_dish_restaurant_div_pop::getMaxChbillnoAdd1_cey_u_master("R");
    if(w_scr_dish_restaurant_div_pop::cey_u_master_insert(query, ret_ch_billno)) {
        if(query.execInsert("cey_u_table",qrtVariantPairList()
                            << qrtVariantPair("ch_billno", ret_ch_billno)
                            << qrtVariantPair("ch_tableno", ch_tableno)
                            << qrtVariantPair("int_person", lds_query::selectValue(QString("select int_person from cey_bt_table where ch_tableno = '%1' ").arg(ch_tableno))) // select_table_dialog.ui->lineEdit_int_person->value())
                            << qrtVariantPair("ch_state", 1)
                            << qrtVariantPair("vch_operID", public_sql::gs_operid)

                            << qrtVariantPair("dt_operdate", curdt)
                            << qrtVariantPair("ch_order_state", 0)
                            << qrtVariantPair("vch_waiter", "")
                            << qrtVariantPair("vch_memo", "")

                            << qrtVariantPair("vch_qr_code_sn", qr_code_master.sn)
                            << qrtVariantPair("num_cost", qr_code_master.mount)

                            )) {
            if(w_scr_dish_restaurant_div_pop::cey_bt_table_update(query, ch_tableno, ret_ch_billno, 0)) {
                if(w_scr_dish_restaurant_dialog::cey_bt_table_bar_replace(query, ch_tableno, 0, ret_ch_billno)) {
                    goto orderdish;
                }
            }
        }
    }
    info = query.recordError();
    goto rollback;

orderdish:

    tablemodel.setTable("cey_u_orderdish");
    tablemodel.setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    tablemodel.select(ret_ch_billno, 0, true);
    query.execSelect(QString("select max(int_id) from cey_u_orderdish where ch_billno  = '%1' ")
                     .arg(ret_ch_billno));
    query.next();
    int_id = query.recordValue(0).toInt();

    for(int row = 0; row < standmodel_detail->rowCount(); row ++) {
        int_id ++;
        ch_dishno = standmodel_detail->model_data(row, "ch_dishno").toString();
        num_num = standmodel_detail->model_data(row, "num_num").toDouble();
        num_total = standmodel_detail->model_data(row, "num_total").toDouble();
        num_price = standmodel_detail->model_data(row, "num_price").toDouble();

        //guqing
        info.clear();
        if(!fexpandmain_2::guqing_check(info, ch_dishno, num_num)) {
            goto rollback;
        }
        if(!fexpandmain_2::guqing_update(ch_dishno, num_num)) {
            info = tr("数据库错误")+QString::number(__LINE__);
            goto rollback;
        }
        //tablemodel
        num_price_add =  num_total - num_num * num_price;

        row0 = tablemodel.rowCount();
        tablemodel.insertRow(row);
        w_bt_dish::get_vch_plan_k3o3(vch_plan_k3o3_state, vch_plan_k3o3_id, ch_dishno, s0);
        tablemodel.model_data_set(row0, "ch_billno", ret_ch_billno);
        tablemodel.model_data_set(row0, "ch_tableno", ch_tableno);
        tablemodel.model_data_set(row0, "int_id", int_id);
        tablemodel.model_data_set(row0, "ch_dishno", ch_dishno);
        tablemodel.model_data_set(row0, "num_price", num_price);

        tablemodel.model_data_set(row0, "num_price_org", num_price);
        tablemodel.model_data_set(row0, "num_price_add", num_price_add);
        tablemodel.model_data_set(row0, "num_num", num_num);
        tablemodel.model_data_set(row0, "num_back", 0);
        tablemodel.model_data_set(row0, "int_discount", 100);

        tablemodel.model_data_set(row0, "ch_suitflag", standmodel_detail->model_data(row, "ch_suitflag"));
        tablemodel.model_data_set(row0, "vch_operID", public_sql::gs_operid);
        tablemodel.model_data_set(row0, "dt_operdate", curdt);
        tablemodel.model_data_set(row0, "vch_print_memo", standmodel_detail->model_data(row, "vch_print_memo"));
        tablemodel.model_data_set(row0, "vch_plan_k3o3_id", vch_plan_k3o3_id);

        tablemodel.model_data_set(row0, "vch_plan_k3o3_state", vch_plan_k3o3_state);

        tablemodel.model_data_set(row0, "ch_presentflag", "N");
        tablemodel.model_data_set(row0, "ch_specialflag", "N");
        tablemodel.model_data_set(row0, "int_rate", lds_query::selectValue(QString(" select int_rate from cey_bt_dish where ch_dishno = '%1' ").arg(ch_dishno)));
    }
    if(!tablemodel.try_commit_data(info)) {
        goto rollback;
    }
    if(!qr_code_bill_do_RCV(qr_code_master.sn, info)) {
        goto rollback;
    }
    lds_query::com_mit();
    return true;
rollback:
    lds_query::roll_back();
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, info);
    return false;
}

void w_scr_dish_main_qr_code::updatedetail(const QString &ch_billno_R)
{
    standmodel_detail->removeRows(0, standmodel_detail->rowCount());
    //
    QrCodeMaster d = cur_qr_code_master();
    updatemaster(d);

    if(d.sn.length() > 0) {
        lds_query query;
        query.execSelect(QString(" select *, num_num * num_price + num_price_add as num_toal from cey_u_orderdish where ch_billno = '%1' ").arg(ch_billno_R));
        while(query.next()) {
            standmodel_detail->appendRow(QList<QStandardItem*>()
                                         << lds_model_sqlstandardmodel::newItem(query.recordValue("ch_dishno"))
                                         << lds_model_sqlstandardmodel::newItem(query.recordValue("num_num"))
                                         << lds_model_sqlstandardmodel::newItem(query.recordValue("num_price"))
                                         << lds_model_sqlstandardmodel::newItem(query.recordValue("vch_print_memo"))
                                         << lds_model_sqlstandardmodel::newItem(query.recordValue("num_toal"))
                                         << lds_model_sqlstandardmodel::newItem(query.recordValue("ch_suitflag"))
                                         << lds_model_sqlstandardmodel::newItem("")
                                         );
            if(query.recordValue("ch_suitflag").toString() == "Y")
                standmodel_detail->model_data_set(standmodel_detail->rowCount() - 1, "ch_dishno", int(Qt::AlignVCenter | Qt::AlignRight), Qt::TextAlignmentRole);
        }
    }
    ui->tableView_detail->resizeColumnsToContentsDelay();
}

w_scr_dish_main_qr_code::QrCodeMaster w_scr_dish_main_qr_code::cur_qr_code_master()
{
    int index = ui->comboBox_master->currentIndex();
    QrCodeMaster d;
    d.sn = ui->comboBox_master->model_data(index, 1).toString();
    d.orgName = ui->comboBox_master->model_data(index, 2).toString();
    d.mount= ui->comboBox_master->model_data(index, 3).toDouble();
    d.tableNo = ui->comboBox_master->model_data(index, 4).toString();
    d.date = ui->comboBox_master->model_data(index, 5).toDateTime();
    d.state = billType(ui->comboBox_master->model_data(index, 6).toInt());
    return d;
}

void w_scr_dish_main_qr_code::toupdatedetail(int index)
{
    //!等待
    lds_messagebox_loading_show loading(this, "WAITING...", "WAITING...");
    loading.show();
    //!~等待

    lds_query query;
    if(index >= ui->comboBox_master->count())
        return;
    //
    ui->pushButton_bill_cancel->setEnabled(true);
    standmodel_detail->removeRows(0, standmodel_detail->rowCount());
    QrCodeMaster d = cur_qr_code_master();
    updatemaster(d);
    QList<QrCodeDetail> detail_list = qr_code_detail_get(d.sn);
    for(int k = 0; k < detail_list.count(); k ++) {
        const QrCodeDetail&d = detail_list[k];
        standmodel_detail->appendRow(QList<QStandardItem* >()
                                     << lds_model_sqlstandardmodel::newItem(d.dishNo)
                                     << lds_model_sqlstandardmodel::newItem(d.number)
                                     << lds_model_sqlstandardmodel::newItem(d.price)
                                     << lds_model_sqlstandardmodel::newItem("")
                                     << lds_model_sqlstandardmodel::newItem(d.mount)
                                     << lds_model_sqlstandardmodel::newItem("N")
                                     << lds_model_sqlstandardmodel::newItem(d.nickname)
                                     );
        ;
        //ch_dishno null
        if(lds_query::selectValue(QString(" select ch_dishno from cey_bt_dish where ch_dishno = '%1' " ).arg(standmodel_detail->model_data(standmodel_detail->rowCount() - 1, "ch_dishno").toString())).isNull())
            standmodel_detail->model_data_set(standmodel_detail->rowCount() - 1, "ch_dishno", QColor("red"), Qt::TextColorRole);
        //ch_dishno null
#ifdef QT_DEBUG
#else
        if(lds_query::selectValue(QString(" select ch_dishno from cey_bt_dish where ch_dishno = '%1' " ).arg(d.dishNo)).isNull()) {
            standmodel_detail->model_data_set(standmodel_detail->rowCount() - 1, "ch_dishno", QColor("red"), Qt::TextColorRole);
            if(1 == lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("菜品不同步") + "\n" + d.dishNo, tr("数据同步"), tr("取消"))) {
                standmodel_detail->removeRows(0, standmodel_detail->rowCount());
                ui->label_state->setText(tr("菜品不同步"));
                return;
            } else {
                /*同步*/
                w_sys_manage_cloudsync_with_time_running::exec_keepdata(this);
                k = 0;
                continue;
            }
        }
#endif

        //vch_print_memo
        QString vch_print_memo;
        cJSONHttpData zf = cJSON_Parse(d.zf.toLocal8Bit().data());
        for(int k = 0; k < cJSON_help::array_count(zf.json); k ++ ) {
            vch_print_memo +=  cJSON_help::getItemValue(cJSON_GetArrayItem(zf.json, k)).toString();
        }
        QString ch_suitflag = query.selectValue(QString(" select ch_suitflag from cey_bt_dish where ch_dishno = '%1' ").arg(d.dishNo)).toString();
        //~suit
        if(ch_suitflag != "Y") {
            standmodel_detail->model_data_set(standmodel_detail->rowCount() - 1, "vch_print_memo", vch_print_memo);
        }
        //suit
        if(ch_suitflag == "Y") {
            standmodel_detail->model_data_set(standmodel_detail->rowCount() - 1, "ch_suitflag", "P");
            query.execSelect(QString(" SELECT int_flowId, ch_suitno, ch_dishno, num_num FROM cey_bt_dish_suit where ch_suitno = '%1'; ")
                             .arg(d.dishNo));
            while(query.next()) {
                standmodel_detail->appendRow(QList<QStandardItem* >()
                                             << lds_model_sqlstandardmodel::newItem(query.recordValue("ch_dishno"))
                                             << lds_model_sqlstandardmodel::newItem(d.number * query.recordValue("num_num").toDouble())
                                             << lds_model_sqlstandardmodel::newItem(lds_query::selectValue(QString(" select num_price from cey_bt_dish where ch_dishno = '%1' ").arg(query.recordValue("ch_dishno").toString())))
                                             << lds_model_sqlstandardmodel::newItem(vch_print_memo)
                                             << lds_model_sqlstandardmodel::newItem(0.0)
                                             << lds_model_sqlstandardmodel::newItem("Y")
                                             << lds_model_sqlstandardmodel::newItem(d.nickname)
                                             );
                //子套菜右对齐
                standmodel_detail->model_data_set(standmodel_detail->rowCount() - 1, "ch_dishno", int(Qt::AlignVCenter | Qt::AlignRight), Qt::TextAlignmentRole);
            }
        }
    }
    ui->tableView_detail->resizeColumnsToContentsDelay();
}

void w_scr_dish_main_qr_code::took()
{
    //!等待
    lds_messagebox_loading_show loading(this, "WAITING...", "WAITING...");
    loading.show();
    //!~等待

    if(load_u_orderdish_transaction(ret_ch_billno)) {
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, tr("操作成功"));
        this->accept();
    }
}

void w_scr_dish_main_qr_code::tocancel()
{
    this->reject();
}

void w_scr_dish_main_qr_code::tobillcancel()
{
    if(1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, ui->comboBox_master->currentText() + "," + ui->pushButton_bill_cancel->text(), tr("确定"), tr("取消"))) {
        return;
    }

    //!等待
    lds_messagebox_loading_show loading(this, "WAITING...", "WAITING...");
    loading.show();
    //!~等待

    QString errstring;
    QrCodeMaster d = cur_qr_code_master();
    if(qr_code_bill_do_CANCEL(d.sn, errstring)) {
        for(int k = 0; k < ui->comboBox_master->count(); k++) {
            if(ui->comboBox_master->model_data(k, 1).toString() == d.sn) {
                ui->comboBox_master->removeItem(k);
            }
        }
        if(ui->comboBox_master->count() == 0) {
            lds_messagebox::information(this, MESSAGE_TITLE_VOID, tr("没有订单信息"));
            this->reject();
        } else {
            updatemaster();
            lds_messagebox::information(this, MESSAGE_TITLE_VOID, tr("操作成功"));
        }
        return;
    }
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
}

void w_scr_dish_main_qr_code::updatemaster(const w_scr_dish_main_qr_code::QrCodeMasterList &master_list, const QString &currentOrderSn)
{
    ui->comboBox_master->clear();
    QStandardItemModel *m = qobject_cast<QStandardItemModel *>(ui->comboBox_master->model());

    for(int k = 0; k < master_list.count(); k ++) {
        m->appendRow(QList<QStandardItem*>()
                     << lds_model_sqlstandardmodel::newItem(tr("订单号") +":" +master_list[k].sn)
                     << lds_model_sqlstandardmodel::newItem(master_list[k].sn)
                     << lds_model_sqlstandardmodel::newItem(master_list[k].orgName)
                     << lds_model_sqlstandardmodel::newItem(master_list[k].mount)
                     << lds_model_sqlstandardmodel::newItem(master_list[k].tableNo)
                     << lds_model_sqlstandardmodel::newItem(master_list[k].date)
                     << lds_model_sqlstandardmodel::newItem(master_list[k].state)
                     )
                ;
    }
    ui->comboBox_master->setCurrentIndex(qMax(0, indexof(master_list, currentOrderSn)));//最小值0
    updatemaster();
}

void w_scr_dish_main_qr_code::updatemaster(const w_scr_dish_main_qr_code::QrCodeMaster &d)
{
    ui->label_state->setText(getBillTypeDesc(d.state));
    ui->label_total->setText(QString::number(d.mount, 'f', 2));
    ui->label_orgName->setText(d.orgName);
    ui->label_date->setText(tr("下单时间") + ":" + d.date.toString("yyyy-MM-dd hh:mm:ss"));
    ui->label_bill_count->setText(tr("订单总数") + ":" + QString::number(ui->comboBox_master->count()));
    ui->label_tableNo->setText(tr("桌台") + ":" + ch_tableno);
}

void w_scr_dish_main_qr_code::updatemaster()
{
    QrCodeMaster d = cur_qr_code_master();
    updatemaster(d);
}

void w_scr_dish_main_qr_code::init()
{
    standmodel_detail = new lds_model_sqlstandardmodel(this);
    standmodel_detail->setHorizontalHeaderLabels(QStringList()  << "ch_dishno" << "num_num" << "num_price" << "vch_print_memo" << "num_total" << "ch_suitflag"<< "vch_operid");

    lds_model_sqltablemodel_delegate_com *ch_dishno_d = new lds_model_sqltablemodel_delegate_com(this, "cey_bt_dish", "ch_dishno", "vch_dishname");
    ch_dishno_d->set_paint_transfer_default_value(tr("菜品不存在"));
    ui->tableView_detail->setModel(standmodel_detail);
    ui->tableView_detail->setTransferHheader();
    ui->tableView_detail->setItemDelegateForColumn(standmodel_detail->fieldIndex("ch_dishno"), ch_dishno_d);
    ui->tableView_detail->setItemDelegateForColumn(standmodel_detail->fieldIndex("vch_print_memo"), new lds_model_sqltablemodel_delegate_com(this,  "vch_print_memo"));
    ui->tableView_detail->setItemDelegateForColumn(standmodel_detail->fieldIndex("ch_suitflag"), new lds_model_sqltablemodel_delegate_com(this,
                                                                                                                                          ldsVariantMap("P", tr("主套菜"), "N", "", "Y", tr("子套菜"))));

    //comboBox_master
    ui->comboBox_master->setModel(new QStandardItemModel(this));
    this->setWindowTitle(MESSAGE_TITLE_VOID);
    ui->label_total_desc->setText(tr("总计") + ":");

    connect(ui->pushButton_ok, SIGNAL(clicked()), this, SLOT(took()));
    connect(ui->pushButton_cancel, SIGNAL(clicked()), this, SLOT(tocancel()));
}
