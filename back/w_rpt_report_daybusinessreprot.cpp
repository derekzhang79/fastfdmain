#include "w_rpt_report_daybusinessreprot.h"
//日营业情况统计
#include "ui_w_rpt_report_daybusinessreprot_dialog.h"
#include "lds_messagebox.h"
#include <QVariant>
#include <QStandardItemModel>
#include "lds.h"
#include "n_func.h"

w_rpt_report_daybusinessreprot::w_rpt_report_daybusinessreprot(QWidget *parent) :
    QDialog(parent), ui(new Ui_w_rpt_report_daybusinessreprot_Dialog)
{
    ui->setupUi(this);
    ui->frame->setFixedWidth(w_rpt_report::frame_left_fixed_width);
    QDateTime dt = n_func::f_get_sysdatetime();
    w_rpt_report::reportBeginDtSet(ui->dateTimeEdit);
    ui->dateTimeEdit_2->setDateTime(QDateTime(dt.date(), QTime(23,59,59)));

    tab1sqlinit();

    standmodel=new report_standmodel(this);
    standmodel->disable_data_transfer();

    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);ui->tableView->setModel(standmodel);
    //! toolbar
    ui->widget->setup(QStringList()<<tr("查询")<<tr("打印")<<tr("退出"),Qt::AlignTop);
    connect(ui->widget->index_widget(tr("打印")),SIGNAL(clicked()),this,SLOT(toprint()));
    connect(ui->widget->index_widget(tr("查询")),SIGNAL(clicked()),this,SLOT(toselect()));
    connect(ui->widget->index_widget(tr("退出")),SIGNAL(clicked()),this,SLOT(toexit()));
    //! after
    //导入
    ui->comboBox_beginmonth->addItems(QStringList() << tr("上月") << tr("当月"));
    loadbeginset();
    //上月，当月的com
    //保存日和月
    connect(ui->comboBox_beginmonth,SIGNAL(currentIndexChanged(int)),this,SLOT(updatecombegindate(int)));
    connect(ui->comboBox_beginmonth,SIGNAL(currentIndexChanged(int)),this,SLOT(savebeginset()));
    connect(ui->spinBox_beginday,SIGNAL(valueChanged(int)),this,SLOT(savebeginset()));
    //tabwidget header
    ui->frame_2->hide();
    ui->comboBox_end->hide();
    ui->comboBox_start->hide();
}

void w_rpt_report_daybusinessreprot::tab1sqlinit()
{

}

