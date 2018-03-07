#include "w_bt_dish_cookset.h"
#include "ui_w_bt_dish_cookset_dialog.h"
//菜品厨打设置
#include <QSqlRecord>
#include "lds_messagebox.h"
#include <QSqlError>
#include "lds_tableview_repecify.h"
#include "backheader.h"
#include "lds_tableview_delegate_check.h"
#include "w_sys_manage_basicdataexport.h"
#include "w_sys_manage_cloudsync_with_time.h"
#include "lds_model_sqltablemodel_delegate_com.h"
#include "lds_dialog_input.h"
#include "lds_model_sqltablemodel.h"
#include "ui_w_bt_dish_cookset_typeset_dialog.h"

w_bt_dish_cookset::w_bt_dish_cookset(QWidget *parent) :
    QDialog(parent),ui(new Ui_w_bt_dish_cookset_Dialog)
{
    ui->setupUi(this);
    //! treewidget
    ui->treeWidget->header()->hide();
    ui->treeWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->treeWidget->setColumnCount(1);
    //! tableview

    tablemodel=new lds_model_sqltablemodel(this);
    tablemodel->setTable("cey_bt_dish");
    tablemodel->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    ui->tableView->setEditTriggers(QTableView::AllEditTriggers);
    ui->tableView->setModel(tablemodel);

    tablemodel->save_set_header("ch_dishno",tr("品码"));
    tablemodel->save_set_header("vch_dishname",tr("品名"));
    tablemodel->save_set_header("ch_unitno",tr("单位"));
    tablemodel->save_set_header("num_price",tr("价格"));
    tablemodel->save_set_header("ch_discount",tr("折扣"));

    tablemodel->save_set_header("ch_weight",tr("是否称重"));
    tablemodel->save_set_header("ch_suitflag",tr("套菜"));
    tablemodel->save_set_header("ch_off",tr("常用菜"));
    tablemodel->save_set_header("ch_tempflag",tr("临时菜"));
    tablemodel->save_set_header("ch_curprice",tr("时价"));
    tablemodel->save_set_header("ch_stopflag",tr("停用"));

    ui->tableView->rePecifyHeader(tablemodel->get_save_set_header_saver_column_list());

    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("ch_weight"), new lds_tableview_delegate_check(QVector<int>() << tablemodel->fieldIndex("ch_weight"), ui->tableView, true));
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("item_flag"), new lds_tableview_delegate_check(QVector<int>() << tablemodel->fieldIndex("item_flag"), ui->tableView, true));
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("ch_suitflag"), new lds_tableview_delegate_check(QVector<int>() << tablemodel->fieldIndex("ch_suitflag"), ui->tableView, true));
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("ch_off"), new lds_tableview_delegate_check(QVector<int>() << tablemodel->fieldIndex("ch_off"), ui->tableView, true));
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("ch_discount"), new lds_tableview_delegate_check(QVector<int>() << tablemodel->fieldIndex("ch_discount"), ui->tableView, true));

    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("ch_curprice"), new lds_tableview_delegate_check(QVector<int>() << tablemodel->fieldIndex("ch_curprice"), ui->tableView, true));
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("ch_tempflag"), new lds_tableview_delegate_check(QVector<int>() << tablemodel->fieldIndex("ch_tempflag"), ui->tableView, true));
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("ch_stopflag"), new lds_tableview_delegate_check(QVector<int>() << tablemodel->fieldIndex("ch_stopflag"), ui->tableView, true));
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("ch_unitno"), new lds_model_sqltablemodel_delegate_com(
                                                this, tablemodel, "cey_bt_unit","ch_unitno","vch_unitname"
                                                ));

    //2
    sql_select_3="SELECT  cey_bt_cp_dish.ch_dishno ,           cey_bt_cp_dish.ch_no ,           cey_bt_cp_memo.vch_cp_memo ,           cey_bt_cp_memo.num_add_price     "
                 "FROM cey_bt_cp_dish ,           cey_bt_cp_memo     WHERE(cey_bt_cp_dish.ch_no = cey_bt_cp_memo.ch_no)and         (( cey_bt_cp_dish.ch_dishno = '%1')) ";
    querymodel_2=new QSqlQueryModel;
    querymodel_2->setQuery(sql_select_3.arg("ch_dishno"));
    ui->tableView_2->setEditTriggers(QTableView::AllEditTriggers);
    ui->tableView_2->setModel(querymodel_2);
    ui->tableView_2->setTransferHheader();
    ui->tableView_2->rePecifyHeader(QStringList() << "ch_no" << "vch_cp_memo" << "num_add_price");
    //! toolbar
    ui->widget->setup(QStringList()
                      /*<< tr("做法类型")*/ << tr("特殊做法")
                      << tr("菜品做法设置") << tr("菜品做法删除")
                      << tr("刷新") << tr("导出")
                      << tr("退出")
                      );
    connect(ui->treeWidget,SIGNAL(pressed(QModelIndex)),this,SLOT(t1_t2_t3()));
    connect(ui->lineEdit,SIGNAL(textChanged(QString)),this,SLOT(t1_t2_t3()));
    connect(ui->checkBox,SIGNAL(toggled(bool)),this,SLOT(t1_t2_t3()));

