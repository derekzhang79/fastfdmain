#include "w_sys_manage_cloudsync.h"
//u云同步
#include "lds_messagebox.h"
#include <QSqlError>

#include <QTimer>
#include "cjson_help.h"
#include <QProcess>
#include <QtDebug>
#include "lds.h"
#include "lds.h"
#include "lds_query.h"
#include "w_sys_manage_cloudsync_with_time.h"
#include "public_sql.h"
#include "n_func.h"

QMap<QString, QString> w_sys_manage_cloudsync::yun_MemberParameter;
QMap<QString, QString> w_sys_manage_cloudsync::yun_MemberReadcard;
QMap<QString, QString> w_sys_manage_cloudsync::yun_MemberType;
cJSON *w_sys_manage_cloudsync::json_yun_parameter=NULL;
cJSON *w_sys_manage_cloudsync::json_yun_readcard=NULL;
cJSON *w_sys_manage_cloudsync::json_yun_membertype=NULL;
cJSON *w_sys_manage_cloudsync::json_yun_readcard_select=NULL;
bool w_sys_manage_cloudsync::is_match_local_time=false;

/**
 * 构造函数
 */
w_sys_manage_cloudsync::w_sys_manage_cloudsync()
{
}

w_sys_manage_cloudsync::~w_sys_manage_cloudsync()
{
}

cJSONHttpData w_sys_manage_cloudsync::get_yun_httppos(QWidget *parent, const QString &http_key, const QString &param, const QMap<QString, QString> &headInfo, QString &retparam, const QString &msgtitle, bool ispop, bool timecheck, int mesc)
{
    cJSONHttpData ret_json_err;
    w_sys_manage_cloudsync_transfer transfer;
    yunTime ydt;
    QString errstring;
    int check_result;
    if(timecheck
            &&!is_match_local_time
            &&1 != (check_result = check_local_time(parent, ydt, &errstring))){
        ret_json_err.json = NULL;
        //-1云端失败 0时间不符 1时间相符
        if(-1 == check_result){
            ret_json_err.message = errstring;//tr("连接云端失败");
        }
        if(0 == check_result){
            ret_json_err.message = tr("软件未进行时间校对,可以通过以下操作校对:\n1.云同步数据设置->启动云端->重启\n2.云同步数据设置->时间校对");
        }
    } else {
        ret_json_err = transfer.sendPost2(http_key, param, headInfo,retparam, mesc);
    }
    //! 获取map
    if(200 != ret_json_err.httpStatus) {
        QString msgerr=msgtitle+"\n"+ret_json_err.message;
        if(parent && ispop){
            lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, msgerr);
        } else {
        }
    } else {
    }
end:
    return ret_json_err;
}

cJSONHttpData w_sys_manage_cloudsync::get_yun_httppos(QWidget *parent, const QString &http_key, const QString &param, const QMap<QString, QString> &headInfo, const QString &msgtitle, bool ispop, bool timecheck, int mesc)
{
    QString empty;
    return get_yun_httppos(parent, http_key, param, headInfo, empty, msgtitle, ispop, timecheck, mesc);
}

cJSONHttpData w_sys_manage_cloudsync::get_yun_httpget(QWidget *parent, const QString &http_key, const QMap<QString, QString> &headInfo, QString &retparam, const QString &msgtitle, bool ispop, bool timecheck, int mesc)
{
    cJSONHttpData ret_json_err;
    w_sys_manage_cloudsync_transfer transfer;
    yunTime ydt;
    QString msg;
    int check_result;
    if(timecheck
            &&!is_match_local_time
            &&1 != (check_result = check_local_time(parent, ydt, &msg))){
        if(-1 == check_result){
            ret_json_err.message = msg;//tr("连接云端失败");
        }
        if(0 == check_result){
            ret_json_err.message = tr("软件未进行时间校对,可以通过以下操作校对:\n1.云同步数据设置->启动云端->重启\n2.云同步数据设置->时间校对");
        }
    } else {
        //! 获取map
        ret_json_err = transfer.sendGet(http_key, headInfo,retparam, mesc);
    }
    if(ret_json_err.httpStatus != 200){
        QString msgerr=msgtitle+tr("失败\n")+ret_json_err.message;
        if(parent && ispop){
            lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, msgerr);
        } else {
        }
    }
end:
    return ret_json_err;
}

cJSONHttpData w_sys_manage_cloudsync::get_yun_httpget(QWidget *parent, const QString &http_key, const QMap<QString, QString> &headInfo, const QString &msgtitle, bool ispop, bool timecheck, int mesc)
{
    QString empty;
    return get_yun_httpget(parent, http_key, headInfo, empty, msgtitle, ispop, timecheck, mesc);
}

QString w_sys_manage_cloudsync::print_yun(cJSON *print)
{
    if(print==NULL) {
        return "";
    }

    char* arrayDate = cJSON_Print(print);
    QString param=QString(arrayDate);
    free(arrayDate);
    return param;
}

void w_sys_manage_cloudsync::printalljson_child(cJSON *print)
{
    cJSON *tmp=print;
    while(tmp){
        tmp=tmp->child;
    }
}
/*  "vip_ic_machine": "明华RF",
"member_bank_print": "1",
"discountCard": "0",多出的
"local_member_select": "0",
"findMemberCard": "1",多出的
"m_deposit_negative": "0",
"vip_card_type": "磁卡",
"rechargeableCard": "1",多出的
"deposit_pay_all": "0",
"input_memberno": "0",
"m_fetch_base": "1",
"member_ATT_mode": "0",
"m_member_pay": "1",
"vip_ic_baud": "0",
"vip_ic_port": "USB",
"m_card_paydiscount": "0",
"discount_view": "1",
"vip_ic_password": "",
"m_max_deposit": "30,000"*/

QString w_sys_manage_cloudsync::get_yun_member_parameter(QWidget *parent,const QString &key, bool *ok)
{
    if(ok) *ok = false;
    QString ret;
    if(json_yun_parameter == NULL){
        QMap<QString, QString> headInfo;

        //headInfo.insert("machineCode","");
        headInfo.insert("orgCode",n_func::f_get_sysparm("orgCode"));
        headInfo.insert("userName",n_func::f_get_sysparm("cloud_user"));
        headInfo.insert("password",n_func::f_get_sysparm("cloud_password"));
        QString httpkey="http://" + n_func::f_get_sysparm("cloud_ip")+
                "/CateSystem/cvs/member/parameter-rest";
        cJSONHttpData httpd = get_yun_httpget(parent, httpkey, headInfo,tr("获取云会员参数"));
        json_yun_parameter = httpd.json;
        httpd.json = 0;//cJSONHttpData 析构时不会析构
    }

    if(json_yun_parameter == NULL) return "";
    if(ok) *ok = true;
    if(cJSON_GetArraySize(json_yun_parameter) > 0){
        ret = cJSON_help::getcJSONvalue(json_yun_parameter, key.toLocal8Bit().data()).toString();
    }

    print_yun(json_yun_parameter);

    return ret;
}

bool w_sys_manage_cloudsync::get_yun_member_parameter(const QString &key, QString &retValue, QString &retErr)
{
    if(json_yun_parameter == NULL){
        QMap<QString, QString> headInfo;

        //headInfo.insert("machineCode","");
        headInfo.insert("orgCode",n_func::f_get_sysparm("orgCode"));
        headInfo.insert("userName",n_func::f_get_sysparm("cloud_user"));
        headInfo.insert("password",n_func::f_get_sysparm("cloud_password"));
        QString httpkey="http://" + n_func::f_get_sysparm("cloud_ip")+
                "/CateSystem/cvs/member/parameter-rest";
        cJSONHttpData httpd = get_yun_httpget(0, httpkey, headInfo,tr("获取云会员参数"));
        json_yun_parameter = httpd.json;
        httpd.json = 0;//cJSONHttpData 析构时不会析构

        if(httpd.httpStatus == 200) {
            retValue = cJSON_help::getcJSONvalue(json_yun_parameter, key.toLocal8Bit().data()).toString();
            return true;
        }
        retErr = httpd.message;
        return false;
    }

    retValue = cJSON_help::getcJSONvalue(json_yun_parameter, key.toLocal8Bit().data()).toString();
    return true;
}

