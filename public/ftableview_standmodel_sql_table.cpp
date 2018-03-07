#include "ftableview_standmodel_sql_table.h"
#include "lds_query.h"
#include <QSqlRecord>
#include <QtDebug>
#include <QFile>
#include<QTableView>
#include<QApplication>

ftableview_standmodel_sql_table::ftableview_standmodel_sql_table(QObject *parent) :
    ftableview_standmodel_virtual(parent, TYPE_list)
{
    sqlargarg = " limit %1,%2";
}

QList<ftableview_standmodel_sql_table::checkData> ftableview_standmodel_sql_table::get_value_select()
{
    QList<checkData> checkdata;
    ///记录的是preindex的map
    refresh_cur_select();
    //
    QVariantList datalist = get_datalist_by_allpage_deep_1();
    QVariantList datalist_7 = get_datalist_by_allpage_deep(7);
    for(int k = 0,count = datalist.count(); k < count; k++) {
        checkdata += checkData(datalist[k].toString(), datalist_7[k].toString());
    }
    return checkdata;
}

QList<QVariantList> ftableview_standmodel_sql_table::get_value_select_deep(QList<int> deeps)
{
    ///记录的是preindex的map
    refresh_cur_select();
    QList<QVariantList> data_rect;
    for(int k = 0; k < deeps.count(); k++) {
        data_rect += get_datalist_by_allpage_deep(deeps[k]);
    }
    return data_rect;
}

void ftableview_standmodel_sql_table::enableEndAppendNew(bool enabled)
{
    b_endAppendNew = enabled;
}


void ftableview_standmodel_sql_table::togenerate(int index, int updownum, bool hasnext)
{
    //点菜线程显示停止
    //    tostop();
    //=================================
    int vcount = this->rowCount();
    int hcount = this->columnCount();
    int total = vcount * hcount;
    if(total == 0) return;

    lds_query query;
    ///清除选区
    clear_cur_select();

    preindex = curindex;
    query.execSelect(sql+ sqlargarg.arg(curindex).arg(total - updownum));
    while(query.next()){
        updatedata(index/hcount, index%hcount,query.record());
        toclearcheck(index/hcount, index%hcount);
        index++;
        curindex ++;
    }
    //1填充满了，而且有新增，则最后一个改为》》
    //2填充没有慢，而且有新增，则最后一个为New
    if(b_endAppendNew) {
        if(false == hasnext && index == total) {//1
            hasnext = true;
            index  --;
            curindex  --;
        } else if(false == hasnext &&index < total) {//2
            updatedata(index/hcount, index%hcount, "", "-3", "");
            index++;
            curindex ++;
        }
    }
    ///~
    if(hasnext && index < total){
        updatedata(index/hcount, index%hcount, ">>", "-2", "");
        toclearcheck(index/hcount, index%hcount);
        index++;
    }
    for(;index < total; index++){
        updatedata(index/hcount, index%hcount, "", "", "");
        toclearcheck(index/hcount, index%hcount);
    }

    emit signalFinished();

    ///还原选区
    if(is_save_select_info) {
        restore_cur_select(7/*check*/);
    }
}

void ftableview_standmodel_sql_table::togenerateNext()
{
    ///记录的是preindex的map
    refresh_cur_select();

    lds_query query;
    int updownum = 0;
    int index = 0;
    int vcount = this->rowCount();
    int hcount = this->columnCount();
    int total;
    bool hasnext = false;
    if(curindex > 0){
        updatedata(0,0,"<<","-1","");
        toclearcheck(0, 0);
        updownum ++;
        index ++;
        total = vcount * hcount-1;
    } else {
        total = vcount * hcount;
    }
    query.execSelect(sql + sqlargarg.arg(curindex+total).arg(1));
    if(query.next()){
        hasnext = true;
        updownum ++;
    }

    togenerate(index,updownum,hasnext);
}

void ftableview_standmodel_sql_table::togeneratePre()
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
    updatedata(vcount-1, hcount-1, ">>", "-2", "");
    toclearcheck(vcount-1, hcount-1);
    updownum++;
    if(preindex == 1){
        preindex = 0;
    }
    if(preindex > 0){
        updatedata(0,0, "<<", "-1", "");
        toclearcheck(0, 0);
        updownum++;
        index++;
    }
    curindex  =preindex;

    togenerate(index,updownum,hasnext);
}

void ftableview_standmodel_sql_table::enable_save_select_info(bool f)
{
    is_save_select_info = f;
}

void ftableview_standmodel_sql_table::toinit()
{
    toinit0();
}

void ftableview_standmodel_sql_table::toinitBySqlother(const QString &no)
{
    //1
    key_sqlother = no;
    //2
    sql= sqlother.arg(no);
    toinit0();
}

void ftableview_standmodel_sql_table::toinit0()
{
    if(sql.endsWith(";")){
        qDebug() << "_________" << "sql endswith ';' , please manual check";
    }
    curindex = 0;
    preindex = 0;
    togenerateNext();
    if(selectionmodel) {
        selectionmodel->clear();
    }
    emit signalFinished();
}

void ftableview_standmodel_sql_table::toclearcheck(int r, int c)
{
    this->setData(this->index(r,c), QVariant(), Qt::UserRole+7/*check*/);
}