//    connect(ui->widget->index_widget(tr("做法类型")), SIGNAL(clicked()),this,SLOT(tocooktype()));
    connect(ui->widget->index_widget(tr("特殊做法")), SIGNAL(clicked()),this,SLOT(tospecialcook()));
    connect(ui->widget->index_widget(tr("菜品做法删除")), SIGNAL(clicked()),this,SLOT(todel()));
    connect(ui->widget->index_widget(tr("菜品做法设置")), SIGNAL(clicked()),this,SLOT(todishset()));
    //    connect(ui->widget->index_widget(tr("批量设置")), SIGNAL(clicked()),this,SLOT(todishsets()));
    connect(ui->widget->index_widget(tr("刷新")), SIGNAL(clicked()),this,SLOT(torefresh()));
    connect(ui->widget->index_widget(tr("导出")), SIGNAL(clicked()),this,SLOT(toexport()));
    connect(ui->widget->index_widget(tr("退出")), SIGNAL(clicked()),this,SLOT(toexit()));
    connect(ui->tableView,SIGNAL(selectChanged(int)),this,SLOT(t2_t3()));
    ui->checkBox->setVisible(false);
    //! refresh
    QTimer::singleShot(0, this, SLOT(torefresh()));
}

void w_bt_dish_cookset::torefresh()
{
    //yun
    w_sys_manage_cloudsync_with_time_running::exec_no_up_then_down_byconf(this);
    ui->widget->index_widget(tr("退出"))->show_flag_top_right(w_sys_manage_cloudsync_with_time_running::q_bt_get_ready_total_byconf());
    //~yun

    //treewidget
    ui->treeWidget->deleteAllItem();
    ui->treeWidget->rootitem=new QTreeWidgetItem(QStringList()
                                                 <<tr("全部类型"));
    ui->treeWidget->addTopLevelItem(ui->treeWidget->rootitem);


    lds_query query;
    query.execSelect("select concat(vch_dish_typename,'[',ch_dish_typeno,']') as 'nametype', ch_dish_typeno  from cey_bt_dish_type"
                     );
    while(query.next()){
        QTreeWidgetItem *item=new QTreeWidgetItem(QStringList()
                                                  <<QString("%1")
                                                  .arg(query.recordValue("nametype").toString())
                                                  );
        ui->treeWidget->rootitem->addChild(item);
        //获取[]的内容
        lds_query query_1;
        QString no=query.recordValue("nametype").toString();
        no=no.split("[")[1];
        no.chop(1);
        query_1.execSelect(QString("select concat(vch_sub_typename,'[',ch_sub_typeno,']') as 'nametype' from cey_bt_dish_subtype where ch_dish_typeno = '%1' order by ch_sub_typeno asc;"
                                   )
                           .arg(no)
                           );
        while(query_1.next()){
            QTreeWidgetItem *item1=new QTreeWidgetItem(QStringList()
                                                       <<QString("%1")
                                                       .arg(query_1.recordValue("nametype").toString())
                                                       );
            item->addChild(item1);
        }
    }
    ui->treeWidget->setItemSizeHint();//更新所有的 item的sizehint
    ui->treeWidget->restore();
    //tableview
    t1_t2();
}

void w_bt_dish_cookset::t1_t2_t3()
{
    t1_t2();
    t2_t3();
}

void w_bt_dish_cookset::t1_t2()
{
    QString sqlselect;
    //tree检索
    if(ui->treeWidget->currentItem()){
        int index=ui->treeWidget->deepofitem(ui->treeWidget->currentItem());
        if(index>0){
            QString text=ui->treeWidget->currentItem()->text(0);
            sqlselect=text.split("[")[1];
            sqlselect.chop(1);
            if(index==1){
                sqlselect=QString("cey_bt_dish.ch_dish_typeno='%1'").arg(sqlselect);
            } else if(index==2){
                sqlselect=QString("cey_bt_dish.ch_sub_typeno='%1'").arg(sqlselect);
            }
        }
    }
    if(sqlselect.isEmpty()){

    } else {
        sqlselect +=" and ";
    }
    //是否停用
    sqlselect +=QString("cey_bt_dish.ch_stopflag like '%1' ").arg("N");
    //过滤
    sqlselect += QString(" and (cey_bt_dish.ch_dishno like '%1%' or cey_bt_dish.vch_dishname like '%1%' or cey_bt_dish.vch_spell like '%1%')")
            .arg(ui->lineEdit->text());
    tablemodel->setFilter(sqlselect);
    if(tablemodel->rowCount() > 0) {
        ui->tableView->restore(0);
        ui->tableView->resizeColumnsToContents();
    }

}

void w_bt_dish_cookset::t2_t3()
{
    QString t3_ch_dishno;
    int row=ui->tableView->currentIndex().row();
    if(row>=0){
        t3_ch_dishno=tablemodel->model_data(row, "ch_dishno").toString();
    }
    querymodel_2->setQuery(sql_select_3.arg(t3_ch_dishno));
    ui->tableView_2->restore();
    ui->tableView_2->resizeColumnsToContents();
}

void w_bt_dish_cookset::tocooktype()
{/*
    w_bt_dish_cookset_typeset dialog(this);
    */
//    w_bt_dish_cookset_cooktype dialog(this);
//    dialog.setWindowTitle(_TEXT_SLOT(this));
//    lds_roundeddialog_rect_align(&dialog).exec();
}

