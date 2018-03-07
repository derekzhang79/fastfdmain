#include "ftableview_standmodel_virtual.h"
#include <QtDebug>
#include <QTableView>
#include <QTimer>
#include <QEventLoop>
#include <QApplication>
#include <QStateMachine>
#include <QDir>

ftableview_standmodel_virtual::ftableview_standmodel_virtual(QObject *parent, ftableview_standmodel_virtual::TYPE t)
    :QStandardItemModel(parent)
{
    _type = t;
    indexBegin();
    selectionmodel = 0;
    valuestep_pre = valuestep = 0;
    user_role_equal_max = Qt::UserRole;
}

bool ftableview_standmodel_virtual::model_data_set(int row, int column, const QVariant &value, int role)
{
    return this->setData(this->index(row, column), value, role);
}

void ftableview_standmodel_virtual::set_row_column(int row_count, int column_count)
{
    this->removeRows(0, this->rowCount());
    this->removeColumns(0, this->columnCount());//中餐下的地图模式，若没有清除所有列的话，则是不会tableview 是会有滚动条的
    for(int row = 0; row < row_count; row++) {
        QList<QStandardItem*> items;
        for(int column = 0; column < column_count; column ++) {
            items << new QStandardItem;
        }
        this->appendRow(items);
    }
}

Qt::ItemFlags ftableview_standmodel_virtual::flags(const QModelIndex &index) const
{
    Qt::ItemFlags f = QStandardItemModel::flags(index);

    if(index.data(Qt::UserRole+1).toString().isEmpty()) {//key
        return Qt::ItemIsEnabled;
    }
    return f;
}

void ftableview_standmodel_virtual::updatedata(int r, int c, const QMap<int, QVariant> &map)
{
    clearData(r, c);

    for(QMap<int/*role*/, QVariant>::const_iterator d = map.constBegin();
        d != map.constEnd();
        d++) {
        this->setData(this->index(r, c), d.value(), d.key());
    }
}

/*一般情况下：
edta是显示的数据
udata的索引值
ddata 表示的是图片*/


void ftableview_standmodel_virtual::updatedata(int r, int c, const QVariant &edata, const QVariant &udata, const QVariant &ddata)
{
    clearData(r, c);

    this->setData(this->index(r,c), edata, Qt::UserRole);//显示的文字
    this->setData(this->index(r,c), udata, Qt::UserRole+1);//索引值
    this->setData(this->index(r,c), ddata, Qt::UserRole+2);//图片路径
}

void ftableview_standmodel_virtual::updatedata(int r, int c, const QSqlRecord &record)
{
    clearData(r, c);

    int  count = record.count();
    for(int d = 0; d < count; d++){
        //Qt::UserRole+1);//索引值
        this->setData(this->index(r,c), record.value(d),  Qt::UserRole + d);//显示的文字
    }
}

void ftableview_standmodel_virtual::updatep2pdelay()
{
    emit signalstepstop();
    emit signalstepstart();
}

void ftableview_standmodel_virtual::indexBegin()
{
    curindex = 0;
    preindex = 0;
}

void ftableview_standmodel_virtual::clearData(int r, int c)
{
    for(int k = Qt::UserRole; k <= user_role_equal_max; k++) {
        setData(index(r, c), QVariant(), k);
    }
}

bool ftableview_standmodel_virtual::setData(const QModelIndex &index, const QVariant &value, int role)
{
    user_role_equal_max = qMax(user_role_equal_max, role);
    return QStandardItemModel::setData(index, value ,role);
}