/*
  "vch_memberno": "Y10004",
  "vch_member": "10004",
  "ch_typeno": "11",
  "dt_birthday": null,
  "vch_handtel": "13324569874",
  "vch_tel": "",

  "dt_limit": "2015-07-31 12:00:00",
  "ch_state": "1",
  "num_limit": null,
  "ch_cardflag": "Y",
  "vch_cardno": "10004",
  "int_basedata": 5,

  "vch_typename": "优惠卡1",
  "ch_favourable": "2",
  "ch_pricekind": null,
  "int_discount": 10,
  "ch_calc": "N",
  "ch_calc_type": "1",

  "int_calc_unit": 1,
  "ch_area_flag": "0",
  "int_backcash_rate": 0,
  "int_card_baseamount": 50,
  "password": null,
  "num_amount": 200,

  "num_hangamount": 0,
  "num_point": 0,
  "vch_operID": null,
  "dt_operdate": null,
  "ch_attribute": "",
  "ch_payno": "",

  "ch_paymodeno": "",
  "num_pay": 0,
  "ch_balanceno": "",

  "int_checkflow": ""
*/
QString w_sys_manage_cloudsync::get_yun_member_readcard(QWidget *parent, const QString cardNo, const QString &operId, const QString &key, bool &ok, const QString &def)
{
    QString ret;
    if(json_yun_readcard==NULL
            || (json_yun_readcard!=NULL && cJSON_help::getcJSONvalue(json_yun_readcard, "vch_memberno").toString() != cardNo)){
        if(json_yun_readcard != NULL){
            cJSON_Delete(json_yun_readcard);json_yun_readcard=NULL;
        }
        cJSONHttpData pItem = cJSON_CreateObject();
        cJSON_help::filter_cJSON_AddStringToObject(pItem.json,"cardNo", cardNo);
        cJSON_help::filter_cJSON_AddStringToObject(pItem.json,"operId", operId);
        QString param=pItem.jsonToString();

        //获取map
        QMap<QString, QString> headInfo;

        //headInfo.insert("machineCode","");
        headInfo.insert("orgCode",n_func::f_get_sysparm("orgCode"));
        headInfo.insert("userName",n_func::f_get_sysparm("cloud_user"));
        headInfo.insert("password",n_func::f_get_sysparm("cloud_password"));
        QString http_key="http://" + n_func::f_get_sysparm("cloud_ip")+
                "/CateSystem/cvs/member/dukFuk-rest";
        cJSONHttpData httpd = get_yun_httppos(parent, http_key, param, headInfo,tr("云会员读卡"));
        json_yun_readcard =  httpd.json;
        httpd.json = 0;
    }

    if(json_yun_readcard==NULL) {
        ok = false;
        return "";
    }

    if(cJSON_GetArraySize(json_yun_readcard) > 0){
        ret = cJSON_help::getcJSONvalue(json_yun_readcard, key.toLocal8Bit().data()).toString();
    }
    ok = true;
    return ret;
}

bool w_sys_manage_cloudsync::get_yun_member_readcard(const QString &cardNo, const QString &operId, const QString &key, QString &retValue, QString &retErrstring)
{
    if(json_yun_readcard==NULL
            || (cJSON_help::getcJSONvalue(json_yun_readcard, "vch_memberno").toString() != cardNo)){
        if(json_yun_readcard != NULL){
            cJSON_Delete(json_yun_readcard);json_yun_readcard=NULL;
        }
        cJSONHttpData pItem = cJSON_CreateObject();
        cJSON_help::filter_cJSON_AddStringToObject(pItem.json,"cardNo", cardNo);
        cJSON_help::filter_cJSON_AddStringToObject(pItem.json,"operId", operId);
        QString param=pItem.jsonToString();

        //获取map
        QMap<QString, QString> headInfo;

        //headInfo.insert("machineCode","");
        headInfo.insert("orgCode",n_func::f_get_sysparm("orgCode"));
        headInfo.insert("userName",n_func::f_get_sysparm("cloud_user"));
        headInfo.insert("password",n_func::f_get_sysparm("cloud_password"));
        QString http_key="http://" + n_func::f_get_sysparm("cloud_ip")+
                "/CateSystem/cvs/member/dukFuk-rest";
        cJSONHttpData httpd = get_yun_httppos(0, http_key, param, headInfo,tr("云会员读卡"));
        json_yun_readcard =  httpd.json;
        httpd.json = 0;

        if(httpd.httpStatus == 200) {
            retValue = cJSON_help::getcJSONvalue(json_yun_readcard, key.toLocal8Bit().data()).toString();
            return true;
        }
        retErrstring = httpd.message;
        return false;
    }

    retValue = cJSON_help::getcJSONvalue(json_yun_readcard, key.toLocal8Bit().data()).toString();
    return true;
}

//"message": "ok",
//"maxOrderNo": "1"
QString w_sys_manage_cloudsync::get_yun_member_maxorderno(QWidget *parent, const QString &key, const QString &flag, bool &ok, const QString &def)
{
    QString ret;
    QString param;
    //获取map
    QMap<QString, QString> headInfo;
    ok = false;

    //headInfo.insert("machineCode","");
    headInfo.insert("orgCode",n_func::f_get_sysparm("orgCode"));
    headInfo.insert("userName",n_func::f_get_sysparm("cloud_user"));
    headInfo.insert("password",n_func::f_get_sysparm("cloud_password"));
    headInfo.insert("flag",flag);
    QString http_key="http://" + n_func::f_get_sysparm("cloud_ip")+
            "/CateSystem/cvs/member/maxOrderNo-rest";
#ifdef QT_DEBUG
    parent->objectName();
#endif
    cJSONHttpData root = get_yun_httppos(parent, http_key, param, headInfo, tr("获取最大订单号"));
    if(root.json==NULL) return ret;
    if(cJSON_GetArraySize(root.json) > 0){
        ret = cJSON_help::getcJSONvalue(root.json, key.toLocal8Bit().data()).toString();
    }
    ok = true;
    return ret;
}
//"message": "ok",
//"maxOrderNo": "1"
QString w_sys_manage_cloudsync::get_yun_member_maxmasterno(QWidget *parent,const QString &key, const QString &flag, bool &ok, const QString &def)
{
    QString ret;
    QString param;
    //获取map
    QMap<QString, QString> headInfo;
    ok = false;

    //headInfo.insert("machineCode","");
    headInfo.insert("orgCode",n_func::f_get_sysparm("orgCode"));
    headInfo.insert("userName",n_func::f_get_sysparm("cloud_user"));
    headInfo.insert("password",n_func::f_get_sysparm("cloud_password"));
    headInfo.insert("flag",flag);
    QString http_key="http://" + n_func::f_get_sysparm("cloud_ip")+
            "/CateSystem/cvs/member/maxMasterNo-rest";
#ifdef QT_DEBUG
    parent->objectName();
#endif
    cJSONHttpData root = get_yun_httppos(parent, http_key, param, headInfo,tr("获取最大支付号"));
    if(root.json==NULL) return ret;
    if(cJSON_GetArraySize(root.json) > 0){
        ret = cJSON_help::getcJSONvalue(root.json, key.toLocal8Bit().data()).toString();
    }
    ok = true;
    return ret;
}

/*
  "cvsMemberType": {
    "sn": "5",
    "chTypeno": "11",
    "vchTypename": "优惠卡1",
    "chAttribute": "2",
    "chFavourable": "2",
    "chPricekind": null,
    "intDiscount": 10,
    "chProjectno": null,
    "chCalc": "N",
    "chCalcType": "1",
    "intCalcUnit": 1,
    "chAreaFlag": "0",
    "intBackcashRate": 0,
    "intCardBaseamount": 50,
    "orgCode": "000000060001",
    "date": "2015-07-02 18:27:22",
    "chAttributeStr": "充值卡",
    "chFavourableStr": "固定折扣",
    "idStr": "5",
    "operate": null
},
"message":"ok"*/
QString w_sys_manage_cloudsync::get_yun_member_membertype(QWidget *parent,const QString &typeNo, const QString &key, bool &ok, const QString &def)
{
    if(json_yun_membertype==NULL
            || (json_yun_membertype!=NULL && typeNo!=cJSON_help::getcJSONvalue(cJSON_GetObjectItem(json_yun_membertype, "cvsMemberType"), "chTypeno").toString())
            ){
        cJSON *pItem = cJSON_CreateObject();
        cJSON_help::filter_cJSON_AddStringToObject(pItem,"typeNo", typeNo);
        char* arrayDate = cJSON_Print(pItem);
        QString param=QString(arrayDate);
        cJSON_Delete(pItem);
        free(arrayDate);
        //获取map
        QMap<QString, QString> headInfo;

        //headInfo.insert("machineCode","");
        headInfo.insert("orgCode",n_func::f_get_sysparm("orgCode"));
        headInfo.insert("userName",n_func::f_get_sysparm("cloud_user"));
        headInfo.insert("password",n_func::f_get_sysparm("cloud_password"));
        QString http_key="http://" + n_func::f_get_sysparm("cloud_ip")+
                "/CateSystem/cvs/member/getMemberType-rest";

#ifdef QT_DEBUG
        parent->objectName();
#endif
        cJSONHttpData httpd = get_yun_httppos(parent, http_key, param, headInfo,tr("获取云会员类型"));//, def, "cvsMemberType"
        json_yun_membertype = httpd.json;
        httpd.json = 0;
    }

    if(json_yun_membertype==NULL) {
        ok = false;
        return "";
    }

    ok = true;
    return cJSON_help::getcJSONvalue(cJSON_GetObjectItem(json_yun_membertype,"cvsMemberType"), key.toLocal8Bit().data()).toString();
}

bool w_sys_manage_cloudsync::get_yun_member_membertype(const QString &typeNo, const QString &key, QString &retValue, QString &retErrstring)
{
    if(json_yun_membertype==NULL
            || (typeNo!=cJSON_help::getcJSONvalue(cJSON_GetObjectItem(json_yun_membertype, "cvsMemberType"), "chTypeno").toString())
            ){
        cJSON *pItem = cJSON_CreateObject();
        cJSON_help::filter_cJSON_AddStringToObject(pItem,"typeNo", typeNo);
        char* arrayDate = cJSON_Print(pItem);
        QString param=QString(arrayDate);
        cJSON_Delete(pItem);
        free(arrayDate);
        //获取map
        QMap<QString, QString> headInfo;

        //headInfo.insert("machineCode","");
        headInfo.insert("orgCode",n_func::f_get_sysparm("orgCode"));
        headInfo.insert("userName",n_func::f_get_sysparm("cloud_user"));
        headInfo.insert("password",n_func::f_get_sysparm("cloud_password"));
        QString http_key="http://" + n_func::f_get_sysparm("cloud_ip")+
                "/CateSystem/cvs/member/getMemberType-rest";

        cJSONHttpData httpd = get_yun_httppos(0, http_key, param, headInfo,tr("获取云会员类型"));//, def, "cvsMemberType"
        json_yun_membertype = httpd.json;
        httpd.json = 0;

        if(httpd.httpStatus == 200) {
            retValue = cJSON_help::getcJSONvalue(cJSON_GetObjectItem(json_yun_membertype,"cvsMemberType"), key.toLocal8Bit().data()).toString();
            return true;
        }
        retErrstring = httpd.message;
        return false;
    }
    retValue = cJSON_help::getcJSONvalue(cJSON_GetObjectItem(json_yun_membertype,"cvsMemberType"), key.toLocal8Bit().data()).toString();
    return true;
}

