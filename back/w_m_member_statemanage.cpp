
#include "w_m_member_statemanage.h"
//会员状态管理
#include "ui_w_m_member_statemanage_dialog.h"
#include <QSqlError>
#include <QSqlRecord>
#include "lds_tableview_repecify.h"
#include "w_m_member.h"
#include "w_m_member_list.h"
#include "n_func.h"
#include "lds_messagebox.h"
#include "lds_query.h"
#include "uo_rw_member.h"
#include "backheader.h"
#include "lds_tableview_repecify.h"
#include "lds_tableview_delegate_check.h"
#include "w_scr_dish_m_hang_pay_dialog.h"
#include "w_sys_manage_cloudsync_with_time.h"
#include "lds.h"
#include "lds_model_sqltablemodel_delegate_com.h"


w_m_member_statemanage::w_m_member_statemanage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui_w_m_member_statemanage_Dialog)
{
    ui->setupUi(this);
    //! init
    //! tableview
    tablemodel=new lds_model_sqltablemodel(this);
    tablemodel->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    tablemodel->setTable("t_m_member");
    tablemodel->save_set_header("vch_memberno",tr("会员编号"));
    tablemodel->save_set_header("vch_member",tr("会员名称"));
    tablemodel->save_set_header("ch_typeno",tr("会员类型"));
    tablemodel->save_set_header("dt_birthday",tr("生日"));
    tablemodel->save_set_header("vch_handtel",tr("手机"));
    tablemodel->save_set_header("vch_tel",tr("电话"));
    tablemodel->save_set_header("ch_state",tr("状态"));
    tablemodel->save_set_header("ch_cardflag",tr("持卡"));
    tablemodel->save_set_header("dt_limit",tr("有效日期"));
    tablemodel->save_set_header("vch_operID",tr("创建人员"));

    ui->tableView->setEditTriggers(QTableView::AllEditTriggers);
    ui->tableView->setModel(tablemodel);
    ui->tableView->rePecifyHeader( tablemodel->get_save_set_header_saver_column_list());
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("ch_cardflag"),
                                            new lds_tableview_delegate_check(tablemodel->fieldIndex("ch_cardflag") , ui->tableView));
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("ch_state"),
                                            new lds_model_sqltablemodel_delegate_com(this, &w_m_member::get_VIP_MODEL_STATE()));
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("ch_typeno"),
                                            new lds_model_sqltablemodel_delegate_com(this, tablemodel, "t_m_member_type", "ch_typeno", "vch_typename"));
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("vch_operID"),
                                            new lds_model_sqltablemodel_delegate_com(this, tablemodel, "("+backheader::SELECT_OPERID_NAME + ")t", "vch_operID", "vch_operator"));
    //! toolbar
    ui->widget->setup(QStringList()
                      << tr("刷新") << tr("详情")
                      << tr("冻结") << tr("挂失") << tr("作废") << tr("启用")
                      << tr("退现") << tr("取款") << tr("退卡") << tr("补卡")
                      << tr("挂账付款")
                      << tr("退出")
                      );
    connect(ui->widget->index_widget(tr("刷新")), SIGNAL(clicked()),this,SLOT(torefresh()));
    connect(ui->widget->index_widget(tr("详情")), SIGNAL(clicked()),this,SLOT(todetail()));
    connect(ui->widget->index_widget(tr("冻结")), SIGNAL(clicked()),this,SLOT(tostop()));
    connect(ui->widget->index_widget(tr("挂失")), SIGNAL(clicked()),this,SLOT(toloss()));
    connect(ui->widget->index_widget(tr("作废")), SIGNAL(clicked()),this,SLOT(toblank()));
    connect(ui->widget->index_widget(tr("启用")), SIGNAL(clicked()),this,SLOT(toopen()));
    connect(ui->widget->index_widget(tr("退现")), SIGNAL(clicked()),this,SLOT(toblackcash()));
    connect(ui->widget->index_widget(tr("取款")), SIGNAL(clicked()),this,SLOT(tofetch()));
    connect(ui->widget->index_widget(tr("退卡")), SIGNAL(clicked()),this,SLOT(toblankcard()));
    connect(ui->widget->index_widget(tr("补卡")), SIGNAL(clicked()),this,SLOT(tonewsend()));
    connect(ui->widget->index_widget(tr("退出")), SIGNAL(clicked()),this,SLOT(toexit()));
    connect(ui->widget->index_widget(tr("挂账付款")), SIGNAL(clicked()),this,SLOT(tohangpay()));
    connect(ui->lineEdit,SIGNAL(textChanged(QString)),this,SLOT(treeselect()));
    connect(ui->lineEdit_2,SIGNAL(textChanged(QString)),this,SLOT(treeselect()));

    tablemodel_arg2 = " ( vch_memberno like '%1%' or vch_member like '%1%' )and(ifnull(vch_tel, '') like '%2%' or ifnull(vch_handtel, '') like '%2%' ) ";
    //! refresh
    QTimer::singleShot(0, this, SLOT(torefresh()));
}


lds_tableView *w_m_member_statemanage::getTableview()
{
    return ui->tableView;
}

lds_model_sqltablemodel *w_m_member_statemanage::getTablemodel()
{
    return tablemodel;
}

