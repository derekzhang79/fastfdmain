#include "w_sys_manage_cloudsync_rb_order.h"
#include "ui_w_sys_manage_cloudsync_rb_order.h"
#include <QSqlError>
#include <QtDebug>
#include "lds_messagebox.h"
#include "cjson_help.h"
#include "cJSON.h"
#include "lds.h"
#include "lds_query.h"

w_sys_manage_cloudsync_rb_order::w_sys_manage_cloudsync_rb_order(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_sys_manage_cloudsync_rb_order)
{
    ui->setupUi(this);


    QSqlDatabase db = create_database();
    lds_query_db query(db);
    db.transaction();
    query.execInsert("cey_u_orderdish",
                     " '136', 'Q17040700001', 'Q000', '1', '10017', '16.00', '16.00', '0.00', '1.000', '0.000', '', '', '100', '', '', '', '', '', '', 'N', '', 'N', 'N', '', '', '0000', '2017-04-07 10:08:36', '', 'P17040714100001', '', '100000000000', '011111', '', '', '', '0.00', '' ",
                     ","
                     );
    query.execInsert("cey_u_orderdish",
                     " '137', 'Q17040700001', 'Q000', '2', '10018', '18.00', '18.00', '0.00', '1.000', '0.000', '', '', '100', '', '', '', '', '', '', 'N', '', 'N', 'N', '', '', '0000', '2017-04-07 10:08:36', '', 'P17040714100001', '', '100000000000', '011111', '', '', '', '0.00', '' ",
                     ","
                     );
    query.execInsert("cey_u_orderdish",
                     " '138', 'Q17040700001', 'Q000', '3', '10019', '16.00', '16.00', '0.00', '1.000', '0.000', '', '', '100', '', '', '', '', '', '', 'N', '', 'N', 'N', '', '', '0000', '2017-04-07 10:08:37', '', 'P17040714100001', '', '100000000000', '011111', '', '', '', '0.00', '' ",
                     ","
                     );

    query.execInsert("cey_u_orderdish",
                     " '136', '100000000000', '011111', '2017-04-07 10:08:36', 'Q17040700001', '10017'  ",
                     ","
                     );
    query.execInsert("cey_u_orderdish",
                     " '137', '100000000000', '011111', '2017-04-07 10:08:36', 'Q17040700001', '10018'  ",
                     ","
                     );
    query.execInsert("cey_u_orderdish",
                     " '138', '100000000000', '011111', '2017-04-07 10:08:37', 'Q17040700001', '10019'  ",
                     ","
                     );

    db.commit();


}

w_sys_manage_cloudsync_rb_order::~w_sys_manage_cloudsync_rb_order()
{
    delete ui;
}

bool w_sys_manage_cloudsync_rb_order::create_table()
{
    lds_query_db query(create_database());

    ///cey_u_orderdish_log
    if(!query.execCreate(
                " create table if not exists rb_order_u_table ("
                " `sn` varchar(10) PRIMARY KEY,"
                " `tableNo` varchar(10) ,"
                " `orgName` varchar(64) ,"
                " `mount` decimal(12,2) ,"
                " `unPayMount` decimal(12,2) ,"
                " `state` varchar(1) ,"
                " `date` datetime "
                ")"
                )) {
        return false;
    }

    ///cey_u_orderdish_print_tr
    if(!query.execCreate(
                " create table if not exists cey_u_orderdish_print_tr ("
                " `int_orderflow` INTEGER PRIMARY KEY ,"
                " `vch_plan_k3o3_id` varchar(12) DEFAULT NULL,"
                " `vch_plan_k3o3_state` varchar(6) DEFAULT NULL,"
                " `dt_operdate` datetime DEFAULT NULL,"
                " `ch_billno` varchar(15) DEFAULT NULL,"
                " `ch_dishno` varchar(7) DEFAULT NULL"
                ")"
                )) {
        return false;
    }

    ///cey_u_orderdish
    if(!query.execCreate(
                " create table if not exists cey_u_orderdish ("
                " `int_flowID` INTEGER PRIMARY KEY  AUTOINCREMENT,"
                " `ch_billno` varchar(15) NOT NULL,"
                " `ch_tableno` varchar(4) NOT NULL,"
                " `int_id` int(11) NOT NULL,"
                " `ch_dishno` varchar(7) DEFAULT NULL,"
                " `num_price` decimal(12,2) DEFAULT '0.00',"
                " `num_price_org` decimal(12,2) DEFAULT '0.00',"
                " `num_price_add` decimal(12,2) DEFAULT '0.00',"
                " `num_num` decimal(12,3) DEFAULT '0.000',"
                " `num_back` decimal(12,3) DEFAULT '0.000',"
                " `vch_back_operID` varchar(6) DEFAULT NULL,"
                " `dt_back_operdate` datetime DEFAULT NULL,"
                " `int_discount` int(11) DEFAULT NULL,"
                " `vch_dis_operID` varchar(6) DEFAULT NULL,"
                " `dt_dis_operdate` datetime DEFAULT NULL,"
                " `vch_memberno` varchar(20) DEFAULT NULL,"
                " `ch_consult` varchar(1) DEFAULT NULL,"
                " `ch_printflag` varchar(1) DEFAULT NULL,"
                " `vch_print_memo` varchar(1023) DEFAULT NULL,"
                " `ch_suitflag` varchar(1) DEFAULT NULL,"
                " `ch_suitno` varchar(7) DEFAULT NULL,"
                " `ch_specialflag` varchar(1) DEFAULT NULL,"
                " `ch_presentflag` varchar(1) DEFAULT NULL,"
                " `vch_pre_operID` varchar(6) DEFAULT NULL,"
                " `dt_pre_operdate` datetime DEFAULT NULL,"
                " `vch_operID` varchar(6) DEFAULT NULL,"
                " `dt_operdate` datetime DEFAULT NULL,"
                " `ch_togono` varchar(15) DEFAULT NULL,"
                " `ch_payno` varchar(15) DEFAULT NULL,"
                " `ch_outflag` varchar(1) DEFAULT NULL,"
                " `vch_plan_k3o3_id` varchar(12) DEFAULT NULL,"
                " `vch_plan_k3o3_state` varchar(6) DEFAULT NULL,"
                " `vch_plan_desc` varchar(1023) DEFAULT NULL,"
                " `ch_yun_already_keep` varchar(1)  DEFAULT NULL,"
                " `vch_wx_serial` varchar(32) DEFAULT NULL,"
                " `int_rate` decimal(12,2) DEFAULT '0.00',"
                " `ch_member_belong_state` varchar(1) DEFAULT NULL"
                ")"
                )) {
        return false;
    }
    return true;
}