bool w_sys_manage_cloudsync::get_yun_member_membertypelist(QWidget *parent, QMap<QString, QVariant> &retlist)
{
    QMap<QString, QString> headInfo;

    headInfo.insert("orgCode",n_func::f_get_sysparm("orgCode"));
    headInfo.insert("userName",n_func::f_get_sysparm("cloud_user"));
    headInfo.insert("password",n_func::f_get_sysparm("cloud_password"));
    QString httpkey="http://" + n_func::f_get_sysparm("cloud_ip")+
            "/CateSystem/cvs/member/memberTypeList-rest";

    cJSONHttpData receive_json=get_yun_httpget(parent, httpkey, headInfo, tr("获取会员类别列表"));
    if(receive_json.json == NULL) return false;

    for(int i =0, gcount=cJSON_GetArraySize(receive_json.json);i <gcount;i++){
        cJSON *pItem = cJSON_GetArrayItem(receive_json.json, i);
        retlist.insert(
                    cJSON_help::getcJSONvalue(pItem,"chTypeno").toString(),
                    cJSON_help::getcJSONvalue(pItem,"vchTypename").toString());
    }
    return true;
}

yunpayData w_sys_manage_cloudsync::upload_yun_pay_info(QWidget *parent, member_up_yun &muy, const QString &ch_payno)
{

    cJSON *root = cJSON_CreateObject();
    cJSON *subarray;
    //会员x4
    if(!muy.memberPointListList.isEmpty())
    {
        cJSON_AddItemToObject(root, "memberPointList", subarray=cJSON_CreateArray());
        foreach(const memberPointList& p, muy.memberPointListList)    cJSON_AddItemToArray(subarray,  muy.make_JSON_Array(p, n_func::f_get_sysparm("orgCode")));
    }
    if(!muy.memberPayList.isEmpty())
    {
        cJSON_AddItemToObject(root, "memberPay", subarray=cJSON_CreateArray());
        foreach(const memberPay& p, muy.memberPayList)    cJSON_AddItemToArray(subarray,  muy.make_JSON_Array(p, n_func::f_get_sysparm("orgCode")));
    }
    if(!muy.memberCuramountList.isEmpty())
    {
        cJSON_AddItemToObject(root, "memberCuramount", subarray=cJSON_CreateArray());
        foreach(const memberCuramount& p, muy.memberCuramountList)    cJSON_AddItemToArray(subarray,  muy.make_JSON_Array(p, n_func::f_get_sysparm("orgCode")));
    }
    if(!muy.memberDepositList.isEmpty()){
        cJSON_AddItemToObject(root, "memberDepositList", subarray=cJSON_CreateArray());
        foreach(const memberDeposit& p, muy.memberDepositList)    cJSON_AddItemToArray(subarray,  muy.make_JSON_Array(p, n_func::f_get_sysparm("orgCode")));
    }

    //付款记录 x5
    QString ch_billno_local;
    lds_query query;
    query.execSelect(QString("select * from cey_u_checkout_master where ch_payno = '%1' ; ").arg(ch_payno));
    query.next();
    ch_billno_local = query.recordValue("ch_billno").toString();
    //checkOutMaster
    query.execSelect(QString("select * from cey_u_checkout_master where ch_payno = '%1' ; ").arg(ch_payno));
    query.next();
    cJSON_AddItemToObject(root, "checkOutMaster",  muy.q_u_get_piece_cey_u_checkout_master(query, n_func::f_get_sysparm("orgCode")));
    //orderMaster
    query.execSelect(QString("select * from cey_u_master where ch_billno = '%1' ; ").arg(ch_billno_local));
    query.next();
    cJSON_AddItemToObject(root, "orderMaster",  muy.q_u_get_piece_cey_u_master(query, n_func::f_get_sysparm("orgCode")));
    //tableRecordsConsumption
    query.execSelect(QString("select * from cey_u_table where ch_payno = '%1' ; ").arg(ch_payno));
    query.next();
    cJSON_AddItemToObject(root, "tableRecordsConsumption",  muy.q_u_get_piece_cey_u_table(query, n_func::f_get_sysparm("orgCode")));
    //orderDish
    query.execSelect(QString("select * from cey_u_orderdish where ch_payno = '%1' ; ").arg(ch_payno));
    cJSON_AddItemToObject(root, "orderDish", subarray = cJSON_CreateArray());
    while(query.next()) {
        cJSON_AddItemToArray(subarray,  muy.q_u_get_piece_cey_u_orderdish(query, n_func::f_get_sysparm("orgCode")));
    }
    //checkOutDetail
    query.execSelect(QString("select * from cey_u_checkout_detail where ch_payno = '%1' ; ").arg(ch_payno));
    cJSON_AddItemToObject(root, "checkOutDetail", subarray = cJSON_CreateArray());
    while(query.next()) {
        cJSON_AddItemToArray(subarray,  muy.q_u_get_piece_cey_u_checkout_detail(query, n_func::f_get_sysparm("orgCode")));
    }

    //
    QString param = cJSON_help::getcJSONprint(root);
    cJSON_Delete(root);
    root=NULL;

    //开始向云端插入
    //获取map
    QMap<QString, QString> headInfo;
    //headInfo.insert("machineCode","");
    headInfo.insert("orgCode",n_func::f_get_sysparm("orgCode"));
    headInfo.insert("userName",n_func::f_get_sysparm("cloud_user"));
    headInfo.insert("password",n_func::f_get_sysparm("cloud_password"));
    yunpayData ret;
    QString http_key="http://" + n_func::f_get_sysparm("cloud_ip")+
            "/CateSystem/cvs/member/fuk-rest";
    QString retparam;
#ifdef QT_DEBUG
    parent->objectName();
#endif
    cJSONHttpData receive_json=get_yun_httppos(parent, http_key, param, headInfo, retparam,tr("向云端提交云会员信息"));
    ret.code = cJSON_help::getcJSONvalue(receive_json.json, "code").toString();
    ret.maxOrderNo = cJSON_help::getcJSONvalue(receive_json.json, "maxOrderNo").toString();
    ret.message = cJSON_help::getcJSONvalue(receive_json.json, "message").toString();

    return ret;
}

bool w_sys_manage_cloudsync::get_yun_member_recharge(QWidget *parent,const QString &cardNo, const QString &operId, const QString &chongzje, const QString &shisje, const QString &zhiffs)
{
    QString param;
    QMap<QString, QString> headInfo;

    //headInfo.insert("machineCode","");
    headInfo.insert("orgCode",n_func::f_get_sysparm("orgCode"));
    headInfo.insert("userName",n_func::f_get_sysparm("cloud_user"));
    headInfo.insert("password",n_func::f_get_sysparm("cloud_password"));
    QString httpkey="http://" + n_func::f_get_sysparm("cloud_ip")+
            "/CateSystem/cvs/member/chongz-rest";
    {//param
        cJSON *pItem = cJSON_CreateObject();
        cJSON_help::filter_cJSON_AddStringToObject(pItem,"cardNo", cardNo);
        cJSON_help::filter_cJSON_AddStringToObject(pItem,"operId", operId);
        cJSON_help::filter_cJSON_AddStringToObject(pItem,"chongzje", chongzje);
        cJSON_help::filter_cJSON_AddStringToObject(pItem,"shisje", shisje);
        cJSON_help::filter_cJSON_AddStringToObject(pItem,"zhiffs", zhiffs);
        char* arrayDate = cJSON_Print(pItem);
        param=QString(arrayDate);
        cJSON_Delete(pItem);
        free(arrayDate);
    }

#ifdef QT_DEBUG
    parent->objectName();
#endif
    cJSONHttpData receive_json=get_yun_httppos(parent, httpkey, param, headInfo, tr("连锁会员充值"));

    return receive_json.httpStatus == 200;
}

bool w_sys_manage_cloudsync::get_yun_member_blankcard(QWidget *parent,const QString &cardNo, const QString &tuikje, const QString &zhiffs)
{
    QString param;
    QMap<QString, QString> headInfo;

    //headInfo.insert("machineCode","");
    headInfo.insert("orgCode",n_func::f_get_sysparm("orgCode"));
    headInfo.insert("userName",n_func::f_get_sysparm("cloud_user"));
    headInfo.insert("password",n_func::f_get_sysparm("cloud_password"));
    QString httpkey="http://" + n_func::f_get_sysparm("cloud_ip")+
            "/CateSystem/cvs/member/tuik-rest";
    {//param
        cJSON *pItem = cJSON_CreateObject();
        cJSON_help::filter_cJSON_AddStringToObject(pItem,"cardNo", cardNo);
        cJSON_help::filter_cJSON_AddStringToObject(pItem,"tuikje", tuikje);
        cJSON_help::filter_cJSON_AddStringToObject(pItem,"orgCode", n_func::f_get_sysparm("orgCode"));
        cJSON_help::filter_cJSON_AddStringToObject(pItem,"zhiffs", zhiffs);
        char* arrayDate = cJSON_Print(pItem);
        param=QString(arrayDate);
        cJSON_Delete(pItem);
        free(arrayDate);
    }

#ifdef QT_DEBUG
    parent->objectName();
#endif
    cJSONHttpData receive_json=get_yun_httppos(parent, httpkey, param, headInfo, tr("退卡"));
    return receive_json.httpStatus == 200;
}

