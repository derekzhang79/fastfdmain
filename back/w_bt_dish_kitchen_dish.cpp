﻿#include "w_bt_dish_kitchen_dish.h"
#include "ui_w_bt_dish_kitchen_dish.h"
#include "lds_query.h"
#include <QSqlError>
#include <QtDebug>
#include "lds_query.h"
#include "backheader.h"
#include "lds_tableview_delegate_check.h"
#include "w_bt_dish_kitchen_dish_plan.h"
#include "w_bt_dish_kitchenprinter.h"
#include "lds_messagebox.h"
#include "ui_w_bt_dish_kitchen_dish_more.h"
#include <QSqlDatabase>
#include "printer_pos.h"
#include "n_func.h"
#include "n_func.h"
#include "public_printby_ch_billno.h"
#include "w_bt_dish.h"
#include "fexpandmain_2.h"
#include "commonheader.h"
#include "printer_pos_vidpid.h"
#include "printer_pos_net.h"
#include "w_bt_dish.h"
#include "lds.h"
#include "getfilepath.h"
#include "fexpandmain_model_sqltablemodel.h"
#include "lds_model_sqltablemodel_delegate_com.h"
#include "printer_normal.h"
#include "w_bt_dish_kitchen_dish_state.h"
#include "w_sys_manage_cloudsync_rb_order.h"
#include "w_scr_dish_language_switch_set.h"
#include "btnlanguageswicth.h"
#include "w_sys_manage_outer_pay_set.h"
#include "lds_menu.h"
#include "backheader.h"

static QString ifEmpty(const QString &text, const QString &def){
    if(text.trimmed().isEmpty()) {
        return def;
    }
    return text;
}

///1. 厨打轮询只轮询当前
w_bt_dish_kitchen_dish::w_bt_dish_kitchen_dish(QWidget *parent) :
    lds_Dialog(parent),
    ui(new Ui::w_bt_dish_kitchen_dish)
{
    ui->setupUi(this);
    initViewNormal();
}

w_bt_dish_kitchen_dish::w_bt_dish_kitchen_dish(QWidget *parent, bool tmpdish) :
    lds_Dialog(parent),
    ui(new Ui::w_bt_dish_kitchen_dish)
{
    ui->setupUi(this);
    initViewTmp();
}

w_bt_dish_kitchen_dish::~w_bt_dish_kitchen_dish()
{
    delete ui;
}

void w_bt_dish_kitchen_dish::retranslateView()
{
    ui->retranslateUi(this);
}

void w_bt_dish_kitchen_dish::initView()
{
    lds::setwflagFrameless(this);
    //! treewidget
    ui->treeWidget->header()->hide();
    ui->treeWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->treeWidget->setColumnCount(1);
    ///taleview
    tablemodel = new lds_model_sqltablemodel(this);
    tablemodel->setTable("cey_bt_dish");
    tablemodel->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    tablemodel->save_set_header("ch_dishno", tr("品码"));
    tablemodel->save_set_header("vch_dishname", tr("品名"));
    tablemodel->save_set_header("ch_kitchen_print_flag", tr("是否厨打"), true);
    tablemodel->save_set_header("vch_kitchen_print_id1", tr("厨打方案1"), true);
    tablemodel->save_set_header("vch_kitchen_print_id2", tr("厨打方案2"), true);
    tablemodel->save_set_header("vch_kitchen_print_id3", tr("厨打方案3"), true);

    tablemodel->save_set_header("ch_kitchen_out_flag", tr("是否出品"), true);
    tablemodel->save_set_header("vch_kitchen_out_id1", tr("出品方案1"), true);
    tablemodel->save_set_header("vch_kitchen_out_id2", tr("出品方案2"), true);
    tablemodel->save_set_header("vch_kitchen_out_id3", tr("出品方案3"), true);


    ui->tableView->setEditTriggers(QTableView::AllEditTriggers);
    ui->tableView->setModel(tablemodel);
    ui->tableView->justShowColumns(QList<int>()
                               << tablemodel->fieldIndex("ch_dishno")<< tablemodel->fieldIndex("vch_dishname")
                               << tablemodel->fieldIndex("ch_kitchen_print_flag")
                               << tablemodel->fieldIndex("vch_kitchen_print_id1")
                               << tablemodel->fieldIndex("vch_kitchen_print_id2")
                               << tablemodel->fieldIndex("vch_kitchen_print_id3")

                               << tablemodel->fieldIndex("ch_kitchen_out_flag")
                               << tablemodel->fieldIndex("vch_kitchen_out_id1")
                               << tablemodel->fieldIndex("vch_kitchen_out_id2")
                               << tablemodel->fieldIndex("vch_kitchen_out_id3")
                               );
    tablemodel->tablename_kvmap_insert("cey_bt_kitchen_plan", "vch_plan_id", "vch_plan_name", "", true, lds_model_sqltablemodel::SELECT_AUTO_REFRESH_KEY);
    lds_model_sqltablemodel_delegate_com *d = new lds_model_sqltablemodel_delegate_com(this, tablemodel->tablename_kvmap_d("cey_bt_kitchen_plan"));

    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("ch_kitchen_print_flag"), new lds_tableview_delegate_check(QVector<int>() << tablemodel->fieldIndex("ch_kitchen_print_flag"), ui->tableView, true));
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("vch_kitchen_print_id1"), d);
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("vch_kitchen_print_id2"), d);
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("vch_kitchen_print_id3"), d);

    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("ch_kitchen_out_flag"), new lds_tableview_delegate_check(QVector<int>() << tablemodel->fieldIndex("ch_kitchen_out_flag"), ui->tableView, true));
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("vch_kitchen_out_id1"), d);
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("vch_kitchen_out_id2"), d);
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("vch_kitchen_out_id3"), d);


    connect(ui->treeWidget,SIGNAL(pressed(QModelIndex)),this,SLOT(treeselect()));
    connect(ui->pushButton_exit, SIGNAL(clicked()),this,SLOT(toexit()));
    connect(ui->pushButton_kitchen_plan, SIGNAL(clicked()),this,SLOT(tokitchenplan()));
    connect(ui->pushButton_printer_manager,SIGNAL(clicked()),this,SLOT(toprintset()));
    connect(ui->pushButton_bath, SIGNAL(clicked()),this,SLOT(tomore()));
    connect(ui->pushButton_save, SIGNAL(clicked()),this,SLOT(tosave()));
    connect(ui->pushButton_kitchen_parameter, SIGNAL(clicked()),this,SLOT(tokitchenparam()));
    connect(ui->pushButton_language, SIGNAL(clicked()),this,SLOT(toSwitchLanguage()));
    ui->pushButton_language->hide();
}

void w_bt_dish_kitchen_dish::initViewTmp()
{
    sql_where_app = "cey_bt_dish.ch_tempflag = 'Y' ";
    initView();

    //! toolbar

    ui->pushButton_kitchen_plan->hide();
    ui->pushButton_printer_manager->hide();
    ui->pushButton_kitchen_parameter->hide();
    ui->pushButton_language->hide();
}

void w_bt_dish_kitchen_dish::initViewNormal()
{
    initView();
}

void w_bt_dish_kitchen_dish::refresh()
{
    QTimer::singleShot(0, this, SLOT(torefresh()));
}

void w_bt_dish_kitchen_dish::torefresh()
{
    ///treewidget
    lds_query query;
    ui->treeWidget->deleteAllItem();
    ui->treeWidget->rootitem = new QTreeWidgetItem(QStringList()
                                                   <<tr("全部类型"));
    ui->treeWidget->addTopLevelItem(ui->treeWidget->rootitem);

    query.execSelect("select  concat(vch_dish_typename,'[',ch_dish_typeno,']')     as 'nametype', ch_dish_typeno  from cey_bt_dish_type"
                     );
    while(query.next()){
        QTreeWidgetItem *item = new QTreeWidgetItem(QStringList()
                                                    << query.record().value("nametype").toString()
                                                    );
        ui->treeWidget->rootitem->addChild(item);
        //获取[]的内容
        lds_query query_1;
        QString no=query.record().value("nametype").toString();
        no=no.split("[")[1];
        no.chop(1);
        query_1.execSelect(QString(
                               "select concat(vch_sub_typename,'[',ch_sub_typeno,']') as 'nametype' from cey_bt_dish_subtype where ch_dish_typeno = '%1' order by ch_sub_typeno asc;"
                               )
                           .arg(no)
                           );
        while(query_1.next()){
            QTreeWidgetItem *item1=new QTreeWidgetItem(QStringList()
                                                       <<query_1.record().value("nametype").toString()
                                                       );
            item->addChild(item1);
        }
    }
    ui->treeWidget->setItemSizeHint();//更新所有的 item的sizehint
    ui->treeWidget->restore();
    ///tableview
    treeselect();
    if(tablemodel->rowCount() > 0){
        ui->tableView->resizeColumnsToContents();
    }
}