void w_m_member_statemanage::torefresh()
{
    //yun
    w_sys_manage_cloudsync_with_time_running::exec_no_up_then_down_byconf(this);
    ui->widget->index_widget(tr("退出"))->show_flag_top_right(w_sys_manage_cloudsync_with_time_running::q_bt_get_ready_total_byconf());
    //~yun

    treeselect();
}

void w_m_member_statemanage::treeselect()
{
    tablemodel->setFilter(tablemodel_arg2.arg(ui->lineEdit->text()).arg(ui->lineEdit_2->text()));
    ui->tableView->restore();
    ui->tableView->resizeColumnsToContents();
}

void w_m_member_statemanage::todetail()
{
    w_m_member_list::statictosendcard(this, tablemodel, ui->tableView, false, tr("会员卡详情"));
}

void w_m_member_statemanage::tostop()
{
    //ue_stop (none) returns (none)
    long ll_row;
    QString ls_memberno;
    QString ls_state;
    QString ls_memo;
    QString ls_err;
    QString ls_save_hq;
    QDateTime ldt;
    QDateTime ldt_old;


    ll_row = ui->tableView->currentIndex().row();
    if(ll_row<0){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("没有选中需要止付的会员"));
        return;
    }
    ls_memberno=tablemodel->model_data(ll_row, "vch_memberno").toString();
    ls_state=tablemodel->model_data(ll_row, "ch_state").toString();
    //    items.clear();items<<new QStandardItem("1")<<new QStandardItem(tr("正常"));VIP_MODEL_STATE.appendRow(items);
    //    items.clear();items<<new QStandardItem("2")<<new QStandardItem(tr("止付"));VIP_MODEL_STATE.appendRow(items);
    //    items.clear();items<<new QStandardItem("3")<<new QStandardItem(tr("挂失"));VIP_MODEL_STATE.appendRow(items);
    //    items.clear();items<<new QStandardItem("4")<<new QStandardItem(tr("作废"));VIP_MODEL_STATE.appendRow(items);
    if(ls_state=="2"){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("指定会员状态已为止付"));
        return;

    } else if(ls_state=="4"){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("指定会员状态为作废，不能止付"));
        return;
    }
    w_m_member_statemanage_state dialog(this);
    dialog.setTitile(tr("会员号:")+ls_memberno+tr(",处理状态:止付"));
    dialog.textkey=tr("止付");
    dialog.setWindowTitle(_TEXT_SLOT(this));
    if(lds_roundeddialog_rect_align(&dialog).exec()==QDialog::Rejected) return;
    ls_memo=dialog.getExplain();

    if(!ls_memo.isEmpty()){
    }
    if(ls_memo=="OK")ls_memo="";
    ldt = n_func::f_get_sysdatetime();
    ldt_old=tablemodel->model_data(ll_row, "dt_operdate").toDateTime();
    if(ldt_old.isValid()  &&  ldt_old > ldt){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("系统时间小于上次保存时间，请确认系统时间是否正确"));
        return;

    }
    //! <--transaction begin-->
    lds_query::tran_saction();
    lds_query query;
    switch(0){
    case 0:
        ls_err=n_func::f_m_insert_log(ls_memberno,tr("止付"),ls_memo,0);
        if(!ls_err.isEmpty()){
            break;
        }
        if(!query.execUpdate("t_m_member",qrtVariantPairList()
                             << qrtVariantPair("ch_state", 2)
                             << qrtVariantPair("dt_operdate", ldt),
                             QString("vch_memberno = '%1' ").arg(ls_memberno),
                             &ls_err
                             )) {
            break;
        }
    }
    if(ls_err.isEmpty()){
        lds_query::com_mit();
        ls_err=tr("状态变更成功");

    } else {

        lds_query::roll_back();
        ls_err=tr("状态变更失败");

    }
    //! <--transaction end-->
    torefresh();
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID,ls_err);

}