bool w_sys_manage_cloudsync::get_yun_member_fetch(QWidget *parent,const QString &cardNo, const QString &operId, const QString &qukje, const QString &zhiffs)
{
    QString param;
    QMap<QString, QString> headInfo;

    //headInfo.insert("machineCode","");
    headInfo.insert("orgCode",n_func::f_get_sysparm("orgCode"));
    headInfo.insert("userName",n_func::f_get_sysparm("cloud_user"));
    headInfo.insert("password",n_func::f_get_sysparm("cloud_password"));
    QString httpkey="http://" + n_func::f_get_sysparm("cloud_ip")+
            "/CateSystem/cvs/member/quk-rest";
    {//param
        cJSON *pItem = cJSON_CreateObject();
        cJSON_help::filter_cJSON_AddStringToObject(pItem,"cardNo", cardNo);
        cJSON_help::filter_cJSON_AddStringToObject(pItem,"operId", operId);
        cJSON_help::filter_cJSON_AddStringToObject(pItem,"qukje", qukje);
        cJSON_help::filter_cJSON_AddStringToObject(pItem,"zhiffs", zhiffs);
        char* arrayDate = cJSON_Print(pItem);
        param=QString(arrayDate);
        cJSON_Delete(pItem);
        free(arrayDate);
    }

#ifdef QT_DEBUG
    parent->objectName();
#endif
    cJSONHttpData receive_json=get_yun_httppos(parent, httpkey, param, headInfo, tr("取款"));
    return receive_json.httpStatus == 200;
}

bool w_sys_manage_cloudsync::get_yun_member_add(QWidget *parent,const membernewparam &value)
{
    QString param;
    QMap<QString, QString> headInfo;

    //headInfo.insert("machineCode","");
    headInfo.insert("orgCode",n_func::f_get_sysparm("orgCode"));
    headInfo.insert("userName",n_func::f_get_sysparm("cloud_user"));
    headInfo.insert("password",n_func::f_get_sysparm("cloud_password"));
    QString httpkey="http://" + n_func::f_get_sysparm("cloud_ip")+
            "/CateSystem/cvs/member/save-rest";
    {//param
        cJSON *pItem = cJSON_CreateObject();
        cJSON_help::filter_cJSON_AddStringToObject(pItem, "miChCardFlag", value.miChCardFlag);
        cJSON_help::filter_cJSON_AddStringToObject(pItem, "miChState", value.miChState);
        cJSON_help::filter_cJSON_AddStringToObject(pItem, "miChStateStr", value.miChStateStr);
        cJSON_help::filter_cJSON_AddStringToObject(pItem, "miChTypeNo", value.miChTypeNo);

        cJSON_help::filter_cJSON_AddStringToObject(pItem, "miDtBirthday", value.miDtBirthday);
        cJSON_help::filter_cJSON_AddStringToObject(pItem, "miDtLimit", value.miDtLimit);
        cJSON_help::filter_cJSON_AddStringToObject(pItem, "miDtOperDate", value.miDtOperDate);
        cJSON_help::filter_cJSON_AddStringToObject(pItem, "miDtSendTime", value.miDtSendTime);
        cJSON_help::filter_cJSON_AddStringToObject(pItem, "idStr", value.idStr);

        cJSON_help::filter_cJSON_AddStringToObject(pItem, "miIntBasedata", value.miIntBasedata);
        cJSON_help::filter_cJSON_AddStringToObject(pItem, "miIntBasedataStr", value.miIntBasedataStr);
        cJSON_help::filter_cJSON_AddStringToObject(pItem, "nosn", value.nosn);
        cJSON_help::filter_cJSON_AddStringToObject(pItem, "numLimit", value.numLimit);
        cJSON_help::filter_cJSON_AddStringToObject(pItem, "operate", value.operate);

        cJSON_help::filter_cJSON_AddStringToObject(pItem, "orgCode", value.orgCode);
        cJSON_help::filter_cJSON_AddStringToObject(pItem, "remark", value.remark);
        cJSON_help::filter_cJSON_AddStringToObject(pItem, "miVchAddress", value.miVchAddress);
        cJSON_help::filter_cJSON_AddStringToObject(pItem, "miVchCardNo", value.miVchCardNo);
        cJSON_help::filter_cJSON_AddStringToObject(pItem, "miVchHandTel", value.miVchHandTel);

        cJSON_help::filter_cJSON_AddStringToObject(pItem, "miVchId", value.miVchId);
        cJSON_help::filter_cJSON_AddStringToObject(pItem, "miVchMember", value.miVchMember);
        cJSON_help::filter_cJSON_AddStringToObject(pItem, "miVchMemberNo", value.miVchMemberNo);
        cJSON_help::filter_cJSON_AddStringToObject(pItem, "miVchOperId", value.miVchOperId);
        cJSON_help::filter_cJSON_AddStringToObject(pItem, "miVchPassword", value.miVchPassword);

        cJSON_help::filter_cJSON_AddStringToObject(pItem, "miVchTel", value.miVchTel);

        char* arrayDate = cJSON_Print(pItem);
        param=QString(arrayDate);

        cJSON_Delete(pItem);
        free(arrayDate);
    }

#ifdef QT_DEBUG
    parent->objectName();
#endif
    cJSONHttpData receive_json=get_yun_httppos(parent, httpkey, param, headInfo, tr("新增会员卡"));
    return receive_json.httpStatus == 200;
}

bool w_sys_manage_cloudsync::get_yun_member_send(QWidget *parent,const QString &cardNo)
{
    QString param;
    QString orgCode;
    QMap<QString, QString> headInfo;

    orgCode = n_func::f_get_sysparm("orgCode");
    //headInfo.insert("machineCode","");
    headInfo.insert("orgCode", orgCode);
    headInfo.insert("userName",n_func::f_get_sysparm("cloud_user"));
    headInfo.insert("password",n_func::f_get_sysparm("cloud_password"));
    QString httpkey="http://" + n_func::f_get_sysparm("cloud_ip")+
            "/CateSystem/cvs/member/fak-rest";
    {//param
        cJSON *pItem = cJSON_CreateObject();
        cJSON_help::filter_cJSON_AddStringToObject(pItem,"cardNo", cardNo);
        cJSON_help::filter_cJSON_AddStringToObject(pItem,"orgCode",orgCode);
        char* arrayDate = cJSON_Print(pItem);
        param=QString(arrayDate);
        cJSON_Delete(pItem);
        free(arrayDate);
    }

#ifdef QT_DEBUG
    parent->objectName();
#endif
    cJSONHttpData receive_json=get_yun_httppos(parent, httpkey, param, headInfo, tr("云会员发卡"));
    return 200 == receive_json.httpStatus;
}

bool w_sys_manage_cloudsync::get_yun_member_select_pre100(QWidget *parent, const QString &filter, QList<yunMemberSelectInfo > &selects)
{
    QString httpkey;
    QMap<QString, QString> headInfo;
    int rowcount;
getyunmlist:
    if(json_yun_readcard_select==NULL){
        httpkey="http://" + n_func::f_get_sysparm("cloud_ip")+
                "/CateSystem/cvs/member/memberCardList-rest";
        headInfo.clear();
        headInfo.insert("orgCode",n_func::f_get_sysparm("orgCode"));
        headInfo.insert("userName",n_func::f_get_sysparm("cloud_user"));
        headInfo.insert("password",n_func::f_get_sysparm("cloud_password"));
        headInfo.insert("param",filter);//!开始会检索所有的，后面会按照条件来检索
        cJSONHttpData httpd = get_yun_httpget(parent, httpkey, headInfo, tr("检索云端会员"));
        json_yun_readcard_select=httpd.json;
        httpd.json = 0;
    }
    // 访问失败
    if(json_yun_readcard_select==NULL)
        return false;
    rowcount = cJSON_GetArraySize(json_yun_readcard_select);
    //! 固定是前五个
    for(int row = 0; row < rowcount && selects.count() <= 5/*最多显示5个*/; row++){
        cJSON *pItem = cJSON_GetArrayItem(json_yun_readcard_select, row);
        yunMemberSelectInfo select;
        select.mname = cJSON_help::getcJSONvalue(pItem, "miVchMember").toString();
        select.mhandtel =  cJSON_help::getcJSONvalue(pItem, "miVchHandTel").toString();
        select.mcardno = cJSON_help::getcJSONvalue(pItem, "miVchMemberNo").toString();

        if(select.mhandtel.contains(filter)
                ||select.mcardno.contains(filter)){
            selects.append(select);
        }
    }
    //获取数据为空或者云端返回为空
    if(selects.isEmpty() || rowcount == 0){
        cJSON_Delete(json_yun_readcard_select);
        json_yun_readcard_select=NULL;
        //云端返回不为空则重新获取
        if(rowcount > 0)  {
            goto getyunmlist;
        }
    }

    return rowcount > 0;
}

