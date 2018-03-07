#ifndef CJSON_HELP_H
#define CJSON_HELP_H
#include <QVariant>
class cJSON;
class cJSON_help
{
public:
    cJSON_help();
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    static cJSON* getcJSON(cJSON *root, const char *string);
    static QVariant getcJSONvalue(cJSON *root, const char *string);
    static QVariant getcJSONvalue(const QByteArray &d, const char *string);
    static QVariant getItemValue(cJSON *item);
    static void replaceItemValue(cJSON *item, const char *key, const QVariant &data);
    static void debug(cJSON *item, QString *ret = 0);
    static QString getcJSONprint(cJSON *print);
    static void jsonPrint(cJSON *item);
    static void filter_cJSON_AddStringToObject(cJSON *item, const char *key, const QVariant &data);
    static void _cJSON_AddStringToObject(cJSON *item, const char *key, const QVariant &data, bool skip_empty = false);

    static int array_count(cJSON *array);
};

class cJSON_auto_del{
public:
    cJSON_auto_del(cJSON* o) ;
    ~cJSON_auto_del();

private:
    cJSON *_o;
};


//A=B>, C = B>, D=C>;
//A=A>
class cJSONHttpData{
public:
    cJSONHttpData();
    /*
1. cJSONHttpData a(new cJSON);
2. cJSONHttpData a  = new cJSON;
*/
    cJSONHttpData(cJSON *json);

    /*
1. cJSONHttpData  c;
    cJSONHttpData  d;
    c = d*/
    cJSONHttpData &operator=(const cJSONHttpData &other);

    ~cJSONHttpData();

    void delJson();

    cJSONHttpData &initRef();
    void jsonPrint();
    QString jsonToString();

    int httpStatus;//success:200
    cJSON *json;
    QString message;
    QString code;
    bool valueTo;
    bool valueFrom;
};

#endif // CJSON_HELP_H
