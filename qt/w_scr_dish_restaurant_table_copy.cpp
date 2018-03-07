#include "w_scr_dish_restaurant_table_copy.h"
#include "ui_w_scr_dish_restaurant_table_copy.h"
#include "w_scr_dish_restaurant_div_pop.h"
#include "lds_query.h"

#include "ftableview_standmodel_sql_table.h"
#include "restauranttableview.h"
#include "public_sql.h"
#include "lds_messagebox.h"
#include "n_func.h"
#include <QPainter>
#include <QSqlError>
#include "w_bt_dish.h"
#include "frontviewdialog_virtual.h"

w_scr_dish_restaurant_table_copy::w_scr_dish_restaurant_table_copy(const QString &ch_tableno, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_scr_dish_restaurant_table_copy)
{
    ui->setupUi(this);

    ui->label_cur_table->setText(QString("%1(%2)")
                                 .arg(public_sql::getTableName(ch_tableno))
                                 .arg(ch_tableno));
    //
    lds_query query;
    query.execSelect(QString("select  "+restraurantDelegate::sql_ch_billnos+" as ch_billnos"
                             " from cey_bt_table y"
                             " where y.ch_tableno = '%1' ")
                     .arg(ch_tableno));
    query.next();

    QString ch_billnos = query.recordValue(0).toString();

    QMap<int, QString> ch_billno_map = restraurantDelegate::get_ch_billno_div_map(ch_billnos);
    //九宫格
    _9Box = new w_scr_dish_restaurant_div_pop_only_table(this);
    _9Box->update_by_ch_billnos(ch_billno_map);
    ui->horizontalLayout_div->addWidget(_9Box);
    if(restraurantDelegate::ch_billno_is_div(ch_billno_map)){
        _9Box->enableSinglSelectedWhenMutil();
    } else {
        _9Box->setcurindex_by_sqr(0);
        _9Box->trycombine();
    }
    //
    query.execSelect("select ch_typeno , vch_typename  FROM cey_bt_table_type union select '%' as ch_typeno, '"+tr("所有类型")+"' as vch_typename ORDER BY ch_typeno ASC");
    while(query.next()){
        ui->comboBox_tabletype->addItem(query.recordValue("vch_typename").toString(), query.recordValue("ch_typeno").toString());
    }
    //
    restraurantDelegate *delegatetable = new restraurantDelegate(ui->tableView);
    delegatetable->preprinttablemap = &(public_sql::preprinttablemap);
    modeltable = new ftableview_standmodel_sql_table(ui->tableView);
    modeltable->sql =
            " select y.vch_tablename, y.ch_tableno, "
            + restraurantDelegate::sql_ch_billnos + "as ch_billnos ,"
              " ifnull((select int_person from cey_u_table where cey_u_table.ch_billno = y.ch_billno), y.int_person) as int_person,"
              " (select num_cost from cey_u_table where cey_u_table.ch_billno = y.ch_billno) as num_cost "
              " from cey_bt_table y"
              " where "
              " ch_tableno <> '"+ch_tableno+"'"
            ///未开台餐桌
            + " and "+restraurantDelegate::sql_ch_billnos +" <> ',,,,,,,,,'  ";
    ;
    modeltable->sqlother = modeltable->sql+ " and y.ch_typeno like '%1%' ";
    ui->tableView->setModel(modeltable);
    ui->tableView->setItemDelegate(delegatetable);
    ui->tableView->updateFixed_fillData_addAnimation(4, 6, 90, 90);
    ui->tableView->enablePop(restaurantTableView::pop_mutil| restaurantTableView::pop_select_KAITAI);
    ui->tableView->enableMutiSelected();
    modeltable->selectionmodel = ui->tableView->selectionModel();
    connect(ui->pushButton_ok,SIGNAL(clicked()),this,SLOT(took()));
    connect(ui->pushButton_cancel,SIGNAL(clicked()),this,SLOT(tocancel()));

    connect(ui->comboBox_tabletype,SIGNAL(currentIndexChanged(int)),this,SLOT(totabletype(int)));
    connect(_9Box, SIGNAL(signal_div_click(QModelIndex)),this, SLOT(toSetgrayTo(QModelIndex)));
    connect(ui->tableView, SIGNAL(released(QModelIndex)), this,SLOT(toupdatedestination(QModelIndex)));

    totabletype(0);

    ui->lineEdit_destination->setText("");

    //3
    ui->pushButton_cur_table->setIcon(restraurantDelegate::getPixmapByStateList(QList<tableState>()<<tableState_KAITAI <<  tableState_YUDA));
    ui->pushButton_destination->setIcon(restraurantDelegate::getPixmapByStateList(QList<tableState>()<<tableState_KAITAI <<  tableState_YUDA));
    //4
    ui->label_desc->setText(tr("未开台餐桌的将不会显示"));
    ui->label_destination_sum->setText("");

    ui->checkBox_tranfer_print->setChecked(lds::sysconf->value("w_scr_dish_restaurant_table_copy/kitchen_turn_check", false).toBool());
    ui->checkBox_tranfer_out->setChecked(lds::sysconf->value("w_scr_dish_restaurant_table_copy/out_kitchen_turn_check", false).toBool());
    connect(ui->checkBox_tranfer_out,SIGNAL(toggled(bool)), this, SLOT(tosavekitchenout(bool)));
    connect(ui->checkBox_tranfer_print,SIGNAL(toggled(bool)), this, SLOT(tosavekitchenprint(bool)));

}