QSqlDatabase w_sys_manage_cloudsync_rb_order::create_database()
{
    if(!QSqlDatabase::connectionNames().contains("rb_order")) {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "rb_order");
        db.setDatabaseName("userdata/settings/db_rb_order.db");
        if(db.open()) {
            create_table();
        } else {
            qDebug() << db.lastError();
        }
    }
    return  QSqlDatabase::database("rb_order");
}

void w_sys_manage_cloudsync_rb_order::to_get_kichen_date_from_rb_server()
{
    QNetworkAccessManager m;
    QEventLoop loop;

    QObject::connect(&m, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));

    QNetworkRequest request;
    request.setUrl(QString("http://192.168.1.114:8080/rest/user/paiddishorder/list/000000020002000300020002"));
    //    "http://www.yqspos.com/you/rest/user/paiddishPracsorder/listorgcode/" + lds::terminalCode;
    QNetworkReply *rply = m.get(request);

    //    loop.exec();
}

int w_sys_manage_cloudsync_rb_order::toget(QNetworkReply *x, QString *errstring)
{
    QByteArray ret = x->readAll();
    cJSON *retparse=cJSON_Parse(ret.data());
    cJSON_auto_del d(retparse);
    if(!retparse) {
        qDebug("retparse is null");
        return 0;
    }

    cJSON *retparse2 = cJSON_GetObjectItem(retparse, "payingorder");
    int count = cJSON_GetArraySize(retparse2);
    QString  vch_plan_k3o3_id, vch_plan_k3o3_state, ch_billno, ch_dishno;
    int int_id = -1;

    QSqlDatabase db = create_database();
    lds_query_db query(db);
    db.transaction();
    for(int k = 0; k < count; k++) {
        cJSON *pItem = cJSON_GetArrayItem(retparse2, k);

        ch_billno = cJSON_help::getcJSONvalue(pItem, "billno").toString();
        ch_dishno = cJSON_help::getcJSONvalue(pItem, "dishid").toString();
        if(-1 == int_id) {
            query.execSelect(QString("SELECT max(int_id) from cey_u_orderdish where ch_billno = '%1' ")
                             .arg(ch_billno));
            query.next();
            int_id = query.recordValue(0).toInt();
        }
        int_id ++;

        ///cey_u_orderdish
        if(!query.execInsert("cey_u_orderdish",qrtVariantPairList()
                             << qrtVariantPair("ch_billno", ch_billno)
                             << qrtVariantPair("ch_tableno", cJSON_help::getcJSONvalue(pItem, "tableno"))
                             << qrtVariantPair("int_id", int_id)
                             << qrtVariantPair("ch_dishno", ch_dishno)
                             << qrtVariantPair("num_price", cJSON_help::getcJSONvalue(pItem, "price"))

                             << qrtVariantPair("num_price_org", cJSON_help::getcJSONvalue(pItem, "price"))
                             << qrtVariantPair("num_price_add", 0)
                             << qrtVariantPair("num_num", cJSON_help::getcJSONvalue(pItem, "qty"))
                             << qrtVariantPair("num_back", 0)
                             << qrtVariantPair("int_discount", 100)

                             << qrtVariantPair("vch_print_memo", "")
                             << qrtVariantPair("ch_suitflag", "N")
                             << qrtVariantPair("ch_suitno", "")
                             << qrtVariantPair("ch_presentflag", "N")
                             << qrtVariantPair("vch_operID", "0000")

                             << qrtVariantPair("dt_operdate", cJSON_help::getcJSONvalue(pItem, "ordertime"))
                             << qrtVariantPair("ch_payno", cJSON_help::getcJSONvalue(pItem, "payno"))
                             << qrtVariantPair("vch_plan_k3o3_id", vch_plan_k3o3_id)
                             << qrtVariantPair("vch_plan_k3o3_state", vch_plan_k3o3_state)
                             << qrtVariantPair("vch_plan_desc", ""))) {
            *errstring = QString::number(__LINE__)+query.recordError();
            db.rollback();
            return -1;
        }
        ///~cey_u_orderdish

        ///cey_u_orderdish_print_tr
        if(!query.execInsert("cey_u_orderdish_print_tr",
                             QString("select int_flowID as `int_orderflow`, "
                                     "vch_plan_k3o3_id, "
                                     "vch_plan_k3o3_state, "
                                     "dt_operdate, "
                                     "ch_billno,"

                                     "ch_dishno from cey_u_orderdish where ch_billno = '%1' and int_id = %2 ")
                             .arg(ch_billno)
                             .arg(int_id)
                             )) {
            *errstring = QString::number(__LINE__)+query.recordError();
            db.rollback();
            return -1;
        }
        ///~cey_u_orderdish_print_tr
    }
    db.commit();
    return count;
}