void w_bt_dish_cookset::tospecialcook()
{
    w_bt_dish_cookset_typeset dialog(this);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    lds_roundeddialog_rect_align(&dialog).exec();
}

void w_bt_dish_cookset::todel()
{
    int row=ui->tableView_2->currentIndex().row();
    if(row<0){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("请选择需要删除的记录"));
        return;
    }

    QString ch_no=querymodel_2->record(row).value("ch_no").toString();
    QString ch_dishno=querymodel_2->record(row).value("ch_dishno").toString();

    if(0==lds_messagebox::question(this, MESSAGE_TITLE_VOID,tr("确认删除指定做法记录"),tr("确认"),tr("取消"))){
        lds_query::tran_saction();
        lds_query query;
        if(query.execDelete("cey_bt_cp_dish", QString("ch_dishno ='%1' and ch_no='%2'").arg(ch_dishno,ch_no))){
            lds_query::com_mit();
        } else {
            lds_query::roll_back();
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("数据库错误"));
        }
        torefresh();
    }
}

void w_bt_dish_cookset::todishset()
{
    w_bt_dish_cookset_dishset dialog(this);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    lds_roundeddialog_rect_align(&dialog).exec();
    torefresh();
}

void w_bt_dish_cookset::todishsets()
{
}

void w_bt_dish_cookset::toexport()
{
    w_sys_manage_basicdataexport dialog(this);
    dialog.reset_export_tablenames(QStringList() << tablemodel->tableName());
    dialog.reset_export_confname(QString("%1/path").arg(this->metaObject()->className()));
    dialog.setWindowTitle(tr("基础数据导出"));
    lds_roundeddialog_rect_align(&dialog).exec();
}

void w_bt_dish_cookset::toexit()
{
    w_sys_manage_cloudsync_with_time_running::exec_keepdata_byconf(this);
    this->reject();
}

//=====================================
#include "ui_w_bt_dish_unitset_dialog.h"
w_bt_dish_cookset_cooktype::w_bt_dish_cookset_cooktype(QWidget *parent):QDialog(parent),
    ui(new Ui_w_bt_dish_unitset_Dialog)
{
    ui->setupUi(this);
    tablemodel=new lds_model_sqltablemodel(this);
    tablemodel->setTable("cey_bt_cp_type");
    tablemodel->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    ui->tableView->setEditTriggers(QTableView::AllEditTriggers);ui->tableView->setModel(tablemodel);
    tablemodel->save_set_header("ch_typeno", tr("编号"));
    tablemodel->save_set_header("vch_typename", tr("特殊做法类型"), true);
    tablemodel->enable_vheaderdata_dirty_translate();
    ui->tableView->rePecifyHeader(tablemodel->get_save_set_header_saver_column_list());
    ui->tableView->horizontalHeader()->setStretchLastSection(true);

    //! toolbar
    ui->widget->setup(QStringList()<<tr("新增")<<tr("删除")<<tr("刷新")<<tr("保存")<<tr("退出"), Qt::AlignRight, lds_keytoolbutton::SYSKEY);
    connect(ui->widget->index_widget(tr("新增")),SIGNAL(clicked()),this,SLOT(tonew()));
    connect(ui->widget->index_widget(tr("删除")),SIGNAL(clicked()),this,SLOT(todel()));
    connect(ui->widget->index_widget(tr("刷新")),SIGNAL(clicked()),this,SLOT(torefresh()));
    connect(ui->widget->index_widget(tr("保存")),SIGNAL(clicked()),this,SLOT(tosave()));
    connect(ui->widget->index_widget(tr("退出")),SIGNAL(clicked()),this,SLOT(toexit()));

    //! refresh
    torefresh();

}

void w_bt_dish_cookset_cooktype::tonew()
{
    int rowcount=tablemodel->rowCount();
    tablemodel->insertRow(rowcount);

    tablemodel->model_data_set(rowcount, "ch_typeno",
                               tablemodel->max_column_add_1_dec("ch_typeno"));

    ui->tableView->setCurrentIndex(tablemodel->model_index(rowcount, "vch_typename"));

}

void w_bt_dish_cookset_cooktype::todel()
{
    int row=ui->tableView->currentIndex().row();
    if(row<0){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("请先选择删除记录"));
        return;
    }
    if(1==lds_messagebox::question(this, MESSAGE_TITLE_VOID,tr("确认删除"),tr("确认"),tr("取消"))){
        return;
    }
    QString ls_typeno=tablemodel->model_data(row, 0).toString();
    lds_query query;
    query.execSelect(QString("select count(0) from cey_bt_cp_memo where ch_typeno='%1'").arg(ls_typeno));
    query.next();
    if(query.recordValue(0).toLongLong()>0){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("指定类型已经被使用，不能删除"));
        return;
    }
    tablemodel->removeRow(row);
}