bool w_sys_manage_cloudsync::get_yun_member_byanyvchmemberno(QWidget *parent, const QString &cardNo, membernewparam &arg0)
{
    QString ret;
    QString param;
    QMap<QString, QString> headInfo;

    headInfo.insert("orgCode",n_func::f_get_sysparm("orgCode"));
    headInfo.insert("userName",n_func::f_get_sysparm("cloud_user"));
    headInfo.insert("password",n_func::f_get_sysparm("cloud_password"));
    QString httpkey="http://" + n_func::f_get_sysparm("cloud_ip")+
            "/CateSystem/cvs/member/getCvsMemberInfoByCard-rest";
    {//param
        cJSON *pItem = cJSON_CreateObject();
        cJSON_help::filter_cJSON_AddStringToObject(pItem,"cardNo", cardNo);
        char* arrayDate = cJSON_Print(pItem);
        param=QString(arrayDate);
        cJSON_Delete(pItem);
        free(arrayDate);
    }

#ifdef QT_DEBUG
    parent->objectName();
#endif
    cJSONHttpData receive_json=get_yun_httppos(parent, httpkey, param, headInfo, tr("获取会员信息"));
    if(receive_json.json == NULL) return false;
    ret = receive_json.message;

    //===
    arg0.idStr = cJSON_help::getcJSONvalue(receive_json.json, "idStr").toString();
    arg0.miChCardFlag = cJSON_help::getcJSONvalue(receive_json.json, "miChCardFlag").toString();
    arg0.miChState = cJSON_help::getcJSONvalue(receive_json.json, "miChState").toString();
    arg0.miChStateStr = cJSON_help::getcJSONvalue(receive_json.json, "miChStateStr").toString();

    arg0.miChTypeNo = cJSON_help::getcJSONvalue(receive_json.json, "miChTypeNo").toString();
    arg0.miDtBirthday = cJSON_help::getcJSONvalue(receive_json.json, "miDtBirthday").toString();
    arg0.miDtLimit = cJSON_help::getcJSONvalue(receive_json.json, "miDtLimit").toString();
    arg0.miDtOperDate = cJSON_help::getcJSONvalue(receive_json.json, "miDtOperDate").toString();
    arg0.miDtSendTime = cJSON_help::getcJSONvalue(receive_json.json, "miDtSendTime").toString();

    arg0.miIntBasedata = cJSON_help::getcJSONvalue(receive_json.json, "miIntBasedata").toString();
    arg0.miIntBasedataStr = cJSON_help::getcJSONvalue(receive_json.json, "miIntBasedataStr").toString();
    arg0.miVchAddress = cJSON_help::getcJSONvalue(receive_json.json, "miVchAddress").toString();
    arg0.miVchCardNo = cJSON_help::getcJSONvalue(receive_json.json, "miVchCardNo").toString();
    arg0.miVchHandTel = cJSON_help::getcJSONvalue(receive_json.json, "miVchHandTel").toString();

    arg0.miVchId = cJSON_help::getcJSONvalue(receive_json.json, "miVchId").toString();
    arg0.miVchMember = cJSON_help::getcJSONvalue(receive_json.json, "miVchMember").toString();
    arg0.miVchMemberNo = cJSON_help::getcJSONvalue(receive_json.json, "miVchMemberNo").toString();
    arg0.miVchOperId = cJSON_help::getcJSONvalue(receive_json.json, "miVchOperId").toString();
    arg0.miVchPassword = cJSON_help::getcJSONvalue(receive_json.json, "miVchPassword").toString();

    arg0.miVchTel = cJSON_help::getcJSONvalue(receive_json.json, "miVchTel").toString();
    arg0.nosn = cJSON_help::getcJSONvalue(receive_json.json, "nosn").toString();
    arg0.numLimit = cJSON_help::getcJSONvalue(receive_json.json, "numLimit").toString();
    arg0.operate = cJSON_help::getcJSONvalue(receive_json.json, "operate").toString();
    arg0.orgCode = cJSON_help::getcJSONvalue(receive_json.json, "orgCode").toString();

    arg0.remark = cJSON_help::getcJSONvalue(receive_json.json, "remark").toString();
    //===
    return true;
}

bool w_sys_manage_cloudsync::put_yun_member_point_add_sub(QWidget *parent, const QString &memberno, const QString &type, const QString &payno, float point, const QString &memo, const QString &operid)
{
    QString param;
    QMap<QString, QString> headInfo;

    headInfo.insert("orgCode",n_func::f_get_sysparm("orgCode"));
    headInfo.insert("userName",n_func::f_get_sysparm("cloud_user"));
    headInfo.insert("password",n_func::f_get_sysparm("cloud_password"));
    QString httpkey="http://" + n_func::f_get_sysparm("cloud_ip")+
            "/CateSystem/cvs/member/memberTypeList-rest";
    {//param
        cJSON *pItem = cJSON_CreateObject();
        cJSON_help::filter_cJSON_AddStringToObject(pItem,"memberno", memberno);
        cJSON_help::filter_cJSON_AddStringToObject(pItem,"type", type);
        cJSON_help::filter_cJSON_AddStringToObject(pItem,"payno", payno);
        cJSON_AddNumberToObject(pItem,"point", point);
        cJSON_help::filter_cJSON_AddStringToObject(pItem,"memo", memo);
        cJSON_help::filter_cJSON_AddStringToObject(pItem,"operid", operid);
        char* arrayDate = cJSON_Print(pItem);
        param=QString(arrayDate);
        cJSON_Delete(pItem);
        free(arrayDate);
    }
#ifdef QT_DEBUG
    parent->objectName();
#endif
    cJSONHttpData receive_json=get_yun_httppos(parent, httpkey, param, headInfo, tr("会员积分充值"));
    if(receive_json.json == NULL) return false;

    return true;
}

w_sys_manage_cloudsync::member_r_f_b_info w_sys_manage_cloudsync::get_yun_member_r_f_b(QWidget *parent, const QString &operId, const QString &dateStart, const QString &dateEnd, bool &ok)
{
    member_r_f_b_info ret;
    QString param;
    QMap<QString, QString> headInfo;
    ok = false;

    headInfo.insert("orgCode",n_func::f_get_sysparm("orgCode"));
    headInfo.insert("userName",n_func::f_get_sysparm("cloud_user"));
    headInfo.insert("password",n_func::f_get_sysparm("cloud_password"));

    {//param
        cJSON *pItem = cJSON_CreateObject();
        cJSON_help::filter_cJSON_AddStringToObject(pItem,"operId", operId);
        cJSON_help::filter_cJSON_AddStringToObject(pItem,"dateStart", dateStart);
        cJSON_help::filter_cJSON_AddStringToObject(pItem,"dateEnd", dateEnd);
        char* arrayDate = cJSON_Print(pItem);
        param=QString(arrayDate);
        cJSON_Delete(pItem);
        free(arrayDate);
    }
    QString httpkey="http://" + n_func::f_get_sysparm("cloud_ip")+
            "/CateSystem/cvs/member/getCvsMemberOperaInfo-rest";

#ifdef QT_DEBUG
    parent->objectName();
#endif
    cJSONHttpData receive_json=get_yun_httppos(parent, httpkey, param, headInfo, tr("获取会员操作信息"), false);
    if(receive_json.json == NULL) return ret;
    ret.rechargeAmount=QString::number(cJSON_help::getcJSONvalue(receive_json.json, "rechargeAmount").toDouble());
    ret.realRechargeAmount=QString::number(cJSON_help::getcJSONvalue(receive_json.json, "realRechargeAmount").toDouble());
    ret.withdrawalAmount=QString::number(cJSON_help::getcJSONvalue(receive_json.json, "withdrawalAmount").toDouble());
    ret.backCardAmount=QString::number(cJSON_help::getcJSONvalue(receive_json.json, "backCardAmount").toDouble());
    ok = true;
    return ret;
}

void w_sys_manage_cloudsync::json_free()
{
    if(json_yun_parameter!=NULL){cJSON_Delete(json_yun_parameter);json_yun_parameter=NULL;}
    if(json_yun_readcard!=NULL){cJSON_Delete(json_yun_readcard);json_yun_readcard=NULL;}
    if(json_yun_membertype!=NULL){cJSON_Delete(json_yun_membertype);json_yun_membertype=NULL;}
    if(json_yun_readcard_select!=NULL){cJSON_Delete(json_yun_readcard_select);json_yun_readcard_select=NULL;}
}
//-1云端失败 0本地校对失败 1校对成功 2无需校对//
int w_sys_manage_cloudsync::match_local_time(QWidget *parent)
{
    //获取map
    //2
    QString errstring;
    yunTime ydt;
    is_match_local_time = false;
    //-1云端失败 0时间不符 1时间相符
    int ret = check_local_time(parent, ydt, &errstring);
    if(ret == -1) {
        lds_messagebox::warning(parent, MESSAGE_TITLE_VOID,  tr("时间同步失败") + "\n"+ errstring);//tr("请检测云端设置是否正确或者网线是否拔出"));
        return -1;//-1云端失败
    }
    if(ret == 1) {
        return 2;//2无需校对
    }
    if(ret == 0){//0时间不符
        if(public_sql::getdbHostName() == "127.0.0.1"
                || public_sql::localContainIp(public_sql::getdbHostName())
                ){

        } else {
            lds_messagebox::warning(parent, MESSAGE_TITLE_VOID,  tr("时间同步失败") + "\n"+ tr("服务器非本机服务器,请在服务器所在机器上校对时间"));
            return -1;//-1云端失败
        }
        if(0 == lds_messagebox::question(parent, MESSAGE_TITLE_VOID,  tr("时间同步失败") + "\n"+  tr("数据库时间和云端时间不同步,是否校对"), tr("校对"), tr("取消"))){
            //
            bool success = true;
            //再获取一次时间
            bool ok;
            ydt = w_sys_manage_cloudsync::get_yun_srvdt(parent, ok);
            if(!ok) return -1;//-1云端失败

            FILE *fd = popen(QString("date -s '%1' ").arg(ydt.time.toString("yyyy-MM-dd hh:mm:ss")).toLocal8Bit().data() ,"r");
            if(fd < NULL) success = false;
            pclose(fd);
            fd = 0;

            if(success){
                fd = popen("hwclock -w", "r");
                if(fd < NULL)success = false;
                pclose(fd);
                fd = 0;
            }

            //
            if(success){
                is_match_local_time = true;
                lds_messagebox::information(parent, MESSAGE_TITLE_VOID, tr("同步成功"), true);
                return 1;
            } else {
                lds_messagebox::information(parent, MESSAGE_TITLE_VOID, tr("同步失败"));
            }
        }
    }
    return 0;
}

