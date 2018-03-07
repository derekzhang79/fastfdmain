#include "lds_model_sqltablemodel_d.h"
#include <QComboBox>
#include <QtDebug>
/*
        vch_dishname
10010
10011
10012
10013
*/

void lds_model_sqltablemodel_d::fillData(QComboBox *com, const lds_model_sqltablemodel_d *d)
{
    com->clear();
    for(QMap<QString, int>::const_iterator itr = d->getMapRowstr().begin();
        itr != d->getMapRowstr().end(); itr++) {
        com->addItem(d->data(itr.key()).toString(), itr.key());
    }
}

lds_model_sqltablemodel_d::lds_model_sqltablemodel_d()
{
}

lds_model_sqltablemodel_d::~lds_model_sqltablemodel_d()
{
    clear();
}


bool lds_model_sqltablemodel_d::isExisted(const QString &rowstr, const QString &columnstr) const
{
    return isExistedRow(rowstr) && isExistedColumn(columnstr);
}

bool lds_model_sqltablemodel_d::isExistedRow(const QString &rowstr) const
{
    return map_rowstr.value(rowstr, -1) != -1;
}

bool lds_model_sqltablemodel_d::isExistedColumn(const QString &columnstr) const
{
    return map_columnstr.value(columnstr, -1) != -1;
}

bool lds_model_sqltablemodel_d::isValid(const QString &rowstr, QString columnstr) const
{
    return data(rowstr, columnstr).isValid();
}

bool lds_model_sqltablemodel_d::isFill(const QString &rowstr, QString columnstr) const
{
    return model.find(modelKey(rowstr, columnstr)) != model.end();
}

QVariant lds_model_sqltablemodel_d::data(const QString &rowstr, QString columnstr) const
{
    return model.value(modelKey(rowstr, columnstr));
}

QVariant lds_model_sqltablemodel_d::data(const QString &rowstr) const
{
    return data(rowstr, map_columnstr.key(0));
}

QVariant lds_model_sqltablemodel_d::data(int row, int column) const
{
    return model.value(modelKey(row, column));
}

void lds_model_sqltablemodel_d::setData(const QString &rowstr, const QString &columnstr, const QVariant &value) const
{
    if(!isExistedRow(rowstr)) appendRow(rowstr);
    if(!isExistedColumn(columnstr)) appendColumn(columnstr);

    ((QMap<QString, QVariant> *)&model)->insert(modelKey(rowstr, columnstr), value);
}

void lds_model_sqltablemodel_d::appendRow(const QString &rowstr) const
{
    ((QMap<QString, int> *)&map_rowstr)->insert(rowstr, rowCount());
}

void lds_model_sqltablemodel_d::appendColumn(const QString &columnstr) const
{
    ((QMap<QString, int> *)&map_columnstr)->insert(columnstr, columnCount());
}

void lds_model_sqltablemodel_d::clear() const
{
    clearRow();
}

void lds_model_sqltablemodel_d::clearRow() const
{
    ((QMap<QString, QVariant> *)&model)->clear();
    ((QMap<QString, int> *)&map_rowstr)->clear();
}

int lds_model_sqltablemodel_d::rowCount() const
{
    return map_rowstr.count();
}

int lds_model_sqltablemodel_d::columnCount() const
{
    return map_columnstr.count();
}

const QMap<QString, int> &lds_model_sqltablemodel_d::getMapRowstr() const
{
    return map_rowstr;
}

const QMap<QString, int> &lds_model_sqltablemodel_d::getMapColumnstr() const
{
    return map_columnstr;
}

void lds_model_sqltablemodel_d::print() const
{
    QString line;
    qDebug() << "--------------begin--------------";
    line += "Key\t";
    for(int column = 0; column < columnCount(); column++) {
        line += map_columnstr.key(column);
        line += "\t";
    }
    qDebug() << line;
    for(int row = 0; row < rowCount(); row++) {
        line = "";
        line += map_rowstr.key(row)+":\t";
        for(int column = 0; column < columnCount(); column++) {
            if(data(row, column).isValid()) {
                line += data(row, column).toString();
            } else {
                line += "null";
            }
            if(map_columnstr.key(column).length() > 15) {
                line += "\t";
            }
            line += "\t";
        }
        qDebug() << line;
    }

    qDebug() << "model:" << model;
    qDebug() << "map_rowstr:" << map_rowstr;
    qDebug() << "map_columnstr:" << map_columnstr;
    qDebug() << "--------------end--------------";
}

QString lds_model_sqltablemodel_d::modelKey(int row, int column) const
{
    return QString("(%1,%2)").arg(row).arg(column);
}

QString lds_model_sqltablemodel_d::modelKey(const QString &rowstr, const QString &columnstr) const
{
    return modelKey(map_rowstr.value(rowstr, -1), map_columnstr.value(columnstr, -1));
}