void w_rpt_report_daybusinessreprot::toselect()
{
    lds_messagebox_loading_show loading(this, "WAITING...", "WAITING...");
    loading.show();
    //表头
    standmodel->setHorizontalHeaderLabels(QStringList () << tr("统计类型") << tr("统计项目") << tr("日营业值") << tr("月累计值") << tr("占当月比例") + "%");
    //
    //ue_query (none) returns (none)
    //    QDateTime ldt_begin;
    //    QDateTime ldt_end;
    //    QDateTime ldt_begin_m;
    //    QDateTime ldt_end_m;
    //    QDateTime ldt_date;
    //    QDateTime ldt_mb;
    //    QDateTime ldt_me;
    //    QString ls_begin;
    //    QString ls_end;
    //    QString ls_begin_m;
    //    QString ls_end_m;
    //    QString ls_flag;
    //    long ll_begin;
    //    QString ls_itemtype;
    //    QString ls_item;
    QString ls_rate;
    //    QString ls_date;
    //    QString ls_date_end;
    //    double ldec_profit=0;
    //    double ldec_profit_m=0;
    double ll_count=0;
    double ll_count_month=0;
    double ll_row=0;
    double ll_table=0;
    double ll_person=0;
    double ll_minute=0;
    double ll_table_m=0;
    double ll_person_m=0;
    double ll_minute_m=0;
    double ldec_cost=0;
    double ldec_num=0;
    double ldec_discount=0;
    double ldec_present=0;
    double ldec_service=0;
    double ldec_tax=0;
    double ldec_lowcost=0;
    double ldec_cash=0;
    double ldec_cost_m=0;
    double ldec_num_m=0;
    double ldec_discount_m=0;
    double ldec_present_m=0;
    double ldec_service_m=0;
    double ldec_tax_m=0;
    double ldec_lowcost_m=0;
    double ldec_cash_m=0;
    double ldec_blotout=0;
    double ldec_free=0;
    double ldec_blotout_m=0;
    double ldec_free_m=0;
    double ldec_type=0;
    double ldec_type_m=0;
    QString ls_typeno;
    QString ls_typename;
    //    DECLARE c_series CURSOR FOR
    //    Select ch_dish_typeno , vch_dish_typename
    //    from cey_bt_dish_type
    //    order by ch_dish_typeno
    //    USING sqlca;
    QString ls_paymodeno;
    QString ls_paymodename;
    QString ls_incomeflag;
    double ldec_total=0;
    double ldec_total_m=0;
    double ldec_total1=0;
    double ldec_total1_m=0;
    //    DECLARE c_pay CURSOR FOR
    //    Select a.ch_paymodeno , IFNULL(c.vch_paymodename , a.ch_paymodeno), IFNULL(c.ch_incomeflag , 'Y'), sum(a.num_realamount )
    //    from cey_u_checkout_detail a , cey_u_checkout_master b , cey_bt_paymode c
    //    where a.ch_paymodeno *=c.ch_paymodeno and a.ch_payno =b.ch_payno and(b.ch_state ='Y')and b.dt_operdate >= :ldt_begin_m and b.dt_operdate <= :ldt_end_m
    //    group by a.ch_paymodeno , IFNULL(c.vch_paymodename , a.ch_paymodeno), IFNULL(c.ch_incomeflag , 'Y' )
    //    having sum(a.num_realamount)<> 0
    //    order by a.ch_paymodeno
    //    USING sqlca;
    double ldec_deposit_m=0;
    double ldec_deposit=0;
    w_sys_manage_cloudsync::member_r_f_b_info m_rfb;//云会员充值，退卡等情况
    lds_query query, query_c_series,query_c_pay;

    QString ls_begin;
    QString ls_end;
    ls_begin = ui->dateTimeEdit->dateTimeStr();
    ls_end = ui->dateTimeEdit_2->dateTimeStr();


    query.execSelect(QString("Select count(ch_payno) from cey_u_checkout_master  where(ch_state ='Y')and dt_operdate >= '%1' and dt_operdate <= '%2' ")
                     .arg(ls_begin)
                     .arg(ls_end)
                     );
    query.next();
    ll_count=query.recordValue(0).toLongLong();

    query.execSelect(QString("Select count(distinct ch_billno)from cey_u_checkout_master  where(ch_state ='Y')and dt_operdate >= '%1' and dt_operdate <= '%2'")
                     .arg(ls_begin)
                     .arg(ls_end)
                     );
    query.next();
    ll_count_month=query.recordValue(0).toLongLong();

    QList<QStandardItem *> items;
    standmodel->removeRows(0, standmodel->rowCount());

    //营业分析======================================================================================================================================

    items.clear();
    items << new QStandardItem(itemtextspan(tr("营业分析")));
    items << new QStandardItem(tr("单据数"));
    items << report_standmodel::newItem(ll_count);
    items << report_standmodel::newItem((ll_count_month));
    items << report_standmodel::newItem((ll_count_month==0? 0.0: (ll_count*1.0)/(ll_count_month*1.0) * 100));
    standmodel->appendRow(items);


    query.execSelect(QString("Select count(a.ch_payno), IFNULL(sum(a.int_person), 0), ABS(IFNULL(sum(round(TIME_TO_SEC(TIMEDIFF(a.dt_operDate , b.dt_operdate))/60)),0))   from cey_u_table a , cey_u_checkout_master b  where a.ch_payno =b.ch_payno and b.ch_state ='Y' and b.dt_operdate >=  '%1' and b.dt_operdate <= '%2' ")
                     .arg(ls_begin)
                     .arg(ls_end));
    query.next();
    ll_table=query.recordValue(0).toLongLong();
    ll_person=query.recordValue(1).toLongLong();
    ll_minute=query.recordValue(2).toLongLong();

    query.execSelect(QString("Select count(a.ch_payno), IFNULL(sum(a.int_person), 0),  ABS(IFNULL(sum(round(TIME_TO_SEC(TIMEDIFF(a.dt_operDate , b.dt_operdate))/60)), 0)) from cey_u_table a , cey_u_checkout_master b  where a.ch_payno =b.ch_payno and b.ch_state ='Y' and b.dt_operdate >= '%1' and b.dt_operdate <= '%2' ")
                     .arg(ls_begin)
                     .arg(ls_end)
                     );
    query.next();
    ll_table_m=query.recordValue(0).toLongLong();
    ll_person_m=query.recordValue(1).toLongLong();
    ll_minute_m=query.recordValue(2).toLongLong();

    if(ll_table==0){
        ll_minute=0;
    } else {
        ll_minute=(ll_minute*1.0)/(ll_table*1.0);
    }
    if(ll_table_m==0){
        ll_minute_m=0;
    } else {
        ll_minute_m=(ll_minute_m*1.0)/(ll_table_m*1.0);
    }

    items.clear();
    items << new QStandardItem(itemtextspan(tr("营业分析")));
    items << new QStandardItem(tr("桌位数"));
    items << report_standmodel::newItem((ll_table));
    items << report_standmodel::newItem((ll_table_m));
    items << report_standmodel::newItem((ll_table_m==0? 0.0: (ll_table*1.0)/(ll_table_m*1.0) * 100));
    standmodel->appendRow(items);

    items.clear();
    items << new QStandardItem(itemtextspan(tr("营业分析")));
    items << new QStandardItem(tr("人数"));
    items << report_standmodel::newItem((ll_person));
    items << report_standmodel::newItem((ll_person_m));
    items << report_standmodel::newItem((ll_person_m==0? 0.0: (ll_person*1.0)/(ll_person_m*1.0) * 100));
    standmodel->appendRow(items);

    items.clear();
    items << new QStandardItem(itemtextspan(tr("营业分析")));
    items << new QStandardItem(tr("每桌平均人数"));
    items << report_standmodel::newItem((ll_table==0? 0.0: (ll_person*1.0)/(ll_table*1.0)));
    items << report_standmodel::newItem((ll_table_m==0? 0.0: (ll_person_m*1.0)/(ll_table_m*1.0)));
    items << new QStandardItem("");
    standmodel->appendRow(items);

    items.clear();
    items << new QStandardItem(itemtextspan(tr("营业分析")));
    items << new QStandardItem(tr("每桌平均消费时间(分钟)"));
    items << report_standmodel::newItem((ll_minute));
    items << report_standmodel::newItem((ll_minute_m));
    items << new QStandardItem("");
    standmodel->appendRow(items);

    query.execSelect(QString("Select IFNULL(sum(a.num_num - a.num_back), 0)  from cey_u_orderdish a , cey_u_checkout_master b   where a.ch_payno =b.ch_payno and(b.ch_state ='Y')and b.dt_operdate >= '%1' and b.dt_operdate <= '%2'")
                     .arg(ls_begin)
                     .arg(ls_end));
    query.next();
    ldec_num=query.recordValue(0).toDouble();

    query.execSelect(QString("Select IFNULL(sum(a.num_num - a.num_back), 0) from cey_u_orderdish a , cey_u_checkout_master b   where a.ch_payno =b.ch_payno and(b.ch_state ='Y')and b.dt_operdate >= '%1' and b.dt_operdate <= '%2' ")
                     .arg(ls_begin)
                     .arg(ls_end));
    query.next();
    ldec_num_m=query.recordValue(0).toDouble();

    items.clear();
    items << new QStandardItem(itemtextspan(tr("营业分析")));
    items << new QStandardItem(tr("每桌平均消费菜品数量"));
    items << report_standmodel::newItem((ll_table==0? 0.0 : ldec_num / ll_table));
    items << report_standmodel::newItem((ll_table_m==0? 0.0 : ldec_num_m / ll_table_m));
    items << new QStandardItem("");
    standmodel->appendRow(items);

    query.execSelect(QString("Select IFNULL(sum(num_cost), 0), IFNULL(sum(num_discount), 0), IFNULL(sum(num_present), 0), IFNULL(sum(num_service), 0), IFNULL(sum(num_lowcost), 0) , IFNULL(sum(num_rate), 0) , IFNULL(sum(num_cash), 0)  from cey_u_checkout_master  where ch_state ='Y' and dt_operdate >= '%1' and dt_operdate <= '%2'")
                     .arg(ls_begin)
                     .arg(ls_end));
    query.next();
    ldec_cost=query.recordValue(0).toDouble();
    ldec_discount=query.recordValue(1).toDouble();
    ldec_present=query.recordValue(2).toDouble();
    ldec_service=query.recordValue(3).toDouble();
    ldec_lowcost=query.recordValue(4).toDouble();
    ldec_tax = query.recordValue(5).toDouble();
    ldec_cash = query.recordValue(6).toDouble();


    query.execSelect(QString("Select IFNULL(sum(num_cost), 0), IFNULL(sum(num_discount), 0), IFNULL(sum(num_present), 0), IFNULL(sum(num_service), 0), IFNULL(sum(num_lowcost), 0), IFNULL(sum(num_rate), 0) , IFNULL(sum(num_cash), 0)  from cey_u_checkout_master    where ch_state ='Y' and dt_operdate >= '%1' and dt_operdate <= '%2'")
                     .arg(ls_begin)
                     .arg(ls_end));
    query.next();
    ldec_cost_m=query.recordValue(0).toDouble();
    ldec_discount_m=query.recordValue(1).toDouble();
    ldec_present_m=query.recordValue(2).toDouble();
    ldec_service_m=query.recordValue(3).toDouble();
    ldec_lowcost_m=query.recordValue(4).toDouble();
    ldec_tax_m = query.recordValue(5).toDouble();
    ldec_cash_m = query.recordValue(6).toDouble();

    items.clear();
    items << new QStandardItem(itemtextspan(tr("营业分析")));
    items << new QStandardItem(tr("每桌平均消费金额"));
    items << report_standmodel::newItem((ll_table==0? 0.0 : ldec_cash / ll_table));
    items << report_standmodel::newItem((ll_table_m==0? 0.0 : ldec_cash_m / ll_table_m));
    items << new QStandardItem("");
    standmodel->appendRow(items);

    items.clear();
    items << new QStandardItem(itemtextspan(tr("营业分析")));
    items << new QStandardItem(tr("人均消费金额"));
    items << report_standmodel::newItem((ll_person==0? 0.0 : ldec_cash / ll_person));
    items << report_standmodel::newItem((ll_person_m==0? 0.0 : ldec_cash_m / ll_person_m));
    items << new QStandardItem("");
    standmodel->appendRow(items);
    //营业统计======================================================================================================================================
    items.clear();
    items << new QStandardItem(itemtextspan(tr("营业统计")));
    items << new QStandardItem(tr("总消费金额"));
    items << report_standmodel::newItem((ldec_cost));
    items << report_standmodel::newItem((ldec_cost_m));
    items << report_standmodel::newItem((ldec_cost_m==0? 0.0: (ldec_cost*1.0)/(ldec_cost_m*1.0) * 100));
    standmodel->appendRow(items);

    query_c_series.execSelect("select ch_dish_typeno , vch_dish_typename   from cey_bt_dish_type order by ch_dish_typeno");
    while(query_c_series.next()){//0
        ls_typeno=query_c_series.recordValue("ch_dish_typeno").toString();
        ls_typename=query_c_series.recordValue("vch_dish_typename").toString();

        query.execSelect(QString("Select IFNULL(sum(( a.num_num - a.num_back)*a.num_price + a.num_price_add) , 0)  from cey_u_orderdish a , cey_bt_dish b , cey_u_checkout_master c    where a.ch_payno =c.ch_payno and c.ch_state ='Y' and b.ch_dish_typeno = '%1' and a.ch_dishno =b.ch_dishno and c.dt_operdate >= '%2' and c.dt_operdate < '%3'")
                         .arg(ls_typeno)
                         .arg(ls_begin)
                         .arg(ls_end));
        query.next();
        ldec_type=query.recordValue(0).toDouble();

        query.execSelect(QString("Select IFNULL(sum(( a.num_num - a.num_back)* a.num_price + a.num_price_add) , 0)  from cey_u_orderdish a , cey_bt_dish b , cey_u_checkout_master c    where a.ch_payno =c.ch_payno and c.ch_state ='Y' and b.ch_dish_typeno = '%1' and a.ch_dishno =b.ch_dishno and c.dt_operdate >= '%2' and c.dt_operdate < '%3' ")
                         .arg(ls_typeno)
                         .arg(ls_begin)
                         .arg(ls_end));
        query.next();
        ldec_type_m=query.recordValue(0).toDouble();

        if(ldec_type==0 && ldec_type_m==0){

        } else {
            items.clear();
            items << new QStandardItem(itemtextspan(tr("营业统计")));
            items << new QStandardItem(" **"+ls_typename);
            items << report_standmodel::newItem((ldec_type));
            items << report_standmodel::newItem((ldec_type_m));
            items << report_standmodel::newItem((ldec_type_m==0? 0.0: (ldec_type*1.0)/(ldec_type_m*1.0) * 100));
            standmodel->appendRow(items);
        }
    }//0
    query_c_series.clear();


    items.clear();
    items << new QStandardItem(itemtextspan(tr("营业统计")));
    items << new QStandardItem(tr("折扣"));
    items << report_standmodel::newItem((ldec_discount));
    items << report_standmodel::newItem((ldec_discount_m));
    items << report_standmodel::newItem((ldec_discount_m==0? 0.0: (ldec_discount*1.0)/(ldec_discount_m*1.0) * 100));
    standmodel->appendRow(items);

    items.clear();
    items << new QStandardItem(itemtextspan(tr("营业统计")));
    items << new QStandardItem(tr("赠送金额"));
    items << report_standmodel::newItem((ldec_present));
    items << report_standmodel::newItem((ldec_present_m));
    items << report_standmodel::newItem((ldec_present_m==0? 0.0: (ldec_present*1.0)/(ldec_present_m*1.0) * 100));
    standmodel->appendRow(items);

    items.clear();
    items << new QStandardItem(itemtextspan(tr("营业统计")));
    items << new QStandardItem(tr("服务费"));
    items << report_standmodel::newItem((ldec_service));
    items << report_standmodel::newItem((ldec_service_m));
    items << report_standmodel::newItem((ldec_service_m==0? 0.0: (ldec_service*1.0)/(ldec_service_m*1.0) * 100));
    standmodel->appendRow(items);

    items.clear();
    items << new QStandardItem(itemtextspan(tr("营业统计")));
    items << new QStandardItem(tr("税率"));
    items << report_standmodel::newItem((ldec_tax));
    items << report_standmodel::newItem((ldec_tax_m));
    items << report_standmodel::newItem((ldec_tax_m==0? 0.0: (ldec_tax*1.0)/(ldec_tax_m*1.0) * 100));
    standmodel->appendRow(items);

    items.clear();
    items << new QStandardItem(itemtextspan(tr("营业统计")));
    items << new QStandardItem(tr("低消收入"));
    items << report_standmodel::newItem((ldec_lowcost));
    items << report_standmodel::newItem((ldec_lowcost_m));
    items << report_standmodel::newItem((ldec_lowcost_m==0? 0.0: (ldec_lowcost*1.0)/(ldec_lowcost_m*1.0) * 100));
    standmodel->appendRow(items);

    items.clear();
    items << new QStandardItem(itemtextspan(tr("营业统计")));
    items << new QStandardItem(tr("应收金额"));
    items << report_standmodel::newItem((ldec_cash));
    items << report_standmodel::newItem((ldec_cash_m));
    items << report_standmodel::newItem((ldec_cash_m==0? 0.0: (ldec_cash*1.0)/(ldec_cash_m*1.0) * 100));
    standmodel->appendRow(items);

    query.execSelect(QString("select ifnull(sum(num_deposit),0), ifnull(sum(num_realamount),0) from t_m_deposit   where ch_deposit_mode='1' and  ( dt_operdate >= '%1' and dt_operdate <= '%2' )")
                     .arg(ls_begin)
                     .arg(ls_end));
    query.next();
    items.clear();
    items << new QStandardItem(itemtextspan(tr("营业统计")));
    items << new QStandardItem(tr("会员充值"));
    items << new QStandardItem(query.recordValue(0).toString());
    items << new QStandardItem;
    items << new QStandardItem;
    if(query.recordValue(0).toFloat() != 0){
        standmodel->appendRow(items);
    }

    items.clear();
    items << new QStandardItem(itemtextspan(tr("营业统计")));
    items << new QStandardItem(tr("会员充值实收"));
    items << new QStandardItem(query.recordValue(1).toString());
    items << new QStandardItem;
    items << new QStandardItem;
    if(query.recordValue(1).toFloat() != 0){
        standmodel->appendRow(items);
    }

    if(lds::sysconf->value("w_sys_manage_cloudsync_with_time/cloud_yun_is_run", false).toBool()) {
        bool ok;
        m_rfb=
                w_sys_manage_cloudsync::get_yun_member_r_f_b(this, public_sql::gs_operid,ls_begin, ls_begin,ok);
        if(m_rfb.rechargeAmount.toFloat() != 0){
            items.clear();
            items << new QStandardItem(itemtextspan(tr("营业统计")));
            items << new QStandardItem(tr("会员充值")+"("+tr("云")+")");
            items << new QStandardItem(m_rfb.rechargeAmount);
            items << new QStandardItem;
            items << new QStandardItem;
            standmodel->appendRow(items);
        }
        if(m_rfb.realRechargeAmount.toFloat() != 0){
            items.clear();
            items << new QStandardItem(itemtextspan(tr("营业统计")));
            items << new QStandardItem(tr("会员充值实收")+"("+tr("云")+")");
            items << new QStandardItem(m_rfb.realRechargeAmount);
            items << new QStandardItem;
            items << new QStandardItem;
            standmodel->appendRow(items);
        }
    }

    query.execSelect(QString("Select (select aa.vch_paymodename from cey_bt_paymode aa where aa.ch_paymodeno=a.ch_paymodeno) as paymodename ,a.ch_paymodeno,"
                             " count(a.ch_paymodeno) as count, sum(a.num_realamount) as realamount, sum(a.num_face) as face from cey_u_checkout_detail a ,"
                             " cey_u_checkout_master b where ( a.ch_payno =b.ch_payno ) and ( b.ch_state ='Y' ) and  ( b.dt_operdate >= '%1' "
                             " and b.dt_operdate <= '%2' ) and"
                             " a.ch_paymodeno in('02','10')  Group by a.ch_paymodeno")
                     .arg(ls_begin)
                     .arg(ls_end)
                     );
    while(query.next()){
        items.clear();
        items << new QStandardItem(itemtextspan(tr("营业统计")));
        items << new QStandardItem(query.recordValue("paymodename").toString());
        items << new QStandardItem(query.recordValue("realamount").toString());
        items << new QStandardItem;
        items << new QStandardItem;
        if(query.recordValue("realamount").toFloat() != 0){
            standmodel->appendRow(items);
        }
    }


    query_c_pay.execSelect(QString("Select a.ch_paymodeno , IFNULL(c.vch_paymodename , a.ch_paymodeno), IFNULL(c.ch_incomeflag , 'Y'), sum(a.num_realamount)   from cey_u_checkout_detail a , cey_u_checkout_master b , cey_bt_paymode c    where a.ch_paymodeno =c.ch_paymodeno and a.ch_payno =b.ch_payno and(b.ch_state ='Y')and b.dt_operdate >= '%1' and b.dt_operdate <= '%2'  group by a.ch_paymodeno , IFNULL(c.vch_paymodename , a.ch_paymodeno), IFNULL(c.ch_incomeflag , 'Y')   having sum(a.num_realamount)<> 0    order by a.ch_paymodeno")
                           .arg(ls_begin)
                           .arg(ls_end)
                           );
    while(query_c_pay.next()){//0
        ls_paymodeno=query_c_pay.recordValue(0).toString();
        ls_paymodename=query_c_pay.recordValue(1).toString();
        ls_incomeflag=query_c_pay.recordValue(2).toString();
        ldec_cash_m=query_c_pay.recordValue(3).toDouble();

        ldec_cash=0.0;
        query.execSelect(QString("select sum(a.num_realamount)from cey_u_checkout_detail a , cey_u_checkout_master b  where a.ch_payno =b.ch_payno and a.ch_paymodeno ='%1' and(b.ch_state ='Y')and b.dt_operdate >= '%2' and b.dt_operdate <= '%3'")
                         .arg(ls_paymodeno)
                         .arg(ls_begin)
                         .arg(ls_end)
                         );
        query.next();
        ldec_cash=query.recordValue(0).toDouble();
        if(ls_incomeflag=="Y"){
            ldec_total_m += ldec_cash_m;
            ldec_total += ldec_cash;
        }
        ldec_total1 += ldec_cash;
        ldec_total1_m += ldec_cash_m;

        items.clear();
        items << new QStandardItem(itemtextspan(tr("营业统计")));
        items << new QStandardItem("  **" + ls_paymodename);
        items << report_standmodel::newItem((ldec_cash));
        items << report_standmodel::newItem((ldec_cash_m));
        items << report_standmodel::newItem((ldec_cash_m==0? 0.0: (ldec_cash*1.0)/(ldec_cash_m*1.0) * 100));
        standmodel->appendRow(items);
    }//0
    //营业结算======================================================================================================================================
    ll_row=standmodel->rowCount();
    if(ll_row > 0){
        query.execSelect(QString("select sum(num_deposit) from t_m_deposit  where ch_deposit_mode <> '4' and dt_operdate >= '%1' and dt_operdate <= '%2' ")
                         .arg(ls_begin)
                         .arg(ls_end));
        query.next();
        ldec_deposit=query.recordValue(0).toDouble();

        query.execSelect(QString("select sum(num_deposit) from t_m_deposit   where ch_deposit_mode <> '4' and dt_operdate >= '%1' and dt_operdate <= '%2'")
                         .arg(ls_begin)
                         .arg(ls_end));
        query.next();
        ldec_deposit_m=query.recordValue(0).toDouble();

        if(ldec_deposit > 0 && ldec_deposit_m > 0){
            items.clear();
            items << new QStandardItem(tr("营业结算"));
            items << new QStandardItem(tr("充值卡充值额"));
            items << report_standmodel::newItem((ldec_deposit));
            items << report_standmodel::newItem((ldec_deposit_m));
            items << report_standmodel::newItem((ldec_deposit_m==0? 0.0: (ldec_deposit*1.0)/(ldec_deposit_m*1.0) * 100));
            standmodel->appendRow(items);
        }
    }
    if(ll_row > 0){
        ldec_total1_m = ldec_total1_m + ldec_free_m + ldec_blotout_m;
        ldec_total1 = ldec_total1 + ldec_free + ldec_blotout;

        items.clear();
        items << new QStandardItem(itemtextspan(tr("营业结算")));
        items << new QStandardItem(tr("实收金额"));
        items << report_standmodel::newItem((ldec_total1));
        items << report_standmodel::newItem((ldec_total1_m));
        items << report_standmodel::newItem((ldec_total1_m==0? 0.0: (ldec_total1*1.0)/(ldec_total1_m*1.0) * 100));
        standmodel->appendRow(items);
    }
    if(ll_row > 0){
        ls_rate = "0%";
        if(ldec_total_m!=0){
            ls_rate=QString::number(ldec_total/ldec_total_m * 100);
        }

        items.clear();
        items << new QStandardItem(itemtextspan(tr("营业结算")));
        items << new QStandardItem(tr("实际收入"));
        items << report_standmodel::newItem((ldec_total));
        items << report_standmodel::newItem((ldec_total_m));
        items << report_standmodel::newItem((ldec_total_m==0? 0.0: (ldec_total*1.0)/(ldec_total_m*1.0) * 100));
        standmodel->appendRow(items);
    }
    ui->tableView->resizeColumnsToContents();
}

