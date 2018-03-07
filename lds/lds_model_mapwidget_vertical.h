#ifndef LDS_MODEL_MAPWIDGET_VERTICAL_H
#define LDS_MODEL_MAPWIDGET_VERTICAL_H

#include <QDialog>
#include <QDataWidgetMapper>
#include "lds_model_sqltablemodel.h"

class lds_model_mapwidget_vertical : public QDialog
{
public:
    lds_model_mapwidget_vertical(QWidget *parent = 0);
    lds_model_sqltablemodel *tablemodel;
protected:
    void scan_all_objectname_add_map_setFilter(const QString &sql_key_name, const QString &key);

    //不存在则插入空值
    virtual bool insert_sql_objectname(const QString &objectname) = 0;

    QDataWidgetMapper *mapper;

    bool mapper_save_data();
private:
    void scan_one(QObject *o, const QString &filter);
    QMap<QString, QWidget *> map;
};
#endif // LDS_MODEL_MAPWIDGET_VERTICAL_H
