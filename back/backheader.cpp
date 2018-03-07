#include "backheader.h"
#include <qsqlrecord.h>
#include <QApplication>
#include <QDesktopWidget>
#include "printer_0.h"
#include "lds_messagebox.h"
#include "lds.h"
#include "public_ins_maker.h"
#include "printer_normal.h"

public_mainprinter backheader::printer;
QStringList backheader::vch_operid_name;
QStringList backheader::vch_dish_typename;
QStringList backheader::vch_unitname;
QString backheader::SELECT_OPERID_NAME_FLAG =
        "Select '0000' as vch_operID,'system' as vch_operator union Select vch_operID, ifnull(vch_operator, '') as vch_operator from cey_sys_operator where ch_flag = 'Y'  ";
QString backheader::SELECT_OPERID_NAME =
        "Select '0000' as vch_operID,'system' as vch_operator union Select vch_operID, ifnull(vch_operator, '') as vch_operator from cey_sys_operator ";

QMap<QString, QVariant> backheader::operid_name_map;
backheader::backheader(QObject *parent) : QObject(parent)
{
}

QRect backheader::static_calculat_center_rect(QWidget *dialog, const QSize &size, int flag)
{
#if (defined QT_DEBUG) || (defined Q_OS_WIN)
    QPoint full_left_top(100,100);
    QPoint window_left_top(100,100);
#else
    QPoint full_left_top(0,0);
    QPoint window_left_top(0,30);
#endif
    QRect actual_rect;
    if(size.isValid()){
        actual_rect = QStyle::alignedRect(lds::SYS_LAYOUT_DIRECTION, Qt::AlignmentFlag(flag), size,
                                          QRect(window_left_top, lds::MAIN_WINDOW_SIZE)
                                          );
    } else {
        actual_rect = QStyle::alignedRect(lds::SYS_LAYOUT_DIRECTION, Qt::AlignmentFlag(flag), dialog->size(),
                                          QRect(window_left_top, lds::MAIN_WINDOW_SIZE)
                                          );
    }
    return actual_rect;
}

void backheader::static_dialogshow(QDialog &dialog, const QString &title, const QSize &size, int flag)
{
    dialog.setWindowTitle(title);
    QRect rect = static_calculat_center_rect(&dialog, size,flag);
    dialog.move(rect.topLeft());
    dialog.resize(rect.size());
    dialog.show();
}

void backheader::static_dialogshow(QWidget &dialog, const QString &title, const QSize &size, int flag)
{
    dialog.setWindowTitle(title);
    QRect rect = static_calculat_center_rect(&dialog, size,flag);
    dialog.move(rect.topLeft());
    dialog.resize(rect.size());
    dialog.show();
}

bool backheader::operid_right_code(QWidget *parent, const QString &purview_no)
{
    if(public_sql::gs_operid=="0000") {
        return true;
    }

    lds_query query;
    query.execSelect(QString("select vch_purviewname FROM cey_sys_purview_list where ch_purviewno='%1';").arg(purview_no));
    query.next();
    QString vch_purviewname=query.recordValue(0).toString();
    query.execSelect(QString("select ch_purviewno from cey_sys_oper_purview where ch_purviewno = '%1' and vch_operID='%2'")
               .arg(purview_no)
               .arg(public_sql::gs_operid)
               );
    if(query.next()){
        return true;
    }
    lds_messagebox::warning(parent, MESSAGE_TITLE_VOID,  tr("操作员") +":"+ public_sql::gs_operid + "\n"+tr("没有[%1]权限").arg(vch_purviewname.trimmed()));
    return false;
}

void backheader::standmodelfilldata(QStandardItemModel *model, const QString &sql)
{
    model->removeRows(0,  model->rowCount());

    lds_query query;
    query.execSelect(sql);
    while(query.next()) {
        QList<QStandardItem *>  items;
        int count = query.recordCount();
        for(int c = 0; c < count; c++) {
            items << new QStandardItem(query.recordValue(c).toString());
        }
        model->appendRow(items);
    }
}

QString backheader::getinterdata(const QString &_data_)
{
    QString ret;
    if(_data_.contains("[") && _data_.contains("]")){
        ret=_data_;
        ret=ret.split("[")[1];
        ret.chop(1);
    } else if(_data_.contains("(") && _data_.contains(")")){
        ret=_data_;
        ret=ret.split("(")[1];
        ret.chop(1);
    }

    return ret;
}

