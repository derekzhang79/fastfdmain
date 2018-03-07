#include "ftableview_standmodel_sql_none.h"
#include <QItemSelectionModel>
#include <QtDebug>

ftableview_standmodel_sql_none::ftableview_standmodel_sql_none(QObject *parent) :
    ftableview_standmodel_virtual(parent, TYPE_list)
{
}

void ftableview_standmodel_sql_none::add_list_data(const QList<ftableview_standmodel_sql_none::noneData> &list)
{
    _data_list = list;
    //有软件提供主键
    for(int k = 0; k < _data_list.count(); k++) {
        if(_data_list[k].userdata_map.value(Qt::UserRole + 1).isNull())
            _data_list[k].userdata_map.insert(Qt::UserRole + 1, k);
    }
    page_list_select.clear();//做法会保留第二页的部分导致,点菜时总是有默认做法
}

void ftableview_standmodel_sql_none::toinit()
{
    indexBegin();
    togenerateNext();
    if(selectionmodel) {
        selectionmodel->clear();
    }
    emit signalFinished();
}

bool ftableview_standmodel_sql_none::atEnd()
{
    return curindex >= _data_list.count();
}

QString ftableview_standmodel_sql_none::get_value_select()
{
    ///记录的是preindex的map
    refresh_cur_select();
    //
    QString value;
    QVariantList datalist = get_datalist_by_allpage_deep(2);
    foreach(const QVariant & k, datalist) {
        value += k.toString();
    }
    return value;
}

QVariantList ftableview_standmodel_sql_none::get_value_select_list(int deep)
{
    refresh_cur_select();
    return get_datalist_by_allpage_deep(deep);
}

QString ftableview_standmodel_sql_none::get_value_unselect()
{
    ///记录的是preindex的map
    refresh_cur_select();
    //
    QSet<int> select_vect;
    QVariantList datalist = get_datalist_by_allpage_deep_1();
    foreach(const QVariant & k, datalist) {
        select_vect.insert(k.toInt());
    }

    //
    QString value;
    for(int k = 0; k < _data_list.count(); k++) {
        if(select_vect.contains(k)) {

        } else {
            value += _data_list[k].userdata_2();
        }
    }
    return value;
}

bool ftableview_standmodel_sql_none::selectCurUData2(const QString &u2)
{
    int preindex_save = preindex;
    indexBegin();
    while(!atEnd()){
        togenerateNext();
        for(int row = 0; row < this->rowCount(); row ++) {
            for(int column = 0; column < this->columnCount(); column ++) {
                if(data(index(row, column), Qt::UserRole + 2).toString() == u2) {
                    selectionmodel->select(index(row, column), QItemSelectionModel::Select);
                    return true;
                }
            }
        }
    }

    curindex = preindex_save;
    togenerateNext();
    return false;
}

void ftableview_standmodel_sql_none::togenerate(int index, int updownum, bool hasnext)
{
    //点菜线程显示停止
    //    tostop();
    //=================================
    int vcount = this->rowCount();
    int hcount = this->columnCount();
    int total = vcount * hcount;

    preindex = curindex;
    ///清除选区
    clear_cur_select();

    //    _data_list.mid(curindex, total -  updownum);
    for(int k = 0, count = total - updownum; k < count; k++) {
        if((curindex) >= (_data_list.count())) {
            break;
        }
        updatedata(index/hcount, index%hcount, _data_list[curindex].userdata_map);
        index++;
        curindex ++;
    }

    if(hasnext && index < total){
        updatedata(index/hcount, index%hcount, ">>", -2, -2);
        index++;
    }
    for(;index < total; index++){
        updatedata(index/hcount, index%hcount, "", "", "");
    }
    ///还原选区
    restore_cur_select();
}

void ftableview_standmodel_sql_none::togenerateNext()
{
    ///记录的是preindex的map
    refresh_cur_select();

    int updownum = 0;
    int index = 0;
    int vcount = this->rowCount();
    int hcount = this->columnCount();
    int total;
    bool hasnext = false;
    if(curindex > 0){
        updatedata(0,0,"<<", -1,-1);
        updownum ++;
        index ++;
        total = vcount * hcount-1;
    } else {
        total = vcount * hcount;
    }

    //7
    if(_data_list.count() > (curindex + total)) {
        hasnext = true;
        updownum ++;
    }

    togenerate(index,updownum,hasnext);
    //2
}

void ftableview_standmodel_sql_none::togeneratePre()
{
    ///记录的是preindex的map
    refresh_cur_select();

    int updownum = 0;
    int index = 0;
    int vcount = this->rowCount();
    int hcount = this->columnCount();
    int total = vcount * hcount;
    bool hasnext = true;
    preindex = preindex - total + 2;
    updatedata(vcount-1, hcount-1, ">>", -2, -2);
    updownum++;
    if(preindex == 1){
        preindex = 0;
    }
    if(preindex > 0){
        updatedata(0, 0, "<<", -1, -1);
        updownum++;
        index++;
    }
    curindex  =preindex;

    togenerate(index,updownum,hasnext);
}