bool w_bt_dish_cookset_cooktype::tosave()
{
    QString ch_typeno;
    bool ok;
    for(int r=0,r_count=tablemodel->rowCount();r<r_count;r++){
        if(tablemodel->opera(r) == lds_model_sqltablemodel::OpDelete) continue;

        ch_typeno = tablemodel->model_data(r, "ch_typeno").toString();
        ch_typeno.toInt(&ok);
        if(!ok){
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QString::number(r+1) + tr("行")+","+tr("编码必须为数字"));
            return false;
        }
        if(tablemodel->model_data(r, "vch_typename").toString().isEmpty()){
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("特殊做法类型为空，不能保存")+QString("[%1]").arg(r+1));
            return false;
        }
    }

    if(tablemodel->trySubmitAll()){
        lds_messagebox::information(this, MESSAGE_TITLE_VOID,tr("保存成功"));
        return true;
    } else {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("数据库错误")+QString("%1").arg(__LINE__));
        return false;
    }
}

void w_bt_dish_cookset_cooktype::torefresh()
{
    tablemodel->select();
    if(tablemodel->rowCount() > 0) {
        ui->tableView->selectRow(0);
    }
}

void w_bt_dish_cookset_cooktype::toexit()
{
    if(tablemodel->model_is_dirty()){
        switch(lds_messagebox::question(this, MESSAGE_TITLE_VOID,tr("有数据未保存,确认保存?"), tr("保存"),tr("不保存"),tr("取消"))){
        case 0:
            if(!tosave()){
                return;
            }
            break;
        case 1:
            break;
        case 2:
            return;
        }
    }
    this->reject();
}

//================================
#include "ui_w_bt_dish_cookset_specialcook_dialog.h"
w_bt_dish_cookset_specialcook::w_bt_dish_cookset_specialcook(QWidget *parent):
    QDialog(parent),ui(new Ui_w_bt_dish_cookset_specialcook__Dialog)
{
    ui->setupUi(this);
    //! tableview

    tablemodel=new lds_model_sqltablemodel(this);
    tablemodel->setTable("cey_bt_cp_type");
    tablemodel->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    tablemodel->save_set_header("ch_typeno", tr("编号"));
    tablemodel->save_set_header("vch_typename", tr("特殊做法类型"));

    tablemodel_2=new lds_model_sqltablemodel(this);
    tablemodel_2->setTable("cey_bt_cp_memo");
    tablemodel_2->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    tablemodel_2->save_set_header("ch_no", tr("编号"));
    tablemodel_2->save_set_header("vch_cp_memo", tr("特殊做法"), true);
    //    tablemodel_2->save_set_header("ch_typeno", tr("特殊做法类型"));
    tablemodel_2->save_set_header("num_add_price", tr("加价"), true);
    tablemodel_2->enable_vheaderdata_dirty_translate();

    ui->tableView->setEditTriggers(QTableView::AllEditTriggers);
    ui->tableView->setModel(tablemodel);
    ui->tableView->rePecifyHeader(tablemodel->get_save_set_header_saver_column_list());
    ui->tableView_2->setEditTriggers(QTableView::AllEditTriggers);
    ui->tableView_2->setModel(tablemodel_2);
    ui->tableView_2->rePecifyHeader(tablemodel_2->get_save_set_header_saver_column_list());

    //! toolbar

    ui->widget->setup(QStringList()<<tr("新增")<<tr("删除")<<tr("刷新")<<tr("保存")<<tr("退出"),Qt::AlignRight, lds_keytoolbutton::SYSKEY);
    connect(ui->widget->index_widget(tr("新增")),SIGNAL(clicked()),this,SLOT(tonew()));
    connect(ui->widget->index_widget(tr("删除")),SIGNAL(clicked()),this,SLOT(todel()));
    connect(ui->widget->index_widget(tr("刷新")),SIGNAL(clicked()),this,SLOT(torefresh()));
    connect(ui->widget->index_widget(tr("保存")),SIGNAL(clicked()),this,SLOT(tosave()));
    connect(ui->widget->index_widget(tr("退出")),SIGNAL(clicked()),this,SLOT(toexit()));
    connect(ui->tableView,SIGNAL(clicked(QModelIndex)),this,SLOT(totable_2()));

    //! refresh
    torefresh();
}

void w_bt_dish_cookset_specialcook::tonew()
{
    if(ui->tableView->currentIndex().row() < 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("没有任何类别被选中"));
        return;
    }
    int rowcount=tablemodel_2->rowCount();
    tablemodel_2->insertRow(rowcount);
    tablemodel_2->model_data_set(rowcount,0,tablemodel_2->max_column_add_1__max_sql_add_1_dec("ch_no", "%03d"));
    tablemodel_2->model_data_set(rowcount,2,tablemodel->model_data(ui->tableView->currentIndex().row(),"ch_typeno").toString());
    tablemodel_2->model_data_set(rowcount,3, "0");

    ui->tableView_2->setCurrentIndex(tablemodel_2->model_index(rowcount, "vch_cp_memo"));
}

void w_bt_dish_cookset_specialcook::todel()
{
    int row = ui->tableView_2->currentIndex().row();
    if(row < 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("没有数据被选中"));
        return;
    }
    if(1==lds_messagebox::question(this, MESSAGE_TITLE_VOID,tr("确认删除"),tr("确认"),tr("取消"))){
        return;
    }
    tablemodel_2->removeRow(row);
}