//-1云端失败 0时间不符 1时间相符
int w_sys_manage_cloudsync::check_local_time(QWidget *parent, yunTime &retydt, QString *msg)
{
    //3
    bool ok;
    //"------------begin----------------";
    retydt = w_sys_manage_cloudsync::get_yun_srvdt(parent, ok,msg);
    QDateTime local_dt = QDateTime::currentDateTime();
    // "------------end----------------";
    if(!ok) {
        return -1;//云端异常
    }
    //4
    if(qAbs(local_dt.secsTo(retydt.time)) > 6 * 60 * 60){//时间不能超过6个小时
        if(msg)  *msg = local_dt.toString("yyyy-MM-dd hh:mm:ss") +"-->" + retydt.time.toString("yyyy-MM-dd hh:mm:ss");
        return 0;//时间不相符
    }
    is_match_local_time = true;
    return 1;//时间相符
}

bool w_sys_manage_cloudsync::get_weixin_orderinfo(QObject *parent, const QString &beginTime, const QString &endTime, int downFlag)
{

    lds_query query;
    bool retflag;

    QString param;{//param
        cJSON *pItem = cJSON_CreateObject();
        cJSON_help::filter_cJSON_AddStringToObject(pItem,"beginTime", beginTime);
        cJSON_help::filter_cJSON_AddStringToObject(pItem,"endTime", endTime);
        cJSON_AddNumberToObject(pItem,"downFlag", downFlag);
        char* arrayDate = cJSON_Print(pItem);
        param=QString(arrayDate);
        cJSON_Delete(pItem);
        free(arrayDate);
    }


    lds_query::tran_saction();
    //订单订桌外卖汇总表
    QMap<QString, QString> headInfo;


    //headInfo.insert("machineCode","");
    headInfo.insert("orgCode",n_func::f_get_sysparm("orgCode"));
    headInfo.insert("userName",n_func::f_get_sysparm("cloud_user"));
    headInfo.insert("password",n_func::f_get_sysparm("cloud_password"));
    QString http_key="http://" + n_func::f_get_sysparm("cloud_ip")+
            "/CateSystem/weixinOrder-message/getWebOrderInfo-rest";
    //! getWebOrderInfo_json new

#ifdef QT_DEBUG
    parent->objectName();
#endif
    cJSONHttpData getWebOrderInfo_json=get_yun_httppos(0, http_key, param, headInfo, tr("索取微信订单号"), false);//将所有的微信订单号获取

    if(getWebOrderInfo_json.json==NULL) goto failure;


    //遍历所有的云端 订单订桌外卖汇总表
    for(int i =0;i<cJSON_GetArraySize(getWebOrderInfo_json.json);i++){//for
        cJSON *pItem = cJSON_GetArrayItem(getWebOrderInfo_json.json,i);

        if("1"==cJSON_help::getcJSONvalue(pItem,"paystate").toString()){//1 已支付 0未支付
            QString param;{ //param
                cJSON_AddStringToObject(pItem,"orderSn", cJSON_help::getcJSONvalue(pItem,"sn").toByteArray());
                char* arrayDate = cJSON_Print(pItem);
                param=QString(arrayDate);
                cJSON_Delete(pItem);
                free(arrayDate);
            }

            //根据每单外卖汇总表,获取 下载支付流水
            QString http_key=
                    "http://" + n_func::f_get_sysparm("cloud_ip")
                    +"/CateSystem/weixinOrder-message/getPayForWater-rest";


#ifdef QT_DEBUG
            parent->objectName();
#endif
            cJSONHttpData getPayForWater_json=get_yun_httppos(0, http_key, param, headInfo, tr("下载支付流水"), false);
            if(getPayForWater_json.json==NULL)  goto failure; //获取流水号失败
            if(query.execInsert("cey_web_pay_for_water",qrtVariantPairList()

                                << qrtVariantPair("sn",cJSON_help::getcJSONvalue(getPayForWater_json.json,"sn"))
                                << qrtVariantPair("orderSn",cJSON_help::getcJSONvalue(getPayForWater_json.json,"orderSn"))
                                << qrtVariantPair("mount",cJSON_help::getcJSONvalue(getPayForWater_json.json,"mount"))
                                << qrtVariantPair("note",cJSON_help::getcJSONvalue(getPayForWater_json.json,"note"))
                                << qrtVariantPair("openid",cJSON_help::getcJSONvalue(getPayForWater_json.json,"openid"))

                                << qrtVariantPair("orgCode",cJSON_help::getcJSONvalue(getPayForWater_json.json,"orgCode"))
                                << qrtVariantPair("date",cJSON_help::getcJSONvalue(getPayForWater_json.json,"date"))
                                << qrtVariantPair("upLoadFlag", 0))) {

            } else {
                goto failure;
            }

        }//! //==1

        //更新或插入支付流水信息,若是已有流水表,则更具云端更新对应的状态
        query.execSelect(QString("select * from cey_web_order_info where sn='%1' ").arg(cJSON_help::getcJSONvalue(pItem,"sn").toString()));
        if(query.next()){

            if(query.execUpdate("cey_web_order_info", qrtVariantPairList()
                                << qrtVariantPair("orderState", cJSON_help::getcJSONvalue(pItem,"orderState")),
                                QString(" sn = '%1'")
                                .arg(cJSON_help::getcJSONvalue(pItem,"sn").toString()))) {
            } else {

                goto failure;
            }
        } else {
            if(query.execInsert("cey_web_order_info",qrtVariantPairList()

                                << qrtVariantPair("sn", cJSON_help::getcJSONvalue(pItem,"sn"))
                                << qrtVariantPair("eatTime", cJSON_help::getcJSONvalue(pItem,"eatTime"))
                                << qrtVariantPair("eatPels", cJSON_help::getcJSONvalue(pItem,"eatPels"))
                                << qrtVariantPair("tableAreaNo", cJSON_help::getcJSONvalue(pItem,"eatTable"))
                                << qrtVariantPair("tel", cJSON_help::getcJSONvalue(pItem,"tel"))

                                << qrtVariantPair("orderer", cJSON_help::getcJSONvalue(pItem,"orderer"))
                                << qrtVariantPair("sex", cJSON_help::getcJSONvalue(pItem,"sex"))
                                << qrtVariantPair("note", cJSON_help::getcJSONvalue(pItem,"note"))
                                << qrtVariantPair("address", cJSON_help::getcJSONvalue(pItem,"address"))
                                << qrtVariantPair("sendTime", cJSON_help::getcJSONvalue(pItem,"sendTime"))

                                << qrtVariantPair("payType", cJSON_help::getcJSONvalue(pItem,"payType"))
                                << qrtVariantPair("payState", cJSON_help::getcJSONvalue(pItem,"payState"))
                                << qrtVariantPair("orderType", cJSON_help::getcJSONvalue(pItem,"orderType"))
                                << qrtVariantPair("orderState", cJSON_help::getcJSONvalue(pItem,"orderState"))
                                << qrtVariantPair("openid", cJSON_help::getcJSONvalue(pItem,"openid"))

                                << qrtVariantPair("orgCode", cJSON_help::getcJSONvalue(pItem,"orgCode"))
                                << qrtVariantPair("date", cJSON_help::getcJSONvalue(pItem,"date"))
                                << qrtVariantPair("refuseReason", cJSON_help::getcJSONvalue(pItem,"refuseReason"))
                                << qrtVariantPair("downFlag", cJSON_help::getcJSONvalue(pItem,"downFlag")))) {

            } else {

                goto failure;
            }
        }
    }//! for

    //根据当前信息,提示

    query.execSelect(QString("select * from cey_web_order_info where (orderType='1' or orderType='2') and orderState='01' and date >= '%1'")
                     .arg(n_func::f_get_sysdatetime().toString("yyyy-MM-dd")));
    if(query.next()){
        QMetaObject::invokeMethod(parent,
                                  "sendError",
                                  Qt::QueuedConnection,
                                  Q_ARG(QString, QObject::tr("新的网络订单通知")),
                                  Q_ARG(QString, QObject::tr("您有新的网络订单通知"))
                                  );
    }

    query.execSelect(QString("select * from cey_web_order_info where (orderType='1' or orderType='2') and orderState='02' and date >= '%1'")
                     .arg(n_func::f_get_sysdatetime().toString("yyyy-MM-dd")));
    if(query.next()){
        QMetaObject::invokeMethod(parent,
                                  "sendError",
                                  Qt::QueuedConnection,
                                  Q_ARG(QString, QObject::tr("新的网络退订申请通知")),
                                  Q_ARG(QString, QObject::tr("您有新的网络退订申请通知"))
                                  );
    }
    goto success;
failure:
    lds_query::roll_back();
    retflag = false;
    goto end;
success:
    lds_query::com_mit();
    retflag = true;
end:

    return retflag;
}