void w_m_member_statemanage::toloss()
{
    //ue_loss (none) returns (none)
    long ll_row;
    QString ls_memberno;
    QString ls_state;
    QString ls_err;
    QString ls_memo;
    QDateTime ldt;
    QDateTime ldt_old;
    ll_row = ui->tableView->currentIndex().row();
    if(ll_row<0){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("没有选中需要挂失会员"));
        return;
    }
    ls_memberno=tablemodel->model_data(ll_row, "vch_memberno").toString();
    ls_state=tablemodel->model_data(ll_row, "ch_state").toString();
    //    items.clear();items<<new QStandardItem("1")<<new QStandardItem(tr("正常"));VIP_MODEL_STATE.appendRow(items);
    //    items.clear();items<<new QStandardItem("2")<<new QStandardItem(tr("止付"));VIP_MODEL_STATE.appendRow(items);
    //    items.clear();items<<new QStandardItem("3")<<new QStandardItem(tr("挂失"));VIP_MODEL_STATE.appendRow(items);
    //    items.clear();items<<new QStandardItem("4")<<new QStandardItem(tr("作废"));VIP_MODEL_STATE.appendRow(items);
    if(ls_state=="3"){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("指定会员状态已为挂失"));
        return;

    } else if(ls_state=="4"){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("指定会员状态为作废，不能止付"));
        return;
    }
    w_m_member_statemanage_state dialog(this);
    dialog.setTitile(tr("会员号:")+ls_memberno+tr(",处理状态:挂失"));
    dialog.textkey=tr("挂失");
    dialog.setWindowTitle(_TEXT_SLOT(this));
    if(lds_roundeddialog_rect_align(&dialog).exec()==QDialog::Rejected) return;
    ls_memo=dialog.getExplain();

    if(!ls_memo.isEmpty()){
    }
    if(ls_memo=="OK")ls_memo="";
    ldt = n_func::f_get_sysdatetime();
    ldt_old=tablemodel->model_data(ll_row, "dt_operdate").toDateTime();
    if(ldt_old.isValid()  &&  ldt_old > ldt){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("系统时间小于上次保存时间，请确认系统时间是否正确"));
        return;
    }

    //! <--transaction begin-->
    lds_query::tran_saction();
    lds_query query;
    switch(0){
    case 0:
        ls_err=n_func::f_m_insert_log(ls_memberno,tr("挂失"),ls_memo,0);
        if(!ls_err.isEmpty()){
            break;
        }
        if(!query.execUpdate("t_m_member",qrtVariantPairList()

                             << qrtVariantPair("ch_state", 3)
                             << qrtVariantPair("dt_operdate", ldt),
                             QString(" vch_memberno = '%1' ").arg(ls_memberno),
                             &ls_err
                             )){
            break;
        }
    }
    if(ls_err.isEmpty()){
        lds_query::com_mit();
        ls_err=tr("状态变更成功");

    } else {

        lds_query::roll_back();
        ls_err=tr("状态变更失败");

    }
    //! <--transaction end-->
    torefresh();
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID,ls_err);
}

void w_m_member_statemanage::toblank()
{
    //ue_blank (none) returns (none)
    long ll_row;
    QString ls_memberno;
    QString ls_err;
    //    st_member lst_member;
    uo_rw_member::read_card_type lst_card;
    uo_rw_member uo_member;
    QDateTime ldt;
    QDateTime ldt_old;
    QString ls_type;
    QString ls_bute;
    QString ls_cardflag;
    QString ls_state;
    QString ls_memo;
    double ldec_base=0;

    ll_row = ui->tableView->currentIndex().row();
    if(ll_row<0){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("没有选中需要作废会员"));
        return;
    }
    ls_memberno=tablemodel->model_data(ll_row, "vch_memberno").toString();
    ls_state=tablemodel->model_data(ll_row, "ch_state").toString();
    ls_cardflag=tablemodel->model_data(ll_row, "ch_cardflag").toString();
    //    items.clear();items<<new QStandardItem("1")<<new QStandardItem(tr("正常"));VIP_MODEL_STATE.appendRow(items);
    //    items.clear();items<<new QStandardItem("2")<<new QStandardItem(tr("止付"));VIP_MODEL_STATE.appendRow(items);
    //    items.clear();items<<new QStandardItem("3")<<new QStandardItem(tr("挂失"));VIP_MODEL_STATE.appendRow(items);
    //    items.clear();items<<new QStandardItem("4")<<new QStandardItem(tr("作废"));VIP_MODEL_STATE.appendRow(items);
    if(ls_state=="4"){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("指定会员状态已为作废"));
        return;

    } else if(ls_state=="2"){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("止付卡不能作废"));
        return;
    }
    if(ls_cardflag!="Y"){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("该会员卡还未制卡，不需要作废"));
        return;
    }

    //获取会员详细数据
    uo_member.of_init(this, "4");
    lst_card=uo_member.of_read_card(this, ls_memberno);
    if(lst_card.flag == false){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,lst_card.errtext);
        return;
    }
    lds_query query;
    query.execSelect(QString("select int_basedata,ch_typeno from t_m_member where vch_memberno='%1'").arg(ls_memberno));
    if(query.next()){

    } else {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("查询会员保本金额失败"));
        return;

    }
    ldec_base=query.recordValue("int_basedata").toDouble();
    ls_type=query.recordValue("ch_typeno").toString();

    query.execSelect(QString("select ch_attribute from t_m_member_type where ch_typeno = '%1' ")
                     .arg(ls_type));
    if(query.next()){
        ls_bute=query.recordValue("ch_attribute").toString();
    } else {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("查询会员类型失败"));
        return;
    }
    if(ls_bute!="1"){
        if(n_func::f_m_get_parameter(this,"m_fetch_base","0")=="1"){
            if(lst_card.amount>0){
                lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("卡余额(包含保本金额,保本金额可取)不为零,不能作废,~r~n可进行取款操作，待余额为零再作废"));
                return;
            }
            ldec_base=0;
        } else {
            if(lst_card.amount>ldec_base){
                lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("卡余额(不包含保本金额,保本金额不可取)不为零,不能作废,~r~n可进行取款操作，待余额为零再作废"));
                return;
            }
        }
    }
    w_m_member_statemanage_state dialog(this);
    dialog.setTitile(tr("会员号:")+ls_memberno+tr(",处理状态:作废"));
    dialog.textkey=tr("作废");
    dialog.setWindowTitle(_TEXT_SLOT(this));
    if(lds_roundeddialog_rect_align(&dialog).exec()==QDialog::Rejected) return;
    ls_memo=dialog.getExplain();
    if(!ls_memo.isEmpty()){
    }
    if(ls_memo=="OK")ls_memo="";
    ldt = n_func::f_get_sysdatetime();
    ldt_old=tablemodel->model_data(ll_row, "dt_operdate").toDateTime();
    if(ldt_old.isValid()  &&  ldt_old > ldt){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("系统时间小于上次保存时间，请确认系统时间是否正确"));
        return;
    }

    //! <--transaction begin-->
    lds_query::tran_saction();
    switch(0){
    case 0:
        ls_err=n_func::f_m_insert_log(ls_memberno,tr("作废"),ls_memo,0);
        if(!ls_err.isEmpty()){
            break;
        }
        if(!query.execUpdate("t_m_member",qrtVariantPairList()

                             << qrtVariantPair("ch_state", 4)
                             << qrtVariantPair("dt_operdate", ldt),
                             QString(" vch_memberno = '%1' ").arg(ls_memberno),
                             &ls_err)) {
            break;
        }
    }
    if(ls_err.isEmpty()){
        lds_query::com_mit();
        ls_err=tr("状态变更成功");

    } else {

        lds_query::roll_back();
        ls_err=tr("状态变更失败");

    }
    //! <--transaction end-->
    torefresh();
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID,ls_err);

}

