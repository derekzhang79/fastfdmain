#include "lds_query.h"
#include "lds_thread.h"
#include "lds.h"
#include <QtDebug>
#include <QApplication>
#include <QSqlError>
#include <QMap>

QVariant lds_query::selectValue(lds_query_db &query, const QString &sql, int i)
{
    query.execSelect(sql);
    query.next();
    return query.recordValue(i);
}

QVariant lds_query::selectValue(const QString &sql, int i)
{
    lds_query q;
    return selectValue(q, sql, i);
}

lds_query::lds_query()
    :lds_query_db(lds_thread::getThreadDbAndOpen())
{

}

bool lds_query::exec(const QString &query)
{
    bool ret = lds_query_db::exec(query);
    return ret;
}

bool lds_query::exec()
{
    return lds_query_db::exec();
}

bool lds_query::tran_saction()
{
#ifdef QT_DEBUG
    qDebug() << __FUNCTION__ << get_db_state();
    if(get_db_state() != 0) {
        qDebug("transaction , state != 0 ");
        qDebug() << get_db_state();
        QObject *o = 0;
        o->children();
    }
#endif
    get_db_state()++;
    return lds_thread::getThreadDbAndOpen().transaction();
}

bool lds_query::com_mit()
{
#ifdef QT_DEBUG;
    qDebug() << __FUNCTION__ << get_db_state();
    if(get_db_state() != 1) {
        qDebug("commit , state != 1 ");
        qDebug() << get_db_state();
        QObject *o = 0;
        o->children();
    }
#endif
    get_db_state()--;
    return lds_thread::getThreadDbAndOpen().commit();
}

bool lds_query::roll_back()
{
#ifdef QT_DEBUG
    qDebug() << __FUNCTION__ << get_db_state();
    if(get_db_state() != 1) {
        qDebug("rollback , state != 1 ");
        qDebug() << get_db_state();
        QObject *o = 0;
        o->children();
    }
#endif
    get_db_state()--;
    return lds_thread::getThreadDbAndOpen().rollback();
}
