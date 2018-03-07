#ifndef W_I_INVENTORY_H
#define W_I_INVENTORY_H

#include <QObject>
#include "lds_query.h"

#include <QVariant>
#include <QStringList>
#include <QStandardItemModel>
#include <QtDebug>
#include "backheader.h"

//w_i_inventory::MODEL_INVENTORY_INIT();

class w_i_inventory : public QObject
{
    Q_OBJECT
private:
    explicit w_i_inventory(QObject *parent = 0);
    //会员状态对照表
    static QStandardItemModel MODEL_INVENTORY_TYPE;//t_m_member, ch_state
    //会员状态对照表
    static QStandardItemModel MODEL_INVENTORY_SHEET_TYPE;//t_m_member, ch_state
    //报表状态，审核，作废之类
    static QStandardItemModel MODEL_INVENTORY_STATE_DO;//t_m_member, ch_state
    //报表状态，审核，作废之类
    static QStandardItemModel MODEL_INVENTORY_STATE_TYPE;//t_m_member, ch_state
public:

    static void clearCache();
    //会员状态对照表
    static QStandardItemModel& get_MODEL_INVENTORY_TYPE();//t_m_member, ch_state
    //会员状态对照表
    static QStandardItemModel& get_MODEL_INVENTORY_SHEET_TYPE();//t_m_member, ch_state
    //报表状态，审核，作废之类
    static QStandardItemModel& get_MODEL_INVENTORY_STATE_DO();//t_m_member, ch_state
    //报表状态，审核，作废之类
    static QStandardItemModel& get_MODEL_INVENTORY_STATE_TYPE();//t_m_member, ch_state

    static QStringList inventory_queryfilter(const QString &sql);

    //获取操作员的 name 更具id
    static QString inventory_getopername(const QString &operid);
};


#endif // W_I_INVENTORY_H