void w_m_member_statemanage::toopen()
{
    //ue_loss (none) returns (none)
    long ll_row;
    QString ls_memberno;
    QString ls_state;
    QString ls_err;
    QString ls_memo;
    QDateTime ldt;
    QDateTime ldt_old;
    ll_row = ui->tableView->currentIndex().row();
    if(ll_row<0){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("没有选中需要启用会员"));
        return;
    }
    ls_memberno=tablemodel->model_data(ll_row, "vch_memberno").toString();
    ls_state=tablemodel->model_data(ll_row, "ch_state").toString();
    //    items.clear();items<<new QStandardItem("1")<<new QStandardItem(tr("正常"));VIP_MODEL_STATE.appendRow(items);
    //    items.clear();items<<new QStandardItem("2")<<new QStandardItem(tr("止付"));VIP_MODEL_STATE.appendRow(items);
    //    items.clear();items<<new QStandardItem("3")<<new QStandardItem(tr("挂失"));VIP_MODEL_STATE.appendRow(items);
    //    items.clear();items<<new QStandardItem("4")<<new QStandardItem(tr("作废"));VIP_MODEL_STATE.appendRow(items);
    if(ls_state=="1"){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("指定会员状态已为正常"));
        return;

    } else if(ls_state=="4"){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("指定会员状态为作废，不能启用"));
        return;
    }
    w_m_member_statemanage_state dialog(this);
    dialog.setTitile(tr("会员号:")+ls_memberno+tr(",处理状态:启用"));
    dialog.textkey=tr("启用");
    dialog.setWindowTitle(_TEXT_SLOT(this));
    if(lds_roundeddialog_rect_align(&dialog).exec()==QDialog::Rejected) return;
    ls_memo=dialog.getExplain();

    if(!ls_memo.isEmpty()){
    }
    if(ls_memo=="OK")ls_memo="";
    ldt = n_func::f_get_sysdatetime();
    ldt_old=tablemodel->model_data(ll_row, "dt_operdate").toDateTime();
    if(ldt_old.isValid()  &&  ldt_old > ldt){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("系统时间小于上次保存时间，请确认系统时间是否正确"));
        return;
    }

    //! <--transaction begin-->
    lds_query::tran_saction();
    lds_query query;
    switch(0){
    case 0:
        ls_err=n_func::f_m_insert_log(ls_memberno,tr("启用"),ls_memo,0);
        if(!ls_err.isEmpty()){
            break;
        }

        if(!query.execUpdate("t_m_member",qrtVariantPairList()

                             << qrtVariantPair("ch_state", 1)
                             << qrtVariantPair("dt_operdate", ldt),
                             QString(" vch_memberno = '%1' ").arg(ls_memberno),
                             &ls_err)) {
            break;
        }
    }
    if(ls_err.isEmpty()){
        lds_query::com_mit();
        ls_err=tr("状态变更成功");

    } else {

        lds_query::roll_back();
        ls_err=tr("状态变更失败");

    }
    //! <--transaction end-->
    torefresh();
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID,ls_err);
}