void w_rpt_report_daybusinessreprot::toexit()
{
    this->reject();
}

void w_rpt_report_daybusinessreprot::updatecombegindate(int yearindex)
{
    ui->spinBox_beginday->setRange(0, 99);
    switch(yearindex){
    case 0://上月
        ui->spinBox_beginday->setMinimum(16);
        break;
    case 1://号
        ui->spinBox_beginday->setMaximum(15);
        break;
    }
}

void w_rpt_report_daybusinessreprot::savebeginset()
{

    lds::sysconf->setValue("w_rpt_report_daybusinessreprot/rpt_month", ui->comboBox_beginmonth->currentIndex());
    lds::sysconf->setValue("w_rpt_report_daybusinessreprot/rpt_day", ui->spinBox_beginday->value());
    lds::sysconf->sync();
}

void w_rpt_report_daybusinessreprot::loadbeginset()
{

    ui->comboBox_beginmonth->setCurrentIndex(lds::sysconf->value("w_rpt_report_daybusinessreprot/rpt_month").toInt());
    ui->spinBox_beginday->setValue(lds::sysconf->value("w_rpt_report_daybusinessreprot/rpt_day").toInt());

    //直接更新range
    updatecombegindate(ui->comboBox_beginmonth->currentIndex());
}

QString w_rpt_report_daybusinessreprot::itemtextspan(const QString &itemtext)
{
    for(int r=0,r_count=standmodel->rowCount(); r<r_count;r++){
        if(standmodel->item(r, 0)->text() == itemtext){
            return "";
        }
    }
    return itemtext;
}