w_scr_dish_restaurant_table_copy::~w_scr_dish_restaurant_table_copy()
{
    delete ui;
}

void w_scr_dish_restaurant_table_copy::totabletype(int index)
{
    modeltable->toinitBySqlother(ui->comboBox_tabletype->itemData(index).toString());
}

void w_scr_dish_restaurant_table_copy::tocurindex_checklist(const QString &checklist)
{
    modeltable->setData(ui->tableView->currentIndex(), checklist, Qt::UserRole+7/*check*/);
}

void w_scr_dish_restaurant_table_copy::tocancel()
{
    this->reject();
}

void w_scr_dish_restaurant_table_copy::took()
{
    QString errstring;
    lds_query query;
    lds_query querygoing;

    QStringList ch_billno_to_list;
    QString ch_billno_to_list_desc;
    foreach(const QModelIndex &index, ui->tableView->selectionModel()->selectedIndexes()){
        QStringList index_list = ui->tableView->getCheckList(index);//需要过滤disable的获取check

        QMap<int, QString> ch_billno_map = restraurantDelegate::get_ch_billno_div_map(index.data(Qt::UserRole + 2/*ch_billnos*/).toString());
        if(restraurantDelegate::ch_billno_is_div(ch_billno_map)){//可以分单32
            foreach(const QString &sqr, index_list){
                ch_billno_to_list.append(ui->tableView->getChbillnoBySqr(index, sqr.toInt()));

                ch_billno_to_list_desc += QString("%1(%2)")
                        .arg(index.data(Qt::UserRole).toString())
                        .arg(index.data(Qt::UserRole + 1).toString())
                        + ("," + tr("分单号") + ":" + sqr)
                        + "\n"
                        ;
            }
        } else {
            if(!ch_billno_map.isEmpty()){
                ch_billno_to_list.append(ch_billno_map.value(0));
                ch_billno_to_list_desc += QString("%1(%2)")
                        .arg(index.data(Qt::UserRole/*vch_tablename*/).toString())
                        .arg(index.data(Qt::UserRole + 1/*ch_tableno*/).toString())
                        + "\n"
                        ;
            }
        }
    }

    QModelIndexList indexlist = _9Box->get_cur_check_index_list();
    errstring = ui->pushButton_cur_table->text()+tr("未设置");
    if(!indexlist.isEmpty()){
        QString ch_billno_from = indexlist[0].data(w_scr_dish_restaurant_div_pop::Model_ch_billno).toString();//都是单选
        //手工处理，自己copy自己时，排在最后
        //        if(ch_billno_to_list.contains(ch_billno_from)){
        //            ch_billno_to_list.removeAll(ch_billno_from);
        //            ch_billno_to_list.append(ch_billno_from);
        //        }

        errstring = ui->pushButton_destination->text()+tr("未设置");
        if(!ch_billno_to_list.isEmpty()){
            QString transferinfo =
                    ui->pushButton_cur_table->text()+":\n"+ui->label_cur_table->text()        +  ( _9Box->iscombine ? "" : ( "," + tr("分单号") + indexlist[0].data(Qt::UserRole + 1).toString() ))+"\n" +
                    ui->pushButton_destination->text()+":\n"+ch_billno_to_list_desc//都是单选
                    ;
            errstring = "";
            if(0 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, transferinfo,tr("确认"), tr("取消"))){
                //!2判断估清, 这边 不适合使用tryWarn函数，他是对单个菜品判断的
                lds_query::tran_saction();
                QString state;
                if(ui->checkBox_tranfer_print->isChecked()) {
                    state += QString(3, w_bt_dish::k2o3_state2hex_char(w_bt_dish::KITCHEN_FUZHI));
                } else {
                    state += QString(3, w_bt_dish::k2o3_state2hex_char(w_bt_dish::KITCHEN_FINISH));
                }
                if(ui->checkBox_tranfer_out->isChecked()){
                    state += QString(3, w_bt_dish::k2o3_state2hex_char(w_bt_dish::KITCHEN_FUZHI));
                } else {
                    state += QString(3, w_bt_dish::k2o3_state2hex_char(w_bt_dish::KITCHEN_FINISH));
                }
                //!插入数据库
                foreach(const QString &ch_billno_to, ch_billno_to_list){

                    query.execSelect(QString("select ch_tableno from cey_u_table where ch_billno = '%1' ")
                                     .arg(ch_billno_to));
                    if(!query.next()) goto rollback;
                    QString ch_tableno_to = query.recordValue(0).toString();


                    query.execSelect(QString("select ifnull(max(int_id), 0) from cey_u_orderdish where ch_billno = '%1' ")
                                     .arg(ch_billno_to));
                    query.next();
                    int int_id_to_max = query.recordValue(0).toLongLong();

                    querygoing.execSelect(QString("select ch_tableno ,ch_dishno,num_price,num_price_org,num_price_add,num_num,num_back,vch_back_operID,dt_back_operdate,int_discount,vch_dis_operID,dt_dis_operdate,vch_memberno,ch_consult,vch_print_memo,ch_suitflag,ch_suitno,ch_specialflag,ch_presentflag,vch_pre_operID,dt_pre_operdate,vch_operID,dt_operdate,ch_togono,ch_payno, int_rate, vch_plan_k3o3_id from cey_u_orderdish where ch_billno='%1' ")
                                          .arg(ch_billno_from));
                    while(querygoing.next()){
                        if(!query.execInsert("cey_u_orderdish",qrtVariantPairList()
                                             << qrtVariantPair("ch_billno", ch_billno_to)
                                             << qrtVariantPair("ch_tableno", ch_tableno_to)
                                             << qrtVariantPair("int_id", ++int_id_to_max)
                                             << qrtVariantPair("ch_dishno", querygoing.recordValue("ch_dishno"))
                                             << qrtVariantPair("num_price", querygoing.recordValue("num_price"))

                                             << qrtVariantPair("num_price_org", querygoing.recordValue("num_price_org"))
                                             << qrtVariantPair("num_price_add", querygoing.recordValue("num_price_add"))
                                             << qrtVariantPair("num_num", querygoing.recordValue("num_num"))
                                             << qrtVariantPair("num_back", querygoing.recordValue("num_back"))
                                             << qrtVariantPair("vch_back_operID", querygoing.recordValue("vch_back_operID"))

                                             << qrtVariantPair("dt_back_operdate", querygoing.recordValue("dt_back_operdate"))
                                             << qrtVariantPair("int_discount", querygoing.recordValue("int_discount"))
                                             << qrtVariantPair("vch_dis_operID", querygoing.recordValue("vch_dis_operID"))
                                             << qrtVariantPair("dt_dis_operdate", querygoing.recordValue("dt_dis_operdate"))
                                             << qrtVariantPair("vch_memberno", querygoing.recordValue("vch_memberno"))

                                             << qrtVariantPair("ch_consult", querygoing.recordValue("ch_consult"))
                                             << qrtVariantPair("ch_suitflag", querygoing.recordValue("ch_suitflag"))
                                             << qrtVariantPair("ch_suitno", querygoing.recordValue("ch_suitno"))
                                             << qrtVariantPair("ch_specialflag", querygoing.recordValue("ch_specialflag"))
                                             << qrtVariantPair("ch_presentflag", querygoing.recordValue("ch_presentflag"))

                                             << qrtVariantPair("vch_pre_operID", querygoing.recordValue("vch_pre_operID"))
                                             << qrtVariantPair("dt_pre_operdate", querygoing.recordValue("dt_pre_operdate"))
                                             << qrtVariantPair("vch_operID", querygoing.recordValue("vch_operID"))
                                             << qrtVariantPair("dt_operdate", querygoing.recordValue("dt_operdate"))
                                             << qrtVariantPair("ch_togono", querygoing.recordValue("ch_togono"))

                                             << qrtVariantPair("ch_payno", querygoing.recordValue("ch_payno"))
                                             << qrtVariantPair("vch_print_memo", querygoing.recordValue("vch_print_memo"))
                                             << qrtVariantPair("vch_plan_k3o3_id", querygoing.recordValue("vch_plan_k3o3_id"))
                                             << qrtVariantPair("vch_plan_k3o3_state", state)
                                             << qrtVariantPair("vch_plan_desc", transferinfo)

                                             << qrtVariantPair("int_rate", querygoing.recordValue("int_rate"))
                                             )){
                            errstring = tr("数据库错误");
                            
                            goto rollback;
                        }
                    }
                }
                //!更新总价
                foreach(const QString &ch_billno_to, ch_billno_to_list){
                    if(!query.execUpdate("cey_u_table",qrtVariantPairList()
                                          << qrtVariantPair("num_cost", public_sql::getTotalBy_ch_billno(ch_billno_to)),
                                         QString(" ch_billno = '%1' ").arg(ch_billno_to))) {
                        errstring = tr("数据库错误");
                        goto rollback;
                    }
                }

                //!成功
                lds_query::com_mit();
                lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("复制成功"));
                this->accept();
                return;
            }
        }
    }