void w_m_member_statemanage::toblackcash()
{
    //ue_blank_cash (none) returns (none)
    long ll_row;
    QString ls_memberno;
    QString ls_err;
    n_func::st_member_type  lst_member;
    uo_rw_member::read_card_type lst_card;
    uo_rw_member uo_member;
    QDateTime ldt;
    QDateTime ldt_old;
    double ldec_bcash;

    ll_row = ui->tableView->currentIndex().row();
    if(ll_row<0){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("没有选中需要退现会员"));
        return;
    }
    ls_memberno=tablemodel->model_data(ll_row, "vch_memberno").toString();
    lst_member = n_func::f_m_get_limit(this, ls_memberno);
    //    items.clear();items<<new QStandardItem("1")<<new QStandardItem(tr("正常"));VIP_MODEL_STATE.appendRow(items);
    //    items.clear();items<<new QStandardItem("2")<<new QStandardItem(tr("止付"));VIP_MODEL_STATE.appendRow(items);
    //    items.clear();items<<new QStandardItem("3")<<new QStandardItem(tr("挂失"));VIP_MODEL_STATE.appendRow(items);
    //    items.clear();items<<new QStandardItem("4")<<new QStandardItem(tr("作废"));VIP_MODEL_STATE.appendRow(items);


    if(lst_member.as_m_state != "3"){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("退现功能只针对挂失状态会员，操作中止"));
        return;
    }
    if(lst_member.as_m_att != "2"){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("退现功能只针对充值类会员，操作中止"));
        return;
    }

    ldt = n_func::f_get_sysdatetime();
    ldt_old=tablemodel->model_data(ll_row, "dt_operdate").toDateTime();
    if(ldt_old.isValid()  &&  ldt_old > ldt){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("系统时间小于上次保存时间，请确认系统时间是否正确"));
        return;
    }

    //    uo_member = CREATE uo_rw_member
    uo_member.of_init(this, "4");
    lst_card = uo_member.of_read_card(this, ls_memberno);
    if(lst_card.flag == false){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,lst_card.errtext);
        return;
    }
    if(lst_card.flag){
        if(lst_card.amount==0){
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("会员当前余额为零，无需退现"));
            return;
        }
        ldec_bcash = lst_card.amount - lst_member.adec_m_limitdata;
        if(ldec_bcash <= 0){
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("会员当前无保本外可退现金额，操作中止"));
            return;
        }


        if(0==lds_messagebox::question(this, MESSAGE_TITLE_VOID, QString("%1[%2]%3[%4]%5")
                                       .arg(tr("确定要将会员"))
                                       .arg(lst_member.as_member)
                                       .arg(tr("账户余额"))
                                       .arg(QString().sprintf("%0.2f", ldec_bcash))
                                       .arg(tr("作退现处理吗？")),
                                       tr("确定"), tr("取消"))){
            //! <--transaction begin-->
            lds_query::tran_saction();
            ls_err= n_func::f_m_insert_log(ls_memberno,
                                           tr("退现"),
                                           tr("金额:") + QString().sprintf("%0.2f", ldec_bcash),
                                           0);
            if(ls_err.isEmpty()){
                ls_err=n_func::f_m_deposit(this, ls_memberno, - ldec_bcash, - ldec_bcash,lst_member.adec_m_limitdata,"5","5",tr("挂失退现转结"));
            }
            if(ls_err.isEmpty()){
                lds_query::com_mit();
            } else {
                lds_query::roll_back();
            }
            //! <--transaction end-->
            torefresh();
            if(ls_err.isEmpty()){
                lds_messagebox::information(this, MESSAGE_TITLE_VOID,tr("退现成功"));

            } else {
                lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("读取会员帐务数据失败，操作中止"));
            }
        }
    } else {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,lst_card.errtext);
    }
}

void w_m_member_statemanage::tofetch()
{
    //ue_fetch (none) returns (none)
    w_m_member_list::statictorecharge(this, tablemodel, ui->tableView,"C");
    torefresh();
}

void w_m_member_statemanage::toblankcard()
{
    //ue_blank_card (none) returns (none)
    w_m_member_list::statictorecharge(this, tablemodel, ui->tableView,"D");
    torefresh();
}

void w_m_member_statemanage::tonewsend()
{
    //ue_new_send (none) returns (none)
    long ll_row;
    QString ls_memberno;
    QString ls_err;
    QString ls_value;
    n_func::st_member_type  lst_member;
    uo_rw_member::read_card_type lst_card;
    uo_rw_member uo_member;
    QDateTime ldt;
    QDateTime ldt_old;
    double ldec_bcash;

    uo_member.of_init(this, "4");
    ll_row = ui->tableView->currentIndex().row();
    if(ll_row<0){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("没有选中需要补卡会员"));
        return;
    }
    ls_memberno=tablemodel->model_data(ll_row, "vch_memberno").toString();
    lst_member = n_func::f_m_get_limit(this, ls_memberno);

    if(lst_member.as_m_state != "3"){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("补卡功能只针对挂失状态会员，操作中止"));
        return;
    }
    if(lst_member.as_m_att != "2"){
        if(lst_member.as_m_att == "3"){
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("挂帐会员请结清帐务后，创建新会员并发卡"));
            return;
        } else {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("补卡功能只针对充值类会员，操作中止"));
            return;
        }
    }

    if(lst_member.as_m_cardflag!="Y"){
        ls_value = n_func::f_m_get_parameter(this,"vip_card_type",tr("无会员卡"));
        if(ls_value == tr("IC卡") || ls_value == tr("磁卡")){
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("会员卡%1还未发卡,不能补卡").arg(ls_memberno));
            return;
        }
    }

    ldt = n_func::f_get_sysdatetime();
    ldt_old = tablemodel->model_data(ll_row, "dt_operdate").toDateTime();
    if(ldt_old.isValid() && ldt_old > ldt){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("系统时间小于上次保存时间，请确认系统时间是否正确"));
        return;
    }
    lst_card = uo_member.of_read_card(this, ls_memberno);
    if(lst_card.flag == false){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,lst_card.errtext);
        return;
    }
    if(lst_card.flag){
        if(lst_card.amount==0){
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("会员当前余额为零，无需补卡，请直接创建新会员并发卡"));
            return;
        }
        ldec_bcash = lst_card.amount - lst_member.adec_m_limitdata;
        if(ldec_bcash <= 0){
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("会员当前无保本外可用金额，不能补卡"));
            return;
        }
        /*开始补卡*/
        w_m_member_statemanage_trans dialog(this);
        if(dialog.initData1()){
            dialog.setWindowTitle(_TEXT_SLOT(this));lds_roundeddialog_rect_align(&dialog).exec();
            torefresh();
        }
        return;
    }
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("读取会员帐务数据失败，操作中止"));
    return;
}