bool w_bt_dish_cookset_specialcook::tosave()
{
    QString ch_no;
    bool ok;
    for(int r=0,r_count=tablemodel_2->rowCount();r < r_count;r++){

        if(tablemodel_2->opera(r) == lds_model_sqltablemodel::OpDelete)continue;

        ch_no = tablemodel_2->model_data(r, "ch_no").toString();
        ch_no.toInt(&ok);
        if(!ok){
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QString::number(r+1)+tr("行")+","+tr("编码必须为数字"));
            return false;
        }
        if(ch_no.length() != 3){
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID,  QString::number(r+1)+tr("行")+","+tr("编码必须为3位"));
            return false;
        }
        if(tablemodel_2->model_data(r, "vch_cp_memo").toString().isEmpty()){
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID,  QString::number(r+1)+tr("行")+","+tr("特殊做法为空，不能保存"));
            return false;
        }
    }

    if(tablemodel_2->trySubmitAll()){
        lds_messagebox::information(this, MESSAGE_TITLE_VOID,tr("保存成功"));
        return true;
    } else {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("数据库错误")+QString("%1").arg(__LINE__));
        return false;
    }
}

void w_bt_dish_cookset_specialcook::torefresh()
{
    tablemodel->select();
    if(ui->tableView->currentIndex().row() < 0) {
        ui->tableView->selectRow(0);
    }
    ui->tableView->resizeColumnsToContents();

    tablemodel_2->setFilter(QString("ch_typeno='%1'")
                            .arg(tablemodel->model_data(ui->tableView->currentIndex().row(), "ch_typeno").toString())
                            );
    ui->tableView_2->resizeColumnsToContents();
}

void w_bt_dish_cookset_specialcook::toexit()
{
    if(tablemodel_2->model_is_dirty()){
        switch(lds_messagebox::question(this, MESSAGE_TITLE_VOID,tr("有数据未保存,确认保存?"), tr("保存"),tr("不保存"),tr("取消"))){
        case 0:
            if(!tosave()){
                return;
            }
            break;
        case 1:
            break;
        case 2:
            return;
        }
    }
    this->reject();
}

void w_bt_dish_cookset_specialcook::totable_2()
{
    if(tablemodel_2->model_is_dirty()){
        if(0==lds_messagebox::question(this, MESSAGE_TITLE_VOID,tr("有数据未保存,确认保存?"), tr("保存"),tr("不保存"))){
            if(!tosave()){
                ui->tableView->select_beforerow();
                return;
            }
        }
    }
    tablemodel_2->setFilter(QString("ch_typeno='%1'")
                            .arg(tablemodel->model_data(ui->tableView->currentIndex().row(), "ch_typeno").toString())
                            );
    ui->tableView_2->resizeColumnsToContents();
}


//=============================
#include "ui_w_bt_dish_cookset_dishset_dialog.h"
w_bt_dish_cookset_dishset::w_bt_dish_cookset_dishset(QWidget *parent):
    QDialog(parent),ui(new Ui_w_bt_dish_cookset_dishset_Dialog),_parent(static_cast<w_bt_dish_cookset *>(parent))
{
    ui->setupUi(this);
    //! toolbar


    ui->widget->setup(QStringList()<<tr("全选")<<tr("全不选")<<tr("应用到所有") /*<< tr("应用")*/<<tr("确认")<<tr("退出"),Qt::AlignRight);
    //! treewidget
    ui->treeWidget->header()->hide();
    ui->treeWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->treeWidget->setColumnCount(1);

    //! tableview
    tablemodel_2=new lds_model_sqltablemodel(this);
    tablemodel_2->setTable("cey_bt_cp_memo");
    tablemodel_2->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);

    ui->tableView->setEditTriggers(QTableView::AllEditTriggers);
    ui->tableView->setModel(tablemodel_2);
    ui->tableView->setSelectionMode(QAbstractItemView::MultiSelection);

    //! refresh
    torefresh();
    tablemodel_2->save_set_header("ch_no",tr("编号"));
    tablemodel_2->save_set_header("vch_cp_memo",tr("特殊做法"));
    tablemodel_2->save_set_header("num_add_price",tr("加价"));
    ui->tableView->rePecifyHeader(tablemodel_2->get_save_set_header_saver_column_list());
    //! connect

    connect(ui->widget->index_widget(tr("全选")),SIGNAL(clicked()),ui->tableView,SLOT(toSelectAll()));
    connect(ui->widget->index_widget(tr("全不选")),SIGNAL(clicked()),ui->tableView,SLOT(toClearAll()));
    connect(ui->widget->index_widget(tr("应用到所有")), SIGNAL(clicked()),this,SLOT(toappall()));
    //    connect(ui->widget->index_widget(tr("应用")),SIGNAL(clicked()),this,SLOT(toapp()));
    connect(ui->widget->index_widget(tr("确认")),SIGNAL(clicked()),this,SLOT(took()));
    connect(ui->widget->index_widget(tr("退出")),SIGNAL(clicked()),this,SLOT(toexit()));
    connect(ui->treeWidget,SIGNAL(pressed(QModelIndex)),this,SLOT(treeselect()));

    ui->label_dish->setText(_parent->tablemodel->model_data(_parent->ui->tableView->currentIndex().row(), "vch_dishname").toString());
    ui->widget->index_widget(tr("确认"))->setEnabled(_parent->ui->tableView->currentIndex().isValid());
}

void w_bt_dish_cookset_dishset::toSelectAll()
{
    ui->tableView->selectAll();
}