QString backheader::getouterdata(const QString &_data_)
{
    QString ret;
    if(_data_.contains("[") && _data_.contains("]")){
        ret=_data_.split("[")[0];
    } else if(_data_.contains("(") && _data_.contains(")")){
        ret=_data_.split("(")[0];
    }

    return ret;
}

QString backheader::getouterdata(const QStringList &_data_s, const QString &cmpvalue_1)
{
    foreach(QString _data_, _data_s){
        if(getinterdata(_data_)==cmpvalue_1){
            return getouterdata(_data_);
        }
    }
    return "";
}

int backheader::set_index_of_interdata(const QString &inter, QComboBox *c)
{
    int ret = -1;
    if(c){
        for(int row = 0, rowcount = c->count(); row < rowcount; row++){
            if(inter == getinterdata(c->itemText(row))){
                ret = row;
                return ret;
            }
        }
    }
    return ret;
}

void backheader::asyn_dataprint(const blockIns &printdata)
{
    asyn_dataprint(printdata, lds::sysconf->value("system_setting/printerpath").toString());
}

void backheader::asyn_dataprint(const blockIns &printdata, const QString &path)
{
    if(lds::sysconf->value("system_setting/printonthread", true).toBool()){
        printer.asynchPrint(printdata, path);
    } else {
        printer.synchPrint(printdata, path);
    }
}

void backheader::asyn_dataprint(const QString &printdata, const QString &path)
{
    blockIns bins;
    bins += printer.drawText("", printdata);
    asyn_dataprint(bins,path);
}

void backheader::  asyn_dataprin_test(const QString &path)
{
    //拼接指令
    blockIns printdata;
    printdata += backheader::printer.drawText("i,a00,s11", tr("打印测试成功")+"\n");//清除缓存 居中 2x2x
    printdata += backheader::printer.drawText("i,a00,s00", tr("打印测试成功")+"\n");//清除缓存 居中 2x2x
    printdata += backheader::printer.drawText("i", "");//清除缓存
    if(path.isEmpty()) {
        asyn_dataprint(printdata);
    } else {
        asyn_dataprint(printdata,path);
    }
}

QString backheader::print_def_path()
{
    return lds::sysconf->value("system_setting/printerpath").toString();
}

bool backheader::syn_dataprint(const blockIns &printdata)
{
    return syn_dataprint(printdata,lds::sysconf->value("system_setting/printerpath").toString());
}

bool backheader::syn_dataprint(const blockIns &printdata, const QString &path)
{
    return printer.synchPrint(printdata, path);
}

void backheader::syn_dataprin_test(const QString &path)
{
    blockIns printdata;
    printdata += backheader::printer.drawText("i,a00,s11", tr("打印测试成功")+"\n");//清除缓存 居中 2x2x
    printdata += backheader::printer.drawText("i,a00,s00", tr("打印测试成功")+"\n");//清除缓存 居中 2x2x
    printdata += backheader::printer.drawText("-", "");//清除缓存
    printdata += backheader::printer.drawText("i", "");//清除缓存
    if(path.isEmpty()) {
        syn_dataprint(printdata);
    } else {
        syn_dataprint(printdata,path);
    }
}

void backheader::syn_dataprin_test_byresolveself(const QString &path, int instype, const QString &printtype, int paperwidth, int xinye_beep_count)
{
    blockIns printdata;
    printdata += backheader::printer.drawText("i,a00,s11", tr("打印测试成功")+"\n");//清除缓存 居中 2x2x
    printdata += backheader::printer.drawText("i,a00,s00", tr("打印测试成功")+"\n");//清除缓存 居中 2x2x
    printdata += backheader::printer.drawText("-", "");//清除缓存
    printdata += backheader::printer.drawText("i", "");//清除缓存
    Printer_Normal::resolve(instype, printtype, paperwidth, xinye_beep_count, printdata);

    printer.synchPrintWithoutresolve(printdata, path);
}

/**
避免多线程造成的数据异常
*/
void backheader::resolve(const QString &path, lds_query &query, blockIns &ins)
{
    public_ins_maker::resolve(path, query, ins);

}

void backheader::resolve(int instype, const QString &printtype, int paperwidth,  int xinye_beep_count, blockIns &ins)
{
    Printer_Normal::resolve(instype, printtype, paperwidth, xinye_beep_count,  ins);
}