void w_m_member_statemanage::toexit()
{
    w_sys_manage_cloudsync_with_time_running::exec_keepdata_byconf(this);

    this->reject();
}

void w_m_member_statemanage::tohangpay()
{
    QPushButton *b0 = qobject_cast<QPushButton *>(this->sender());
    w_scr_dish_m_hang_pay_Dialog dialog(this);
    dialog.setWindowTitle(b0->text());
    lds_roundeddialog_rect_align(&dialog).exec();
}

//===============================
#include "ui_w_m_member_state_dialog.h"
w_m_member_statemanage_state::w_m_member_statemanage_state(QWidget *parent):
    QDialog(parent),ui(new Ui_w_m_member_state_Dialog)
{
    ui->setupUi(this);
    ui->widget->setup(QStringList()<<tr("确定")<<tr("取消"), Qt::AlignRight);
    connect(ui->widget->index_widget(tr("确定")),SIGNAL(clicked()),this,SLOT(took()));
    connect(ui->widget->index_widget(tr("取消")),SIGNAL(clicked()),this,SLOT(reject()));

}

void w_m_member_statemanage_state::setTitile(const QString &text)
{
    ui->label->setText(text);
}

QString w_m_member_statemanage_state::getExplain()
{
    return ui->lineEdit->text();
}

void w_m_member_statemanage_state::took()
{
    if(ui->lineEdit->text().isEmpty()){
        //        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("附加说明不能为空"));
        this->accept();
    } else  if(0==lds_messagebox::question(this, MESSAGE_TITLE_VOID,tr("确定指定会员")+textkey+"?", tr("确定"), tr("取消"))){
        this->accept();
    } else {
        this->reject();
    }
}

void w_m_member_statemanage_state::tocancel()
{
}

//===================================
#include "ui_w_m_member_trans_dialog.h"
w_m_member_statemanage_trans::w_m_member_statemanage_trans(QWidget *parent):
    QDialog(parent),
    ui(new Ui_w_m_member_trans_Dialog),
    _parent(static_cast<w_m_member_statemanage *>(parent))
{

    ui->setupUi(this);
    cb_read1=new uo_rw_member;
    cb_read2=new uo_rw_member;

    cb_read1->of_init(this, "1");
    cb_read2->of_init(this, "1");

    is_type=tr("补卡");
    is_lmemberno=_parent->getTablemodel()->model_data(_parent->getTableview()->currentIndex().row(), "vch_memberno").toString();
    //! toolbar
    ui->widget->setup(QStringList() << tr("补卡") << tr("退出"), Qt::AlignRight, lds_keytoolbutton::SYSKEY);
    connect(ui->widget->index_widget(tr("补卡")), SIGNAL(clicked()),this,SLOT(tonewcard()));
    connect(ui->widget->index_widget(tr("退出")), SIGNAL(clicked()),this,SLOT(toexit()));

}
w_m_member_statemanage_trans::~w_m_member_statemanage_trans()
{
    delete cb_read1;
    delete cb_read2;
}