void w_bt_dish_cookset_dishset::toselectno()
{
    ui->tableView->clearSelection();
}

void w_bt_dish_cookset_dishset::toapp()
{
    appone();
}

void w_bt_dish_cookset_dishset::toappall()
{
    QString ch_dishno;
    QString errtext;
    if(ui->tableView->getSelectedRowList().isEmpty()) {
        if(1 == lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("没有做法被选中，继续将会清空所有菜品做法"), tr("继续"), tr("取消"))) {
            return;
        }
    }

    lds_query::tran_saction();
    for(int row = 0, rowcount = _parent->tablemodel->rowCount(); row < rowcount; row++) {
        ch_dishno=_parent->tablemodel->model_data(row, "ch_dishno").toString();
        if(app(ch_dishno, &errtext)) {
            continue;
        }
        goto end;
    }
    lds_query::com_mit();
    lds_messagebox::information(this, MESSAGE_TITLE_VOID,tr("操作成功"));
    return;
end:
    lds_query::roll_back();
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("操作失败")+ errtext);
    return;

}

void w_bt_dish_cookset_dishset::took()
{
    if(appone()) {
        this->accept();
    }
}

void w_bt_dish_cookset_dishset::toexit()
{
    this->reject();
}

void w_bt_dish_cookset_dishset::torefresh()
{
    //treewidget
    ui->treeWidget->deleteAllItem();
    ui->treeWidget->rootitem=new QTreeWidgetItem(QStringList()
                                                 <<tr("全部类型"));
    ui->treeWidget->addTopLevelItem(ui->treeWidget->rootitem);

    lds_query query;
    query.execSelect("select concat(vch_typename,'[',ch_typeno,']') as 'nametype' from cey_bt_cp_type"
                     );
    while(query.next()){
        QTreeWidgetItem *item=new QTreeWidgetItem(QStringList()
                                                  <<QString("%1")
                                                  .arg(query.recordValue("nametype").toString())
                                                  );
        ui->treeWidget->rootitem->addChild(item);
    }
    ui->treeWidget->setItemSizeHint();//更新所有的 item的sizehint
    ui->treeWidget->restore();
    //tableview
    treeselect();
    ui->tableView->restore();
}

void w_bt_dish_cookset_dishset::treeselect()
{
    t2_ch_typeno.clear();
    QString filter;
    if(ui->treeWidget->currentItem()){
        int index=ui->treeWidget->deepofitem(ui->treeWidget->currentItem());
        if(index==1){
            t2_ch_typeno=ui->treeWidget->currentItem()->text(0);
            t2_ch_typeno=t2_ch_typeno.split("[")[1];
            t2_ch_typeno.chop(1);
            filter=QString("ch_typeno='%1'").arg(t2_ch_typeno);
        }
    }
    tablemodel_2->setFilter(filter);
    ui->tableView->restore();
}

bool w_bt_dish_cookset_dishset::appone()
{
    QString ch_dishno;
    if(ui->tableView->getSelectedRowList().isEmpty()) {
        if(1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID,tr("没有行被选中"), tr("继续"), tr("取消"))) {
            return false;
        }
    }
    int row = _parent->ui->tableView->currentIndex().row();
    if(row < 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("没有可应用的菜品"));
        return false;
    }
    lds_query::tran_saction();
    ch_dishno=_parent->tablemodel->model_data(row, "ch_dishno").toString();

    if(app(ch_dishno)) {
        lds_query::com_mit();
        return true;
    }
    lds_query::roll_back();

    lds_messagebox::warning(this, MESSAGE_TITLE_VOID,ui->label_dish->text() + tr("操作失败"));


    return false;
}

bool w_bt_dish_cookset_dishset::app(const QString &ch_dishno, QString *errstring)
{
    QList<int>currows= ui->tableView->getSelectedRowList();
    lds_query query;
    if(query.execDelete("cey_bt_cp_dish", QString("ch_dishno = '%1' ").arg(ch_dishno))){
        foreach(int r, currows){
            if(query.execInsert("cey_bt_cp_dish",qrtVariantPairList()
                                << qrtVariantPair("ch_dishno", ch_dishno)
                                << qrtVariantPair("ch_no", tablemodel_2->model_data(r, "ch_no")))){
                continue;
            }
            goto failed;
        }
        return true;
    }

failed:
    if(errstring) *errstring = tr("数据库错误");
    return false;
}