failure:
    if(!errstring.isEmpty()){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
    }
    return;
rollback:
    lds_query::roll_back();
    goto failure;
}

void w_scr_dish_restaurant_table_copy::toSetgrayTo(const QModelIndex &index_from)
{
    QString sqr = index_from.data(Qt::UserRole + 1/*sqr*/).toString();
    QString ch_billno = index_from.data(Qt::UserRole/*ch_billno*/).toString();
    QString ch_tableno;
    lds_query query;
    query.execSelect(QString("select ch_tableno from cey_u_table where ch_billno = '%1' ")
                     .arg(ch_billno));
    query.next();
    ch_tableno = query.recordValue("ch_tableno").toString();

    QModelIndex index_to = ui->tableView->getIndexByChtableno(ch_tableno);
    ui->tableView->m->setData(index_to, sqr, Qt::UserRole + 8/*disable*/);
}

void w_scr_dish_restaurant_table_copy::tosavekitchenout(bool f)
{
    lds::sysconf->setValue("w_scr_dish_restaurant_table_copy/out_kitchen_turn_check", f);

}

void w_scr_dish_restaurant_table_copy::tosavekitchenprint(bool f)
{
    lds::sysconf->setValue("w_scr_dish_restaurant_table_copy/kitchen_turn_check", f);
}

void w_scr_dish_restaurant_table_copy::toupdatedestination(const QModelIndex &index)
{
    Q_UNUSED (index)
    QList<ftableview_standmodel_sql_table::checkData> checkdata = modeltable->get_value_select();

    QString text;
    foreach(const ftableview_standmodel_sql_table::checkData & d, checkdata) {
        if(!d.ch_tableno.isEmpty()) text += d.ch_tableno;
        //        if(!d.checklist.isEmpty()) text += "["+ d.checklist + "]";
        text += ",";
    }
    if(!text.isEmpty()) text.chop(1);
    ui->lineEdit_destination->setText(text);
}

