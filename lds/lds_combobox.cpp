#include "lds_combobox.h"
#include <QAbstractItemView>
#include <QTableView>
#include <QStandardItemModel>
#include <qdebug.h>
#include "lds_query.h"

lds_ComboBox::lds_ComboBox(QWidget *parent) :
    QComboBox(parent)
{
}

void lds_ComboBox::addItems(const QStringList &texts)
{
    QStandardItemModel *commodel=new QStandardItemModel(this);
    foreach(QString text,texts){
        commodel->appendRow(new QStandardItem(text));
        commodel->setData(commodel->index(commodel->rowCount()-1, 0), QSize(100,35), Qt::SizeHintRole);
    }
    this->setModel(commodel);
}

void lds_ComboBox::addItems1(const QList<lds_ComboBox::PAIRLIST> &texts)
{
    QStandardItemModel *commodel=new QStandardItemModel(this);
    foreach(PAIRLIST text,texts){
        commodel->appendRow(new QStandardItem);
        commodel->setData(commodel->index(commodel->rowCount()-1, 0), QSize(100,35), Qt::SizeHintRole);
        commodel->setData(commodel->index(commodel->rowCount()-1, 0), text.key, Qt::UserRole);
        commodel->setData(commodel->index(commodel->rowCount()-1, 0), text.usrvalue, Qt::EditRole);
    }
    this->setModel(commodel);
}

void lds_ComboBox::addItems1(const QMap<QString, QVariant> &texts)
{
    QStandardItemModel *commodel=new QStandardItemModel(this);
    QMapIterator<QString, QVariant> i(texts);
    while (i.hasNext()) {
        i.next();
        commodel->appendRow(new QStandardItem);
        commodel->setData(commodel->index(commodel->rowCount()-1, 0), QSize(100,35), Qt::SizeHintRole);
        commodel->setData(commodel->index(commodel->rowCount()-1, 0), i.key(), Qt::UserRole);
        commodel->setData(commodel->index(commodel->rowCount()-1, 0), i.value(), Qt::EditRole);
    }
    this->setModel(commodel);
}

void lds_ComboBox::addItems(const QMap<QString, QVariant> *map)
{
    for(QMap<QString, QVariant>::const_iterator i = map->begin(); i != map->end(); i++) {
        this->addItem(i.value().toString(), i.key());
    }
}

void lds_ComboBox::addItemsBySql(const QString &key_value_sql)
{
    lds_query query;
    query.execSelect(key_value_sql);
    while(query.next()) {
        this->addItem(query.recordValue(1).toString(), query.recordValue(0));
    }
}

void lds_ComboBox::addItemsByModel(QAbstractItemModel *key_value_model)
{
    for(int row = 0; row < key_value_model->rowCount(); row ++) {
        this->addItem(key_value_model->data(key_value_model->index(row, 1)).toString(),
                      key_value_model->data(key_value_model->index(row, 0)));
    }
}

void lds_ComboBox::setindex_c1(const QString c1_text)
{
    for(int r=0,r_count=this->count(); r<r_count;r++){
        if(this->itemText(r).endsWith(QString("(%1)").arg(c1_text))){
            setCurrentIndex(r);
            return;
        }
    }
    setCurrentIndex(-1);
}
/**
 * @brief lds_ComboBox::setcomusrdata
 * @param usrtext
 * desc 针对model和mapper的格式制定的函数，当model并没有relation时，可以对齐进行直接的修改
 *model-》com key；com-》model key;显示的value
 */

void lds_ComboBox::setcomusrdata(const QVariant &usrtext)
{
    QAbstractItemModel *model=this->model();
    if(model){
        for(int r=0,r_count=model->rowCount();r<r_count;r++){
            if(model->data(model->index(r, 0), Qt::UserRole)==usrtext){
                setCurrentIndex(r);
                return;
            }
        }
    }
    setCurrentIndex(-1);
}

QVariant lds_ComboBox::comusrdata(int comindex)
{
    if(comindex == -1){
        comindex = this->currentIndex();
    }
    QAbstractItemModel *model=this->model();
    if(model
            &&model->rowCount()>0
            &&comindex >=0
            &&comindex < model->rowCount()){
        return model->data(model->index(comindex, 0), Qt::UserRole);
    }
    return QVariant();
}

QVariant lds_ComboBox::curusrdata()
{
    return comusrdata(this->currentIndex());
}

QString lds_ComboBox::comText(const QVariant &d)
{
    for(int k = 0, count = this->count(); k < count; k++) {
        if(itemData(k) == d) {
            return itemText(k);
        }
    }
    return "";
}


void lds_ComboBox::enableEmitColumn(int emitColumn)
{
    this->emitColumn = emitColumn;
    connect(this,SIGNAL(currentIndexChanged(int)),this,SLOT(toEmitColumn(int)));
}

QVariant lds_ComboBox::model_data(int row, int column, int role)
{
    QAbstractItemModel *model=this->model();
    if(model == 0
            || row >= model->rowCount()
            || column >= model->columnCount()) return QVariant();

    return model->data(model->index(row, column), role);
}

void lds_ComboBox::toEmitColumn(int index)
{
    QAbstractItemModel *model=this->model();
    if(!model) return;
    emit signalEmitColumn(model->index(index, emitColumn).data().toString());
}