#include "ui_w_bt_dish_cookset_dishset.h"
w_bt_dish_cookset_typeset::w_bt_dish_cookset_typeset(QWidget *parent)
    : QDialog(parent),
      ui(new Ui_w_bt_dish_cookset_typeset_dialog)
{
    ui->setupUi(this);

    ///tableview
    tablemodel=new lds_model_sqltablemodel(this);
    tablemodel->setTable("cey_bt_cp_type");
    tablemodel->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    tablemodel->save_set_header("ch_typeno", tr("类别编码"));
    tablemodel->save_set_header("vch_typename", tr("类别名称"));

    ui->tableView->setEditTriggers(QTableView::AllEditTriggers);
    ui->tableView->setModel(tablemodel);
    ui->tableView->rePecifyHeader(tablemodel->get_save_set_header_saver_column_list());

    tablemodel_2=new lds_model_sqltablemodel(this);
    tablemodel_2->setTable("cey_bt_cp_memo");
    tablemodel_2->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    tablemodel_2->save_set_header("ch_no", tr("做法编码"));
    tablemodel_2->save_set_header("vch_cp_memo", tr("做法名称"), true);
    tablemodel_2->save_set_header("num_add_price", tr("做法加价"), true);
    tablemodel_2->enable_vheaderdata_dirty_translate();

    ui->tableView_2->setEditTriggers(QTableView::AllEditTriggers);
    ui->tableView_2->setModel(tablemodel_2);
    ui->tableView_2->rePecifyHeader(tablemodel_2->get_save_set_header_saver_column_list());
    //! tableview

    ///toolbar
    ui->widget->setup(QStringList()<<tr("新增")<<tr("修改")<<tr("删除"), Qt::AlignLeft, lds_keytoolbutton::NOKEY);
    connect(ui->widget->index_widget(tr("新增")),SIGNAL(clicked()),this,SLOT(tonew()));
    connect(ui->widget->index_widget(tr("修改")),SIGNAL(clicked()),this,SLOT(tochange()));
    connect(ui->widget->index_widget(tr("删除")),SIGNAL(clicked()),this,SLOT(todel()));
    connect(ui->tableView,SIGNAL(selectChanged(QModelIndex)),this,SLOT(tableview2Update(QModelIndex)));

    ui->widget_2->setup(QStringList() << tr("新增") << tr("删除") << lds_toolbar::stretch << tr("保存") << tr("取消"), 0);
    connect(ui->widget_2->index_widget(tr("新增")),SIGNAL(clicked()),this,SLOT(tonew2()));
    connect(ui->widget_2->index_widget(tr("删除")),SIGNAL(clicked()),this,SLOT(todel2()));

    connect(ui->widget_2->index_widget(tr("保存")) ,SIGNAL(clicked()),this,SLOT(tosave()));
    connect(ui->widget_2->index_widget(tr("取消")) ,SIGNAL(clicked()),this,SLOT(toexit()));
    ///!toolbar

    QTimer::singleShot(10, this, SLOT(toRefreshType()));
}

w_bt_dish_cookset_typeset::~w_bt_dish_cookset_typeset()
{
    delete ui;
}

void w_bt_dish_cookset_typeset::tonew2()
{
    //请先新增大类
    int row=ui->tableView->currentIndex().row();
    if(row<0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("请先选择类别"));
        return;
    }
    //sql_ch_no_max_1 = 数据库里的最大值 + 新增的个数 + 1
    int rowcount=tablemodel_2->rowCount();
    QString  ch_typeno = tablemodel->model_data(row, "ch_typeno").toString();
    int insert_count = tablemodel_2->rowCountOp(lds_model_sqltablemodel::OpInsert);
    lds_query query;
    query.execSelect(" select max(ch_no) from cey_bt_cp_memo  ");
    query.next();
    QString sql_ch_no_max_1 = QString().sprintf("%03d", (query.recordValue(0).toInt() + insert_count + 1));

    tablemodel_2->insertRow(rowcount);
    tablemodel_2->model_data_set(rowcount, "ch_no", sql_ch_no_max_1);
    tablemodel_2->model_data_set(rowcount, "ch_typeno", ch_typeno);
    ui->tableView_2->setCurrentIndex(tablemodel_2->model_index(rowcount, "vch_cp_memo"));
}

void w_bt_dish_cookset_typeset::todel2()
{
    int row = ui->tableView_2->currentIndex().row();
    if(row < 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("没有做法被选中"));
        return;
    }

    lds_query query;
    query.execSelect(QString("select 1 from cey_bt_cp_dish where ch_no = '%1' ")
                     .arg(tablemodel_2->model_data(row, "ch_no").toString()));
    if(query.next()){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("该做法下已有菜品，不能删除"));
        return;
    }
    if(1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, tr("确认删除"), tr("确认"), tr("取消"))){
        return;
    }
    tablemodel_2->removeRow(row);
}

void w_bt_dish_cookset_typeset::tosave()
{
    saveData();
}

void w_bt_dish_cookset_typeset::toRefreshType()
{
    tablemodel->select();
    ui->tableView->restore(0);
}

void w_bt_dish_cookset_typeset::toexit()
{
    if(tablemodel_2->model_is_dirty()){
        switch(lds_messagebox::question(this, MESSAGE_TITLE_VOID,tr("有数据未保存,确认保存?"), tr("保存"),tr("不保存"),tr("取消"))){
        case 0:
            if(!saveData()){
                return;
            }
            this->accept();
            break;
        case 1:
            break;
        case 2:
            return;
        }
    }
    this->reject();
}

void w_bt_dish_cookset_typeset::tableview2Update(const QModelIndex &typeIndex)
{
    if(tablemodel_2->model_is_dirty()){
        switch(lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("有数据未保存,确认保存?"), tr("保存"),tr("不保存"))){
        case 0:
            if(!saveData()) return;
            break;
        case 1:
            break;
        }
    }
    if(false == typeIndex.isValid()) {
        tablemodel_2->setFilter("1 = 2");
    } else {
        tablemodel_2->setFilter(QString("ch_typeno='%1'").arg(tablemodel->model_data(typeIndex.row(), "ch_typeno").toString()));
    }
    ui->tableView_2->restore(0);
}