void w_sys_manage_cloudsync_rb_order::on_pushButton_clicked()
{
    to_get_kichen_date_from_rb_server();
}


rb_order_data::rb_order_data()
{
    isrunning = false;
}

void rb_order_data::get_kichen_date_from_rb_server()
{
    QNetworkAccessManager m;
    QEventLoop loop;
    QObject::connect(&m, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
    QNetworkReply *reply;
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader,  "application/json;charset=utf-8");
    request.setRawHeader("terminalCode",  "FA1101");
    request.setRawHeader("state",  "1");//已下单
    request.setRawHeader("tableNo",  "0002");//已下单
    request.setUrl(QUrl("http://115.28.155.165/qryqs/dishREST/queryUserOrders-rest"));
    reply = m.get(request);
    loop.exec();

    cJSONHttpData root = cJSON_Parse(reply->readAll().data());
    root.httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if(200 == root.httpStatus) {
        cJSON *array = cJSON_help::getcJSON(root.json, "data");
        for(int k = 0; k < cJSON_help::array_count(array); k ++) {
            qDebug() << "sn" << cJSON_help::getcJSONvalue(cJSON_GetArrayItem(array, k), "sn");/*
            qDebug() << "date"<< cJSON_help::getcJSONvalue(cJSON_GetArrayItem(array, k), "date");
            qDebug() << "tableNo"<< cJSON_help::getcJSONvalue(cJSON_GetArrayItem(array, k), "tableNo");*/

        }
    }
}

bool rb_order_data::mk_sn_file(const QString &sn, const QString &orgName, const QString &date)
{
    lds_query query;
    QNetworkAccessManager m;
    QEventLoop loop;
    QNetworkRequest request;
    QNetworkReply *reply;
    QObject::connect(&m, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
    request.setHeader(QNetworkRequest::ContentTypeHeader,  "application/json;charset=utf-8");
    request.setRawHeader("terminalCode",  "FA1101");
    request.setRawHeader("orderSn",  sn.toLocal8Bit().data());//sn
    request.setUrl(QUrl("http://115.28.155.165/qryqs/dishREST/getOrderDishInfo-rest"));
    reply = m.get(request);
    loop.exec();

    cJSONHttpData root = cJSON_Parse(reply->readAll().data());
    root.httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if(200 == root.httpStatus) {
        cJSON *array = cJSON_help::getcJSON(root.json, "data");
        for(int k = 0; k < cJSON_help::array_count(array); k ++) {
            cJSON *item = cJSON_GetArrayItem(array, k);
//            cJSON_AddItemToObject(item, "orgName", cJSON_CreateString(orgName.toLocal8Bit().data()));
//            cJSON_AddItemToObject(item, "date", cJSON_CreateString(date.toLocal8Bit().data()));

//            query.execSelect(" select ");
//            cJSON_GetObjectItem(item, "zf");//["101", "454]
//            cJSON_GetObjectItem(item, "tc");//["111,22", "099", "22"]
            qDebug() << __LINE__ << "sn" << sn<< "nickname:" << cJSON_help::getcJSONvalue(item, "nickname").toString();
        }
//        root.jsonPrint();
    }
}