bool w_sys_manage_cloudsync::get_weixin_orderdish(QObject *parent, const QString &beginTime, const QString &endTime, int downFlag)
{
    bool retflag;

    //遍历订单菜品明细表,插入本地
    lds_query query;
    lds_query::tran_saction();
    QString param;{//param
        cJSON *pItem = cJSON_CreateObject();
        cJSON_help::filter_cJSON_AddStringToObject(pItem,"beginTime", beginTime);
        cJSON_help::filter_cJSON_AddStringToObject(pItem,"endTime", endTime);
        cJSON_AddNumberToObject(pItem,"downFlag", downFlag);
        char* arrayDate = cJSON_Print(pItem);
        param=QString(arrayDate);
        cJSON_Delete(pItem);
        free(arrayDate);
    }
    QMap<QString, QString> headInfo;

    //headInfo.insert("machineCode","");
    headInfo.insert("orgCode",n_func::f_get_sysparm("orgCode"));
    headInfo.insert("userName",n_func::f_get_sysparm("cloud_user"));
    headInfo.insert("password",n_func::f_get_sysparm("cloud_password"));
    QString http_key="http://" + n_func::f_get_sysparm("cloud_ip")+
            "/CateSystem/weixinOrder-message/getWebOrderDish-rest";

#ifdef QT_DEBUG
    parent->objectName();
#endif
    cJSONHttpData getWebOrderDish_json=get_yun_httppos(0, http_key, param, headInfo, tr("遍历订单菜品明细表"), false);
    if(getWebOrderDish_json.json==NULL) goto failure;

    for(int i =0;i<cJSON_GetArraySize(getWebOrderDish_json.json);i++){//for
        cJSON *pItem = cJSON_GetArrayItem(getWebOrderDish_json.json,i);

        if(query.execInsert("cey_web_order_dish",qrtVariantPairList()

                            << qrtVariantPair("sn",cJSON_help::getcJSONvalue(pItem,"sn"))
                            << qrtVariantPair("orderSn",cJSON_help::getcJSONvalue(pItem,"orderSn"))
                            << qrtVariantPair("dishNo",cJSON_help::getcJSONvalue(pItem,"dishNo"))
                            << qrtVariantPair("dishName",cJSON_help::getcJSONvalue(pItem,"dishName"))
                            << qrtVariantPair("number",cJSON_help::getcJSONvalue(pItem,"number"))

                            << qrtVariantPair("price",cJSON_help::getcJSONvalue(pItem,"price"))
                            << qrtVariantPair("note",cJSON_help::getcJSONvalue(pItem,"note"))
                            << qrtVariantPair("orgCode",cJSON_help::getcJSONvalue(pItem,"orgCode"))
                            << qrtVariantPair("date",cJSON_help::getcJSONvalue(pItem,"date"))
                            << qrtVariantPair("downFlag",cJSON_help::getcJSONvalue(pItem,"downFlag")))){

        } else {
            goto failure;
        }

        //插入本地后下载确认,更新云端状态
        QString param;{//param
            cJSON *pItem0 = cJSON_CreateObject();
            cJSON_AddStringToObject(pItem0,"sn", cJSON_help::getcJSONvalue(pItem,"sn").toByteArray());
            char* arrayDate = cJSON_Print(pItem0);
            param=QString(arrayDate);
            cJSON_Delete(pItem0);
            free(arrayDate);
        }
        QString http_key="http://" + n_func::f_get_sysparm("cloud_ip")+
                "/CateSystem/weixinOrder-message/confirmWebOrderDish-rest";

#ifdef QT_DEBUG
        parent->objectName();
#endif
        cJSONHttpData confirmWebOrderDish = get_yun_httppos(0, http_key, param, headInfo, tr("插入本地后下载确认,更新云端状态") ,false);
        if(confirmWebOrderDish.json==NULL)goto failure;
    }//! for
    goto success;
failure:
    lds_query::roll_back();
    retflag = false;
    goto end;
success:
    lds_query::com_mit();
    retflag = true;
end:

    return retflag;
}

bool w_sys_manage_cloudsync::set_weixin_payforwater(QObject *parent, const QString &beginTime, const QString &endTime, int downFlag)
{

    bool retflag=true;
    lds_query query;
    QMap<QString, QString> headInfo;
    cJSONHttpData array = cJSON_CreateArray();
    QString http_key;


    //headInfo.insert("machineCode","");
    headInfo.insert("orgCode",n_func::f_get_sysparm("orgCode"));
    headInfo.insert("userName",n_func::f_get_sysparm("cloud_user"));
    headInfo.insert("password",n_func::f_get_sysparm("cloud_password"));

    //获取本地菜品明细
    lds_query::tran_saction();
    QString param;{//param
        query.execSelect("select * from cey_web_pay_for_water where upLoadFlag=0");
        while(query.next()){
            cJSONHttpData item = cJSON_CreateObject();


            cJSON_help::filter_cJSON_AddStringToObject(item.json,"sn", query.recordValue("sn"));
            cJSON_help::filter_cJSON_AddStringToObject(item.json,"orderSn", query.recordValue("orderSn"));
            cJSON_help::filter_cJSON_AddStringToObject(item.json,"mount", query.recordValue("mount"));
            cJSON_help::filter_cJSON_AddStringToObject(item.json,"note", query.recordValue("note"));
            cJSON_help::filter_cJSON_AddStringToObject(item.json,"note", query.recordValue("note"));
            cJSON_help::filter_cJSON_AddStringToObject(item.json,"openid", query.recordValue("openid"));
            cJSON_help::filter_cJSON_AddStringToObject(item.json,"orgCode", query.recordValue("orgCode"));
            cJSON_help::filter_cJSON_AddStringToObject(item.json,"date", query.recordValue("date"));
            cJSON_help::filter_cJSON_AddStringToObject(item.json,"upLoadFlag", query.recordValue("upLoadFlag"));


            cJSON_AddItemToArray(array.json, item.json);


            if(query.execUpdate("cey_web_pay_for_water", qrtVariantPairList()
                                << qrtVariantPair("upLoadFlag", 1),
                                QString(" sn = '%1' ").arg(query.recordValue("sn").toString()))) {

            } else {
                goto failure;

            }
        }
        if(cJSON_GetArraySize(array.json) > 0){
            param = array.jsonToString();
        }
    }
    //上传到云端


    http_key="http://" + n_func::f_get_sysparm("cloud_ip")+
            "/CateSystem/weixinOrder-message/setPayForWater-rest";
    if(!param.isEmpty()){

#ifdef QT_DEBUG
        parent->objectName();
#endif

        cJSONHttpData root=get_yun_httppos(0, http_key, param, headInfo, tr("收到确认,更新本地数据库"), false);
        if(root.json == NULL){
            goto failure;

        }
    }
    goto success;
failure:
    lds_query::roll_back();

    retflag = false;
    goto end;
success:
    lds_query::com_mit();

    retflag = true;
end:

    return retflag;
}

bool w_sys_manage_cloudsync::up_weixin_change_takkeoutstate(QWidget *parent, const QString &sn, const QString &orderState, const QString &refuseReason)
{
    QMap<QString, QString> headInfo;

    //headInfo.insert("machineCode","");
    headInfo.insert("orgCode",n_func::f_get_sysparm("orgCode"));
    headInfo.insert("userName",n_func::f_get_sysparm("cloud_user"));
    headInfo.insert("password",n_func::f_get_sysparm("cloud_password"));
    QString param;{//param
        cJSONHttpData pItem = cJSON_CreateObject();
        cJSON_help::filter_cJSON_AddStringToObject(pItem.json,"sn", sn);
        cJSON_help::filter_cJSON_AddStringToObject(pItem.json,"orderState", orderState);
        cJSON_help::filter_cJSON_AddStringToObject(pItem.json,"refuseReason", refuseReason);
        param=pItem.jsonToString();
    }

    QString http_key="http://" + n_func::f_get_sysparm("cloud_ip")
            +"/CateSystem/weixinOrder-message/changeTakeoutState-rest";
#ifdef QT_DEBUG
    parent->objectName();
#endif

    cJSONHttpData changeOrderState_json=get_yun_httppos(parent, http_key, param, headInfo, tr("订单状态更新"));
    if(changeOrderState_json.json==NULL) return false;
    return true;
}

bool w_sys_manage_cloudsync::up_weixin_change_orderstate(QWidget *parent, const QString &sn, const QString &orderState, const QString &refuseReason)
{
    QMap<QString, QString> headInfo;

    //headInfo.insert("machineCode","");
    headInfo.insert("orgCode",n_func::f_get_sysparm("orgCode"));
    headInfo.insert("userName",n_func::f_get_sysparm("cloud_user"));
    headInfo.insert("password",n_func::f_get_sysparm("cloud_password"));
    QString param;{//param
        cJSONHttpData pItem = cJSON_CreateObject();
        cJSON_help::filter_cJSON_AddStringToObject(pItem.json,"sn", sn);
        cJSON_help::filter_cJSON_AddStringToObject(pItem.json,"orderState", orderState);
        param=pItem.jsonToString();
    }

    QString http_key="http://" + n_func::f_get_sysparm("cloud_ip")
            +"/CateSystem/weixinOrder-message/changeOrderState-rest";
#ifdef QT_DEBUG
    parent->objectName();
#endif

    cJSONHttpData changeOrderState_json=get_yun_httppos(parent, http_key, param, headInfo, tr("订单状态更新"));
    if(changeOrderState_json.json==NULL) return false;
    return true;
}