int backheader::VIPFINDVALUE(const QStandardItemModel &model, const QString &cmpvalue, int cmpindex)
{
    int ret=-1;
    for(int r=0,r_count=model.rowCount(),c_count=model.columnCount();r<r_count;r++){
        if(cmpindex >=c_count) {
            break;
        }
        if(model.item(r, cmpindex)->text()==cmpvalue){
            ret=r;
            break;
        }
    }
    return ret;
}

QString backheader::VIPFINDVALUE(const QStandardItemModel &model, const QString &cmpvalue_0)
{
    QString ret;
    for(int r=0,r_count=model.rowCount();r<r_count;r++){
        if(model.item(r, 0)->text()==cmpvalue_0){
            ret=model.item(r, 1)->text();
            break;
        }
    }
    return ret;
}

void backheader::modelSetHeader(QAbstractItemModel *model, const QStringList &headers)
{
    for(int r=0,r_count=headers.count(); r < r_count; r++)
        model->setHeaderData(r, Qt::Horizontal, headers[r]);
}

void backheader::regetvch_operid_name()
{
    vch_operid_name.clear();
    lds_query query;
    query.execSelect("select concat(ifnull(vch_operator,''), '(', vch_operID, ')')as typename , vch_operID from ("+SELECT_OPERID_NAME_FLAG+")t order by vch_operID asc");
    while(query.next()){
        vch_operid_name.append(query.recordValue(0).toString());
    }
}

const QMap<QString, QVariant> *backheader::get_operid_name_map()
{
    operid_name_map.clear();
    lds_query query;
    query.execSelect(SELECT_OPERID_NAME_FLAG);
    while(query.next()) {
        operid_name_map.insert(query.recordValue("vch_operID").toString(), query.recordValue("vch_operator"));
    }
    return &operid_name_map;
}

void backheader::regetvch_dish_typename()
{
    vch_dish_typename.clear();
    lds_query query;
    //增加判断逻辑删除；
    query.execSelect("select concat(vch_dish_typename, '(', ch_dish_typeno, ')')as typename from cey_bt_dish_type ");
    while(query.next()){
        vch_dish_typename.append(query.recordValue(0).toString());
    }
}


void backheader::regetvch_unitname()
{
    vch_unitname.clear();
    lds_query query;
    query.execSelect("select concat(vch_unitname, '(', ch_unitno, ')')as typename from cey_bt_unit ");
    while(query.next()){
        vch_unitname.append(query.recordValue(0).toString());
    }
}

void backheader::ldsComSetdataByKVM(const QAbstractItemModel *model, lds_ComboBox *com)
{
    com->clear();
    QList<lds_ComboBox::PAIRLIST> citems;
    for(int row=0,rowcount=model->rowCount();row<rowcount;row++){
        citems.append(lds_ComboBox::PAIRLIST(model->index(row, 0).data().toString()
                                             ,model->index(row, 1).data().toString()));
    }
    com->addItems1(citems);
}

void backheader::fillData(QComboBox *com, const QMap<QString, QVariant> *map, bool first_is_empty, const QString &emptyTile)
{
    com->clear();
    if(first_is_empty) {
        com->addItem(emptyTile, "");
    }
    for(QMap<QString, QVariant>::const_iterator itr = map->begin(); itr != map->end(); itr ++ ){
        com->addItem(itr.value().toString(), itr.key());
    }
}

void backheader::fillData(QComboBox *com, const QMap<int, QString> *map, bool first_is_empty, const QString &emptyTile)
{
    com->clear();
    if(first_is_empty)   com->addItem(emptyTile, "");
    for(QMap<int, QString>::const_iterator itr = map->begin(); itr != map->end(); itr ++ ){
        com->addItem(itr.value(), itr.key());
    }
}

void backheader::fillData(QComboBox *com, const QMap<QString, QString> *map, bool first_is_empty, const QString &emptyTile)
{
    com->clear();
    if(first_is_empty)   com->addItem(emptyTile, "");
    for(QMap<QString, QString>::const_iterator itr = map->begin(); itr != map->end(); itr ++ ){
        com->addItem(itr.value(), itr.key());
    }
}

void backheader::fillData(QComboBox *com, const QAbstractItemModel *m, bool first_is_empty, const QString &emptyTile)
{
    com->clear();
    if(first_is_empty)    com->addItem(emptyTile, "");
    for(int row=0,rowcount=m->rowCount();row<rowcount;row++){
        com->addItem(m->data(m->index(row, 1)).toString(), m->data(m->index(row, 0)));
    }
}
