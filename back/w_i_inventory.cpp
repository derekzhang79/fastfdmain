#include "w_i_inventory.h"

QStandardItemModel w_i_inventory::MODEL_INVENTORY_TYPE;
QStandardItemModel w_i_inventory::MODEL_INVENTORY_STATE_TYPE;
QStandardItemModel w_i_inventory::MODEL_INVENTORY_STATE_DO;
QStandardItemModel w_i_inventory::MODEL_INVENTORY_SHEET_TYPE;

w_i_inventory::w_i_inventory(QObject *parent) :
    QObject(parent)
{
}

void w_i_inventory::clearCache()
{
    MODEL_INVENTORY_TYPE.clear();
    MODEL_INVENTORY_STATE_TYPE.clear();
    MODEL_INVENTORY_STATE_DO.clear();
    MODEL_INVENTORY_SHEET_TYPE.clear();

}

QStandardItemModel& w_i_inventory::get_MODEL_INVENTORY_TYPE()
{
    QList<QStandardItem *> items;
    if(MODEL_INVENTORY_TYPE.rowCount() == 0){
        items.clear();items<<new QStandardItem("001")<<new QStandardItem(tr("采购入库"));MODEL_INVENTORY_TYPE.appendRow(items);
        items.clear();items<<new QStandardItem("002")<<new QStandardItem(tr("退货出货"));MODEL_INVENTORY_TYPE.appendRow(items);
        items.clear();items<<new QStandardItem("004")<<new QStandardItem(tr("物料报损"));MODEL_INVENTORY_TYPE.appendRow(items);
    }
    return MODEL_INVENTORY_TYPE;
}

QStandardItemModel& w_i_inventory::get_MODEL_INVENTORY_SHEET_TYPE()
{
    QList<QStandardItem *> items;
    if(MODEL_INVENTORY_SHEET_TYPE.rowCount() == 0){
        items.clear();items<<new QStandardItem("RK")<<new QStandardItem(tr("001"));MODEL_INVENTORY_SHEET_TYPE.appendRow(items);
        items.clear();items<<new QStandardItem("TH")<<new QStandardItem(tr("002"));MODEL_INVENTORY_SHEET_TYPE.appendRow(items);
        items.clear();items<<new QStandardItem("CK")<<new QStandardItem(tr("004"));MODEL_INVENTORY_SHEET_TYPE.appendRow(items);
    }
    return MODEL_INVENTORY_SHEET_TYPE;
}

QStandardItemModel &w_i_inventory::get_MODEL_INVENTORY_STATE_DO()
{
    QList<QStandardItem *> items;
    if(MODEL_INVENTORY_STATE_DO.rowCount() == 0){
        items.clear();items<<new QStandardItem("N")<<new QStandardItem(tr("未审核"));MODEL_INVENTORY_STATE_DO.appendRow(items);
        items.clear();items<<new QStandardItem("Y")<<new QStandardItem(tr("审核"));MODEL_INVENTORY_STATE_DO.appendRow(items);
        items.clear();items<<new QStandardItem("O")<<new QStandardItem(tr("作废"));MODEL_INVENTORY_STATE_DO.appendRow(items);
    }
    return MODEL_INVENTORY_STATE_DO;
}

QStandardItemModel& w_i_inventory::get_MODEL_INVENTORY_STATE_TYPE()
{
    QList<QStandardItem *> items;
    if(MODEL_INVENTORY_STATE_TYPE.rowCount() == 0){
        items.clear();items<<new QStandardItem("N")<<new QStandardItem(tr("未审核"));MODEL_INVENTORY_STATE_TYPE.appendRow(items);
        items.clear();items<<new QStandardItem("Y")<<new QStandardItem(tr("已审核"));MODEL_INVENTORY_STATE_TYPE.appendRow(items);
        items.clear();items<<new QStandardItem("O")<<new QStandardItem(tr("已作废"));MODEL_INVENTORY_STATE_TYPE.appendRow(items);
    }
    return MODEL_INVENTORY_STATE_TYPE;
}


QStringList w_i_inventory::inventory_queryfilter(const QString &sql)
{
    QStringList rets;
    if(sql.isEmpty()) return rets;
    lds_query query;
    query.execSelect(sql);
    while(query.next()){
        rets << QString("%1(%2)")
                .arg(query.recordValue(1).toString())
                .arg(query.recordValue(0).toString());
    }
    return rets;
}

QString w_i_inventory::inventory_getopername(const QString &operid)
{
    lds_query query;
    query.execSelect(QString("select * from ("+backheader::SELECT_OPERID_NAME+" )t where t.vch_operID='%1'  ")
               .arg(operid));
    query.next();
    return query.recordValue("vch_operator").toString();

}