void w_m_member_statemanage_trans::tonewcard()
{
    //clicked (none) returns long [pbm_bnclicked]
    //long ancestorreturnvalue
    uo_rw_member::read_card_type lst_card1;
    uo_rw_member::read_card_type lst_card2;
    uo_rw_member::read_card_type lst_card;
    n_func::st_member_type lst_member1;
    uo_rw_member uo_member;
    QString ls_memberno1;
    QString ls_memberno2;
    QString ls_err;
    QString ls_memo;
    QString ls_hq_save;
    QString ls_cardno;
    double ldec_trans;
    QDateTime ldt;


    uo_member.of_init(this, "4");
    ls_memberno1 = ui->lineEdit_oldvipno->text();
    ls_memberno2 = ui->lineEdit_newvipno->text();
    ls_cardno = ui->lineEdit_newcardno->text();
    if(ls_memberno1.isEmpty() || ls_memberno2.isEmpty()){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("请先输入新卡卡号"));
        return;
    }
    if(ls_cardno.trimmed().isEmpty()){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("请先输入新卡卡面卡号"));
        return;
    }

    ldec_trans = ui->lineEdit_tonewnum->value();
    if(ldec_trans <= 0){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("转入新卡金额为零"));
        return;
    }
    if(1==lds_messagebox::question(this, MESSAGE_TITLE_VOID,tr("确定要执行此%1操作吗?").arg(this->is_type), tr("确定"), tr("取消"))){
        return;
    }

    //! <--transaction begin-->
    lds_query::tran_saction();
    lds_query query;
    switch(0){//4
    case 0:
        if(ls_memberno1==ls_memberno2){
            ls_err=tr("转入库会员卡号不能与转出卡号相同");
            break;
        }

        if((ls_err=this->wf_addsendcard()).isEmpty()){//3
            lst_card1 = uo_member.of_read_card(this, ls_memberno1);
            if(lst_card1.flag == false){
                ls_err=lst_card1.errtext;
                break;
            }
            lst_card2 = uo_member.of_read_card(this, ls_memberno2);
            if(lst_card2.flag == false){
                ls_err=lst_card2.errtext;
                break;
            }
            lst_member1 = n_func::f_m_get_limit(this, ls_memberno1);

            if(ui->checkBox_istonewyajin->isChecked()){
                if(lst_card1.amount<ldec_trans){
                    ls_err=tr("原挂失会员余额不足以完成本次补卡操作");
                    break;
                }
            } else {
                if((lst_card1.amount - lst_member1.adec_m_limitdata) < ldec_trans){
                    ls_err=tr("转出会员余额") + QString().sprintf("%0.2f", lst_card1.amount) + tr("除保本金额") + QString().sprintf("%0.2f", lst_member1.adec_m_limitdata) + tr("外不足以完成本次转帐操作");
                    break;
                }
            }
            if((ls_err=n_func::f_m_deposit(this, ls_memberno1, - ldec_trans,0,lst_card1.amount - ldec_trans,"5","5",tr("挂失补卡转帐"))).isEmpty()){//3
                if((ls_err=n_func::f_m_deposit(this, ls_memberno2,ldec_trans,0,lst_card2.amount + ldec_trans,"5","5",tr("挂失补卡转帐"))).isEmpty()){//3
                    if(lst_card1.point !=0 ){//2
                        if(!query.execUpdate("t_m_curamount",qrtVariantPairList()

                                             << qrtVariantPair("num_point", lst_card1.point),
                                             QString(" vch_memberno = '%1' ").arg(ls_memberno2),
                                             &ls_err)) {
                            break;
                        }
                        if(!query.execInsert("t_m_point",qrtVariantPairList()

                                             << qrtVariantPair("vch_memberno", ls_memberno2)
                                             << qrtVariantPair("ch_type", 5)
                                             << qrtVariantPair("num_point", lst_card1.point)
                                             << qrtVariantPair("vch_operid", public_sql::gs_operid)
                                             << qrtVariantPair("dt_operdate", n_func::f_get_sysdatetime())

                                             << qrtVariantPair("vch_memo", tr("系统")+tr("补卡")),
                                             &ls_err)) {
                            break;
                        }
                    }//2
                    if((ls_err=cb_read2->of_send_card(this, ls_memberno2)).isEmpty()){//1
                        if((ls_err=n_func::f_m_insert_log(ls_memberno1,tr("补卡"),tr("目标卡号:") + ls_memberno2 + tr("转出金额:") + QString().sprintf("%0.2f", ldec_trans),0)).isEmpty()){//1
                            if((ls_err=n_func::f_m_insert_log(ls_memberno2,tr("补卡"),tr("原卡号:") + ls_memberno1 + tr("转入金额:") + QString().sprintf("%0.2f", ldec_trans),0)).isEmpty()){//1
                                /*success*/
                                break;
                            }//1
                        }//1
                    }//1
                    break;
                }//3
            }//3
        }//3
    }//4

    if(ls_err.isEmpty()) {
        lds_query::com_mit();
        ls_err=tr("%1成功").arg(this->is_type);
    } else {
        lds_query::roll_back();
    }
    //! <--transaction end-->

    _parent->torefresh();
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID,ls_err);
}

//    update t_m_curamount
//    set num_point =:lst_card1.point
//    where vch_memberno =:ls_memberno2
//    USING sqlca;
//    IF sqlca.sqlcode < 0 THEN //50
//    GOTO LABEL_KENSHU_74
//    END IF //50
//    iNSERT t_m_point(vch_memberno , ch_type , num_point , vch_operid , dt_operdate , vch_memo )
//    values(:ls_memberno2 , '5' , :lst_card1.point , '系统' , getdate() , '补卡' )
//    USING sqlca;
//    IF sqlca.sqlcode < 0 THEN //53
//    GOTO LABEL_KENSHU_74
//    END IF //53
//    END IF //48
//    IF  NOT (PARENT.cb_read2.of_send_card(this, ls_memberno2)) THEN //55
//    GOTO LABEL_KENSHU_74
//    END IF //55
//    f_m_insert_log(ls_memberno1,"补卡","目标卡号:" + ls_memberno2 + "转出金额:" + string(ldec_trans,"0"),0)
//    f_m_insert_log(ls_memberno2,"补卡","原卡号:" + ls_memberno1 + "转入金额:" + string(ldec_trans,"0"),0)
//    ELSE //47
//    f_m_insert_log(ls_memberno1,"转帐","目标卡号:" + ls_memberno2 + "转出金额:" + string(ldec_trans,"0"),0)
//    f_m_insert_log(ls_memberno2,"转帐","原卡号:" + ls_memberno1 + "转入金额:" + string(ldec_trans,"0"),0)
//    END IF //47
//    COMMIT USING sqlca;
//    PARENT.sle_1.text = "";PARENT.sle_2.text = ""
//    PARENT.sle_3.text = "";PARENT.sle_4.text = ""
//    PARENT.sle_5.text = "";PARENT.sle_6.text = ""
//    PARENT.sle_7.text = "";PARENT.sle_8.text = ""
//    PARENT.sle_amount.text = "0"
//    messagebox(gvar.gs_appname,PARENT.is_type + "成功")
//    IF PARENT.is_type = "补卡" THEN //69
//    PARENT.cb_4.EVENT clicked()
//    RETURN
//    END IF //69
//    GOTO LABEL_KENSHU_77
//    RETURN
//    END IF //45
//    END IF //43
//    LABEL_KENSHU_74:
//    IF ls_err = "" THEN ls_err = sqlca.sqlerrtext
//    ROLLBACK USING sqlca;
//    messagebox(gvar.gs_appname,PARENT.is_type + "操作失败 " + ls_err)
//    END IF //19
//    END IF //6
//    LABEL_KENSHU_77:
//    DESTROY uo_member
//    PARENT.sle_amount.setfocus()
//    IF PARENT.is_type = "补卡" THEN //79
//    PARENT.sle_amount.selecttext(1,len(PARENT.sle_amount.text))
//    END IF //79
//    RETURN

