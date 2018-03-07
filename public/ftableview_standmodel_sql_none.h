#ifndef FTABLEVIEW_STANDMODEL_SQL_NONE_H
#define FTABLEVIEW_STANDMODEL_SQL_NONE_H

#include  "ftableview_standmodel_virtual.h"
#include <QSet>
#include <QMap>


class ftableview_standmodel_sql_none : public ftableview_standmodel_virtual
{
    Q_OBJECT
public:
    struct noneData{
        noneData(){}
        noneData(const QString &u, const QString &u2){
            setData(Qt::UserRole, u);
            setData(Qt::UserRole + 2, u2);
        }
        inline void setData(int role, const QVariant &k){  userdata_map.insert(role, k);}
        inline QString userdata() const {return userdata_map.value(Qt::UserRole).toString();}
        inline QString userdata_2() const {return userdata_map.value(Qt::UserRole + 2).toString();}
        inline QString userdata_1() const {return userdata_map.value(Qt::UserRole + 1).toString();}
        QMap<int, QVariant> userdata_map;
    };

public:
    explicit ftableview_standmodel_sql_none(QObject *parent = 0);
    //!
    void add_list_data(const QList<noneData> &list);
    void toinit();
    bool atEnd();
    /**
setSelectionModel 外部调用该函数才可以使用本函数*/
    QString get_value_select();//u u1 u2 返回u2的拼接，若：001，003， 004=》001003004
    QVariantList get_value_select_list(int deep);
    QString get_value_unselect();//u u1 u2 返回u2的拼接
    bool selectCurUData2(const QString &u2);
private:
    QList<noneData> _data_list;
    //!~
public:
    virtual void togenerate(int index, int updownum, bool hasnext);
    virtual void togenerateNext();
    virtual void togeneratePre();

};

#endif // FTABLEVIEW_STANDMODEL_SQL_NONE_H