void w_bt_dish_kitchen_dish::treeselect()
{
    if(0 > checkDirty()) {//取消或操作失败
        ui->treeWidget->select_beforerow();
        return;
    }

    QString sqlselect;
    //tree检索
    if(ui->treeWidget->currentItem()){
        int index=ui->treeWidget->deepofitem(ui->treeWidget->currentItem());
        if(index>0){
            sqlselect=backheader::getinterdata(ui->treeWidget->currentItem()->text(0));
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
    sqlselect += " cey_bt_dish.ch_stopflag <> 'Y' ";

    if(!sql_where_app.isEmpty()) {
        sqlselect += " and " + sql_where_app;
    }
    //检索
    tablemodel->setFilter(sqlselect);
    ui->tableView->restore();
}

void w_bt_dish_kitchen_dish::toexit()
{
    if(0 <= checkDirty()) {//不保存 保存成功
        this->reject();
    }
}

void w_bt_dish_kitchen_dish::tokitchenplan()
{
    if(0 > checkDirty()) return;//取消或操作失败

    w_bt_dish_kitchen_dish_plan dialog(this);
    lds::setwflagFrameless(&dialog);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    dialog.move(0,0);
    dialog.setFixedSize(lds::MAIN_WINDOW_SIZE);
    dialog.exec();
    torefresh();
}

void w_bt_dish_kitchen_dish::toprintset()
{
    w_bt_dish_Kitchenprinter_printermanager dialog(this);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    lds_roundeddialog_rect_align(&dialog).exec();
}

void w_bt_dish_kitchen_dish::tomore()
{
    if(0 > checkDirty()) return;//取消或操作失败

    QString desc, sqlselect;
    if(ui->treeWidget->currentItem()){
        int index=ui->treeWidget->deepofitem(ui->treeWidget->currentItem());
        if(index>0){
            sqlselect=backheader::getinterdata(ui->treeWidget->currentItem()->text(0));
            if(index==1){
                desc = tr("大类编号") + ":" + sqlselect;
            } else if(index==2){
                desc = tr("小类编号") + ":" + sqlselect;
            }
        }
    }

    w_bt_dish_kitchen_dish_more dialog(this);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    dialog.setDesc(desc);
    lds_roundeddialog_rect_align(&dialog).exec();

}

void w_bt_dish_kitchen_dish::tosave()
{
    if(saveData()) {
        this->accept();
    }
}

void w_bt_dish_kitchen_dish::tokitchenparam()
{
    w_bt_dish_kitchen_dish_param dialog(this);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    lds_roundeddialog_rect_align(&dialog).exec();
}

void w_bt_dish_kitchen_dish::toSwitchLanguage()
{
    w_scr_dish_language_switch_set dialog(this);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    if(QDialog::Accepted == lds_roundeddialog_rect_align(&dialog).exec()) {
        QString key = lds::get_soft_curent_language_first();
        retranslateSystem(key);
        retranslateView();
    }
}

bool w_bt_dish_kitchen_dish::saveData()
{
    if(tablemodel->trySubmitAll()){
        lds_messagebox::information(this, MESSAGE_TITLE_VOID,tr("保存成功"));
        return true;
    }
    //错误提示
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("数据库错误"));
    return false;
}

//-1 取消/操作失败 0 不保存 1 保存成功
int w_bt_dish_kitchen_dish::checkDirty()
{
    if(tablemodel->model_is_dirty()){
        switch(lds_messagebox::question(this, MESSAGE_TITLE_VOID,tr("有数据未保存,确认保存?"), tr("保存"),tr("不保存"),tr("取消"))){
        case 0:
            if(!saveData()){
                return -1;//操作失败
            }
            return 1;//保存成功
        case 1:
            return 0;//不保存
        case 2:
            return -1;//取消
        }
    }
    return 1;//保存成功
}

/////////////////////////////////////////
w_bt_dish_kitchen_dish_more::w_bt_dish_kitchen_dish_more(w_bt_dish_kitchen_dish *parent)
    : QDialog(parent) ,ui( new Ui_w_bt_dish_kitchen_dish_more),_parent(parent)
{
    ui->setupUi(this);


    ///
    lds_model_sqltablemodel_d::fillData(ui->comboBox_vch_kitchen_print_id1, _parent->tablemodel->tablename_kvmap_d("cey_bt_kitchen_plan"));
    lds_model_sqltablemodel_d::fillData(ui->comboBox_vch_kitchen_print_id2, _parent->tablemodel->tablename_kvmap_d("cey_bt_kitchen_plan"));
    lds_model_sqltablemodel_d::fillData(ui->comboBox_vch_kitchen_print_id3, _parent->tablemodel->tablename_kvmap_d("cey_bt_kitchen_plan"));

    lds_model_sqltablemodel_d::fillData(ui->comboBox_vch_kitchen_out_id1, _parent->tablemodel->tablename_kvmap_d("cey_bt_kitchen_plan"));
    lds_model_sqltablemodel_d::fillData(ui->comboBox_vch_kitchen_out_id2, _parent->tablemodel->tablename_kvmap_d("cey_bt_kitchen_plan"));
    lds_model_sqltablemodel_d::fillData(ui->comboBox_vch_kitchen_out_id3, _parent->tablemodel->tablename_kvmap_d("cey_bt_kitchen_plan"));

    ///
    connect(ui->pushButton_cancel,SIGNAL(clicked()),this,SLOT(tocancel()));
    connect(ui->pushButton_ok,SIGNAL(clicked()),this,SLOT(took()));

}

w_bt_dish_kitchen_dish_more::~w_bt_dish_kitchen_dish_more()
{
    delete ui;
}

void w_bt_dish_kitchen_dish_more::setDesc(const QString &desc)
{
    ui->label_desc->setText(desc);
}

void w_bt_dish_kitchen_dish_more::took()
{
    for(int row = 0, rowcount = _parent->tablemodel->rowCount(); row < rowcount; row++){
        _parent->tablemodel->model_data_set(row, _parent->tablemodel->fieldIndex("ch_kitchen_print_flag"), ui->checkBox_ch_kitchen_print_flag->isChecked()? "Y":"N");
        _parent->tablemodel->model_data_set(row, _parent->tablemodel->fieldIndex("vch_kitchen_print_id1"), ui->comboBox_vch_kitchen_print_id1->comusrdata().toString());
        _parent->tablemodel->model_data_set(row, _parent->tablemodel->fieldIndex("vch_kitchen_print_id2"), ui->comboBox_vch_kitchen_print_id2->comusrdata().toString());
        _parent->tablemodel->model_data_set(row, _parent->tablemodel->fieldIndex("vch_kitchen_print_id3"), ui->comboBox_vch_kitchen_print_id3->comusrdata().toString());

        _parent->tablemodel->model_data_set(row, _parent->tablemodel->fieldIndex("ch_kitchen_out_flag"), ui->checkBox_ch_kitchen_out_flag->isChecked()? "Y":"N");
        _parent->tablemodel->model_data_set(row, _parent->tablemodel->fieldIndex("vch_kitchen_out_id1"), ui->comboBox_vch_kitchen_out_id1->comusrdata().toString());
        _parent->tablemodel->model_data_set(row, _parent->tablemodel->fieldIndex("vch_kitchen_out_id2"), ui->comboBox_vch_kitchen_out_id2->comusrdata().toString());
        _parent->tablemodel->model_data_set(row, _parent->tablemodel->fieldIndex("vch_kitchen_out_id3"), ui->comboBox_vch_kitchen_out_id3->comusrdata().toString());
    }
    this->accept();
}

void w_bt_dish_kitchen_dish_more::tocancel()
{
    this->reject();
}


void w_bt_dish_kitchen_dish_more::fillData_1(QComboBox *com, const QMap<QString, QVariant> *map)
{
    //    com->addItem("", "");
    for(QMap<QString, QVariant>::const_iterator itr = map->begin(); itr != map->end(); itr ++ ){
        com->addItem(itr.value().toString(), itr.key());
    }
}

int BoxPrintIndex::getIndex(const QString &ch_billno, const QString &vch_print_id, const QString &vch_plan_id, int int_dish_type, const QString &ch_dish_typeno, const QString &ch_sub_typeno, QString &int_indexbox_desc)
{
    lds_query query;
    QString key = QString("%1_%2_%3_%4").arg(ch_billno).arg(vch_print_id).arg(int_dish_type).arg(vch_plan_id);
    int idx = keyIdxMap.count();

    switch (int_dish_type) {
    case 0://一桌一单0
        break;
    case 1://一大类一单1
        key += "_"+ch_dish_typeno;
        query.execSelect(QString("select vch_dish_typename from cey_bt_dish_type where ch_dish_typeno = '%1' ").arg(ch_dish_typeno));
        query.next();
        int_indexbox_desc = query.record().value(0).toString();
        break;
    case 2://一小类一单2
        key += "_"+ch_sub_typeno;
        query.execSelect(QString("select vch_sub_typename from cey_bt_dish_subtype where ch_sub_typeno = '%1' ").arg(ch_sub_typeno));
        query.next();
        int_indexbox_desc = query.record().value(0).toString();
        break;
    case 3://一菜一单3
        key += "_"+QString::number(idx);
        break;
    }

    if(keyIdxMap.find(key) == keyIdxMap.end()) {
        keyIdxMap.insert(key, idx);
    }

    return keyIdxMap.value(key);
}


//当有立刻有新的厨打单发送过来，判断是，当前菜品正在厨打

void w_bt_dish_kitchen_dish_thread_event::toExec()
{
    if(tablemodel.getPointer() == 0) {
        tablemodel.setPointer(new lds_model_sqltablemodel);
    }
    print_format.setPrintTranslate(f_get_sysparm("kt_comboBox_vch_language"), this);
    tablemodel.getPointer()->tablename_kvmap_clear_for_select_and_autoselect();
    print_open_fail_clear();

    for(int state =  w_bt_dish::KITCHEN_MIN_DEF; state < w_bt_dish::KITCHEN_MAX; state ++) {
        if(state == w_bt_dish::KITCHEN_FINISH) continue;
        if(state == w_bt_dish::KITCHEN_VOID) continue;

        {
            lds_query_db query(lds_thread::getThreadDbAndOpen());
            kitchen_key(query, w_bt_dish::k2o3_stae2hex_str(w_bt_dish::kitchenState(state)));
        }
        {
            //二维码点餐
            lds_query_db query(w_sys_manage_cloudsync_rb_order::create_database());
            kitchen_key(query, w_bt_dish::k2o3_stae2hex_str(w_bt_dish::kitchenState(state)));
        }
    }

    tablemodel.delPointer();
}

void w_bt_dish_kitchen_dish_thread_event::kitchen_key(lds_query_db &query, const QString &key)
{
    //
    if(!QSqlDatabase::connectionNames().contains("w_bt_dish_kitchen_dish_thread_event")){
        QSqlDatabase tmpdb = QSqlDatabase::addDatabase("QSQLITE", "w_bt_dish_kitchen_dish_thread_event");
        tmpdb.setDatabaseName(":memory:");
    }
    QSqlDatabase db_local = QSqlDatabase::database("w_bt_dish_kitchen_dish_thread_event");
    lds_query_db query_local(db_local);
    const w_bt_dish::kitchenState key_state = (w_bt_dish::kitchenState)key.toInt(0, 16);
    //
    query_local.execCreate("create table if not exists table_kitchen ("
                           "int_flowid int(11),"
                           "int_kitchen_section int(1),"//0 1 2 3 4 5 6
                           "int_print_id char(2) ,"
                           "vch_unitname char(60),"
                           "vch_dishname char(60),"
                           "vch_dishname_2 char(60),"

                           "ch_suitflag char(1),"
                           "num_num decimal(12, 2),"
                           "num_price decimal(12, 2),"
                           "int_indexbox int(3),"///0-999 用于区别打印顺序
                           "vch_indexbox_desc char(60),"//大类小类名称
                           "ch_printflag char(1),"

                           "ch_billno char(15),"
                           "ch_tableno  char(18),"//4 + 12 + 2
                           "dt_operdate datetime,"
                           "int_person int(11),"
                           "vch_operid char(6),"

                           "ch_presentflag char(1),"
                           "vch_print_memo	char(60),"
                           "vch_plan_id char(2),"
                           "num_price_add decimal(12, 2),"
                           "ch_waidai_flag char(1)"
                           ")");


    /// 打印机编号 cey_u_orderdish的厨打序号及时更新
    /// 一次只打印一个单号的
    ///最后排序已打印id(int_print_id)和分组序号已为标准(int_indexbox)\vch_indexbox_desc里记录大类等信息
    ///
    QStringList ch_billno_list;
    BoxPrintIndex indexbox;
    QString ch_areaNo_table;
    QString ch_tableno;

    query.execDelete("cey_u_orderdish_print_tr", " date(dt_operdate)  <>  " + query.curdatestr());
    if(w_bt_dish::KITCHEN_QUIT == key_state) {
        query.execSelect("select a.ch_billno FROM cey_u_orderdish a, cey_u_orderdish_log e        where a.int_flowid = e.int_orderflow   and e.vch_plan_k3o3_state <> '111111' and e.vch_plan_k3o3_state like '%"+key+"%' group by a.ch_billno;");
    } else {
        query.execSelect("select a.ch_billno FROM cey_u_orderdish a, cey_u_orderdish_print_tr e   where a.int_flowid = e.int_orderflow   and e.vch_plan_k3o3_state <> '111111' and e.vch_plan_k3o3_state like '%"+key+"%' group by a.ch_billno;");
    }
    while(query.next()) ch_billno_list << query.record().value(0).toString();

    foreach(const QString &ch_billno, ch_billno_list) {
        query_local.execDelete("table_kitchen");

        query.execSelect(QString("SELECT ch_tableno from cey_u_orderdish where ch_billno = '%1' ")
                         .arg(ch_billno));
        if(!query.next()) continue;

        ch_tableno = query.recordValue("ch_tableno").toString();
        ch_areaNo_table = f_get_from_cey_bt_table(ch_tableno, "ch_areano").toString();
        //快餐下，Q000， 中餐下ch_tableno
        if(w_bt_dish::KITCHEN_QUIT == key_state) {
            query.execSelect("select a.vch_plan_k3o3_id,a.ch_suitflag,a.int_flowid,a.num_num,a.num_back,a.num_price, a.ch_billno,a.ch_tableno,a.dt_operdate,a.vch_operid,a.ch_presentflag,a.ch_waidai_flag, "
                             /** 不同*/
                             "0-e.num_price_add as num_price_add, e.vch_print_memo, 0-e.num_back_cur as dish_num, "
                             "e.vch_plan_k3o3_state,  a.ch_dishno, a.ch_tableno  from cey_u_orderdish a  ,"
                             /** 不同*/
                             "cey_u_orderdish_log e         "
                             "where a.dt_operdate >= " + query.curdatestr()+
                             " and e.int_orderflow = a.int_flowid and e.vch_plan_k3o3_state like '%"+key+"%' and a.ch_billno = '"+ch_billno+"' ;");

        } else {
            query.execSelect("select a.vch_plan_k3o3_id,a.ch_suitflag,a.int_flowid,a.num_num,a.num_back,a.num_price, a.ch_billno,a.ch_tableno,a.dt_operdate,a.vch_operid,a.ch_presentflag, a.ch_waidai_flag, "
                             /** 不同*/
                             "a.num_price_add,a.vch_print_memo, a.num_num - a.num_back as dish_num,"

                             "e.vch_plan_k3o3_state,  a.ch_dishno, a.ch_tableno  from cey_u_orderdish a  ,"
                             /** 不同*/
                             "cey_u_orderdish_print_tr e  "
                             "where a.dt_operdate >= " + query.curdatestr()+
                             " and e.int_orderflow = a.int_flowid and e.vch_plan_k3o3_state like '%"+key+"%' and a.ch_billno = '"+ch_billno+"' ;");
        }

        //
        while(query.next()) {
            QString vch_plan_k3o3_state = query.record().value("vch_plan_k3o3_state").toString();
            QString vch_plan_k3o3_id = query.record().value("vch_plan_k3o3_id").toString();
            QString ch_dishno = query.record().value("ch_dishno").toString();
            QString int_flowid = query.record().value("int_flowid").toString();
            QString ch_suitflag = query.record().value("ch_suitflag").toString();
            if(query.recordValue("dish_num").toDouble() == 0 ) continue;

            //有6列， 每列的符合key的完成厨打
            //判断本intflow是否有数据需要打印
            for(int index = 0, count = vch_plan_k3o3_state.count(); index < count; index++) {
                //2是未配置厨打， 1已经厨打完毕，0未厨打
                if(vch_plan_k3o3_state.mid(index, 1) == key) {//未厨打
                    QString vch_plan_id = vch_plan_k3o3_id.mid(index*2, 2);
                    if(vch_plan_id.toInt() >= 10) {//厨打方案的编号不能小于10
                        QVariant int_suit_type = f_get_from_cey_bt_kitchen_plan(vch_plan_id, "int_suit_type");
                        if(false == int_suit_type.isValid()) continue;//没有找到用方案
                        QString ch_printflag = "Y";
                        switch(int_suit_type.toInt()) {//打印在一起0 只打印主套菜1 只打印套菜明细2
                        case 0://打印在一起0
                            break;
                        case 1://只打印主套菜1
                            if(ch_suitflag == "Y") {//子套菜不打
                                ch_printflag = "N";
                            }
                            break;
                        case 2://只打印套菜明细2
                            if(ch_suitflag == "P") {//主套菜不打
                                ch_printflag = "N";
                            }
                            break;
                        }
                        QString vch_indexbox_desc;
                        int int_indexbox = indexbox.getIndex(ch_billno,
                                                             f_get_from_cey_bt_kitchen_plan(vch_plan_id, "int_print_id").toString(),
                                                             vch_plan_id,
                                                             f_get_from_cey_bt_kitchen_plan(vch_plan_id, "int_dish_type").toInt(),
                                                             f_get_from_cey_bt_dish(ch_dishno, "ch_dish_typeno").toString(),
                                                             f_get_from_cey_bt_dish(ch_dishno, "ch_sub_typeno").toString(),
                                                             vch_indexbox_desc);
                        query_local.execInsert("table_kitchen", qrtVariantPairList()
                                               << qrtVariantPair("int_flowid", int_flowid)
                                               << qrtVariantPair("int_kitchen_section", index)
                                               << qrtVariantPair("int_print_id", f_get_from_cey_bt_kitchen_plan(vch_plan_id, "int_print_id"))
                                               << qrtVariantPair("vch_unitname", f_get_from_cey_bt_unit(f_get_from_cey_bt_dish(ch_dishno, "ch_unitno").toString(), "vch_unitname"))
                                               << qrtVariantPair("vch_dishname", f_get_from_cey_bt_dish(ch_dishno, "vch_dishname"))

                                               << qrtVariantPair("vch_dishname_2", f_get_from_cey_bt_dish(ch_dishno, "vch_dishname_2"))
                                               << qrtVariantPair("ch_suitflag", query.record().value("ch_suitflag"))
                                               << qrtVariantPair("num_num", query.record().value("dish_num"))
                                               << qrtVariantPair("num_price", query.record().value("num_price"))
                                               << qrtVariantPair("int_indexbox", int_indexbox)

                                               << qrtVariantPair("vch_indexbox_desc", vch_indexbox_desc)
                                               << qrtVariantPair("ch_printflag", ch_printflag)
                                               << qrtVariantPair("ch_billno", ch_billno)
                                               << qrtVariantPair("ch_tableno", ch_tableno)
                                               << qrtVariantPair("dt_operdate", query.record().value("dt_operdate"))

                                               << qrtVariantPair("int_person", f_get_from_cey_u_table(ch_billno, "int_person"))
                                               << qrtVariantPair("vch_operid", query.record().value("vch_operid"))
                                               << qrtVariantPair("ch_presentflag", query.record().value("ch_presentflag"))
                                               << qrtVariantPair("vch_print_memo", fexpandmain_model_sqltablemodel::get_desc_vch_print_memo(query.record().value("vch_print_memo").toString()))
                                               << qrtVariantPair("vch_plan_id", vch_plan_id)

                                               << qrtVariantPair("num_price_add", query.record().value("num_price_add"))
                                               << qrtVariantPair("ch_waidai_flag", query.record().value("ch_waidai_flag")));
                    }
                }
            }
        }//空的数据直接付111111
        query_local.execSelect("select int_flowid from table_kitchen ");
        if(!query_local.next()) {
            kitchen_update_transaction(query, ch_billno, key_state);
            continue;
        }

        //        {
        //                        int y = 0;
        //                        query_local.execSelect("select * from table_kitchen order by int_indexbox asc");
        //                        int max = query_local.recordCount();

        //                        QString str;
        //                        str += query_local.record().fieldName(y++)+ "\t" + query_local.record().fieldName(y++)+ "\t" + query_local.record().fieldName(y++)+ "\t" + query_local.record().fieldName(y++)+ "\t" + query_local.record().fieldName(y++)+ "\t";
        //                        str += query_local.record().fieldName(y++)+ "\t" + query_local.record().fieldName(y++)+ "\t" + query_local.record().fieldName(y++)+ "\t" + query_local.record().fieldName(y++)+ "\t" + query_local.record().fieldName(y++)+ "\t";
        //                        str += query_local.record().fieldName(y++)+ "\t" + query_local.record().fieldName(y++)+ "\t" + query_local.record().fieldName(y++)+ "\t" + query_local.record().fieldName(y++)+ "\t" + query_local.record().fieldName(y++)+ "\t";
        //                        str += query_local.record().fieldName(y++)+ "\t" + query_local.record().fieldName(y++)+ "\t" +query_local.record().fieldName(y++)+ "\t";
        //                        while(query_local.next()) {
        //                            y = 0;

        //                            str += query_local.record().value(y++).toString()+ "\t" + query_local.record().value(y++).toString()+ "\t" + query_local.record().value(y++).toString()+ "\t" + query_local.record().value(y++).toString()+ "\t" + query_local.record().value(y++).toString()+ "\t"
        //                                    + query_local.record().value(y++).toString()+ "\t" + query_local.record().value(y++).toString()+ "\t" + query_local.record().value(y++).toString()+ "\t" + query_local.record().value(y++).toString()+ "\t" + query_local.record().value(y++).toString()+ "\t"
        //                                    + query_local.record().value(y++).toString()+ "\t" + query_local.record().value(y++).toString()+ "\t" + query_local.record().value(y++).toString()+ "\t" + query_local.record().value(y++).toString()+ "\t" + query_local.record().value(y++).toString()+ "\t"
        //                                    + query_local.record().value(y++).toString()+ "\t" + query_local.record().value(y++).toString()+ "\t" +query_local.record().value(y++).toString()+ "\t";
        //                        }
        //        }

        for(int x = 0; x < 6; x ++) {
            QList<int> int_indexbox_list;
            query_local.execSelect(QString("select int_indexbox  from table_kitchen where int_kitchen_section = %1  group by int_indexbox ").arg(x));//打印盒子， 检索全部
            while(query_local.next())
            {
                int_indexbox_list << query_local.record().value("int_indexbox").toInt();
            }

            //先将state更新下
            //100000100000 888 888 - > 800 800

            foreach(int int_indexbox , int_indexbox_list) {
                ///指令即 若是flag <> Y则打印的内容为空格，但依旧参与打印
                query_local.execSelect(QString("select vch_plan_id from table_kitchen where int_kitchen_section = %1 and int_indexbox = %2  " ).arg(x).arg(int_indexbox));//检测全部
                query_local.next();
                QString vch_plan_id = query_local.record().value("vch_plan_id").toString();

                const int int_print_count = f_get_from_cey_bt_kitchen_plan(vch_plan_id, "int_print_count").toInt();
                const QString ch_print_id_second_use = f_get_from_cey_bt_kitchen_plan(vch_plan_id, "ch_print_id_second_use").toString();
                const QString ch_check_state = f_get_from_cey_bt_kitchen_plan(vch_plan_id, "ch_check_state").toString();
                QString int_print_id = f_get_from_cey_bt_kitchen_plan(vch_plan_id, "int_print_id").toString();
                QString int_print_id_second = f_get_from_cey_bt_kitchen_plan(vch_plan_id, "int_print_id_second").toString();

                if(ch_print_id_second_use != "Y") {
                    int_print_id_second = "";
                }

                int vch_plan_id_TRY_count = 0;
int_print_id_TRY://vch_plan_id

                if(vch_plan_id_TRY_count == 2) continue;//如果已经尝试一次则进入下移循环
                {
                    vch_plan_id_TRY_count++;
                    const QString ch_areaNo_print = f_get_from_cey_bt_kitchen_print(int_print_id, "ch_areaNo").toString();
                    const QString int_id_print = int_print_id;
                    if(!ch_areaNo_print.isEmpty()
                            && !ch_areaNo_table.isEmpty()
                            && ch_areaNo_table != ch_areaNo_print) {// 如果打印机的区域号不为空、打印机的区域号和餐桌区域号不匹配则跳过                                    if(!kitchen_update_transaction(query_local, query, x, int_indexbox, key_state)) {
                        if(!kitchen_update_transaction(query_local, query, x, int_indexbox, key_state)) {

                        }
                        QMetaObject::invokeMethod(public_sql::save_login_object(),
                                                  "showprinterr",
                                                  Qt::QueuedConnection,
                                                  Q_ARG(QString, tr("未在餐桌所在区域的单据已经被忽略")),
                                                  Q_ARG(QString, tr("区域厨打"))
                                                  );
                        continue;
                    }

                    print_format.clear();
                    kitchen_make_ins(key_state, print_format, query_local, x, int_indexbox, vch_plan_id_TRY_count);

                    QString errstring;
                    if(!print_open_fail_exist(print_format.print_path)) {//print_open_fail_exist
                        ///打印机判断
                        Printer_POS *printer = CommonHeader::getposprint_bypath(print_format.print_path);// getprinterBypath(ipport);
                        Printer_POS_auto_destroy pbox(printer);
                        ///~打印机判断
                        if(pbox.printer){//pbox.printer
                            if(pbox.printer->tryOpen(print_format.print_path)){
                                //"------------厨打" << "判断打印机状态" << __LINE__;
                                bool gonext = false;
                                QByteArray v;
                                if(ch_check_state == "Y") {
                                    pbox.printer->writeHex("100402");
                                    errstring = tr("无应答");
                                    if(pbox.printer->waitForReadyReadv(1000)) {
                                        v = pbox.printer->readAll();
                                        gonext = true;
                                    }
                                }
                                if(ch_check_state != "Y") {
                                    v.append(0x12);
                                    gonext = true;
                                }
                                if(gonext){
                                    if(v.endsWith(0x12)){//0001 0010
                                        login_emit_signal_name_id_state(f_get_from_cey_bt_kitchen_plan(vch_plan_id, "int_print_id").toString(), 0x12);

                                        //打印成功
                                        print_format.resolve();

                                        Printer_Normal::toprint(pbox.printer,print_format.print_ins, int_print_count);

                                        QByteArray d;
                                        d += pbox.printer->port() + "\n";
                                        foreach(const pieceIns &o, print_format.print_ins){
                                            d += o.second + "\n";
                                        }
                                        if(!kitchen_update_transaction(query_local, query, x, int_indexbox, key_state)) {
                                            goto indexbox_END;
                                        }
                                        continue;
                                    } else if(v.endsWith(0x16)){//0001 0110
                                        login_emit_signal_name_id_state(f_get_from_cey_bt_kitchen_plan(vch_plan_id, "int_print_id").toString(), 0x16);
                                        errstring = tr("打印机盖开启");
                                    } else  if(v.endsWith(0x32)){//0011 0010
                                        login_emit_signal_name_id_state(f_get_from_cey_bt_kitchen_plan(vch_plan_id, "int_print_id").toString(), 0x32);
                                        errstring = tr("打印机缺纸");
                                    } else {
                                        login_emit_signal_name_id_state(f_get_from_cey_bt_kitchen_plan(vch_plan_id, "int_print_id").toString(), 0x10);//打印机故障
                                    }

                                    //"厨打间隔-开始";
                                    lds_thread::secSleep(f_get_sysparm("kt_spinBox_vtimeout", "5").toInt());
                                    //~"厨打间隔-结束";
                                }
                            } else {//tryOpen
                                errstring = pbox.printer->lastError();
                                print_open_fail_insert(print_format.print_path);
                                login_emit_signal_name_id_state(f_get_from_cey_bt_kitchen_plan(vch_plan_id, "int_print_id").toString(), 0x00);//无响应
                            }//tryOpen
                            // "------------厨打" << "打印结束" << __LINE__;
                        }//pbox.printer
                        if(errstring.isEmpty()) errstring = tr("无响应");
indexbox_END:
                        if(!print_format.print_path.isEmpty()) {
                            QMetaObject::invokeMethod(public_sql::save_login_object(),
                                                      "showprinterr",
                                                      Qt::QueuedConnection,
                                                      Q_ARG(QString, errstring),
                                                      Q_ARG(QString, print_format.print_path+tr("厨打错误")),
                                                      Q_ARG(int, public_sql::error_kitchen),
                                                      Q_ARG(QString, int_id_print)
                                                      );
                        }
                    }//print_open_fail_exist
                    // "------------厨打" << "若是有备打打印机方案" << __LINE__;
                    if(!int_print_id_second.isEmpty()) {//若是有备打打印机方案
                        int_print_id = int_print_id_second;
                        int_print_id_second.clear();
                        goto int_print_id_TRY;
                    }
                }//int_print_id_TRY
            }//for indexbox
        }//for(0 - 5
    }//foreach(const QString &ch_billno
    //
    // "厨打结束";
}

bool w_bt_dish_kitchen_dish_thread_event::kitchen_update_transaction(lds_query_db &query_local, lds_query_db &query, int int_kitchen_section, int int_indexbox, int key_state)
{
    // "------------厨打:"<<__LINE__ ;
    // __LINE__ << "transaction";
    if("rb_order" == query.db.connectionName()) {
        return true;
    }

    QString vch_plan_k3o3_state ;
    QString int_flowid;
    QString tablename;
    errstring = tr("数据库错误");
    query.db.transaction();
    query_local.execSelect(QString("select int_flowid  from table_kitchen where int_kitchen_section = %1 and int_indexbox = '%2'  ")
                           .arg(int_kitchen_section).arg(int_indexbox));//更新部分，更新所有
    while(query_local.next()) {
        if(w_bt_dish::KITCHEN_QUIT == key_state) {
            tablename = "cey_u_orderdish_log";
        } else {
            tablename = "cey_u_orderdish_print_tr";
        }

        int_flowid = query_local.record().value("int_flowid").toString();
        query.execSelect(QString("SELECT vch_plan_k3o3_state from  "+tablename+" where int_orderflow = '%1'  ")
                         .arg(int_flowid));
        query.next();
        vch_plan_k3o3_state = query.record().value("vch_plan_k3o3_state").toString();

        if(query.execUpdate(tablename,
                            "vch_plan_k3o3_state", getSqlReplaceIndex(vch_plan_k3o3_state, int_kitchen_section, w_bt_dish::k2o3_state2hex_char(w_bt_dish::KITCHEN_FINISH)),
                            qrtEqual("int_orderflow", int_flowid))){
            continue;
        }
        query.db.rollback();
        return false;
    }
    query.execDelete("cey_u_orderdish_print_tr", " date(dt_operdate)  <>  " + query.curdatestr());
    query.db.commit();
    return true;
}

bool w_bt_dish_kitchen_dish_thread_event::kitchen_update_transaction(lds_query_db &query, const QString &ch_billno, int key_state)
{
    if("rb_order" == query.db.connectionName()) {
        return true;
    }
    query.db.transaction();
    QString tablename;
    if(w_bt_dish::KITCHEN_QUIT == key_state) {
        tablename = "cey_u_orderdish_log";
    } else {
        tablename = "cey_u_orderdish_print_tr";
    }

    if(!query.execUpdate(tablename,
                         QStringList() << "int_orderflow",
                         QString("select b.int_orderflow, '111111' as `vch_plan_k3o3_state` from "
                                 "cey_u_orderdish a, " +tablename+ " b where "
                                 "a.int_flowID = b.int_orderflow and a.ch_billno = '%1' ")
                         .arg(ch_billno))) {
        query.db.rollback();
        return false;
    }
    query.db.commit();
    return true;
}

QString w_bt_dish_kitchen_dish_thread_event::f_get_sysparm(const QString &as_parameter, const QVariant &as_default)
{
    QVariant d = tablemodel.getPointer()->tablename_kvmap_value("cey_sys_parameter", as_parameter, "vch_parameter", "vch_value", "", as_default);
    return d.toString();
}

QString w_bt_dish_kitchen_dish_thread_event::f_get_vch_company()
{
    lds_query query;
    query.execSelect("select vch_company from cey_sys_company;");
    query.next();
    return query.record().value(0).toString();
}


QVariant w_bt_dish_kitchen_dish_thread_event::f_get_serial_no_cur(const QString &ch_billno)
{
    QString serial_no_cur = tablemodel.getPointer()->tablename_kvmap_value("cey_u_master", ch_billno, "ch_billno", "vch_memo").toString();
    if(serial_no_cur.isEmpty()) {//默认
        serial_no_cur = QString().sprintf("%03d", ch_billno.right(5).toInt());
    }
    serial_no_cur = QString().sprintf("%03d", serial_no_cur.toInt());
    return serial_no_cur;
}

bool w_bt_dish_kitchen_dish_thread_event::print_open_fail_exist(const QString &ip)
{
    return tablemodel.getPointer()->tablename_kvmap_value_direct("print_open_fail_exist", ip).isValid();
}

void  w_bt_dish_kitchen_dish_thread_event::print_open_fail_insert(const QString &ip)
{
    tablemodel.getPointer()->tablename_kvmap_insert("print_open_fail_exist", ip, "");
}

void w_bt_dish_kitchen_dish_thread_event::print_open_fail_clear()
{
    tablemodel.getPointer()->tablename_kvmap_clear("print_open_fail_exist");
}

QVariant w_bt_dish_kitchen_dish_thread_event::f_get_vch_tablename(const QString &ch_tableno)
{
    if("Q000" == ch_tableno) {
        return tr("快餐");
    }

    return f_get_from_cey_bt_table(ch_tableno, "vch_tablename");
}


/////////////
QVariant w_bt_dish_kitchen_dish_thread_event::f_get_from_cey_bt_kitchen_print(const QString &int_id, const QString &valuename)
{
    return tablemodel.getPointer()->tablename_kvmap_value("cey_bt_kitchen_print", int_id, "int_id", valuename);
}

QVariant w_bt_dish_kitchen_dish_thread_event::f_get_from_cey_u_table(const QString &ch_billno, const QString &valuename)
{
    return tablemodel.getPointer()->tablename_kvmap_value("cey_u_table", ch_billno, "ch_billno", valuename);
}

QVariant w_bt_dish_kitchen_dish_thread_event::f_get_from_cey_bt_dish(const QString &ch_dishno, const QString &valuename)
{
    return tablemodel.getPointer()->tablename_kvmap_value("cey_bt_dish", ch_dishno, "ch_dishno", valuename);
}

QVariant w_bt_dish_kitchen_dish_thread_event::f_get_from_cey_bt_unit(const QString &ch_unitno, const QString &valuename)
{
    return tablemodel.getPointer()->tablename_kvmap_value("cey_bt_unit", ch_unitno, "ch_unitno", valuename);
}

QVariant w_bt_dish_kitchen_dish_thread_event::f_get_from_cey_bt_table(const QString &ch_tableno, const QString &valuename)
{
    return tablemodel.getPointer()->tablename_kvmap_value("cey_bt_table", ch_tableno, "ch_tableno", valuename);
}

QVariant w_bt_dish_kitchen_dish_thread_event::f_get_from_cey_bt_kitchen_plan(const QString &vch_plan_id, const QString &valuename)
{
    return tablemodel.getPointer()->tablename_kvmap_value("cey_bt_kitchen_plan", vch_plan_id, "vch_plan_id", valuename);
}

void w_bt_dish_kitchen_dish_thread_event::login_emit_signal_name_id_state(const QString &int_id, uchar state)
{
    QMetaObject::invokeMethod(public_sql::save_login_object(),
                              "signal_kitchen_print_state",
                              Qt::QueuedConnection,
                              Q_ARG(QString, int_id),
                              Q_ARG(uchar, state)
                              );
}

void w_bt_dish_kitchen_dish_thread_event::kitchen_make_ins(const w_bt_dish::kitchenState key_state, public_ins_maker &print_format, lds_query_db &query_local, int x, int int_indexbox, int vch_plan_id_TRY_count)
{
    lds_query query;
    QByteArray bp_content_len = "s"+f_get_sysparm("kt_lineEdit_content_scale", "00").toLocal8Bit();
    int bp_content_len_scale_h = 1;
    int s0 = bp_content_len.mid(1,1).toInt();//s00
    if(s0 != 0){
        bp_content_len_scale_h = s0 + 1;
    }

    int bp_price_dec = f_get_sysparm("bp_fmt_price_dec").toInt();
    int bp_num_dec = f_get_sysparm("bp_fmt_num_dec").toInt();
    int kt_spinBox_vch_dishname_width = f_get_sysparm("kt_spinBox_vch_dishname_width").toInt() / bp_content_len_scale_h;
    int kt_spinBox_num_price_width = f_get_sysparm("kt_spinBox_num_price_width").toInt() / bp_content_len_scale_h;
    int kt_spinBox_num_num_width = f_get_sysparm("kt_spinBox_num_num_width").toInt() / bp_content_len_scale_h;
    int kt_spinBox_unit_width = f_get_sysparm("kt_spinBox_unit_width").toInt() / bp_content_len_scale_h;

    int kt_checkBox_ch_tailtotal  = f_get_sysparm("kt_checkBox_ch_tailtotal").toInt();
    int kt_checkBox_ch_rowno  = f_get_sysparm("kt_checkBox_ch_rowno").toInt();
    int kt_checkBox_ktitle2shopname  = f_get_sysparm("kt_checkBox_ktitle2shopname").toInt();
    int kt_rowno;

    // "------------厨打:"<<__LINE__ ;
    query_local.execSelect(QString("select * from table_kitchen where int_kitchen_section = %1 and int_indexbox = %2 and ch_printflag ='Y' and num_num <> 0 " ).arg(x).arg(int_indexbox));//打印部分,只打印ch_printflag=‘Y’的部分
    if(query_local.next()) {
        QString vch_indexbox_desc = query_local.record().value("vch_indexbox_desc").toString();
        QString vch_plan_id = query_local.record().value("vch_plan_id").toString();
        QString ch_billno = query_local.record().value("ch_billno").toString();
        QString int_print_id = f_get_from_cey_bt_kitchen_plan(vch_plan_id, "int_print_id").toString();
        const int int_dish_type = f_get_from_cey_bt_kitchen_plan(vch_plan_id, "int_dish_type").toInt();
        int int_tail_block = f_get_from_cey_bt_kitchen_plan(vch_plan_id, "int_tail_block").toInt();
        int int_head_block = f_get_from_cey_bt_kitchen_plan(vch_plan_id, "int_head_block").toInt();
        QString kt_fmt_serial_no_desc = f_get_sysparm("kt_fmt_serial_no_desc", print_format.printTr(QT_TR_NOOP("流水号")));
        QString vch_dishname_No = f_get_from_cey_bt_kitchen_plan(vch_plan_id, "vch_dishname_No").toString();

        QString ch_tableno = query_local.record().value("ch_tableno").toString();
        QString vch_tablename = f_get_from_cey_bt_table(ch_tableno, "vch_tablename").toString();

        QString vch_operid = f_get_from_cey_u_table(ch_billno, "vch_operid").toString();
        QString vch_operator= tablemodel.getPointer()->tablename_kvmap_value("("+backheader::SELECT_OPERID_NAME +")  b", vch_operid, "vch_operid", "vch_operator").toString();

        print_format.paperwidth = f_get_from_cey_bt_kitchen_print(int_print_id, "int_paperwidth").toInt();
        print_format.print_path = f_get_from_cey_bt_kitchen_print(int_print_id, "vch_printip").toString();

        int bp_beginins = print_format.paperwidth /16;

        print_format.drawText("a10,s00");
        if(f_get_sysparm("kt_checkBox_vch_plan_id") == "1") {
            print_format.drawText("", print_format.printTr(QT_TR_NOOP("厨打方案编号")) + vch_plan_id);
            print_format.drawN();
        }
        if(vch_plan_id_TRY_count == 2){
            print_format.drawText("", print_format.printTr(QT_TR_NOOP("备打打印机编号")) + f_get_from_cey_bt_kitchen_plan(vch_plan_id, "int_print_id_second").toString());
            print_format.drawN();
        }

        //只是修改title
        if(x >= 0 && x <= 2){
            if(kt_checkBox_ktitle2shopname == 1) {
                print_format.drawText("a00,s11", f_get_vch_company());
            } else {
                print_format.drawText("a00,s11", print_format.printTr(QT_TR_NOOP("厨打单")));
            }
        }
        if(x >= 3 && x <= 5) {
            print_format.drawText("a00,s11", print_format.printTr(QT_TR_NOOP("出品单")));
        }

        if(w_bt_dish::KITCHEN_QUIT <= key_state && key_state <= w_bt_dish::KITCHEN_MAX){
            print_format.drawText("", "/"+w_bt_dish::k2o3_name(key_state));
        }
        if(lds::get_soft_curent_language_first() == "EN" && int_dish_type == 0) {//一桌一单
            print_format.drawN();
            print_format.drawText("a00", print_format.printTr(QT_TR_NOOP("汇总"))+"\n");
        }
        //添加title、说明
        query.execSelect(QString("select vch_plan_desc from cey_u_orderdish where int_flowID = %1  ").arg(query_local.record().value("int_flowid").toString()));
        query.next();
        QString vch_plan_desc = query.record().value("vch_plan_desc").toString();
        if(w_bt_dish::KITCHEN_ZHUANTAI == key_state) {
            if(!vch_plan_desc.isEmpty()) {
                print_format.drawN();
                print_format.drawText("a10,s00");
                print_format.drawText("", vch_plan_desc);
            }
        }
        if(w_bt_dish::KITCHEN_FUZHI == key_state) {
            if(!vch_plan_desc.isEmpty()) {
                print_format.drawN();
                print_format.drawText("a10,s00");
                print_format.drawText("", vch_plan_desc);
            }
        }
        if(w_bt_dish::KITCHEN_BINGTAI == key_state) {
            if(!vch_plan_desc.isEmpty()) {
                print_format.drawN();
                print_format.drawText("a10,s00");
                print_format.drawText("", vch_plan_desc);
            }
        }
        if(w_bt_dish::KITCHEN_ZHUANTAI_CAIPIN == key_state) {
            if(!vch_plan_desc.isEmpty()) {
                print_format.drawN();
                print_format.drawText("a10,s00");
                print_format.drawText("", vch_plan_desc);
            }
        }
        ///开始打印
        print_format.drawN();

        print_format.drawText("a10,s00");
        print_format.drawText("a10");
        print_format.begin_column_print(bp_beginins);
        if(f_get_sysparm("kt_checkBox_ch_tableno") == "1") print_format.drawText("s" + f_get_sysparm("kt_lineEdit_ch_tableno_scale", "00").toLocal8Bit(), print_format.printTr(QT_TR_NOOP("餐桌")) + ":" + get_bp_fmt_table(vch_tablename, ch_tableno));//query_local.record().value("ch_tableno").toString());
        if(f_get_sysparm("kt_checkBox_ch_serialno") == "1") print_format.drawText("s" + f_get_sysparm("kt_lineEdit_ch_serialno_scale", "00").toLocal8Bit(), kt_fmt_serial_no_desc + ":" + f_get_serial_no_cur(ch_billno).toString());
        if(f_get_sysparm("kt_checkBox_ch_billno") == "1") print_format.drawText("s" + f_get_sysparm("kt_lineEdit_ch_billno_scale", "00").toLocal8Bit(), print_format.printTr(QT_TR_NOOP("单号")) + ":" + ch_billno);
        if(f_get_sysparm("kt_checkBox_vch_operid") == "1") print_format.drawText("s" + f_get_sysparm("kt_lineEdit_vch_operid_scale", "00").toLocal8Bit(), print_format.printTr(QT_TR_NOOP("点菜人")) + ":" + get_bp_fmt_operid(vch_operator, vch_operid));

        if(f_get_sysparm("kt_checkBox_dt_operdate") == "1") print_format.drawText("s" + f_get_sysparm("kt_lineEdit_dt_operdate_scale", "00").toLocal8Bit(), print_format.printTr(QT_TR_NOOP("点菜时间")) + ":" + query_local.record().value("dt_operdate").toDateTime().toString("yyyy-MM-dd hh:mm:ss"));
        if(f_get_sysparm("kt_checkBox_dt_cur") == "1") print_format.drawText("s" + f_get_sysparm("kt_lineEdit_dt_cur_scale", "00").toLocal8Bit(), print_format.printTr(QT_TR_NOOP("打印时间")) + ":" + n_func::f_get_sysdatetime_str());
        if(f_get_sysparm("kt_checkBox_int_person") == "1") print_format.drawText("s" + f_get_sysparm("kt_lineEdit_int_person_scale", "00").toLocal8Bit(), print_format.printTr(QT_TR_NOOP("人数")) + ":" + query_local.record().value("int_person").toString());

        print_format.end_column_print();
        print_format.drawN();

        // "------------厨打:"<<__LINE__ ;
        query_local.execSelect(QString(
                                   " select"
                                   "  num_num"
                                   ", num_price"
                                   ", num_price_add"
                                   ", ch_presentflag"
                                   ", ch_waidai_flag"
                                   ", vch_print_memo"
                                   ", int_flowid"
                                   ", vch_unitname"
                                   ", ch_suitflag"
                                   ", vch_dishname"
                                   ", vch_dishname_2"
                                   "  from table_kitchen  where int_kitchen_section = %1 and int_indexbox = %2 and ch_printflag ='Y' and num_num <> 0 ")
                               //ch_printflag若是设置了套菜厨打方案，起作用
                               .arg(x).arg(int_indexbox)
                               );
        bool isheader = true;
        float total = 0;
        print_format.drawText("a10,s00");
        print_format.drawText("-");
        print_format.drawN();
        kt_rowno = 0;

        while(query_local.next()) {///while
            QString ch_presentflag  ;
            QString ch_waidai_flag  ;
            QString vch_print_memo ;
            QString num_num;
            QString unit;
            QString num_price ;
            QString num_price_add ;
            QString vch_dishname;
            QString ch_suitflag;
            QString int_flowid = query_local.record().value("int_flowid").toString();
            QString preapp;
BILL_CMB_HEADER:
            if(isheader) {
                ch_presentflag = "";
                ch_waidai_flag = "";
                vch_print_memo = "";
                num_num = print_format.printTr(QT_TR_NOOP("数量"));
                unit = print_format.printTr(QT_TR_NOOP("单位"));
                num_price = print_format.printTr(QT_TR_NOOP("单价"));
                num_price_add = "";
                vch_dishname = print_format.printTr(QT_TR_NOOP("品名"));
            } else {
                ch_presentflag  = query_local.record().value("ch_presentflag").toString();
                ch_waidai_flag  = query_local.record().value("ch_waidai_flag").toString();
                vch_print_memo  = query_local.record().value("vch_print_memo").toString();
                num_num  = query_local.record().value("num_num").toString();
                unit  = query_local.record().value("vch_unitname").toString();
                num_price  = query_local.record().value("num_price").toString();
                num_price_add  = query_local.record().value("num_price_add").toString();
                ch_suitflag  = query_local.record().value("ch_suitflag").toString();

                //菜品名称
                if(vch_dishname_No != "vch_dishname"
                        && vch_dishname_No != "vch_dishname_2") {
                    vch_dishname_No = "vch_dishname";
                }
                vch_dishname  = query_local.record().value(vch_dishname_No).toString();
                if(ch_suitflag == "P") {
                    vch_dishname = print_format.printTr(QT_TR_NOOP("套"))+")"+vch_dishname;
                }
                if(ch_suitflag == "Y") {
                    vch_dishname = "   "+vch_dishname;
                }
                //~菜品名称
            }

            if(!isheader && kt_checkBox_ch_rowno == 1) {
                kt_rowno ++;
                query.execSelect(QString("select int_id from cey_u_orderdish where int_flowID = %1  ").arg(int_flowid));
                query.next();
                preapp += QString("%1.").arg(query.record().value("int_id").toString());
            }
            if(ch_presentflag == "Y") {
                preapp += print_format.printTr(QT_TR_NOOP("赠"))+")";
            }
            if(ch_waidai_flag == "Y") {
                preapp += print_format.printTr(QT_TR_NOOP("外带"))+")";
            }
            if(isheader) {
            } else {
                num_num =  QString().sprintf((char*)((QByteArray("%.")+ QByteArray::number(bp_num_dec)+"f").data()), num_num.toFloat());
                num_price =  QString().sprintf((char*)((QByteArray("%.")+ QByteArray::number(bp_price_dec)+"f").data()), num_price.toFloat());
            }
            if(ch_presentflag != "Y") {
                total += num_num.toFloat() * num_price.toFloat() + num_price_add.toFloat();
            }

            if(isheader&&f_get_sysparm("kt_checkBox_title") != "1") {//打印表头，并且不启用打印
                isheader = false;
                goto BILL_CMB_HEADER;
            }
            // "num_num" << num_num;
            if(f_get_sysparm("kt_spinBox_vch_dishname_row", "1") == "1" && f_get_sysparm("kt_checkBox_vch_dishname") == "1")print_format.drawText(bp_content_len, public_printby_ch_billno::getfixedlen(preapp + vch_dishname , kt_spinBox_vch_dishname_width, "0"), kt_spinBox_vch_dishname_width, Qt::AlignLeft);
            if(f_get_sysparm("kt_spinBox_num_num_row", "1") == "1" && f_get_sysparm("kt_checkBox_num_num") == "1")print_format.drawText(bp_content_len, public_printby_ch_billno::getfixedlen(num_num , kt_spinBox_num_num_width, "0"), kt_spinBox_num_num_width, Qt::AlignRight);
            if(f_get_sysparm("kt_spinBox_unit_row", "1") == "1" && f_get_sysparm("kt_checkBox_unit") == "1")print_format.drawText(bp_content_len, public_printby_ch_billno::getfixedlen(unit , kt_spinBox_unit_width, "0"), kt_spinBox_unit_width, Qt::AlignRight);
            if(f_get_sysparm("kt_spinBox_num_price_row", "1") == "1" && f_get_sysparm("kt_checkBox_num_price") == "1")print_format.drawText(bp_content_len, public_printby_ch_billno::getfixedlen(num_price , kt_spinBox_num_price_width, "0"), kt_spinBox_num_price_width, Qt::AlignRight);
            print_format.drawN();
            if(f_get_sysparm("kt_spinBox_vch_dishname_row", "1") == "2" && f_get_sysparm("kt_checkBox_vch_dishname") == "1")print_format.drawText(bp_content_len, public_printby_ch_billno::getfixedlen(preapp + vch_dishname , kt_spinBox_vch_dishname_width, "0"), kt_spinBox_vch_dishname_width, Qt::AlignLeft);
            if(f_get_sysparm("kt_spinBox_num_num_row", "1") == "2" && f_get_sysparm("kt_checkBox_num_num") == "1")print_format.drawText(bp_content_len, public_printby_ch_billno::getfixedlen(num_num , kt_spinBox_num_num_width, "0"), kt_spinBox_num_num_width, Qt::AlignRight);
            if(f_get_sysparm("kt_spinBox_unit_row", "1") == "2" && f_get_sysparm("kt_checkBox_unit") == "1")print_format.drawText(bp_content_len, public_printby_ch_billno::getfixedlen(unit , kt_spinBox_unit_width, "0"), kt_spinBox_unit_width, Qt::AlignRight);
            if(f_get_sysparm("kt_spinBox_num_price_row", "1") == "2" && f_get_sysparm("kt_checkBox_num_price") == "1")print_format.drawText(bp_content_len, public_printby_ch_billno::getfixedlen(num_price , kt_spinBox_num_price_width, "0"), kt_spinBox_num_price_width, Qt::AlignRight);
            print_format.drawN();

            ///做法默认添加
            if(!vch_print_memo.isEmpty())print_format.drawText(bp_content_len, print_format.printTr(QT_TR_NOOP("做法"))+":"+vch_print_memo);
            print_format.drawN();

            if(isheader) {
                print_format.drawText("a10,s00");
                print_format.drawText("-");
                print_format.drawN();
                if(!vch_indexbox_desc.isEmpty()){
                    print_format.drawText("a00", "---"+vch_indexbox_desc.toLocal8Bit()+"---");
                    print_format.drawN();
                    print_format.drawText("a10");
                }
                isheader = false;
                goto BILL_CMB_HEADER;
            }
        } //while
        // "------------厨打:" << __LINE__;
        print_format.drawN();
        if(kt_checkBox_ch_tailtotal == 1) {
            print_format.drawText("a10,s00");
            print_format.drawText("-");
            print_format.drawText("s" + f_get_sysparm("kt_checkBox_ch_tailtotal_scale", "00").toLocal8Bit(), print_format.printTr(QT_TR_NOOP("小计")) + ":" + QString().sprintf("%.2f", total));
            print_format.drawN();
        }
        while(int_tail_block > 0 && int_tail_block--) {//票尾留白
            print_format.drawText("", "\n");
        }
        while(int_head_block > 0 && int_head_block--) {//票尾留白
            print_format.print_ins.prepend(pieceIns("", "\n"));
        }

        if(f_get_sysparm("kt_checkBox_vch_memo") == "1") print_format.drawText("s" + f_get_sysparm("kt_checkBox_vch_memo_scale", "00").toLocal8Bit(), print_format.printTr(QT_TR_NOOP("开台备注")) + ":" + f_get_from_cey_u_table(ch_billno, "vch_memo").toString());
        print_format.drawTextEnd("1B6405 1D5601");//走纸5x， 切纸
    }//while
    //解析
    // "------------厨打" << "打印" << __LINE__;
}

QString w_bt_dish_kitchen_dish_thread_event::get_bp_fmt_table(const QString &vch_tablename, const QString &ch_tableno)
{
    QString bp_fmt_table_fmts = f_get_sysparm("kt_fmt_table_fmts");

    if(bp_fmt_table_fmts == "name(no)") {
        return vch_tablename+"("+ch_tableno+")";
    }
    if(bp_fmt_table_fmts == "name") {
        return vch_tablename;
    }
    if(bp_fmt_table_fmts == "no") {
        return ch_tableno;
    }
    return vch_tablename+"("+ch_tableno+")";
}

QString w_bt_dish_kitchen_dish_thread_event::get_bp_fmt_operid(const QString &vch_operator, const QString &vch_operid)
{
    QString kt_fmt_vch_operid = f_get_sysparm("kt_fmt_vch_operid", "no");

    if(kt_fmt_vch_operid == "name(no)") {
        return vch_operator+"("+vch_operid+")";
    }
    if(kt_fmt_vch_operid == "name") {
        return vch_operator;
    }
    if(kt_fmt_vch_operid == "no") {
        return vch_operid;
    }
    return vch_operator+"("+vch_operid+")";
}

QString w_bt_dish_kitchen_dish_thread_event::getSqlReplaceIndexSql(const QString &key, int index, char rv)
{
    index++;
    // index = 3
    //    concat(substr(vch_plan_k3o3_state, 1, 2), '1', substr(vch_plan_k3o3_state,4, 3))

    //substr(vch_plan_k3o3_state, 1, 0), '1', substr(vch_plan_k3o3_state,6, 1)

    //    // "getSqlReplaceIndex" << index;
    return QString("concat(substr(%1, 1, %2), '%3', substr(%1,%4, %5))").arg(key).arg(index - 1).arg(rv).arg(index+1).arg(6 - index);
}

QString w_bt_dish_kitchen_dish_thread_event::getSqlReplaceIndex(const QString &key, int index, char rv)
{
    index++;
    // index = 3
    //    concat(substr(vch_plan_k3o3_state, 1, 2), '1', substr(vch_plan_k3o3_state,4, 3))

    //substr(vch_plan_k3o3_state, 1, 0), '1', substr(vch_plan_k3o3_state,6, 1)

    //    // "getSqlReplaceIndex" << index;
    QString k = key.mid(0, index - 1) + rv + key.mid(index, 6 - index);
    return k;
    //    return QString("concat(substr(%1, 1, %2), '%3', substr(%1,%4, %5))").arg(key).arg(index - 1).arg(rv).arg(index+1).arg(6 - index);
}

w_bt_dish_kitchen_dish_thread_event::w_bt_dish_kitchen_dish_thread_event(QObject *parent)
    : lds_thread_event(parent)
{

}


w_bt_dish_kitchen_dish_thread::w_bt_dish_kitchen_dish_thread(QObject *parent)
    : lds_thread("厨打", parent)
{

}

w_bt_dish_kitchen_dish_thread::~w_bt_dish_kitchen_dish_thread()
{

}

void w_bt_dish_kitchen_dish_thread::run()
{
    w_bt_dish_kitchen_dish_thread_event event;
    execEvent(&event);
}

//============
#include "ui_w_bt_dish_kitchen_dish_param.h"
w_bt_dish_kitchen_dish_param::w_bt_dish_kitchen_dish_param(QWidget *parent) :
    lds_model_mapwidget_vertical(parent), ui(new Ui_w_bt_dish_kitchen_dish_param)
{
    ui->setupUi(this);

    //vch_language
    w_sys_manage_outer_pay_set::addLanguageList(ui->kt_comboBox_vch_language);
    ui->kt_spinBox_vch_dishname_row->addItem("1", 1);
    ui->kt_spinBox_vch_dishname_row->addItem("2", 2);

    ui->kt_spinBox_num_num_row->addItem("1", 1);
    ui->kt_spinBox_num_num_row->addItem("2", 2);

    ui->kt_spinBox_unit_row->addItem("1", 1);
    ui->kt_spinBox_unit_row->addItem("2", 2);

    ui->kt_spinBox_num_price_row->addItem("1", 1);
    ui->kt_spinBox_num_price_row->addItem("2", 2);


    ui->kt_fmt_table_fmts->addItem(tr("餐桌编码"), "no");
    ui->kt_fmt_table_fmts->addItem(tr("餐桌"), "name");
    ui->kt_fmt_table_fmts->addItem(tr("餐桌")+"("+tr("编码")+")", "name(no)");

    ui->kt_fmt_vch_operid->addItem(tr("点菜人编码"), "no");
    ui->kt_fmt_vch_operid->addItem(tr("点菜人"), "name");
    ui->kt_fmt_vch_operid->addItem(tr("点菜人")+"("+tr("编码")+")", "name(no)");

    //1
    tablemodel = new lds_model_sqltablemodel;
    tablemodel->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    tablemodel->setTable("cey_sys_parameter");
    //2
    mapper = new QDataWidgetMapper(this);
    mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
    mapper->setOrientation(Qt::Vertical);
    mapper->setModel(tablemodel);

    scan_all_objectname_add_map_setFilter("vch_parameter", "kt_");
    mapper->setCurrentIndex(1);

    ///connect
    connect(ui->pushButton_cancel,SIGNAL(clicked()),this,SLOT(tocancel()));
    connect(ui->pushButton_ok,SIGNAL(clicked()),this,SLOT(took()));

    ///厨打服务器
    QString kitchen_service_terminal = n_func::f_get_sysparm("kitchen_service_terminal");
    ui->kt_checkBox_kitchen_start->setChecked(!kitchen_service_terminal.isEmpty());
    if(!kitchen_service_terminal.isEmpty()) {
        ui->label_kitchen_ip->setText(tr("厨打服务器")+"("+kitchen_service_terminal+")\n"+(kitchen_service_terminal ==lds::terminalCode ? "": (tr("厨打服务器")+tr("非本机")+"("+lds::terminalCode+")")));
        if(kitchen_service_terminal != lds::terminalCode) {
            ui->label_kitchen_ip->setStyleSheet("color:#ff650b;");
            ui->label_kitchen_ip->style()->polish(ui->label_kitchen_ip);
        }
    } else {
        ui->label_kitchen_ip->setText("");
        ui->label_kitchen_ip->setStyleSheet("");
        ui->label_kitchen_ip->style()->polish(ui->label_kitchen_ip);
    }
    connect(ui->kt_checkBox_kitchen_start, SIGNAL(clicked(bool)),this,SLOT(to_kitchen_start_toggled(bool)));
    connect(ui->kt_comboBox_vch_language, SIGNAL(currentIndexChanged(int)), this, SLOT(toSaaSCheck(int)));

    connect(ui->pushButton_hide_ExSet, SIGNAL(clicked()), this, SLOT(toHideExSet()));
    connect(ui->kt_checkBox_ch_serialno, SIGNAL(timeout()), this, SLOT(toShowExSet()));
    connect(ui->kt_checkBox_ch_tableno, SIGNAL(timeout()), this, SLOT(toShowExSet()));
    connect(ui->kt_checkBox_vch_operid, SIGNAL(timeout()), this, SLOT(toShowExSet()));

    //长按可修改
    ui->kt_checkBox_ch_serialno->enableLongPress(true);
    ui->kt_checkBox_ch_tableno->enableLongPress(true);
    ui->kt_checkBox_vch_operid->enableLongPress(true);
    //ini
    ui->kt_checkBox_ch_serialno->setText(ui->kt_fmt_serial_no_desc->text());
    ui->kt_checkBox_ch_tableno->setText(ui->kt_fmt_table_fmts->currentText());
    ui->kt_checkBox_vch_operid->setText(ui->kt_fmt_vch_operid->currentText());
    //kt_fmt_serial_no_desc作为备用
    ui->gridLayout->removeWidget(ui->kt_fmt_serial_no_desc);
    ui->gridLayout->removeWidget(ui->kt_fmt_table_fmts);
    ui->gridLayout->removeWidget(ui->kt_fmt_vch_operid);
    ui->gridLayout->removeWidget(ui->pushButton_hide_ExSet);
    //备用不显示
    lds::hideWidget(ui->kt_fmt_serial_no_desc);
    lds::hideWidget(ui->kt_fmt_table_fmts);
    lds::hideWidget(ui->kt_fmt_vch_operid);
    lds::hideWidget(ui->pushButton_hide_ExSet);
}

void w_bt_dish_kitchen_dish_param::took()
{
    if(saveData()) {
        this->accept();
    }
}

void w_bt_dish_kitchen_dish_param::tocancel()
{
    if(tablemodel->model_is_dirty()){
        switch(lds_messagebox::question(this, MESSAGE_TITLE_VOID,tr("有数据未保存,确认保存?"), tr("保存"),tr("不保存"),tr("取消"))){
        case 0:
            if(!saveData()){
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

void w_bt_dish_kitchen_dish_param::to_kitchen_start_toggled(bool checked)
{
    ///厨打服务器
    QString kitchen_service_terminal = n_func::f_get_sysparm("kitchen_service_terminal");

    if(!kitchen_service_terminal.isEmpty() && lds::terminalCode != kitchen_service_terminal) {
        if(1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, ui->label_kitchen_ip->text()+"\n" + tr("是否继续修改"), tr("继续"), tr("取消"))) {
            ui->kt_checkBox_kitchen_start->setChecked(!checked);
            return;
        }
    }
    if(checked) {
        n_func::f_set_sysparm("kitchen_service_terminal", lds::terminalCode);
    } else {
        n_func::f_set_sysparm("kitchen_service_terminal", "");
    }

    ui->label_kitchen_ip->setText(tr("厨打服务器")+":"+n_func::f_get_sysparm("kitchen_service_terminal"));
}

void w_bt_dish_kitchen_dish_param::toSaaSCheck(int index)
{
    Q_UNUSED(index)

    if(ui->kt_comboBox_vch_language == this->sender()) {
        if(!ui->kt_comboBox_vch_language->curusrdata().toString().isEmpty()) {//"" as 默认
            if(!saas_pay_detail::SaaSFuncCheck(this, "0512"/*多语言*/)) {
                ui->kt_comboBox_vch_language->setcomusrdata("");
            }
        }
    }
}

void w_bt_dish_kitchen_dish_param::toHideExSet()
{
    //餐桌
    if(lds::widgetIsShow(ui->pushButton_hide_ExSet)
            && ui->pushButton_hide_ExSet->pos() == ui->kt_lineEdit_ch_tableno_scale->pos()) {
        lds::hideWidget(ui->kt_fmt_table_fmts);
        lds::hideWidget(ui->pushButton_hide_ExSet);
        ui->kt_checkBox_ch_tableno->setText(ifEmpty(ui->kt_fmt_table_fmts->currentText(), ui->kt_fmt_table_fmts->itemText(0)));
    }
    //流失号
    if(lds::widgetIsShow(ui->pushButton_hide_ExSet)
            && ui->pushButton_hide_ExSet->pos() == ui->kt_lineEdit_ch_serialno_scale->pos()) {
        lds::hideWidget(ui->kt_fmt_serial_no_desc);
        lds::hideWidget(ui->pushButton_hide_ExSet);
        ui->kt_checkBox_ch_serialno->setText(ifEmpty(ui->kt_fmt_serial_no_desc->text(), tr("流水号")));
    }
    //操作员
    if(lds::widgetIsShow(ui->pushButton_hide_ExSet)
            && ui->pushButton_hide_ExSet->pos() == ui->kt_lineEdit_vch_operid_scale->pos()) {
        lds::hideWidget(ui->kt_fmt_vch_operid);
        lds::hideWidget(ui->pushButton_hide_ExSet);
        ui->kt_checkBox_vch_operid->setText(ifEmpty(ui->kt_fmt_vch_operid->currentText(), ui->kt_fmt_vch_operid->itemText(0)));

    }
}

void w_bt_dish_kitchen_dish_param::toShowExSet()
{
    //已经有设置中的空间会报存并关闭
    toHideExSet();

    ui->pushButton_hide_ExSet->raise();
    //餐桌
    if(this->sender() == ui->kt_checkBox_ch_tableno) {
        ui->kt_fmt_table_fmts->raise();
        lds::showWidget(ui->kt_fmt_table_fmts);
        lds::showWidget(ui->pushButton_hide_ExSet);
        ui->kt_fmt_table_fmts->setGeometry(ui->kt_checkBox_ch_tableno->geometry());
        ui->pushButton_hide_ExSet->setGeometry(ui->kt_lineEdit_ch_tableno_scale->geometry());
    }
    //流失号
    if(this->sender() == ui->kt_checkBox_ch_serialno) {
        ui->kt_fmt_serial_no_desc->raise();
        lds::showWidget(ui->kt_fmt_serial_no_desc);
        lds::showWidget(ui->pushButton_hide_ExSet);
        ui->kt_fmt_serial_no_desc->setGeometry(ui->kt_checkBox_ch_serialno->geometry());
        ui->pushButton_hide_ExSet->setGeometry(ui->kt_lineEdit_ch_serialno_scale->geometry());
    }
    //操作员
    if(this->sender() == ui->kt_checkBox_vch_operid) {
        ui->kt_fmt_vch_operid->raise();
        lds::showWidget(ui->kt_fmt_vch_operid);
        lds::showWidget(ui->pushButton_hide_ExSet);
        ui->kt_fmt_vch_operid->setGeometry(ui->kt_checkBox_vch_operid->geometry());
        ui->pushButton_hide_ExSet->setGeometry(ui->kt_lineEdit_vch_operid_scale->geometry());
    }
}

bool w_bt_dish_kitchen_dish_param::insert_sql_objectname(const QString &objectname)
{
    lds_query query;
    query.execSelect(QString("SELECT vch_parameter from cey_sys_parameter where vch_parameter = '%1' ").arg(objectname));
    if(!query.next()) {
        QString vch_value;
        if(objectname == ui->kt_fmt_serial_no_desc->objectName()) {
            vch_value = ui->kt_checkBox_ch_serialno->text();//默认
        }
        if(objectname == ui->kt_fmt_vch_operid->objectName()) {
            vch_value = ui->kt_fmt_vch_operid->itemData(0).toString();//默认
        }
        if(objectname == ui->kt_fmt_table_fmts->objectName()) {
            vch_value = ui->kt_fmt_table_fmts->itemData(0).toString();//默认
        }
        query.execInsert("cey_sys_parameter",qrtVariantPairList()
                                << qrtVariantPair("vch_parameter", objectname)
                                << qrtVariantPair("vch_value", vch_value));
    }
    return true;
}

bool w_bt_dish_kitchen_dish_param::saveData()
{
    lds_query::tran_saction();
    if(mapper_save_data()) {
        lds_query::com_mit();
        lds_messagebox::information(this, MESSAGE_TITLE_VOID,tr("保存成功"));
        return true;
    }
    lds_query::roll_back();
    //错误提示
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("数据库错误"));
    return false;
}



//0x12   正常
//0x16   开盖
//0x32   缺纸
//0x10   故障
//0x00   无应答
void w_bt_dish_kitchen_dish_print_state_thread_event::toExec()
{
    ///
    lds_query query;
    Printer_POS *printer=0;
    query.execSelect("select b.int_id, b.vch_printip, b.vch_printname from cey_bt_kitchen_plan a, cey_bt_kitchen_print b where a.int_print_id = b.int_id;");
    while(query.next()) {///while
        QString vch_printip = query.record().value("vch_printip").toString();
        QString int_id = query.record().value("int_id").toString();
        printer = CommonHeader::getposprint_bypath(vch_printip);
        Printer_POS_auto_destroy pbox(printer);

        if(pbox.printer && pbox.printer->tryOpen(vch_printip)) {
            pbox.printer->writeHex("100402");
            if(pbox.printer->waitForReadyReadv(1000)){
                QByteArray v = pbox.printer->readAll();
                if(v.endsWith(0x12)){//0001 0010
                    login_emit_signal_name_id_state(int_id, 0x12);
                } else  if(v.endsWith(0x16)){//0001 0010
                    login_emit_signal_name_id_state(int_id, 0x16);
                }else if(v.endsWith(0x32)){//0001 0010
                    login_emit_signal_name_id_state(int_id, 0x32);
                } else {
                    login_emit_signal_name_id_state(int_id, 0x10);//打印机故障
                }
                continue;
            }
        }//tryopen
        login_emit_signal_name_id_state(int_id, 0x00);//无响应
    }//while
}

void w_bt_dish_kitchen_dish_print_state_thread_event::login_emit_signal_name_id_state(const QString &int_id, uchar state)
{
    QMetaObject::invokeMethod(public_sql::save_login_object(),
                              "signal_kitchen_print_state",
                              Qt::QueuedConnection,
                              Q_ARG(QString, int_id),
                              Q_ARG(uchar, state)
                              );
}


w_bt_dish_kitchen_dish_print_state_thread::w_bt_dish_kitchen_dish_print_state_thread(QObject *parent) :
    lds_thread("打印机状态线程", parent)
{
}

w_bt_dish_kitchen_dish_print_state_thread::~w_bt_dish_kitchen_dish_print_state_thread()
{

}

void w_bt_dish_kitchen_dish_print_state_thread::run()
{
    w_bt_dish_kitchen_dish_print_state_thread_event event;
    execEvent(&event);
}





