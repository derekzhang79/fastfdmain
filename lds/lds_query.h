#ifndef LDS_QUERY_H
#define LDS_QUERY_H

#include "lds_query_db.h"

class lds_query:public lds_query_db
{
public:
    static QVariant selectValue(lds_query_db &query, const QString &sql, int i = 0);
    static QVariant selectValue(const QString &sql, int i = 0);
public:
    explicit lds_query();

    static bool tran_saction();
    static bool com_mit();
    static bool roll_back();

protected:
    bool exec(const QString &query);
    bool exec();
};


#endif // LDS_QUERY_H
