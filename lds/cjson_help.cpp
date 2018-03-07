#include "cjson_help.h"
#include "cJSON.h"
#include <QtDebug>
#include <QDateTime>

static QString dateFormatStr="yyyy-MM-dd hh:mm:ss";

cJSON_help::cJSON_help()
{
}

cJSON *cJSON_help::getcJSON(cJSON *root, const char *string)
{
    if(root == 0)
        return 0;
    return cJSON_GetObjectItem(root, string);
}

QVariant cJSON_help::getcJSONvalue(cJSON *root, const char *string)
{
    if(root == 0) {
        qDebug() << "root == 0";
        return QVariant();
    }

    cJSON *item = cJSON_GetObjectItem(root,string);
    return getItemValue(item);
}

QVariant cJSON_help::getcJSONvalue(const QByteArray &d, const char *string)
{
    cJSON * p = cJSON_Parse(d.data());
    if(p) {
        cJSON_auto_del x(p);
        return getcJSONvalue(p, string);
    }
    return QVariant();
}

QVariant cJSON_help::getItemValue(cJSON *item)
{
    QVariant ret;
    if(0 == item) {
        return ret;
    }
    if(item->type == cJSON_String){
        ret = item->valuestring;
    } else if(item->type == cJSON_Number){
        if(item->valuedouble != double(item->valueint)){
            ret = item->valuedouble;
        } else {
            ret = item->valueint;
        }
    }
    return ret;
}

void cJSON_help::replaceItemValue(cJSON *item, const char *key, const QVariant &data)
{
    if(cJSON_GetObjectItem(item, key)) {
        cJSON_DeleteItemFromObject(item, key);
        filter_cJSON_AddStringToObject(item, key, data);
    }
}

void cJSON_help::debug(cJSON *print, QString *ret) {
    if(print==NULL) {
        return;
    }

    char* arrayDate = cJSON_Print(print);
    QString param=QString(arrayDate);
    free(arrayDate);
    if(ret) *ret = param;
}

QString cJSON_help::getcJSONprint(cJSON *print)
{
    if(print==NULL) {
        return "";
    }

    char* arrayDate = cJSON_Print(print);
    QString param=QString(arrayDate);
    free(arrayDate);

    return param;
}

void cJSON_help::jsonPrint(cJSON *item)
{
    qDebug() << getcJSONprint(item);
}

void cJSON_help::filter_cJSON_AddStringToObject(cJSON *item, const char *key, const QVariant &data)
{
    _cJSON_AddStringToObject(item, key, data, true);
}

void cJSON_help::_cJSON_AddStringToObject(cJSON *item, const char *key, const QVariant &data, bool skip_empty)
{
    if(data.type() == QVariant::DateTime) {
        if(data.isNull() || false == data.toDateTime().isValid() || data.toString().isEmpty()) {
            cJSON_AddNullToObject(item, key);
            return;
        }
        cJSON_AddStringToObject(item, key, data.toDateTime().toString(dateFormatStr).toLocal8Bit().data());
        return;
    }

    if(data.type() == QVariant::Double ) {
        cJSON_AddNumberToObject(item, key, data.toDouble());
        return;
    }
    if(data.type() == QVariant::Int ) {
        cJSON_AddNumberToObject(item, key, data.toInt());
        return;
    }
    if(data.type() == QVariant::LongLong ) {
        cJSON_AddNumberToObject(item, key, data.toLongLong());
        return;
    }
    if(data.type() == QVariant::Bool) {
        cJSON_AddBoolToObject(item, key, data.toBool());
        return;
    }
    if(skip_empty && data.toString().isEmpty()) {
        return;
    }
    cJSON_AddStringToObject(item, key, data.toString().toLocal8Bit().data());
}

int cJSON_help::array_count(cJSON *array)
{
    if(array == 0) return 0;
    return cJSON_GetArraySize(array);
}

cJSON_auto_del::cJSON_auto_del(cJSON *o)
{
    _o = o;
}

cJSON_auto_del::~cJSON_auto_del()
{
    if(_o) {
        cJSON_Delete(_o);
        _o = 0;
    }
}

/////////////

cJSONHttpData::cJSONHttpData()
{
    this->json = 0;//必须在单独初始化下
    initRef();
}

cJSONHttpData::cJSONHttpData(cJSON *json)
{
    this->json = 0;//必须在单独初始化下
    initRef();
    this->json = json;
    valueFrom = true;
}

cJSONHttpData::~cJSONHttpData()
{
    delJson();
}

void cJSONHttpData::delJson()
{
    if(false == valueTo && json) {
        qDebug("delete json");
        cJSON_Delete(json);
        json = 0;
    }
}

//A=C
//A=B
//A=B=C, B=D
cJSONHttpData &cJSONHttpData::operator=(const cJSONHttpData &other)
{
    //B = A, C = A交叉传递不允许
    if(other.valueTo) {
        qWarning("Warning:valueTo = true");
        QObject *o = 0;
        o->children();
    }
    //A = B, 若 A.json必须为0
    if(valueFrom) {
        qWarning("Warning:valueFrom = true");
        QObject *o = 0;
        o->children();
    }

    this->httpStatus = other.httpStatus;
    this->json = other.json;
    this->message = other.message;
    this->code = other.code;

    ((cJSONHttpData*)&other)->valueTo = true;
    this->valueFrom = true;
    return *this;
}

//C = B, B = A;, 当B = E; 由于B的valueTo = true所有delJson是不会清空B.json的
cJSONHttpData &cJSONHttpData::initRef()
{
    delJson();

    httpStatus = 0;
    json = 0;
    message = "";
    code = "";
    valueTo = false;
    valueFrom = false;

    return *this;
}

void cJSONHttpData::jsonPrint()
{
    QString ret;
    if(json ) {
        ret = cJSON_help::getcJSONprint(json);
    }
    qDebug() << "json data:" << ret;
}

QString cJSONHttpData::jsonToString()
{
    QString ret;
    if(json ) {
        ret = cJSON_help::getcJSONprint(json);
    }
    return ret;
}