void w_rpt_report_daybusinessreprot::toprint()
{
    if(standmodel->rowCount() == 0){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("没有需要打印的数据"));
        return;
    }
    blockIns printins;
    QAbstractItemModel*model = standmodel;
    int printrow, printcolumn;//打印的行数 列数;
    QVector<int> columnwidth;
    columnwidth << 12 << 22 << 8;
    printrow = model->rowCount();
    printcolumn = 3;

    printins.append(backheader::printer.drawText("a00,s11",  this->windowTitle() +"\n","a10,s00"));
    printins.append(backheader::printer.drawText("", tr("操作员")+":"+public_sql::gs_operid+"\n"));
    printins.append(backheader::printer.drawText("", tr("时间")+":"+n_func::f_get_sysdatetime_str()+"\n"));

    for(int r = 0; r < printrow; r++){
        for(int c = 0; c < printcolumn; c++){
            QString data = model->data(model->index(r , c)).toString();
            if(!data.isEmpty()){
                switch(c) {
                case 0:
                    printins.append(backheader::printer.drawText("1B4501", data, "1B4500", columnwidth[c]));
                    printins.append(backheader::printer.drawText("", "-----------------\n"));
                    break;
                case 1:
                    printins.append(backheader::printer.drawText("", data, columnwidth[c]));
                    break;
                case 2:
                    printins.append(backheader::printer.drawText("", ui->tableView->dg_paint_transfer(r, c), columnwidth[c], Qt::AlignRight));
                    break;
                }
            }
        }
        printins.append(backheader::printer.drawText("", "\n"));
    }

    printins.append(backheader::printer.drawTextEnd("1B6405 1D5601", ""));//走纸5x， 切纸
    printins = backheader::printer.setPrintCount(printins, 1);

    backheader::asyn_dataprint(printins);


}