void ftableview_standmodel_virtual::refresh_cur_select()
{
    if(!selectionmodel) {
        qDebug() << "selectionmodel is empty";
        return;
    }

    ///记录的是preindex的map
    QModelIndexList indexlist = selectionmodel->selectedIndexes();
    QList< QVariantList  /*data:usr+*/ > datarect;
    for(int end = indexlist.count() - 1; end >=0; end--) {
        if(  indexlist[end].data(Qt::UserRole + 1).toString() == "-1"
             || indexlist[end].data(Qt::UserRole + 1).toString() == "-2"
             || indexlist[end].data(Qt::UserRole + 1).toString().isEmpty()
             ) {
            continue;
        }
        QVariantList datalist;
        for(int d = Qt::UserRole; d <= user_role_equal_max; d++) {
            datalist += indexlist[end].data(d);
        }
        datarect += datalist;
    }
    page_list_select.insert(preindex, datarect);
}

void ftableview_standmodel_virtual::restore_cur_select(int uesrcheck)
{
    if(!selectionmodel) return;

    QVariantList datalist = get_datalist_by_page_deep_1(preindex);//唯一的key
    int k;
    for(int row = 0; row < this->rowCount(); row++) {
        for(int c = 0; c < this->columnCount(); c++) {
            if((k = datalist.indexOf(this->data(this->index(row, c), Qt::UserRole + 1))) >= 0) {
                selectionmodel->setCurrentIndex(this->index(row, c), QItemSelectionModel::Select);
                for(int usr = Qt::UserRole; usr <= user_role_equal_max; usr++) {
                    setData(index(row, c), page_list_select.value(preindex)/*page*/.value(k)/*rowXcolumn*/.value(usr-Qt::UserRole)/*userdeep*/, usr);//更新check by uesrcheck
                }
                //                if(uesrcheck >= 0){
                //                    setData(index(row, c), page_list_select.value(preindex)/*page*/.value(k)/*rowXcolumn*/.value(uesrcheck)/*userdeep*/, Qt::UserRole + uesrcheck);//更新check by uesrcheck
                //                }
            }
        }
    }
}

void ftableview_standmodel_virtual::clear_cur_select()
{
    if(!selectionmodel) return;
    selectionmodel->clear();
}

QVariantList ftableview_standmodel_virtual::get_datalist_by_allpage_deep_1()
{
    return get_datalist_by_allpage_deep(1);
}

QVariantList ftableview_standmodel_virtual::get_datalist_by_allpage_deep(int deep)
{
    QVariantList retdata;
    foreach(const int &k, page_list_select.keys())
        retdata += get_datalist_by_page_deep(k, deep);
    return retdata;
}

QVariantList ftableview_standmodel_virtual::get_datalist_by_page_deep_1(int page)
{
    return get_datalist_by_page_deep(page, 1);
}

QVariantList ftableview_standmodel_virtual::get_datalist_by_page_deep(int page, int deep)
{
    QList< QVariantList  /*data:usr+*/ > datarect = page_list_select.value(page);
    return get_datalist_by_rect_deep(datarect, deep);
}

QVariantList ftableview_standmodel_virtual::get_datalist_by_rect_deep(const QList<QVariantList> &datarect, int deep)
{
    QVariantList retdata;
    for(int k = 0; k< datarect.count(); k++) {
        retdata += datarect[k].value(deep);
    }
    return retdata;
}

void ftableview_standmodel_virtual::refreshcur()
{
    curindex = preindex;
    togenerateNext();
}

int ftableview_standmodel_virtual::step() const
{
    return valuestep;
}

void ftableview_standmodel_virtual::setstep(int value)
{
    valuestep = value;
    int columncount = this->columnCount();
    int addvalue = (valuestep_pre <= value? 1:-1);
    for(; valuestep_pre != value; valuestep_pre += addvalue){
        if(addvalue == 1){
            int r = valuestep_pre/columncount;
            int c = valuestep_pre%columncount;
            QModelIndex index = this->index(r, c);
            QVariant d = this->data(index, Qt::UserRole+2);
            if(d.canConvert<QString>()){
                QPixmap p(imagedishdir+QDir::separator()+ d.toString());
                if(!p.isNull()) {
                    this->setData(index, p, Qt::UserRole+2);
                }
            }
        }
    }
}