bool w_sys_manage_cloudsync::get_yun_member_checkbillnopay(QWidget *parent,const QString &billno, const QString &memberno)
{
    QString ls_pay;
    //获取map
    QMap<QString, QString> headInfo;

    headInfo.insert("orgCode",n_func::f_get_sysparm("orgCode"));
    headInfo.insert("userName",n_func::f_get_sysparm("cloud_user"));
    headInfo.insert("password",n_func::f_get_sysparm("cloud_password"));
    headInfo.insert("memberNo",memberno);
    headInfo.insert("billNo",billno);

    QString http_key="http://" + n_func::f_get_sysparm("cloud_ip")+
            "/CateSystem/cvs/member/checkBillNoPay-rest";
    cJSONHttpData root = get_yun_httpget(parent, http_key, headInfo,tr("会员付款核实,是否已经付款"));

    if(root.json==NULL) return false;
    if(cJSON_GetArraySize(root.json) > 0){
        ls_pay = cJSON_help::getcJSONvalue(root.json, "pay").toString();
    }

    return ls_pay=="true";
}
//获取、服务器时间，会跳过时间校验
yunTime w_sys_manage_cloudsync::get_yun_srvdt(QWidget *parent, bool &ok, QString *errstring)
{
    yunTime ydt;
    ok = false;
    //获取map
    QMap<QString, QString> headInfo;

    headInfo.insert("orgCode",n_func::f_get_sysparm("orgCode"));
    headInfo.insert("userName",n_func::f_get_sysparm("cloud_user"));
    headInfo.insert("password",n_func::f_get_sysparm("cloud_password"));

    QString http_key="http://" + n_func::f_get_sysparm("cloud_ip")+
            "/CateSystem/cvs/member/getServerTime-rest";
    cJSONHttpData httpd = get_yun_httpget(parent, http_key, headInfo,tr("本地同步云端时间"),false, false);//允许弹出警告窗口、不会进行时间校验
    if(200 == httpd.httpStatus){
        if(cJSON_GetArraySize(httpd.json) > 0){
            ydt.time = QDateTime::fromString(cJSON_help::getcJSONvalue(httpd.json, "time").toString(), "yyyy-MM-dd hh:mm:ss");
            ydt.printCheck = cJSON_help::getcJSONvalue(httpd.json, "printCheck").toString();
            ydt.printContext = cJSON_help::getcJSONvalue(httpd.json, "printContext").toString();
            ok = true;
        }
    }
    if(false == ok) {
        if(errstring) *errstring = httpd.message;
    }
    return ydt;
}

void member_up_yun::clearallList()
{   
    memberPointListList.clear();
    memberPayList.clear();
    memberCuramountList.clear();
    memberDepositList.clear();
}

int member_up_yun::finditem(const memberCuramount &o)
{
    for(int k = 0; k < memberCuramountList.count(); k++) {
        if(memberCuramountList[k].vchMemberno == o.vchMemberno)
            return k;
    }
    return -1;
}

cJSON *member_up_yun::q_u_get_piece_cey_u_checkout_master(lds_query &query, const QString &orgCode)
{
    cJSON *item = w_sys_manage_cloudsync_with_time_running::q_u_get_piece_cey_u_checkout_master(query , orgCode);
    cJSON_help::replaceItemValue(item, "comChPayNo", ch_payno);
    cJSON_help::replaceItemValue(item, "comChBillNo", ch_billno);
    return item;
}

cJSON *member_up_yun::q_u_get_piece_cey_u_master(lds_query &query, const QString &orgCode)
{
    cJSON *item = w_sys_manage_cloudsync_with_time_running::q_u_get_piece_cey_u_master(query , orgCode);
    cJSON_help::replaceItemValue(item, "omChBillNo", ch_billno);
    return item;
}

cJSON *member_up_yun::q_u_get_piece_cey_u_table(lds_query &query, const QString &orgCode)
{
    cJSON *item = w_sys_manage_cloudsync_with_time_running::q_u_get_piece_cey_u_table(query , orgCode);
    cJSON_help::replaceItemValue(item, "chPayno", ch_payno);
    cJSON_help::replaceItemValue(item, "chBillno", ch_billno);
    return item;
}

cJSON *member_up_yun::q_u_get_piece_cey_u_orderdish(lds_query &query, const QString &orgCode)
{
    cJSON *item = w_sys_manage_cloudsync_with_time_running::q_u_get_piece_cey_u_orderdish(query , orgCode);
    cJSON_help::replaceItemValue(item, "bdChPayNo", ch_payno);
    cJSON_help::replaceItemValue(item, "bdChBillNo", ch_billno);
    return item;
}

cJSON *member_up_yun::q_u_get_piece_cey_u_checkout_detail(lds_query &query, const QString &orgCode)
{
    cJSON *item = w_sys_manage_cloudsync_with_time_running::q_u_get_piece_cey_u_checkout_detail(query , orgCode);
    cJSON_help::replaceItemValue(item, "codChPayNo", ch_payno);
    return item;
}


cJSON * member_up_yun::make_JSON_Array(const memberPointList &p, const QString &orgCode)
{
    cJSON *root = cJSON_CreateObject();
    cJSON_help::filter_cJSON_AddStringToObject(root, "mpIntFlowId", p.mpIntFlowId);
    cJSON_help::filter_cJSON_AddStringToObject(root, "mpVchMemberNo", p.mpVchMemberNo);
    cJSON_help::filter_cJSON_AddStringToObject(root, "mpChType", p.mpChType);
    cJSON_help::filter_cJSON_AddStringToObject(root, "mpChPayNo", p.mpChPayNo);
    cJSON_help::filter_cJSON_AddStringToObject(root, "mpNumPoint", p.mpNumPoint);
    cJSON_help::filter_cJSON_AddStringToObject(root, "mpVchMemo", p.mpVchMemo);
    cJSON_help::filter_cJSON_AddStringToObject(root, "mpVchOperId", p.mpVchOperId);
    cJSON_help::filter_cJSON_AddStringToObject(root, "mpDtOperDate", p.mpDtOperDate);

    cJSON_help::filter_cJSON_AddStringToObject(root, "orgCode", orgCode);

    return root;
}

cJSON * member_up_yun::make_JSON_Array(const memberPay &p, const QString &orgCode)
{
    cJSON *root = cJSON_CreateObject();
    cJSON_help::filter_cJSON_AddStringToObject(root, "intFlowid", p.intFlowid);
    cJSON_help::filter_cJSON_AddStringToObject(root, "mpVchMemberNo", p.mpVchMemberNo);
    cJSON_help::filter_cJSON_AddStringToObject(root, "mpChPayNo", p.mpChPayNo);
    cJSON_help::filter_cJSON_AddStringToObject(root, "mpChPayModeNo", p.mpChPayModeNo);
    cJSON_help::filter_cJSON_AddStringToObject(root, "mpNumAmount", p.mpNumAmount);
    cJSON_help::filter_cJSON_AddStringToObject(root, "mpNumPay", p.mpNumPay);
    cJSON_help::filter_cJSON_AddStringToObject(root, "mpVchOperId", p.mpVchOperId);
    cJSON_help::filter_cJSON_AddStringToObject(root, "mpDtOperDate", p.mpDtOperDate);
    cJSON_help::filter_cJSON_AddStringToObject(root, "mpChBalanceNo", p.mpChBalanceNo);
    cJSON_help::filter_cJSON_AddStringToObject(root, "mpIntCheckFlow", p.mpIntCheckFlow);

    cJSON_help::filter_cJSON_AddStringToObject(root, "orgCode", orgCode);

    return root;
}

cJSON * member_up_yun::make_JSON_Array(const memberCuramount &p, const QString &orgCode)
{
    cJSON *root = cJSON_CreateObject();
    cJSON_help::filter_cJSON_AddStringToObject(root, "vchMemberno", p.vchMemberno);
    cJSON_help::filter_cJSON_AddStringToObject(root, "numAmount", p.numAmount);
    cJSON_help::filter_cJSON_AddStringToObject(root, "numHangamount", p.numHangamount);
    cJSON_help::filter_cJSON_AddStringToObject(root, "numPoint", p.numPoint);
    cJSON_help::filter_cJSON_AddStringToObject(root, "vchOperId", p.vchOperId);
    cJSON_help::filter_cJSON_AddStringToObject(root, "dtOperdate", p.dtOperdate);
    cJSON_help::filter_cJSON_AddStringToObject(root, "chBranchno", p.chBranchno);

    cJSON_help::filter_cJSON_AddStringToObject(root, "orgCode", orgCode);

    return root;
}

cJSON * member_up_yun::make_JSON_Array(const memberDeposit &p, const QString &orgCode)
{
    cJSON *root = cJSON_CreateObject();
    cJSON_help::filter_cJSON_AddStringToObject(root, "mdIntFlowId", p.mdIntFlowId);
    cJSON_help::filter_cJSON_AddStringToObject(root, "mdVchMemberNo", p.mdVchMemberNo);
    cJSON_help::filter_cJSON_AddStringToObject(root, "mdNumDeposit", p.mdNumDeposit);
    cJSON_help::filter_cJSON_AddStringToObject(root, "mdNumRealaMount", p.mdNumRealaMount);
    cJSON_help::filter_cJSON_AddStringToObject(root, "mdChDepositMode", p.mdChDepositMode);
    cJSON_help::filter_cJSON_AddStringToObject(root, "mdChPayMode", p.mdChPayMode);
    cJSON_help::filter_cJSON_AddStringToObject(root, "mdVchOperId", p.mdVchOperId);
    cJSON_help::filter_cJSON_AddStringToObject(root, "dtOperDate", p.dtOperDate);
    cJSON_help::filter_cJSON_AddStringToObject(root, "mdVchMemo", p.mdVchMemo);
    cJSON_help::filter_cJSON_AddStringToObject(root, "mdRemainAmt", p.mdRemainAmt);
    cJSON_help::filter_cJSON_AddStringToObject(root, "comChPayNo", p.comChPayNo);

    cJSON_help::filter_cJSON_AddStringToObject(root, "orgCode", orgCode);

    return root;
}
