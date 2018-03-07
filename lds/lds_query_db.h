#ifndef LDS_QUERY_DB_H
#define LDS_QUERY_DB_H

#include <QDateTime>
#include <QSqlQuery>
#include <QVariant>

class qrtVariant {
public:
    enum qrtVariantType{
        NONE = 0,
        FUNC = 1,
        VAL = 2,
    };

    inline qrtVariant(int i, qrtVariantType type = VAL):_value(i),_type(type){}
    inline qrtVariant(uint ui, qrtVariantType type = VAL):_value(ui),_type(type){}
    inline qrtVariant(qlonglong ll, qrtVariantType type = VAL):_value(ll),_type(type){}
    inline qrtVariant(qulonglong ull, qrtVariantType type = VAL):_value(ull),_type(type){}
    inline qrtVariant(bool b, qrtVariantType type = VAL):_value(b),_type(type){}
    inline qrtVariant(double d, qrtVariantType type = VAL):_value(d),_type(type){}
    inline qrtVariant(float f, qrtVariantType type = VAL):_value(f),_type(type){}

#ifndef QT_NO_CAST_FROM_ASCII
    QT_ASCII_CAST_WARN qrtVariant(const char *str, qrtVariantType type = VAL):_value(str),_type(type){}
#endif
    inline qrtVariant(const QByteArray &bytearray, qrtVariantType type = VAL):_value(bytearray),_type(type){}
    inline qrtVariant(const QBitArray &bitarray, qrtVariantType type = VAL):_value(bitarray),_type(type){}
    inline qrtVariant(const QString &string, qrtVariantType type = VAL):_value(string),_type(type){}
    inline qrtVariant(const QDate &date, qrtVariantType type = VAL):_value(date),_type(type){}
    inline qrtVariant(const QTime &time, qrtVariantType type = VAL):_value(time),_type(type){}
    inline qrtVariant(const QDateTime &datetime, qrtVariantType type = VAL):_value(datetime),_type(type){}

    inline qrtVariant(const QVariant &val, qrtVariantType type = VAL):_value(val),_type(type){}

    inline operator QVariant () const{ return _value;}//对于debug和qvarint d = qrtVariant() 起作用
    inline const QVariant & getValue() const{ return _value;}
    inline qrtVariantType getType() const {return _type;}

    void setType(qrtVariantType type) const;
    QString toString()const;
protected:
    QVariant _value;
    qrtVariantType _type;
};


#define qrtEqual(key, value)		QString("%1 = '%2'").arg(key).arg(QString("%1").arg(value).replace("'", "\\'"))
#define qrtFunc(value)  qrtVariant(value, qrtVariant::FUNC)
#define yyyyMMddhhmmss "yyyy-MM-dd hh:mm:ss"

typedef QPair<QString, qrtVariant> qrtVariantPair;
typedef QList<qrtVariantPair> qrtVariantPairList;
typedef QPair<QString, QString> QStringPair;
typedef QList<QStringPair> QStringPairList;

class lds_query_db:public QSqlQuery
{
protected:
    static QMap<QString, int> dbname_state_map;
    static int & get_db_state();
public:
    explicit lds_query_db(QSqlDatabase db);
    QSqlDatabase db;
    QDateTime curdate();
    QString curdatestr();

    ///
    bool execUpdate(const QString &table,
                    const qrtVariantPairList &key_value_list,
                    const QString &where_sql,
                    QString *errstring = 0);
    bool execUpdate(const QString &table,
                    const QString &key1, const qrtVariant &value1,
                    const QString &where_sql,
                    QString *errstring = 0);

    bool execUpdate(const QString &table,
                    const QString &key1, const qrtVariant &value1,
                    const QString &key2, const qrtVariant &value2,
                    const QString &where_sql,
                    QString *errstring = 0);
    bool execUpdate(const QString &table,
                    const QStringList &unique_key_list,
                    const QString &selectStr_selectKeyInTable,
                    QString *errstring = 0);
    //insert duplicate update
    bool execInsertDuplicateUpdate(const QString &table,
                                   const QStringList &unique_key_list,
                                   const qrtVariantPairList &key_value_list,
                                   QString *errstring = 0);
    bool execInsertDuplicateUpdate(const QString &table,
                                   const QString &unique_key1,
                                   const qrtVariantPairList &key_value_list,
                                   QString *errstring = 0);
    bool execInsertDuplicateUpdate(const QString &table,
                                   const QString &unique_key1,
                                   const QString &unique_key2,
                                   const qrtVariantPairList &key_value_list,
                                   QString *errstring = 0);
    ///
    bool execInsert(const QString &table,
                    const qrtVariantPairList &key_value_list,
                    QString *errstring = 0);
    bool execInsert(const QString &table,
                    const QString &keys_str, //a, b, c
                    const QString &values_str, //a, b, c
                    const QString &split,
                    QString *errstring = 0);
    bool execInsert(const QString &table,
                    const QString &values_str,//a, b, c
                    const QString &split,
                    QString *errstring = 0);
    bool execInsert(const QString &table,
                    const QString &selectStr_selectKeyInTable,//select sql
                    QString *errstring = 0);

    ///
    bool execDelete(const QString &table,
                    QString *errstring = 0);
    bool execDelete(const QString &table,
                    const QString &where_sql,
                    QString *errstring = 0);
    bool execDeleteSelectKey(const QStringList &tableList,
                    const QString &selectStr_tableListKeyForColumnIndex,
                    QString *errstring = 0);
    bool execDeleteSelectKey(const QString &table,
                    const QString &selectStr_tableKeyForColumn0,
                    QString *errstring = 0);
    bool execDeleteSelectKey(const QString &table,
                    const QString &table2,
                    const QString &selectStr_tableKeyForColumn0_table2KeyForColumn1,
                    QString *errstring = 0);
    bool execDeleteSelectKey(const QString &table,
                    const QString &table2,
                    const QString &table3,
                    const QString &selectStr_tableKeyForColumn0_table2KeyForColumn1_table3KeyForColumn2,
                    QString *errstring = 0);
    ///
    bool execSelect(const QString &sql, QString *errstring = 0);
    ///
    bool execCreate(const QString &sql, QString *errstring = 0);
    ///
    bool execDirect(const QString &sql, QString *errstring = 0);
    ///
    bool execAlter(const QString &sql, QString *errstring = 0);
    ///
    bool execTrigger(const QString &sql, QString *errstring = 0);
    ///
    bool execDrop(const QString &table, QString *errstring = 0);

    ///
    int recordCount();
    QVariant recordValue(const QString& name);
    QVariant recordValue(int i);

    QString recordError();
    QString insertSql(const QString &table, const qrtVariantPairList &key_value_list);
    QString updateSql(const QString &table, const qrtVariantPairList &key_value_list, const QString &where_sql);
protected:
    bool exec(const QString &query, QString *errstring = 0);
    bool exec(QString *errstring = 0);
    bool prepare(const QString &query, QString *errstring = 0);
    QSqlError lastError() const;
    bool emptyDataIsDateTime(const QString &table, const qrtVariantPair &pair);
    QString getWhereSql(const QStringList &unique_key_list, const qrtVariantPairList &key_value_list);

    //更新的触发器
    void execUpdateTrigger(const QString &table,
                           const qrtVariantPairList &key_value_list,
                           const QString &where_sql);

    //插入的触发器
    void execInsertTrigger(const QString &table,
                           const qrtVariantPairList &key_value_list);
};

#endif // LDS_QUERY_DB_H
