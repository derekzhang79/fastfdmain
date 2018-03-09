#include "w_sys_manage.h"
#include <QDir>

w_sys_manage::w_sys_manage(QObject *parent) :
    QObject(parent)
{
}


QStringList w_sys_manage::get_flilename_path(const QString &path, const QStringList &filter)
{
    QStringList filenames;
    QDir *dir=new QDir(path);
    //filter<<"*.dat";
    //dir->setNameFilters(filter);
    QList<QFileInfo> *fileInfo=new QList<QFileInfo>(dir->entryInfoList(filter));

    //注释掉的两行是用来文件过滤用的，如果你要指定文件后缀名在 filter<<"*.dat"  写就好了
    QString fileName;
    for(int i = 0; i < fileInfo->count(); ++i)
    {
        fileName = fileInfo->at(i).fileName();
        if(fileName=="."
                || fileName=="..")continue;
        filenames << fileName;

    }
    return filenames;
}

QString w_sys_manage::set_flilename_path(const QString &flilename, const QString &path)
{
    return QString();
}



cJSON *w_sys_manage::q_bt_get_piece_cey_bt_dish(lds_query &query, const QString &orgCode, int state)
{
    cJSON *pItem = cJSON_CreateObject();

    cJSON_help::filter_cJSON_AddStringToObject( pItem, "diChDishNo", query.recordValue("ch_dishno"));

    cJSON_help::filter_cJSON_AddStringToObject( pItem, "diVchDishName", query.recordValue("vch_dishname"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "diVchSpell", query.recordValue("vch_spell"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "diVchEnglish", query.recordValue("vch_english"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "diChDishTypeNo", query.recordValue("ch_dish_typeno"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "diChUnitNo", query.recordValue("ch_unitno"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "diNumPrice", query.recordValue("num_price"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "diChSuitflag", query.recordValue("ch_suitflag"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "diChDiscount", query.recordValue("ch_discount"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "diChCurprice", query.recordValue("ch_curprice"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "diChTempflag", query.recordValue("ch_tempflag"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "diChStopflag", query.recordValue("ch_stopflag"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "diChPrintFlag", query.recordValue("ch_printflag"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "diVchPrinter", query.recordValue("vch_printer"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "diChSubTypeNo", query.recordValue("ch_sub_typeno"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "diChMaterialNo", query.recordValue("ch_materialno"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "diChServiceFlag", query.recordValue("ch_serviceflag"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "diChLowFlag", query.recordValue("ch_lowflag"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "diNumMprice", query.recordValue("num_m_price"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "diChDeductFlag", query.recordValue("ch_deductflag"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "diChDeductType", query.recordValue("ch_deducttype"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "diIntDeduct", query.recordValue("int_deduct"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "diNumDeduct", query.recordValue("num_deduct"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "diChSaleFlag", query.recordValue("ch_saleflag"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "diChSaleType", query.recordValue("ch_saletype"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "diIntSaleDeduct", query.recordValue("int_sale_deduct"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "diNumSale", query.recordValue("num_sale"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "itemFlag", query.recordValue("item_flag"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "diVchPurUnitNo", query.recordValue("vch_pur_unitno"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "diIntUnitRate", query.recordValue("int_unit_rate"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "diChOff", query.recordValue("ch_off"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "diVchSubNo", query.recordValue("vch_subno"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "diNumSalePrice", query.recordValue("num_sale_price"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "diChOutFlag", query.recordValue("ch_outflag"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "diVchOutPrint", query.recordValue("vch_outprint"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "intRate", query.recordValue("int_rate"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "chWeight", query.recordValue("ch_weight"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "vchBarcode", query.recordValue("vch_barcode"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "chGroupno", query.recordValue("ch_groupno"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "chMaterialFlag", query.recordValue("ch_material_flag"));

    cJSON_help::filter_cJSON_AddStringToObject( pItem, "state", state);//逻辑删除<0话，是逻辑删除
    cJSON_help::filter_cJSON_AddStringToObject(pItem, "orgCode", orgCode);

    return pItem;
}

void w_sys_manage::q_bt_put_piece_cey_bt_dish(qrtVariantPairList &key_value_list, cJSON *pItem)
{

    key_value_list << qrtVariantPair("ch_dishno", cJSON_help::getcJSONvalue(pItem,"diChDishNo"));
    key_value_list << qrtVariantPair("vch_dishname", cJSON_help::getcJSONvalue(pItem,"diVchDishName"));
    key_value_list << qrtVariantPair("vch_spell", cJSON_help::getcJSONvalue(pItem,"diVchSpell"));
    key_value_list << qrtVariantPair("vch_english", cJSON_help::getcJSONvalue(pItem,"diVchEnglish"));
    key_value_list << qrtVariantPair("ch_dish_typeno", cJSON_help::getcJSONvalue(pItem,"diChDishTypeNo"));
    key_value_list << qrtVariantPair("ch_unitno", cJSON_help::getcJSONvalue(pItem,"diChUnitNo"));
    key_value_list << qrtVariantPair("num_price", cJSON_help::getcJSONvalue(pItem,"diNumPrice"));
    key_value_list << qrtVariantPair("ch_suitflag", cJSON_help::getcJSONvalue(pItem,"diChSuitflag"));
    key_value_list << qrtVariantPair("ch_discount", cJSON_help::getcJSONvalue(pItem,"diChDiscount"));
    key_value_list << qrtVariantPair("ch_curprice", cJSON_help::getcJSONvalue(pItem,"diChCurprice"));
    key_value_list << qrtVariantPair("ch_tempflag", cJSON_help::getcJSONvalue(pItem,"diChTempflag"));
    key_value_list << qrtVariantPair("ch_stopflag", cJSON_help::getcJSONvalue(pItem,"diChStopflag"));
    key_value_list << qrtVariantPair("ch_printflag", cJSON_help::getcJSONvalue(pItem,"diChPrintFlag"));
    key_value_list << qrtVariantPair("vch_printer", cJSON_help::getcJSONvalue(pItem,"diVchPrinter"));
    key_value_list << qrtVariantPair("ch_sub_typeno", cJSON_help::getcJSONvalue(pItem,"diChSubTypeNo"));
    key_value_list << qrtVariantPair("ch_materialno", cJSON_help::getcJSONvalue(pItem,"diChMaterialNo"));
    key_value_list << qrtVariantPair("ch_serviceflag", cJSON_help::getcJSONvalue(pItem,"diChServiceFlag"));
    key_value_list << qrtVariantPair("ch_lowflag", cJSON_help::getcJSONvalue(pItem,"diChLowFlag"));
    key_value_list << qrtVariantPair("num_m_price", cJSON_help::getcJSONvalue(pItem,"diNumMprice"));
    key_value_list << qrtVariantPair("ch_deductflag", cJSON_help::getcJSONvalue(pItem,"diChDeductFlag"));
    key_value_list << qrtVariantPair("ch_deducttype", cJSON_help::getcJSONvalue(pItem,"diChDeductType"));
    key_value_list << qrtVariantPair("int_deduct", cJSON_help::getcJSONvalue(pItem,"diIntDeduct"));
    key_value_list << qrtVariantPair("num_deduct", cJSON_help::getcJSONvalue(pItem,"diNumDeduct"));
    key_value_list << qrtVariantPair("ch_saleflag", cJSON_help::getcJSONvalue(pItem,"diChSaleFlag"));
    key_value_list << qrtVariantPair("ch_saletype", cJSON_help::getcJSONvalue(pItem,"diChSaleType"));
    key_value_list << qrtVariantPair("int_sale_deduct", cJSON_help::getcJSONvalue(pItem,"diIntSaleDeduct"));
    key_value_list << qrtVariantPair("num_sale", cJSON_help::getcJSONvalue(pItem,"diNumSale"));
    key_value_list << qrtVariantPair("item_flag", cJSON_help::getcJSONvalue(pItem,"itemFlag"));
    key_value_list << qrtVariantPair("vch_pur_unitno", cJSON_help::getcJSONvalue(pItem,"diVchPurUnitNo"));
    key_value_list << qrtVariantPair("int_unit_rate", cJSON_help::getcJSONvalue(pItem,"diIntUnitRate"));
    key_value_list << qrtVariantPair("ch_off", cJSON_help::getcJSONvalue(pItem,"diChOff"));
    key_value_list << qrtVariantPair("vch_subno", cJSON_help::getcJSONvalue(pItem,"diVchSubNo"));
    key_value_list << qrtVariantPair("num_sale_price", cJSON_help::getcJSONvalue(pItem,"diNumSalePrice"));
    key_value_list << qrtVariantPair("ch_outflag", cJSON_help::getcJSONvalue(pItem,"diChOutFlag"));
    key_value_list << qrtVariantPair("vch_outprint", cJSON_help::getcJSONvalue(pItem,"diVchOutPrint"));
    key_value_list << qrtVariantPair("int_rate", cJSON_help::getcJSONvalue(pItem,"intRate"));
    key_value_list << qrtVariantPair("ch_weight", cJSON_help::getcJSONvalue(pItem,"chWeight"));
    key_value_list << qrtVariantPair("vch_barcode", cJSON_help::getcJSONvalue(pItem,"vchBarcode"));
    key_value_list << qrtVariantPair("ch_groupno", cJSON_help::getcJSONvalue(pItem,"chGroupno"));
    key_value_list << qrtVariantPair("ch_material_flag", cJSON_help::getcJSONvalue(pItem,"chMaterialFlag"));

    key_value_list << qrtVariantPair("ch_special_type", cJSON_help::getcJSONvalue(pItem,"chSpecialType1"));
    key_value_list << qrtVariantPair("ch_special_type2", cJSON_help::getcJSONvalue(pItem,"chSpecialType2"));
    key_value_list << qrtVariantPair("num_special_price", cJSON_help::getcJSONvalue(pItem,"numSpecialPrice"));
}

cJSON *w_sys_manage::q_bt_get_piece_cey_bt_dish_subtype(lds_query &query, const QString &orgCode, int state)
{
    cJSON *pItem = cJSON_CreateObject();

    cJSON_help::filter_cJSON_AddStringToObject( pItem, "dstChSubTypeNo", query.recordValue("ch_sub_typeno"));

    cJSON_help::filter_cJSON_AddStringToObject( pItem, "dstVchSubTypeName", query.recordValue("vch_sub_typename"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "dstChDishTypeNo", query.recordValue("ch_dish_typeno"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "dstChLagelPrintFlag", query.recordValue("ch_labelprint_flag"));

    cJSON_help::filter_cJSON_AddStringToObject(pItem, "state", state);//逻辑删除
    cJSON_help::filter_cJSON_AddStringToObject(pItem, "orgCode", orgCode);

    return pItem;
}

void w_sys_manage::q_bt_put_piece_cey_bt_dish_subtype(qrtVariantPairList &key_value_list, cJSON *pItem)
{
    key_value_list << qrtVariantPair("ch_sub_typeno", cJSON_help::getcJSONvalue(pItem,"dstChSubTypeNo"));
    key_value_list << qrtVariantPair("vch_sub_typename", cJSON_help::getcJSONvalue(pItem,"dstVchSubTypeName"));
    key_value_list << qrtVariantPair("ch_dish_typeno", cJSON_help::getcJSONvalue(pItem,"dstChDishTypeNo"));
    key_value_list << qrtVariantPair("ch_labelprint_flag", cJSON_help::getcJSONvalue(pItem,"dstChLagelPrintFlag"));

}

cJSON *w_sys_manage::q_bt_get_piece_cey_bt_dish_type(lds_query &query, const QString &orgCode, int state)
{
    cJSON *pItem = cJSON_CreateObject();

    cJSON_help::filter_cJSON_AddStringToObject( pItem, "dtChDishTypeNo", query.recordValue("ch_dish_typeno"));

    cJSON_help::filter_cJSON_AddStringToObject( pItem, "dtVchDishTypeName", query.recordValue("vch_dish_typename"));

    cJSON_help::filter_cJSON_AddStringToObject(pItem, "state", state);//逻辑删除
    cJSON_help::filter_cJSON_AddStringToObject(pItem, "orgCode", orgCode);

    return pItem;
}

void w_sys_manage::q_bt_put_piece_cey_bt_dish_type(qrtVariantPairList &key_value_list, cJSON *pItem)
{
    key_value_list << qrtVariantPair("ch_dish_typeno", cJSON_help::getcJSONvalue(pItem,"dtChDishTypeNo"));
    key_value_list << qrtVariantPair("vch_dish_typename", cJSON_help::getcJSONvalue(pItem,"dtVchDishTypeName"));

}

cJSON *w_sys_manage::q_bt_get_piece_cey_bt_dish_suit(lds_query &query, const QString &orgCode, int state)
{
    cJSON *pItem = cJSON_CreateObject();

    cJSON_help::filter_cJSON_AddStringToObject( pItem, "dsIntFlowId", query.recordValue("int_flowid"));

    cJSON_help::filter_cJSON_AddStringToObject( pItem, "dsChSuitNo", query.recordValue("ch_suitno"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "dsChDishNo", query.recordValue("ch_dishno"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "dsNumNum", query.recordValue("num_num"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "dsChDishNoChang", query.recordValue("ch_dishno_chang"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "dsNumNumChange", query.recordValue("num_num_change"));

    cJSON_help::filter_cJSON_AddStringToObject(pItem, "state", state);//逻辑删除
    cJSON_help::filter_cJSON_AddStringToObject(pItem, "orgCode", orgCode);

    return pItem;
}

void w_sys_manage::q_bt_put_piece_cey_bt_dish_suit(qrtVariantPairList &key_value_list, cJSON *pItem)
{
    key_value_list << qrtVariantPair("int_flowid", cJSON_help::getcJSONvalue(pItem,"dsIntFlowId"));
    key_value_list << qrtVariantPair("ch_suitno", cJSON_help::getcJSONvalue(pItem,"dsChSuitNo"));
    key_value_list << qrtVariantPair("ch_dishno", cJSON_help::getcJSONvalue(pItem,"dsChDishNo"));
    key_value_list << qrtVariantPair("num_num", cJSON_help::getcJSONvalue(pItem,"dsNumNum"));
    key_value_list << qrtVariantPair("ch_dishno_chang", cJSON_help::getcJSONvalue(pItem,"dsChDishNoChang"));
    key_value_list << qrtVariantPair("num_num_change", cJSON_help::getcJSONvalue(pItem,"dsNumNumChange"));

}

cJSON *w_sys_manage::q_bt_get_piece_cey_bt_suit_change(lds_query &query, const QString &orgCode, int state)
{
    //复合主键int_flowid,ch_dishno
    cJSON *pItem = cJSON_CreateObject();
    //
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "dscIntFlowId", query.recordValue("int_flowID"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "dscChDishNo", query.recordValue("ch_dishno"));

    cJSON_help::filter_cJSON_AddStringToObject( pItem, "dscNumNumChange", query.recordValue("num_num_change"));

    cJSON_help::filter_cJSON_AddStringToObject(pItem, "state", state);//逻辑删除
    cJSON_help::filter_cJSON_AddStringToObject(pItem, "orgCode", orgCode);

    return pItem;
}

void w_sys_manage::q_bt_put_piece_cey_bt_suit_change(qrtVariantPairList &key_value_list, cJSON *pItem)
{
    key_value_list << qrtVariantPair("int_flowid", cJSON_help::getcJSONvalue(pItem,"dscIntFlowId").toInt());
    key_value_list << qrtVariantPair("ch_dishno", cJSON_help::getcJSONvalue(pItem,"dscChDishNo"));
    key_value_list << qrtVariantPair("num_num_change", cJSON_help::getcJSONvalue(pItem,"dscNumNumChange"));

}

cJSON *w_sys_manage::q_bt_get_piece_cey_bt_unit(lds_query &query, const QString &orgCode, int state)
{
    cJSON *pItem = cJSON_CreateObject();

    cJSON_help::filter_cJSON_AddStringToObject( pItem, "duChUnitNo", query.recordValue("ch_unitno"));

    cJSON_help::filter_cJSON_AddStringToObject( pItem, "duVchUnitName", query.recordValue("vch_unitname"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "duChNumType", query.recordValue("ch_numtype"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "duNumDefault", query.recordValue("num_default").toDouble());

    cJSON_help::filter_cJSON_AddStringToObject(pItem, "state", state);//逻辑删除
    cJSON_help::filter_cJSON_AddStringToObject(pItem, "orgCode", orgCode);

    return pItem;
}

void w_sys_manage::q_bt_put_piece_cey_bt_unit(qrtVariantPairList &key_value_list, cJSON *pItem)
{
    key_value_list << qrtVariantPair("ch_unitno", cJSON_help::getcJSONvalue(pItem,"duChUnitNo"));
    key_value_list << qrtVariantPair("vch_unitname", cJSON_help::getcJSONvalue(pItem,"duVchUnitName"));
    key_value_list << qrtVariantPair("ch_numtype", cJSON_help::getcJSONvalue(pItem,"duChNumType"));
    key_value_list << qrtVariantPair("num_default", cJSON_help::getcJSONvalue(pItem,"duNumDefault"));

}

cJSON *w_sys_manage::q_bt_get_piece_cey_bt_table(lds_query &query, const QString &orgCode, int state)
{
    cJSON *pItem = cJSON_CreateObject();

    cJSON_help::filter_cJSON_AddStringToObject( pItem, "dtChTableNo", query.recordValue("ch_tableno"));

    cJSON_help::filter_cJSON_AddStringToObject( pItem, "dtVchTableName", query.recordValue("vch_tablename"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "dtChTypeNo", query.recordValue("ch_typeno"));
    //    cJSON_help::filter_cJSON_AddStringToObject( pItem, "dtChBillNo", query.recordValue("ch_billno"));
    //    cJSON_help::filter_cJSON_AddStringToObject( pItem, "dtChLockFlag", query.recordValue("ch_lockflag"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "dtIntPerson", query.recordValue("int_person"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "dtVchMemo", query.recordValue("vch_memo"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "dtChState", query.recordValue("ch_state"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "dtDtDateTime", query.recordValue("dt_datetime"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "dtChAreaNo", query.recordValue("ch_areano"));

    cJSON_help::filter_cJSON_AddStringToObject(pItem, "state", state);//逻辑删除
    cJSON_help::filter_cJSON_AddStringToObject(pItem, "orgCode", orgCode);

    return pItem;
}

void w_sys_manage::q_bt_put_piece_cey_bt_table(qrtVariantPairList &key_value_list, cJSON *pItem)
{
    key_value_list << qrtVariantPair("ch_tableno", cJSON_help::getcJSONvalue(pItem,"dtChTableNo"));
    key_value_list << qrtVariantPair("vch_tablename", cJSON_help::getcJSONvalue(pItem,"dtVchTableName"));
    key_value_list << qrtVariantPair("ch_typeno", cJSON_help::getcJSONvalue(pItem,"dtChTypeNo"));
    //   key_value_list << qrtVariantPair("ch_billno", cJSON_help::getcJSONvalue(pItem,"dtChBillNo"));
    //   key_value_list << qrtVariantPair("ch_lockflag", cJSON_help::getcJSONvalue(pItem,"dtChLockFlag"));
    key_value_list << qrtVariantPair("int_person", cJSON_help::getcJSONvalue(pItem,"dtIntPerson").toInt());
    key_value_list << qrtVariantPair("vch_memo", cJSON_help::getcJSONvalue(pItem,"dtVchMemo"));
    key_value_list << qrtVariantPair("ch_state", cJSON_help::getcJSONvalue(pItem,"dtChState"));
    key_value_list << qrtVariantPair("dt_datetime", cJSON_help::getcJSONvalue(pItem,"dtDtDateTime"));
    key_value_list << qrtVariantPair("ch_areano", cJSON_help::getcJSONvalue(pItem,"dtChAreaNo"));

}

cJSON *w_sys_manage::q_bt_get_piece_cey_bt_table_type(lds_query &query, const QString &orgCode, int state)
{
    cJSON *pItem = cJSON_CreateObject();

    cJSON_help::filter_cJSON_AddStringToObject( pItem, "dttChTyperNo", query.recordValue("ch_typeno"));

    cJSON_help::filter_cJSON_AddStringToObject( pItem, "dttVchTypeName", query.recordValue("vch_typename"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "dttChPriceKind", query.recordValue("ch_pricekind"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "dttChServiceMode", query.recordValue("ch_service_mode"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "dttChServiceFlag", query.recordValue("ch_serviceflag"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "dttIntStartMinute", query.recordValue("int_start_minute"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "dttNumStartAmount", query.recordValue("num_start_amount"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "dttIntUnitMinute", query.recordValue("int_unit_minute"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "dttNumUnitAmount", query.recordValue("num_unit_amount"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "dttIntOver",query.recordValue("int_over"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "dttNumRation", query.recordValue("num_ration"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "dttIntRate", query.recordValue("int_rate"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "dttNumMaxService", query.recordValue("num_max_service"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "dttChLowMode", query.recordValue("ch_low_mode"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "dttNumLowCost", query.recordValue("num_lowcost"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "dttIntBeginTime",query.recordValue("int_begin_time"));

    cJSON_help::filter_cJSON_AddStringToObject(pItem, "state", state);//逻辑删除
    cJSON_help::filter_cJSON_AddStringToObject(pItem, "orgCode", orgCode);

    return pItem;
}

void w_sys_manage::q_bt_put_piece_cey_bt_table_type(qrtVariantPairList &key_value_list, cJSON *pItem)
{
    key_value_list << qrtVariantPair("ch_typeno", cJSON_help::getcJSONvalue(pItem,"dttChTyperNo"));
    key_value_list << qrtVariantPair("vch_typename", cJSON_help::getcJSONvalue(pItem,"dttVchTypeName"));
    key_value_list << qrtVariantPair("ch_pricekind", cJSON_help::getcJSONvalue(pItem,"dttChPriceKind"));
    key_value_list << qrtVariantPair("ch_service_mode", cJSON_help::getcJSONvalue(pItem,"dttChServiceMode"));
    key_value_list << qrtVariantPair("ch_serviceflag", cJSON_help::getcJSONvalue(pItem,"dttChServiceFlag"));
    key_value_list << qrtVariantPair("int_start_minute", cJSON_help::getcJSONvalue(pItem,"dttIntStartMinute"));
    key_value_list << qrtVariantPair("num_start_amount", cJSON_help::getcJSONvalue(pItem,"dttNumStartAmount"));
    key_value_list << qrtVariantPair("int_unit_minute", cJSON_help::getcJSONvalue(pItem,"dttIntUnitMinute"));
    key_value_list << qrtVariantPair("num_unit_amount", cJSON_help::getcJSONvalue(pItem,"dttNumUnitAmount"));
    key_value_list << qrtVariantPair("int_over", cJSON_help::getcJSONvalue(pItem,"dttIntOver"));
    key_value_list << qrtVariantPair("num_ration", cJSON_help::getcJSONvalue(pItem,"dttNumRation"));
    key_value_list << qrtVariantPair("int_rate", cJSON_help::getcJSONvalue(pItem,"dttIntRate"));
    key_value_list << qrtVariantPair("num_max_service", cJSON_help::getcJSONvalue(pItem,"dttNumMaxService"));
    key_value_list << qrtVariantPair("ch_low_mode", cJSON_help::getcJSONvalue(pItem,"dttChLowMode"));
    key_value_list << qrtVariantPair("num_lowcost", cJSON_help::getcJSONvalue(pItem,"dttNumLowCost"));
    key_value_list << qrtVariantPair("int_begin_time", cJSON_help::getcJSONvalue(pItem,"dttIntBeginTime"));

}

cJSON *w_sys_manage::q_bt_get_piece_cey_bt_table_area(lds_query &query, const QString &orgCode, int state)
{
    cJSON *pItem = cJSON_CreateObject();

    cJSON_help::filter_cJSON_AddStringToObject(pItem, "dtaChAreaNo",  query.recordValue("ch_areano"));

    cJSON_help::filter_cJSON_AddStringToObject( pItem, "dtaVchAreaName", query.recordValue("vch_areaname"));

    cJSON_help::filter_cJSON_AddStringToObject(pItem, "state", state);//逻辑删除
    cJSON_help::filter_cJSON_AddStringToObject(pItem, "orgCode", orgCode);


    return pItem;
}

void w_sys_manage::q_bt_put_piece_cey_bt_table_area(qrtVariantPairList &key_value_list, cJSON *pItem)
{
    key_value_list << qrtVariantPair("ch_areano", cJSON_help::getcJSONvalue(pItem,"dtaChAreaNo"));
    key_value_list << qrtVariantPair("vch_areaname", cJSON_help::getcJSONvalue(pItem,"dtaVchAreaName"));

}

cJSON *w_sys_manage::q_bt_get_piece_cey_bt_cp_dish(lds_query &query, const QString &orgCode, int state)
{
    //1 cey_bt_cp_dish、 2concat(%1.ch_dishno, ',', %1.ch_no)、3 isnull(%1.ch_dishno) and isnull(%1.ch_no)
    cJSON *pItem = cJSON_CreateObject();

    cJSON_help::_cJSON_AddStringToObject(pItem, "dpChDishNo",  query.recordValue("ch_dishno"));
    cJSON_help::_cJSON_AddStringToObject(pItem, "dpChNo",  query.recordValue("ch_no"));

    cJSON_help::filter_cJSON_AddStringToObject(pItem, "state", state);//逻辑删除
    cJSON_help::filter_cJSON_AddStringToObject(pItem, "orgCode", orgCode);

    return pItem;
}

void w_sys_manage::q_bt_put_piece_cey_bt_cp_dish(lds_query &sqlQuery, cJSON *pItem)
{
    sqlQuery.bindValue(":ch_dishno", cJSON_help::getcJSONvalue(pItem,"dpChDishNo"));
    sqlQuery.bindValue(":ch_no", cJSON_help::getcJSONvalue(pItem,"dpChNo"));
}

void w_sys_manage::q_bt_put_piece_cey_bt_cp_dish(qrtVariantPairList &key_value_list, cJSON *pItem)
{
    key_value_list << qrtVariantPair("ch_dishno", cJSON_help::getcJSONvalue(pItem,"dpChDishNo"));
    key_value_list << qrtVariantPair("ch_no", cJSON_help::getcJSONvalue(pItem,"dpChNo"));

}

cJSON *w_sys_manage::q_bt_get_piece_cey_bt_cp_memo(lds_query &query, const QString &orgCode, int state)
{
    cJSON *pItem = cJSON_CreateObject();

    cJSON_help::filter_cJSON_AddStringToObject(pItem, "chNo",  query.recordValue("ch_no"));

    cJSON_help::filter_cJSON_AddStringToObject( pItem, "vchCpMemo", query.recordValue("vch_cp_memo"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "chTypeno", query.recordValue("ch_typeno"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "numAddPrice", query.recordValue("num_add_price"));

    cJSON_help::filter_cJSON_AddStringToObject(pItem, "state", state);//逻辑删除
    cJSON_help::filter_cJSON_AddStringToObject(pItem, "orgCode", orgCode);


    return pItem;
}

void w_sys_manage::q_bt_put_piece_cey_bt_cp_memo(qrtVariantPairList &key_value_list, cJSON *pItem)
{
    key_value_list << qrtVariantPair("ch_no", cJSON_help::getcJSONvalue(pItem,"chNo"));
    key_value_list << qrtVariantPair("vch_cp_memo", cJSON_help::getcJSONvalue(pItem,"vchCpMemo"));
    key_value_list << qrtVariantPair("ch_typeno", cJSON_help::getcJSONvalue(pItem,"chTypeno"));
    key_value_list << qrtVariantPair("num_add_price", cJSON_help::getcJSONvalue(pItem,"numAddPrice"));

}

cJSON *w_sys_manage::q_bt_get_piece_cey_bt_cp_type(lds_query &query, const QString &orgCode, int state)
{
    cJSON *pItem = cJSON_CreateObject();

    cJSON_help::filter_cJSON_AddStringToObject( pItem, "dptChTypeNo", query.recordValue("ch_typeno"));

    cJSON_help::filter_cJSON_AddStringToObject( pItem, "dptVchTypeName", query.recordValue("vch_typename"));

    cJSON_help::filter_cJSON_AddStringToObject(pItem, "state", state);//逻辑删除
    cJSON_help::filter_cJSON_AddStringToObject(pItem, "orgCode", orgCode);

    return pItem;
}

void w_sys_manage::q_bt_put_piece_cey_bt_cp_type(qrtVariantPairList &key_value_list, cJSON *pItem)
{
    key_value_list << qrtVariantPair("ch_typeno", cJSON_help::getcJSONvalue(pItem,"dptChTypeNo"));
    key_value_list << qrtVariantPair("vch_typename", cJSON_help::getcJSONvalue(pItem,"dptVchTypeName"));


}

cJSON *w_sys_manage::q_bt_get_piece_cey_bt_dish_warn(lds_query &query, const QString &orgCode, int state)
{
    cJSON *pItem = cJSON_CreateObject();
    //    cJSON_help::filter_cJSON_AddStringToObject( pItem, "dgqIntFlowId", query.recordValue("int_flowID"));

    //    cJSON_help::filter_cJSON_AddStringToObject( pItem, "dgqDtDate", query.recordValue("dt_date"));
    //    cJSON_help::filter_cJSON_AddStringToObject( pItem, "dgqChDishNo", query.recordValue("ch_dishno"));
    //    cJSON_help::filter_cJSON_AddStringToObject( pItem, "dgqNumNum", query.recordValue("num_num"));
    //    cJSON_help::filter_cJSON_AddStringToObject( pItem, "dgqNumSale", query.recordValue("num_sale"));
    //    cJSON_help::filter_cJSON_AddStringToObject( pItem, "dgqChState", query.recordValue("ch_state"));
    //    cJSON_help::filter_cJSON_AddStringToObject( pItem, "dgqVchOperId", query.recordValue("vch_operID"));
    //    cJSON_help::filter_cJSON_AddStringToObject( pItem, "dgqDtOperDate", query.recordValue("dt_operdate"));
    //    cJSON_help::filter_cJSON_AddStringToObject( pItem, "dgqVchOperId2", query.recordValue("vch_operID2"));
    //    cJSON_help::filter_cJSON_AddStringToObject( pItem, "dgqDtOperDate2", query.recordValue("dt_operdate2"));

    //    cJSON_AddNumberToObject(pItem, "state", state);
    //    cJSON_help::filter_cJSON_AddStringToObject(pItem, "orgCode", orgCode);

    return pItem;
}

void w_sys_manage::q_bt_put_piece_cey_bt_dish_warn(qrtVariantPairList &key_value_list, cJSON *pItem)
{
    key_value_list << qrtVariantPair("int_flowid", cJSON_help::getcJSONvalue(pItem,"dgqIntFlowId"));
    key_value_list << qrtVariantPair("dt_date", cJSON_help::getcJSONvalue(pItem,"dgqDtDate"));
    key_value_list << qrtVariantPair("ch_dishno", cJSON_help::getcJSONvalue(pItem,"dgqChDishNo"));
    key_value_list << qrtVariantPair("num_num", cJSON_help::getcJSONvalue(pItem,"dgqNumNum"));
    key_value_list << qrtVariantPair("num_sale", cJSON_help::getcJSONvalue(pItem,"dgqNumSale"));
    key_value_list << qrtVariantPair("ch_state", cJSON_help::getcJSONvalue(pItem,"dgqChState"));
    key_value_list << qrtVariantPair("vch_operID", cJSON_help::getcJSONvalue(pItem,"dgqVchOperId"));
    key_value_list << qrtVariantPair("dt_operdate", cJSON_help::getcJSONvalue(pItem,"dgqDtOperDate"));
    key_value_list << qrtVariantPair("vch_operID2", cJSON_help::getcJSONvalue(pItem,"dgqVchOperId2"));
    key_value_list << qrtVariantPair("dt_operdate2", cJSON_help::getcJSONvalue(pItem,"dgqDtOperDate2"));

}

cJSON *w_sys_manage::q_bt_get_piece_cey_bt_paymode(lds_query &query, const QString &orgCode, int state)
{
    cJSON *pItem = cJSON_CreateObject();

    cJSON_help::filter_cJSON_AddStringToObject( pItem, "pmChPayModeNo", query.recordValue("ch_paymodeno"));

    cJSON_help::filter_cJSON_AddStringToObject( pItem, "pmVchPayModeName", query.recordValue("vch_paymodename"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "pmChIdFlag", query.recordValue("ch_IDflag"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "pmChFaceFlag", query.recordValue("ch_faceflag"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "pmChChangeFlag", query.recordValue("ch_changeflag"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "pmChInComeFlag", query.recordValue("ch_incomeflag"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "pmChKey", query.recordValue("ch_key"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "pmVchARG1", query.recordValue("vch_arg1"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "pmVchARG2", query.recordValue("vch_arg2"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "pmVchARG3", query.recordValue("vch_arg3"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "pmVchARG4", query.recordValue("vch_arg4"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "pmVchARG5", query.recordValue("vch_arg5"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "pmVchARG6", query.recordValue("vch_arg6"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "pmVchARG7", query.recordValue("vch_arg7"));

    cJSON_AddNumberToObject(pItem, "state", state);
    cJSON_help::filter_cJSON_AddStringToObject(pItem, "orgCode", orgCode);

    return pItem;
}

void w_sys_manage::q_bt_put_piece_cey_bt_paymode(qrtVariantPairList &key_value_list, cJSON *pItem)
{
    key_value_list << qrtVariantPair("ch_paymodeno", cJSON_help::getcJSONvalue(pItem,"pmChPayModeNo"));
    key_value_list << qrtVariantPair("vch_paymodename", cJSON_help::getcJSONvalue(pItem,"pmVchPayModeName"));
    key_value_list << qrtVariantPair("ch_IDflag", cJSON_help::getcJSONvalue(pItem,"pmChIdFlag"));
    key_value_list << qrtVariantPair("ch_faceflag", cJSON_help::getcJSONvalue(pItem,"pmChFaceFlag"));
    key_value_list << qrtVariantPair("ch_changeflag", cJSON_help::getcJSONvalue(pItem,"pmChChangeFlag"));

    key_value_list << qrtVariantPair("ch_incomeflag", cJSON_help::getcJSONvalue(pItem,"pmChInComeFlag"));
    key_value_list << qrtVariantPair("ch_key", cJSON_help::getcJSONvalue(pItem,"pmChKey"));
    key_value_list << qrtVariantPair("vch_arg1", cJSON_help::getcJSONvalue(pItem,"pmVchARG1"));
    key_value_list << qrtVariantPair("vch_arg2", cJSON_help::getcJSONvalue(pItem,"pmVchARG2"));
    key_value_list << qrtVariantPair("vch_arg3", cJSON_help::getcJSONvalue(pItem,"pmVchARG3"));

    key_value_list << qrtVariantPair("vch_arg4", cJSON_help::getcJSONvalue(pItem,"pmVchARG4"));
    key_value_list << qrtVariantPair("vch_arg5", cJSON_help::getcJSONvalue(pItem,"pmVchARG5"));
    key_value_list << qrtVariantPair("vch_arg6", cJSON_help::getcJSONvalue(pItem,"pmVchARG6"));
    key_value_list << qrtVariantPair("vch_arg7", cJSON_help::getcJSONvalue(pItem,"pmVchARG7"));

}



cJSON *w_sys_manage::q_bt_get_piece_material_bt_dish(lds_query &query, const QString &orgCode, int state)
{
    cJSON *pItem = cJSON_CreateObject();

    cJSON_help::filter_cJSON_AddStringToObject( pItem, "intFlowID", query.recordValue("int_flowID"));

    cJSON_help::filter_cJSON_AddStringToObject( pItem, "chDishno", query.recordValue("ch_dishno"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "chItemno", query.recordValue("ch_itemno"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "numNum", query.recordValue("num_num"));

    cJSON_AddNumberToObject(pItem, "state", state);
    cJSON_help::filter_cJSON_AddStringToObject(pItem, "orgCode", orgCode);

    return pItem;
}

void w_sys_manage::q_bt_put_piece_material_bt_dish(qrtVariantPairList &key_value_list, cJSON *pItem)
{
    key_value_list << qrtVariantPair("int_flowID", cJSON_help::getcJSONvalue(pItem,"intFlowID"));
    key_value_list << qrtVariantPair("ch_dishno", cJSON_help::getcJSONvalue(pItem,"chDishno"));
    key_value_list << qrtVariantPair("ch_itemno", cJSON_help::getcJSONvalue(pItem,"chItemno"));
    key_value_list << qrtVariantPair("num_num", cJSON_help::getcJSONvalue(pItem,"numNum"));

}

cJSON *w_sys_manage::q_bt_get_piece_promotion()
{
    cJSON *pItem = cJSON_CreateObject();
    lds_query query;
    //不存在依旧上传
    query.execSelect("select vch_value FROM cey_sys_parameter where vch_parameter = 'promotion_setting_am'; ");cJSON_help::_cJSON_AddStringToObject( pItem, "promotionSettingAm", query.recordValue(0));
    query.execSelect("select vch_value FROM cey_sys_parameter where vch_parameter = 'promotion_setting_am_2'; ");cJSON_help::_cJSON_AddStringToObject( pItem, "promotionSettingAm2", query.recordValue(0));
    query.execSelect("select vch_value FROM cey_sys_parameter where vch_parameter = 'promotion_setting_date'; ");cJSON_help::_cJSON_AddStringToObject( pItem, "promotionSettingDate", query.recordValue(0));
    query.execSelect("select vch_value FROM cey_sys_parameter where vch_parameter = 'promotion_setting_date_2'; ");cJSON_help::_cJSON_AddStringToObject( pItem, "promotionSettingDate2", query.recordValue(0));
    query.execSelect("select vch_value FROM cey_sys_parameter where vch_parameter = 'promotion_setting_pm'; ");cJSON_help::_cJSON_AddStringToObject( pItem, "promotionSettingPm", query.recordValue(0));

    query.execSelect("select vch_value FROM cey_sys_parameter where vch_parameter = 'promotion_setting_pm_2'; ");cJSON_help::_cJSON_AddStringToObject( pItem, "promotionSettingPm2", query.recordValue(0));
    query.execSelect("select vch_value FROM cey_sys_parameter where vch_parameter = 'promotion_setting_Mon'; ");cJSON_help::_cJSON_AddStringToObject( pItem, "promotionSettingMon", query.recordValue(0));
    query.execSelect("select vch_value FROM cey_sys_parameter where vch_parameter = 'promotion_setting_Tues'; ");cJSON_help::_cJSON_AddStringToObject( pItem, "promotionSettingTues", query.recordValue(0));
    query.execSelect("select vch_value FROM cey_sys_parameter where vch_parameter = 'promotion_setting_Wed'; ");cJSON_help::_cJSON_AddStringToObject( pItem, "promotionSettingWed", query.recordValue(0));
    query.execSelect("select vch_value FROM cey_sys_parameter where vch_parameter = 'promotion_setting_Thur'; ");cJSON_help::_cJSON_AddStringToObject( pItem, "promotionSettingThur", query.recordValue(0));

    query.execSelect("select vch_value FROM cey_sys_parameter where vch_parameter = 'promotion_setting_Fri'; ");cJSON_help::_cJSON_AddStringToObject( pItem, "promotionSettingFri", query.recordValue(0));
    query.execSelect("select vch_value FROM cey_sys_parameter where vch_parameter = 'promotion_setting_Sat'; ");cJSON_help::_cJSON_AddStringToObject( pItem, "promotionSettingSat", query.recordValue(0));
    query.execSelect("select vch_value FROM cey_sys_parameter where vch_parameter = 'promotion_setting_Sun'; ");cJSON_help::_cJSON_AddStringToObject( pItem, "promotionSettingSun", query.recordValue(0));
    query.execSelect("select vch_value FROM cey_sys_parameter where vch_parameter = 'promotion_setting_target'; ");cJSON_help::_cJSON_AddStringToObject( pItem, "promotionSettingTarget", query.recordValue(0));

    return pItem;
}

bool w_sys_manage::q_bt_put_piece_promotion(cJSON *pItem)
{
    lds_query sqlQuery;

    if(!sqlQuery.execInsertDuplicateUpdate("cey_sys_parameter", "vch_parameter", qrtVariantPairList() << qrtVariantPair("vch_parameter", "promotion_setting_am")     << qrtVariantPair("vch_value", cJSON_help::getcJSONvalue(pItem,"promotionSettingAm")))) return false;
    if(!sqlQuery.execInsertDuplicateUpdate("cey_sys_parameter", "vch_parameter", qrtVariantPairList() << qrtVariantPair("vch_parameter", "promotion_setting_am_2")   << qrtVariantPair("vch_value", cJSON_help::getcJSONvalue(pItem,"promotionSettingAm2")))) return false;
    if(!sqlQuery.execInsertDuplicateUpdate("cey_sys_parameter", "vch_parameter", qrtVariantPairList() << qrtVariantPair("vch_parameter", "promotion_setting_date")   << qrtVariantPair("vch_value", cJSON_help::getcJSONvalue(pItem,"promotionSettingDate")))) return false;
    if(!sqlQuery.execInsertDuplicateUpdate("cey_sys_parameter", "vch_parameter", qrtVariantPairList() << qrtVariantPair("vch_parameter", "promotion_setting_date_2") << qrtVariantPair("vch_value", cJSON_help::getcJSONvalue(pItem,"promotionSettingDate2")))) return false;
    if(!sqlQuery.execInsertDuplicateUpdate("cey_sys_parameter", "vch_parameter", qrtVariantPairList() << qrtVariantPair("vch_parameter", "promotion_setting_pm")     << qrtVariantPair("vch_value", cJSON_help::getcJSONvalue(pItem,"promotionSettingPm")))) return false;

    if(!sqlQuery.execInsertDuplicateUpdate("cey_sys_parameter", "vch_parameter", qrtVariantPairList() << qrtVariantPair("vch_parameter", "promotion_setting_pm_2")   << qrtVariantPair("vch_value", cJSON_help::getcJSONvalue(pItem,"promotionSettingPm2")))) return false;
    if(!sqlQuery.execInsertDuplicateUpdate("cey_sys_parameter", "vch_parameter", qrtVariantPairList() << qrtVariantPair("vch_parameter", "promotion_setting_Mon")    << qrtVariantPair("vch_value", cJSON_help::getcJSONvalue(pItem,"promotionSettingMon")))) return false;
    if(!sqlQuery.execInsertDuplicateUpdate("cey_sys_parameter", "vch_parameter", qrtVariantPairList() << qrtVariantPair("vch_parameter", "promotion_setting_Tues")   << qrtVariantPair("vch_value", cJSON_help::getcJSONvalue(pItem,"promotionSettingTues")))) return false;
    if(!sqlQuery.execInsertDuplicateUpdate("cey_sys_parameter", "vch_parameter", qrtVariantPairList() << qrtVariantPair("vch_parameter", "promotion_setting_Wed")    << qrtVariantPair("vch_value", cJSON_help::getcJSONvalue(pItem,"promotionSettingWed")))) return false;
    if(!sqlQuery.execInsertDuplicateUpdate("cey_sys_parameter", "vch_parameter", qrtVariantPairList() << qrtVariantPair("vch_parameter", "promotion_setting_Thur")   << qrtVariantPair("vch_value", cJSON_help::getcJSONvalue(pItem,"promotionSettingThur")))) return false;

    if(!sqlQuery.execInsertDuplicateUpdate("cey_sys_parameter", "vch_parameter", qrtVariantPairList() << qrtVariantPair("vch_parameter", "promotion_setting_Fri")    << qrtVariantPair("vch_value", cJSON_help::getcJSONvalue(pItem,"promotionSettingFri")))) return false;
    if(!sqlQuery.execInsertDuplicateUpdate("cey_sys_parameter", "vch_parameter", qrtVariantPairList() << qrtVariantPair("vch_parameter", "promotion_setting_Sat")    << qrtVariantPair("vch_value", cJSON_help::getcJSONvalue(pItem,"promotionSettingSat")))) return false;
    if(!sqlQuery.execInsertDuplicateUpdate("cey_sys_parameter", "vch_parameter", qrtVariantPairList() << qrtVariantPair("vch_parameter", "promotion_setting_Sun")    << qrtVariantPair("vch_value", cJSON_help::getcJSONvalue(pItem,"promotionSettingSun")))) return false;
    if(!sqlQuery.execInsertDuplicateUpdate("cey_sys_parameter", "vch_parameter", qrtVariantPairList() << qrtVariantPair("vch_parameter", "promotion_setting_target") << qrtVariantPair("vch_value", cJSON_help::getcJSONvalue(pItem,"promotionSettingTarget")))) return false;

    return true;
}

cJSON *w_sys_manage::q_m_get_piece_t_m_member_type(lds_query &query, const QString &orgCode, int state)
{
    cJSON *pItem = cJSON_CreateObject();

    cJSON_help::filter_cJSON_AddStringToObject( pItem, "chTypeno", query.recordValue("ch_typeno"));

    cJSON_help::filter_cJSON_AddStringToObject( pItem, "vchTypename", query.recordValue("vch_typename"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "chAttribute", query.recordValue("ch_attribute"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "chFavourable", query.recordValue("ch_favourable"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "chPricekind", query.recordValue("ch_pricekind"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "intDiscount", query.recordValue("int_discount"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "chProjectno", query.recordValue("ch_projectno"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "chCalc", query.recordValue("ch_calc"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "chCalcType", query.recordValue("ch_calc_type"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "intCalcUnit", query.recordValue("int_calc_unit"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "chAreaFlag", query.recordValue("ch_area_flag"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "intBackcashRate", query.recordValue("int_backcash_rate"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "intCardBaseamount", query.recordValue("int_card_baseamount"));

    cJSON_AddNumberToObject(pItem, "state", state);
    cJSON_help::filter_cJSON_AddStringToObject(pItem, "orgCode", orgCode);

    return pItem;
}

void w_sys_manage::q_m_put_piece_t_m_member_type(qrtVariantPairList &key_value_list, cJSON *pItem)
{
    key_value_list << qrtVariantPair("ch_typeno", cJSON_help::getcJSONvalue(pItem,"chTypeno"));
    key_value_list << qrtVariantPair("vch_typename", cJSON_help::getcJSONvalue(pItem,"vchTypename"));
    key_value_list << qrtVariantPair("ch_attribute", cJSON_help::getcJSONvalue(pItem,"chAttribute"));
    key_value_list << qrtVariantPair("ch_favourable", cJSON_help::getcJSONvalue(pItem,"chFavourable"));
    key_value_list << qrtVariantPair("ch_pricekind", cJSON_help::getcJSONvalue(pItem,"chPricekind"));
    key_value_list << qrtVariantPair("int_discount", cJSON_help::getcJSONvalue(pItem,"intDiscount"));
    key_value_list << qrtVariantPair("ch_projectno", cJSON_help::getcJSONvalue(pItem,"chProjectno"));
    key_value_list << qrtVariantPair("ch_calc", cJSON_help::getcJSONvalue(pItem,"chCalc"));
    key_value_list << qrtVariantPair("ch_calc_type", cJSON_help::getcJSONvalue(pItem,"chCalcType"));
    key_value_list << qrtVariantPair("int_calc_unit", cJSON_help::getcJSONvalue(pItem,"intCalcUnit"));
    key_value_list << qrtVariantPair("ch_area_flag", cJSON_help::getcJSONvalue(pItem,"chAreaFlag"));
    key_value_list << qrtVariantPair("int_backcash_rate", cJSON_help::getcJSONvalue(pItem,"intBackcashRate"));
    key_value_list << qrtVariantPair("int_card_baseamount", cJSON_help::getcJSONvalue(pItem,"intCardBaseamount"));

}

cJSON *w_sys_manage::q_m_get_piece_t_m_member(lds_query &query, const QString &orgCode, int state)
{
    cJSON *pItem = cJSON_CreateObject();

    cJSON_help::filter_cJSON_AddStringToObject( pItem, "miVchMemberNo", query.recordValue("vch_memberno"));

    cJSON_help::filter_cJSON_AddStringToObject( pItem, "miVchMember", query.recordValue("vch_member"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "miChTypeNo", query.recordValue("ch_typeno"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "miVchId", query.recordValue("vch_id"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "miDtBirthday", query.recordValue("dt_birthday"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "miVchTel", query.recordValue("vch_tel"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "miVchHandTel", query.recordValue("vch_handtel"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "miVchAddress", query.recordValue("vch_address"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "miDtLimit", query.recordValue("dt_limit"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "miChState", query.recordValue("ch_state"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "numLimit",query.recordValue("num_limit"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "miChCardFlag", query.recordValue("ch_cardflag"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "miVchCardNo", query.recordValue("vch_cardno"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "miVchPassword", query.recordValue("vch_password"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "miVchOperId", query.recordValue("vch_operID"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "miDtOperDate", query.recordValue("dt_operdate"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "miIntBasedata", query.recordValue("int_basedata"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "miDtSendTime", query.recordValue("dt_sendtime"));

    cJSON_AddNumberToObject(pItem, "state", state);
    cJSON_help::filter_cJSON_AddStringToObject(pItem, "orgCode", orgCode);

    return pItem;
}

void w_sys_manage::q_m_put_piece_t_m_member(qrtVariantPairList &key_value_list, cJSON *pItem)
{
    key_value_list << qrtVariantPair("vch_memberno", cJSON_help::getcJSONvalue(pItem,"miVchMemberNo"));
    key_value_list << qrtVariantPair("vch_member", cJSON_help::getcJSONvalue(pItem,"miVchMember"));
    key_value_list << qrtVariantPair("ch_typeno", cJSON_help::getcJSONvalue(pItem,"miChTypeNo"));
    key_value_list << qrtVariantPair("vch_id", cJSON_help::getcJSONvalue(pItem,"miVchId"));
    key_value_list << qrtVariantPair("dt_birthday", cJSON_help::getcJSONvalue(pItem,"miDtBirthday"));
    key_value_list << qrtVariantPair("vch_tel", cJSON_help::getcJSONvalue(pItem,"miVchTel"));
    key_value_list << qrtVariantPair("vch_handtel", cJSON_help::getcJSONvalue(pItem,"miVchHandTel"));
    key_value_list << qrtVariantPair("vch_address", cJSON_help::getcJSONvalue(pItem,"miVchAddress"));
    key_value_list << qrtVariantPair("dt_limit", cJSON_help::getcJSONvalue(pItem,"miDtLimit"));
    key_value_list << qrtVariantPair("ch_state", cJSON_help::getcJSONvalue(pItem,"miChState"));
    key_value_list << qrtVariantPair("num_limit", cJSON_help::getcJSONvalue(pItem,"numLimit"));
    key_value_list << qrtVariantPair("ch_cardflag", cJSON_help::getcJSONvalue(pItem,"miChCardFlag"));
    key_value_list << qrtVariantPair("vch_cardno", cJSON_help::getcJSONvalue(pItem,"miVchCardNo"));
    key_value_list << qrtVariantPair("vch_password", cJSON_help::getcJSONvalue(pItem,"miVchPassword"));
    key_value_list << qrtVariantPair("vch_operID", cJSON_help::getcJSONvalue(pItem,"miVchOperId"));
    key_value_list << qrtVariantPair("dt_operdate", cJSON_help::getcJSONvalue(pItem,"miDtOperDate"));
    key_value_list << qrtVariantPair("int_basedata", cJSON_help::getcJSONvalue(pItem,"miIntBasedata"));
    key_value_list << qrtVariantPair("dt_sendtime", cJSON_help::getcJSONvalue(pItem,"miDtSendTime"));

}

cJSON *w_sys_manage::q_m_get_piece_t_m_gift(lds_query &query, const QString &orgCode, int state)
{
    cJSON *pItem = cJSON_CreateObject();

    cJSON_help::filter_cJSON_AddStringToObject( pItem, "intFlowid", query.recordValue("int_flowID"));

    cJSON_help::filter_cJSON_AddStringToObject( pItem, "chGiftno", query.recordValue("ch_giftno"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "vhcGifname", query.recordValue("vch_giftname"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "numPoint", query.recordValue("num_point"));

    cJSON_AddNumberToObject(pItem, "state", state);
    cJSON_help::filter_cJSON_AddStringToObject(pItem, "orgCode", orgCode);

    return pItem;
}

void w_sys_manage::q_m_put_piece_t_m_gift(qrtVariantPairList &key_value_list, cJSON *pItem)
{
    key_value_list << qrtVariantPair("int_flowID",cJSON_help::getcJSONvalue(pItem,"intFlowid"));
    key_value_list << qrtVariantPair("ch_giftno",cJSON_help::getcJSONvalue(pItem,"chGiftno"));
    key_value_list << qrtVariantPair("vch_giftname",cJSON_help::getcJSONvalue(pItem,"vhcGifname"));
    key_value_list << qrtVariantPair("num_point",cJSON_help::getcJSONvalue(pItem,"numPoint"));

}


cJSON *w_sys_manage::q_m_get_piece_t_m_curamount(lds_query &query, const QString &orgCode, int state)
{
    cJSON *pItem = cJSON_CreateObject();

    cJSON_help::filter_cJSON_AddStringToObject( pItem, "vchMemberno", query.recordValue("vch_memberno"));

    cJSON_help::filter_cJSON_AddStringToObject( pItem, "numAmount", query.recordValue("num_amount"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "numHangamount", query.recordValue("num_hangamount"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "numPoint", query.recordValue("num_point"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "vchOperId", query.recordValue("vch_operID"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "dtOperdate", query.recordValue("dt_operdate"));
    cJSON_help::filter_cJSON_AddStringToObject( pItem, "chBranchno", query.recordValue("ch_branchno"));

    cJSON_AddNumberToObject(pItem, "state", state);
    cJSON_help::filter_cJSON_AddStringToObject(pItem, "orgCode", orgCode);

    return pItem;
}

void w_sys_manage::q_m_put_piece_t_m_curamount(qrtVariantPairList &key_value_list, cJSON *pItem)
{
    key_value_list << qrtVariantPair("vch_memberno",cJSON_help::getcJSONvalue(pItem,"vchMemberno"));
    key_value_list << qrtVariantPair("num_amount",cJSON_help::getcJSONvalue(pItem,"numAmount"));
    key_value_list << qrtVariantPair("num_hangamount",cJSON_help::getcJSONvalue(pItem,"numHangamount"));
    key_value_list << qrtVariantPair("num_point",cJSON_help::getcJSONvalue(pItem,"numPoint"));
    key_value_list << qrtVariantPair("vch_operID",cJSON_help::getcJSONvalue(pItem,"vchOperId"));
    key_value_list << qrtVariantPair("dt_operdate",cJSON_help::getcJSONvalue(pItem,"dtOperdate"));
    key_value_list << qrtVariantPair("ch_branchno",cJSON_help::getcJSONvalue(pItem,"chBranchno"));

}

cJSON *w_sys_manage::q_m_get_piece_t_m_parameter(lds_query &query, const QString &orgCode)
{
    cJSON *pItem = cJSON_CreateObject();

    cJSON_help::filter_cJSON_AddStringToObject(pItem, "vchParameter", query.recordValue("vch_parameter"));

    cJSON_help::filter_cJSON_AddStringToObject(pItem, "vchValue", query.recordValue("vch_value"));
    cJSON_help::filter_cJSON_AddStringToObject(pItem, "vchExplain", query.recordValue("vch_explain"));

    cJSON_help::filter_cJSON_AddStringToObject(pItem, "orgCode", orgCode);

    return pItem;
}

void w_sys_manage::q_m_put_piece_t_m_parameter(qrtVariantPairList &key_value_list, cJSON *pItem)
{
    key_value_list << qrtVariantPair("vch_parameter", cJSON_help::getcJSONvalue(pItem,"vchParameter").toString());
    key_value_list << qrtVariantPair("vch_value", cJSON_help::getcJSONvalue(pItem,"vchValue").toString());
    key_value_list << qrtVariantPair("vch_explain", cJSON_help::getcJSONvalue(pItem,"vchExplain").toString());

}