void w_m_member_statemanage_trans::toexit()
{
    this->reject();
}

bool w_m_member_statemanage_trans::initData1()
{
    //clicked (none) returns long [pbm_bnclicked]
    //long ancestorreturnvalue
    uo_rw_member::read_card_type lst_card;
    n_func::st_member_type  lst_member;

    lst_card = cb_read1->of_read_card(this, is_lmemberno);
    if(lst_card.flag == false){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,lst_card.errtext);
        return false;
    }
    if(lst_card.flag){
        QString m_state=backheader::VIPFINDVALUE(w_m_member::get_VIP_MODEL_STATE(), n_func::f_m_get_state(is_lmemberno));
        if(m_state!=tr("挂失")){
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("此卡状态为%1，不能操作").arg(m_state));
            return false;
        }

        lst_member=n_func::f_m_get_item(this, lst_card.cardid);
        if(lst_member.as_m_att != "2"){
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("非充值卡会员，不能操作").arg(m_state));
            return false;
        }

        if(!lst_card.cardid.isEmpty() && lst_card.cardid==ui->lineEdit_newvipno->text()){
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("原挂失会员与新发卡会员不能为同一会员"));
            return false;
        }
        ui->lineEdit_oldvipno->setText(lst_card.cardid);
        ui->lineEdit_oldvipname->setText(lst_member.as_member);
        lst_member=n_func::f_m_get_limit(this, is_lmemberno);
        ii_basedata=int(lst_member.adec_m_limitdata);
        ui->lineEdit_tonewnum->setValue(lst_card.amount-lst_member.adec_m_limitdata);

        ui->lineEdit_oldamount->setValue(lst_card.amount);
        ui->lineEdit_oldpoint->setValue( lst_card.point);

        ui->lineEdit_newvipname->setText(ui->lineEdit_oldvipname->text());
        ui->lineEdit_newamount->setValue(ui->lineEdit_oldamount->value());
        ui->lineEdit_newpoint->setValue(ui->lineEdit_oldpoint->value());

        ui->lineEdit_tonewnum->setFocus();
        ui->lineEdit_tonewnum->selectAll();
    } else {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,lst_card.errtext);
        return false;

    }
    return true;
}

QString w_m_member_statemanage_trans::wf_addsendcard()
{
    //Public function wf_addsendcard (none) returns integer
    QString ls_oldmemberno;
    QString ls_newmemberno;
    QString ls_cardno;
    int li_exists;
    int li_basedata=0;

    if(!ui->checkBox_istonewyajin->isChecked()){
        li_basedata=this->ii_basedata;
    }

    ls_oldmemberno = ui->lineEdit_oldvipno->text();
    ls_newmemberno = ui->lineEdit_newvipno->text();
    ls_cardno = ui->lineEdit_newcardno->text();

    lds_query query;
    query.execSelect(QString("select count(1) from t_m_member where vch_member ='%1'").arg(ls_newmemberno));
    query.next();
    li_exists=query.recordValue(0).toInt();
    if(li_exists > 0){
        return tr("此会员卡号[") + ls_newmemberno + tr("]已被使用，请重新输入");
    }
    li_exists = 0;
    query.execSelect(QString("select count(1) from t_m_member where vch_cardno = '%1'").arg(ls_cardno));
    query.next();
    li_exists=query.recordValue(0).toInt();
    if(li_exists > 0){
        return tr("此会员卡面卡号[") + ls_newmemberno + tr("]已被使用，请重新输入");
    }
    if(query.execInsert("t_m_member",
                        QString("select '%1' as `vch_memberno`, vch_member, ch_typeno, vch_id, dt_birthday,"
                                "vch_tel, vch_handtel, vch_address, dt_limit, 1 as `ch_state`,"
                                "num_limit, 'N' as `ch_cardflag`, vch_password, '%2' as `vch_operID`, now() as `dt_operdate`,"
                                " '%3' as `vch_cardno` from t_m_member where vch_memberno = '%4' ")
                        .arg(ls_newmemberno)
                        .arg(public_sql::gs_operid)
                        .arg(ls_cardno)
                        .arg(ls_oldmemberno))) {
        if(query.execUpdate("t_m_member",qrtVariantPairList()
                            << qrtVariantPair("ch_state", 4)
                            << qrtVariantPair("vch_cardno", QVariant())
                            << qrtVariantPair("int_basedata", li_basedata),
                            QString(" vch_memberno = '%1' ").arg(ls_oldmemberno))) {
            return "";
        }
    }
    return tr("修改原卡状态失败:")+query.recordError();
}
