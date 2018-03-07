#include "lds_model_mapwidget_vertical.h"
#include <QtDebug>
#include "lds.h"


lds_model_mapwidget_vertical::lds_model_mapwidget_vertical(QWidget *parent) : QDialog(parent)
{
    tablemodel = 0;
    mapper = 0;
}

void lds_model_mapwidget_vertical::scan_all_objectname_add_map_setFilter(const QString &sql_key_name, const QString &key)
{
    map.clear();
    mapper->clearMapping();
    //1
    tablemodel->setFilter(sql_key_name + " like '%" + key + "%' ");
    scan_one(this, key);
    //2
    for(int row = 0, rowcount = tablemodel->rowCount(); row < rowcount; row++) {
        QMap<QString, QWidget *>::iterator itr = map.find(tablemodel->model_data(row, "vch_parameter").toString());
        if(itr != map.end()) {
            QString classname = QString(itr.value()->metaObject()->className());
            if(classname == "QTextEdit") {
                mapper->addMapping(itr.value(), row, "plainText");
            } else if(classname == "lds_ComboBox"){
                mapper->addMapping(itr.value(), row, "comusrdata");
            } else if(classname == "lds_timeEdit"){
                mapper->addMapping(itr.value(), row, "time");
            } else if(classname == "lds_datetimeEdit"){
                mapper->addMapping(itr.value(), row, "dateTime");
            } else {
                mapper->addMapping(itr.value(), row);
            }
            map.take(itr.key());
        }
    }
    //3
    if(map.count() > 0) {
        QMapIterator<QString, QWidget *> mapitr(map);
        while(mapitr.hasNext()) {
            mapitr.next();
            insert_sql_objectname(mapitr.key());
        }
        scan_all_objectname_add_map_setFilter(sql_key_name, key);
    }
}

bool lds_model_mapwidget_vertical::mapper_save_data()
{
    QString errstring;
    bool f = tablemodel->trySubmitAll(&errstring);
    mapper->setCurrentIndex(1);
    return f;
}

void lds_model_mapwidget_vertical::scan_one(QObject *o, const QString &filter)
{
    if(o) {
        foreach(QObject *oo, o->children()) {
            scan_one(oo, filter);
            if(oo->objectName().contains(filter)){
                map.insert(oo->objectName(), qobject_cast<QWidget *>(oo));
            }
        }
    }
}