void w_bt_dish_cookset_typeset::tonew()
{
    lds_dialog_input inputdialog(this);
    inputdialog.setWindowTitle(_TEXT_SLOT(this));
    inputdialog.ui->label->setText(tr("类别编码"));
    inputdialog.ui->label_2->setText(tr("类别名称"));
    inputdialog.ui->lineEdit->setEnabled(false);

    inputdialog.showSecondLineEdt();
    inputdialog.hideKey();

    //max_ch_typeno_1
    lds_query query;
    query.execSelect(" select max(ch_typeno) from cey_bt_cp_type");
    query.next();
    QString max_ch_typeno_1 = QString().sprintf("%02d", query.recordValue(0).toInt() + 1);
    inputdialog.ui->lineEdit->setText(max_ch_typeno_1);

    if(QDialog::Accepted != lds_roundeddialog_rect_align(&inputdialog).exec()){
        return;
    }
    //
    if(inputdialog.ui->lineEdit_2->text().trimmed().isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("类别名称不能为空"));
        return;
    }
    lds_query::tran_saction();
    if(query.execInsertDuplicateUpdate("cey_bt_cp_type",
                                       "ch_typeno",
                                       qrtVariantPairList()
                                       << qrtVariantPair("ch_typeno", inputdialog.ui->lineEdit->text())
                                       << qrtVariantPair("vch_typename", inputdialog.ui->lineEdit_2->text())
                                       )) {
        lds_query::com_mit();
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, tr("操作成功"));
        tablemodel->select();
        ui->tableView->restore();
        return;
    }
    lds_query::roll_back();
    lds_messagebox::warning(this ,MESSAGE_TITLE_VOID, tr("操作失败"));
}

void w_bt_dish_cookset_typeset::tochange()
{
    int row = ui->tableView->currentIndex().row();
    if(row < 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("没有数据被选中"));
        return;
    }
    QString ch_typeno = tablemodel->model_data(row, "ch_typeno").toString();
    QString vch_typename = tablemodel->model_data(row, "vch_typename").toString();

    lds_dialog_input inputdialog(this);
    inputdialog.setWindowTitle(_TEXT_SLOT(this));
    inputdialog.ui->label->setText(tr("类别编码"));
    inputdialog.ui->label_2->setText(tr("类别名称"));
    inputdialog.ui->lineEdit->setEnabled(false);

    inputdialog.showSecondLineEdt();
    inputdialog.hideKey();

    inputdialog.ui->lineEdit->setText(ch_typeno);
    inputdialog.ui->lineEdit_2->setText(vch_typename);

    if(QDialog::Accepted != lds_roundeddialog_rect_align(&inputdialog).exec()){
        return;
    }

    //
    if(inputdialog.ui->lineEdit_2->text().trimmed().isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("类别名称不能为空"));
        return;
    }
    lds_query::tran_saction();
    lds_query query;
    if(query.execUpdate("cey_bt_cp_type", "vch_typename", inputdialog.ui->lineEdit_2->text(),
                        qrtEqual("ch_typeno", inputdialog.ui->lineEdit->text()))) {
        lds_query::com_mit();
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, tr("操作成功"));
        tablemodel->select();
        ui->tableView->restore();
        return;
    }
    lds_query::roll_back();
    lds_messagebox::warning(this ,MESSAGE_TITLE_VOID, tr("操作失败"));
}

void w_bt_dish_cookset_typeset::todel()
{
    int row = ui->tableView->currentIndex().row();
    if(row < 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("没有数据被选中"));
        return;
    }
    lds_query query;
    query.execSelect(QString("select count(0) from cey_bt_cp_memo where ch_typeno = '%1' ")
                     .arg(tablemodel->model_data(row, "ch_typeno").toString()));
    query.next();
    if(query.recordValue(0).toLongLong() > 0){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("该大类下已有小类，不能删除"));
        return;
    }

    if(1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, tr("确认删除"), tr("确认"), tr("取消"))) {
        return;
    }
    lds_query::tran_saction();
    if(query.execDelete("cey_bt_cp_type", qrtEqual("ch_typeno", tablemodel->model_data(row, "ch_typeno").toString() ))) {
        lds_query::com_mit();
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, tr("操作成功"));
        tablemodel->select();
        ui->tableView->restore();
        return;
    }
    lds_query::roll_back();
    lds_messagebox::warning(this ,MESSAGE_TITLE_VOID, tr("操作失败"));
}

bool w_bt_dish_cookset_typeset::saveData()
{
    for(int row = 0, rowcount = tablemodel_2->rowCount();row < rowcount;row++){
        if(tablemodel_2->model_data(row, 0).toString().isEmpty()){
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("行数")+QString::number(row+1)+tr("小类编码不能为空"));
            return false;
        }
        if(tablemodel_2->model_data(row, 1).toString().isEmpty()){
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("行数")+QString::number(row+1)+tr("小类名称不能为空"));
            return false;
        }
    }
    if(tablemodel_2->trySubmitAll()) {
        lds_messagebox::information(this, MESSAGE_TITLE_VOID,tr("保存成功"));
        return true;
    }
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("数据库错误"));
    return false;
}
