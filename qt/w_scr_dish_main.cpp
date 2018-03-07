#include "w_scr_dish_main.h"
#include "back_view_dialog.h"
#include <QTimer>
#include <QWidgetAction>
#include "lds_menu.h"
#include "bianma_cook.h"
#include "ftableview_delegate.h"
#include "w_bt_dish_base.h"
#include "ui_w_bt_dish_new_dialog_2.h"
#include "fexpandmain_2.h"
#include "w_scr_dish_group_dialog.h"
#include "w_scr_dish_cook.h"
#include "take_weight.h"
#include "w_scr_dish_num_cook_dialog.h"
#include "w_bt_dish.h"
#include "lds_dialog_input.h"
#include "w_scr_dish_restaurant_div_pop.h"
#include "w_scr_dish_pay_widget.h"
#include "w_scr_dish_dish_change.h"
#include "w_scr_dish_m_hang_pay_dialog.h"
#include "fast_qudan.h"
#include "takeout_cook.h"
#include "take_cashier.h"
#include "w_scr_dish_weixinbill_import.h"
#include "w_scr_dish_weixinwaimai.h"
#include "w_scr_dish_weixin_quitbill.h"
#include "business_documents_query.h"
#include "w_scr_dish_language_switch_set.h"
#include "w_rpt_report_frame_business_summary.h"
#include "w_scr_dish_switch_detail.h"
#include "w_m_member_list.h"
#include "public_logindialog.h"
#include "modify_password.h"
#include "w_scr_dish_guqing_report.h"
#include "lock_screen.h"
#include "btnlanguageswicth.h"
#include "lds_dialog_input.h"
#include <QLineEdit>
#include <QLabel>
#include "w_scr_dish_dish_quit.h"
#include "public_printby_ch_billno.h"
#include "linshi_cook.h"
#include "w_scr_dish_restaurant_paysomedish.h"
#include "lds_model_sqltablemodel_delegate_com.h"
#include "w_scr_dish_cuicaipiece_choose.h"
#include "w_scr_dish_cuicai_choose.h"
#include "w_scr_dish_main_table_pay.h"
#include "w_scr_dish_pay_dialog.h"
#include "public_pushbutton_pay.h"
#include "w_m_member_telcoming.h"
#include "fexpandmain_dish_btn.h"
#include "ui_w_bt_dish_unitset_dialog.h"
#include "public_pushbuton_rt_check.h"
#include "ui_fexpandmain_payover.h"
#include "lds_dialog_input_double.h"
#include "ui_lds_dialog_input_dialog_double.h"


const QString w_scr_dish_main::Q000  = "Q000";
w_scr_dish_main::w_scr_dish_main(const QString &ch_tableno, const QString &ch_billno, windowType window_type, QWidget *parent)
    : lds_roundeddialog_rect0(parent)
{
    this->window_type = window_type;
    this->ch_tableno = ch_tableno;
    this->ch_billno = ch_billno;

    tablemodel = new fexpandmain_model_sqltablemodel_dd(this);//点菜部分
    tablemodel->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    tablemodel->setTable("cey_u_orderdish");
    ///抓取促销信息  vch_parameter, vch_value
    tablemodel->tablename_kvmap_insert("cey_sys_parameter", "vch_parameter", "vch_value", " vch_parameter like 'promotion_setting%' ", false,
                                       lds_model_sqltablemodel::SELECT_AUTO_REFRESH_KEY);
    model_type = new ftableview_standmodel_sql(this);
    model_subtype = new ftableview_standmodel_sql(this);
    model_dish = new ftableview_standmodel_sql(this, ftableview_standmodel_sql::TYPE_icon);

}

void w_scr_dish_main::initLogo()
{
    initLogo(label_logo, label_text);
}

void w_scr_dish_main::refreshChBillno()
{
    if(windowTypeKuaiCan == window_type) {
        torefresh_control_Q000();
    } else {
        torefresh_control(ch_billno);
    }
    //2 更新会员状态
    updatememberchecked();
}

void w_scr_dish_main::torefresh_dish()
{
    torefresh_dish_sql();
    model_type->toinit();
    model_subtype->toinitBySqlother("");
    //菜品重新初始化,必须延时发送，否则会第一次会没有图片
    QTimer::singleShot(10, model_dish, SLOT(toinit()));
}

void w_scr_dish_main::torefresh_dish_sql()
{
    //刷新 type subtype dish
    QString operfileter = QString(" ('0000'='%1' or 0 = (select count(0) from cey_bt_dish_type_oper b where b.vch_operid = '%1') or ch_dish_typeno in(select ch_dish_typeno from cey_bt_dish_type_oper b where b.vch_operid='%1' )) ")
            .arg(public_sql::gs_operid);
    model_dish->imagedishdir = lds::sysconf->value("system_setting/bofanglujing").toString();
    model_type->sql = QString("select '%1','%', '', '', '', '', '', '', '',''  union (select vch_dish_typename,ch_dish_typeno, '', '', '', '', '', '', '',vch_color  from cey_bt_dish_type where "+operfileter+")")
            .arg(tr("全部"));
    model_type->clearCache();

    model_subtype->sql = "select vch_sub_typename, ch_sub_typeno, '', '', '', '', '', '', '', vch_color  from cey_bt_dish_subtype";
    model_subtype->sqlother = model_subtype->sql+"  where ch_dish_typeno like '%1%' and "+operfileter;
    model_subtype->sql = model_subtype->sql+" where"+operfileter;
    model_subtype->clearCache();

    QString qt_show_ingredient;
    if(n_func::f_get_sysparm("qt_show_ingredient","0") == "0") {
        qt_show_ingredient = " and ifnull(ch_material_flag, '') <> 'Y' ";
    }
    //lds::sysconf->value("system_setting/dish_color_keep_subtype", false).toBool()
    model_dish->sql =
            " select "
            " CONCAT(a.vch_dishname,'\n', a.num_price) as name_price"//描述
            ", a.ch_dishno as dishno1"//品码
            ", a.ch_dishno as dishno2"//图片路径
            ",(select count(0) from cey_bt_dish AA where AA.ch_groupno = a.ch_dishno) group_count"
            ", if(a.item_flag = 'Y', ifnull( (select num_num from cey_st_material_num where ch_materialno = a.ch_dishno), 0) , '') as dish_st_remain"//库存剩余
            ", 0 as dishcount "
            ", '' as ischecked "
            ", num_item_warn "
            ", (select GROUP_CONCAT(concat(AA.ch_itemno, '/', ifnull(BB.num_num, ''), '/', CC.num_item_warn)) from material_bt_dish AA , cey_st_material_num BB, cey_bt_dish CC where AA.ch_itemno = BB.ch_materialno and AA.ch_itemno = CC.ch_dishno and ifnull(CC.item_flag, '') = 'Y' and AA.ch_dishno = a.ch_dishno) as ingredient_st "
            ;
    if(lds::sysconf->value("system_setting/dish_color_keep_subtype", false).toBool())
        model_dish->sql += ", (select vch_color from cey_bt_dish_subtype AA where AA.ch_sub_typeno = a.ch_sub_typeno ) as vch_color";
    else
        model_dish->sql += ",a.vch_color";
    model_dish->sql += " from cey_bt_dish a ";
    model_dish->sql += " where a.ch_stopflag <> 'Y' and a.ch_tempflag <> 'Y' ";
    model_dish->sql += " and a.ch_groupno = a.ch_dishno ";
    model_dish->sql += qt_show_ingredient;
    model_dish->sql += " %1 ";

    model_dish->sqlother = model_dish->sql.arg("  and ch_sub_typeno = '%1' and"+operfileter);
    model_dish->sqlother2 = model_dish->sql.arg("  and ch_dish_typeno like '%1%' and"+operfileter);
    model_dish->sqlother3 = model_dish->sql.arg("  and (a.ch_dishno like '%1%' or vch_spell like '%1%' ) and"+operfileter);
    model_dish->sqlother4 = model_dish->sql.arg("  and (ch_off = 'Y' ) and"+operfileter);
    model_dish->sql = model_dish->sql.arg(" and"+operfileter);
    model_dish->clearCache();

}

void w_scr_dish_main::torefresh_control(const QString &ch_billno, bool forQ000)
{
    ///清空 主套菜 折叠
    tablemodel->row_fold_map_RELEASE(tableView);
    ///~清空 主套菜 折叠
    ///
    tablemodel->select(ch_billno, this, forQ000);
    qDebug() << __FILE__ << __LINE__;
    ///初始化 主套菜 折叠
    if("1" == tablemodel->get_cey_sys_parameter_value("qt_Y_dish_fold")) {
        for(int k = 0; k < tableView->verticalHeader()->count(); k++){
            QString ch_suitflag = tablemodel->model_data(k, "ch_suitflag").toString();
            if("P" == ch_suitflag) {
                tablemodel->row_fold_map.insert(k, true);
            }
            if("Y" == ch_suitflag) {
                tableView->hideRow(k);
            }
        }
    }
    qDebug() << __FILE__ << __LINE__;
    ///~初始化 主套菜 折叠

    ///~促销读会员卡
    //最后设置model
    if(0 == tableView->model()) {
        tableView->setModel(tablemodel);
        tableView->setSelectionModel(new QItemSelectionModel(tablemodel));
        tableView->justShowColumn(0);
        connect(tableView->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(updatequitDish(QModelIndex,QModelIndex)));
    }
    qDebug() << __FILE__ << __LINE__;

    to_payfor_info("", w_scr_dish_pay_widget::OPERAT_NONE);
}


void w_scr_dish_main::torefresh_control_Q000()
{
    ch_billno = w_scr_dish_restaurant_div_pop::getMaxChbillnoAdd1_cey_u_master("Q");
    torefresh_control(ch_billno, true);
}

//-1没有设置 0不符合折扣情况 1正常
int w_scr_dish_main::autoDiscount(QString &discount_desc, int &int_discount_value)
{
    bool ch_discount = false;
    int int_discount = 0;
    QString title;
    if(n_func::f_get_sysparm("pay_auto_discount", "0") == "1"){
        ch_discount = true;
        int_discount = n_func::f_get_sysparm("pay_auto_discount_value", 0).toInt();
        title = tr("自动折扣");
    }

    //
    if(tablemodel->vch_membernod.is_m_discount()) {
        ch_discount = true;
        int_discount = tablemodel->vch_membernod.int_discount;
        title = tr("会员折扣");
    }

    //
    if(ch_discount) {
        if(tablemodel->can_discount_limit(int_discount, title, discount_desc)) {
            discount_desc = title + ":" + QString::number(int_discount) + "%";
            int_discount_value = int_discount;
            return 1;
        }
        return 0;
    }
    return -1;
}

void w_scr_dish_main::loadData()
{
    QStringList xy = lds::sysconf->value("system_setting/qt_iconsize").toString().split("x");
    int dish_hcount = qMin(qMax(xy.value(0).toInt(), 4), 20);
    int dish_vcount = qMin(qMax(xy.value(1).toInt(), 4), 20);

    QString cc = lds::sysconf->value("system_setting/qt_type_column_count").toString();
    int type_hcount  =qMin(qMax(4, cc.toInt()), 20);

    //点菜信息
    {
        /// ～抓取促销信息
        connect(tablemodel, SIGNAL(signals_num_total_change(QString,QString)),this,SLOT(to_update_num_total(QString, QString)));
        connect(tablemodel, SIGNAL(signals_num_total_change(QString,QString)),this,SLOT(updateTakebillDesc()));

        tableView->horizontalHeader()->setStretchLastSection(true);
        tableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tableView->setShowGrid(false);
        tableView->setEditTriggers(QTableView::NoEditTriggers);
        connect(tableView, SIGNAL(signal_dishname_rect_release(QModelIndex)),this, SLOT(todetail()));
        connect(tableView, SIGNAL(signal_suit_fold(QModelIndex)),this,SLOT(to_suit_fold(QModelIndex)));

        connect(tableView, SIGNAL(signal_add(QModelIndex)),this,SLOT(toadd()));//(QModelIndex)));
        connect(tableView, SIGNAL(signal_sub(QModelIndex)),this,SLOT(tosub()));//(QModelIndex)));
        connect(tableView, SIGNAL(signal_change(QModelIndex)),this,SLOT(tochangenum()));
        connect(tableView, SIGNAL(signal_detail(QModelIndex)),this,SLOT(todetail()));//(QModelIndex)));
        connect(tableView, SIGNAL(signal_delete(QModelIndex)),this,SLOT(toquitdish()));//(QModelIndex)));
    }

    //4 大类
    {
        tableView_type->setModel(model_type);
        model_type->selectionmodel = tableView_type->selectionModel();
        /// ---------------delegate -------------------begin
        //修改delegate的某些样式单独修改
        ftableview_delegate *d = new ftableview_delegate(tableView_type);
        d->keepConnect(tableView_type);
        d->setMargin(2);
        d->indexisEmptystillDraw = true;
        tableView_type->setItemDelegate(d);
        /// ---------------delegate -------------------end
        tableView_type->fillData_addAnimation(1, type_hcount);
        tableView_type->enableLongPress(n_func::f_get_sysparm("qt_forbide_change_new","0") == "0");
        model_type->enableEndAppendNew(n_func::f_get_sysparm("qt_forbide_change_new","0") == "0");
    }
    {
        tableView_subtype->setModel(model_subtype);
        model_subtype->selectionmodel = tableView_subtype->selectionModel();
        /// ---------------delegate -------------------begin
        //修改delegate的某些样式单独修改
        ftableview_delegate *d = new ftableview_delegate(tableView_subtype);
        d->keepConnect(tableView_subtype);
        d->setMargin(2);
        d->indexisEmptystillDraw = true;
        tableView_subtype->setItemDelegate(d);
        /// ---------------delegate -------------------end

        tableView_subtype->fillData_addAnimation(1, type_hcount);
        tableView_subtype->enableLongPress(n_func::f_get_sysparm("qt_forbide_change_new","0") == "0");
        model_subtype->enableEndAppendNew(n_func::f_get_sysparm("qt_forbide_change_new","0") == "0");
        connect(tableView_type,SIGNAL(signalclick(QString)),model_subtype, SLOT(toinitBySqlother(QString)));
        connect(tableView_type,SIGNAL(timeout(QModelIndex)),this,SLOT(toLongPressTypeSet()));
        connect(tableView_subtype,SIGNAL(timeout(QModelIndex)),this,SLOT(toLongPressTypeSet()));
    }

    //2 菜品
    {
        tableView_dish->setModel(model_dish);
        tableView_dish->enableLongPress(n_func::f_get_sysparm("qt_forbide_change_new","0") == "0");
        model_dish->enableEndAppendNew(n_func::f_get_sysparm("qt_forbide_change_new","0") == "0");
        model_dish->selectionmodel = tableView_dish->selectionModel();
        /// ---------------delegate -------------------begin
        //修改delegate的某些样式单独修改
        ftableview_delegate *d = new ftableview_delegate(tableView_dish);
        d->setColorLight(3);
        d->keepConnect(tableView_dish);
        d->setMargin(2, 2, 2, 0);
        tableView_dish->setItemDelegate(d);
        /// ---------------delegate -------------------end
        tableView_dish->fillData_addAnimation(dish_vcount, dish_hcount);

        connect(tableView_subtype,SIGNAL(signalclick(QString)),model_dish, SLOT(toinitBySqlother(QString)));
        connect(tableView_type,SIGNAL(signalclick(QString)),model_dish,SLOT(toinitBySqlother2(QString)));
        connect(tableView_dish,SIGNAL(signalclick(QString)),this,SLOT(todishclick(QString)));
        connect(tableView_dish,SIGNAL(timeout(QModelIndex)),this,SLOT(toLongPressUpdateDishImage(QModelIndex)));
        //        connect(tableView_dish,SIGNAL(timeout_pre()),this,SLOT(toLongPressUpdateDishImageAdd()));
        //        connect(tableView_dish,SIGNAL(timeout_next()),this,SLOT(toLongPressUpdateDishImageAdd()));

        connect(tableView_subtype,SIGNAL(signalclickNew(QModelIndex)),this, SLOT(toLongPressTypeSet()));
        connect(tableView_type,SIGNAL(signalclickNew(QModelIndex)),this,SLOT(toLongPressTypeSet()));
        connect(tableView_dish,SIGNAL(signalclickNew(QModelIndex)),this,SLOT(toLongPressUpdateDishImage(QModelIndex)));
    }

    //logo
    initLogo();
    //
    update_type_subtype_view();
    //
    torefresh_dish();
    QTimer::singleShot(50, this, SLOT(refreshChBillno()));
}

void w_scr_dish_main::updatequitDish(const QModelIndex &/*current*/, const QModelIndex &/*previous*/)
{

}

void w_scr_dish_main::toAllbillcancel()
{
    removeAllInsertDish();
    menuhide();
}

void w_scr_dish_main::tochange()
{
    QString errstring;
    int row = tableView->currentIndex().row();
    if(row < 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("没有菜品被选中"));
        return;
    }
    if(!tablemodel->can_change_dish(row, &errstring)) {
        lds_messagebox::warning(this,MESSAGE_TITLE_VOID,errstring);
        return;
    }
    int rowp = row;
    double num_num = tablemodel->model_data_NIBOLAN(row, "num_num-num_back");
    double num_price_org = tablemodel->model_data(row, "num_price_org").toDouble();
    if(tablemodel->model_data(row, "ch_suitflag").toString() == "Y") {//是子套菜
        for(;rowp >=0; rowp --)
            if(tablemodel->model_data(rowp, "ch_suitflag").toString() == "P") break;
    }

    lds_query query;
    w_scr_dish_dish_change dialog(
                tablemodel->model_data(row, "ch_dishno").toString(),
                tablemodel->model_data(row, "num_price_org").toFloat(),
                tablemodel->model_data(row, "ch_suitno").toString(),
                this);
    dialog.setWindowTitle(tr("修改菜品"));
    if(QDialog::Accepted == lds_roundeddialog_rect_align(&dialog).exec()){
        QString msg;
        if(!fexpandmain_2::guqing_check(msg, dialog.ch_dishno_new, tablemodel->model_data_NIBOLAN(row, "num_num-num_back"))) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, msg+tr("操作失败"));
            return;
        }
        if(!msg.isEmpty()) {
            lds_messagebox::information(this ,MESSAGE_TITLE_VOID, msg);
        }

        query.execSelect(QString("select ch_dishno, num_price  from cey_bt_dish  where ch_dishno =  '%1' ").arg(dialog.ch_dishno_new));
        query.next();
        double num_price_new = query.recordValue("num_price").toDouble();
        QString ch_dishno = query.recordValue("ch_dishno").toString();
        {//删除，插入为了，估清
            fexpandmain_model_sqltablemodel tmp;
            tmp.setTable("cey_u_orderdish");
            tmp.setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
            tmp.insertRow(0);
            for(int c = 0; c < tablemodel->columnCount(); c++) {
                tmp.model_data_set(0, c , tablemodel->model_data(row, c));
            }
            tablemodel->keepdata_enabled();
            tablemodel->removeRows(row, 1);
            ///数据预插入
            tablemodel->keepdata_disabled_begin_rowcount(row);
            tablemodel->insertRow(row);

            for(int c = 0; c < tablemodel->columnCount(); c++) {
                tablemodel->model_data_set(row, c, tmp.model_data(0, c));
            }
        }

        tablemodel->model_data_set(row, "num_price",num_price_new);
        tablemodel->model_data_set(row, "num_price_org",num_price_new);
        tablemodel->model_data_set(row, "ch_dishno",ch_dishno);
        tablemodel->model_data_set(row, "num_num",dialog.rate*num_num);
        ///～数据预插入
        tablemodel->keepdata_submit(row + 1);

        if(tablemodel->model_data(row, "ch_suitflag").toString() == "Y"){//是套菜
            tablemodel->model_data_set(row, "num_price",0);
            if(!dialog.suitreplace) {//如果不是非可替换菜品，需要补加
                if(num_price_org != num_price_new) {
                    if(0==lds_messagebox::question(this, MESSAGE_TITLE_VOID,tr("替换菜品和被替换菜品价格不一样\n是否在主套菜上补差价?"),tr("确认"),tr("取消"))){
                        double diff = (num_price_new-num_price_org)*num_num;
                        double num_nump = tablemodel->model_data_NIBOLAN(rowp, "num_num-num_back");
                        tablemodel->model_data_set(rowp, "num_price",  tablemodel->model_data(rowp, "num_price").toDouble() + diff/num_nump);
                    }
                }
            }
            /// 换菜后，不在追加
            tablemodel->ch_dishno_set_row_map_DEL(rowp, 1);
        }
    }
}

void w_scr_dish_main::toquitdish()
{
    int ret = QDialogButtonBox::InvalidRole;
    int row = tableView->currentIndex().row();
    if(tablemodel->rowCount() == 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("没有菜品被选中"));
        return ;
    }

    if(windowTypeKuaiCan == window_type) {
        if(row < 0) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("没有菜品被选中"));
            return ;
        }
    }

    if(tablemodel->opera(row) == lds_model_sqltablemodel::OpInsert) {
        QString errstring;
        if(false == to_dish_del(row, &errstring)) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
        }
        return;
    }

    //更新或者没有任何操作
    if(tablemodel->opera(row) == lds_model_sqltablemodel::OpUpdate
            || tablemodel->opera(row) == lds_model_sqltablemodel::OpNone) {
        /** -----------------------------权限-----------------------------begin*/
        QString ls_operid = public_sql::gs_operid;
        if(!n_func::f_check_bus_grant_else_authority(ls_operid, "0218", this, tr("退菜"))){
            return ;
        }
        /** -----------------------------权限-----------------------------end*/
        w_scr_dish_dish_quit dialog(this);
        dialog.setWindowTitle(tr("退菜"));
        dialog.initData(row, tablemodel, this->ch_billno);
        ret = lds_roundeddialog_rect_align(&dialog).exec();
        switch(ret){
        case QDialogButtonBox::AcceptRole://单个退菜
        {
            tablemodel->select(this->ch_billno, this);
            frontViewDialog_virtual::static_display_cash(QString().sprintf("%0.2f", tablemodel->getKeepdata().dish_total()));
            break;
        }
        case QDialogButtonBox::ApplyRole://整单退菜
        {
            tablemodel->select(this->ch_billno, this);
            frontViewDialog_virtual::static_display_cash(QString().sprintf("%0.2f", tablemodel->getKeepdata().dish_total()));

            if(tablemodel->getKeepdata().dish_num() == 0) {
                //消台
                QString errstring;
                lds_query query;
                lds_query::tran_saction();
                QString ch_payno = w_scr_dish_restaurant_div_pop::getMaxChpaynoAdd1_cey_u_checkout_master();
                int sqr = w_scr_dish_restaurant_div_pop::cey_bt_table_div_sqr(ch_tableno, this->ch_billno);
                if(w_scr_dish_restaurant_div_pop::cey_bt_table_update(ch_tableno, "", sqr, &errstring)) {
                    if(query.execInsert("cey_u_checkout_master",qrtVariantPairList()
                                        << qrtVariantPair("ch_billno", this->ch_billno)
                                        << qrtVariantPair("ch_tableno", ch_tableno)
                                        << qrtVariantPair("num_cost", 0)
                                        << qrtVariantPair("num_discount", 0)
                                        << qrtVariantPair("num_present", 0)

                                        << qrtVariantPair("num_service", 0)
                                        //                                        << qrtVariantPair("vch_service_desc", "")
                                        << qrtVariantPair("num_lowcost", 0)
                                        << qrtVariantPair("num_blotout", 0)
                                        << qrtVariantPair("num_run", 0)

                                        << qrtVariantPair("vch_run_operID", "")
                                        << qrtVariantPair("ch_state", "Y")
                                        << qrtVariantPair("vch_operID", public_sql::gs_operid)
                                        << qrtVariantPair("dt_operdate", n_func::f_get_sysdatetime())
                                        << qrtVariantPair("vch_operID2", "")

                                        << qrtVariantPair("num_rate", 0)
                                        << qrtVariantPair("ch_payno", ch_payno)
                                        << qrtVariantPair("vch_memberno", "")
                                        << qrtVariantPair("ch_member_belong_state", "")
                                        << qrtVariantPair("ch_yun_payno", "")

                                        << qrtVariantPair("num_cash", 0) , &errstring)) {
                        if(query.execUpdate("cey_u_table","ch_payno", ch_payno, qrtEqual("ch_billno", this->ch_billno),  &errstring)){
                            if(query.execUpdate("cey_u_orderdish","ch_payno", ch_payno, qrtEqual("ch_billno", this->ch_billno) , &errstring)){
                                lds_query::com_mit();
                                this->accept();
                                return;
                            }
                        }
                    }
                    lds_query::roll_back();
                    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
                }//~消台
            }
            break;
        }
            //case
        }
    }
}

void w_scr_dish_main::toExitbill()
{
    //1
    if(tablemodel->rowCount() == 0)
    {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("没有点菜记录,不能付款"), true);
        return ;
    }
    //2
    /** -----------------------------权限-----------------------------begin*/
    QString ls_operid = public_sql::gs_operid;
    if(!n_func::f_check_bus_grant_else_authority(ls_operid, "0228", this, tr("退单"))){
        return ;
    }
    /** -----------------------------权限-----------------------------end*/
    menuhide();
    //4
    w_scr_dish_pay_dialog dialog(w_scr_dish_pay_widget::OPERAT_QUITBILL|w_scr_dish_pay_widget::TYPE_FAST, this, tablemodel);
    if(QDialog::Accepted == dialog.exec()){//付款成功
        QString old_ch_billno = this->ch_billno;
        refreshChBillno();
        to_payfor_info_ch_billno(old_ch_billno, w_scr_dish_pay_widget::OPERAT_PAY);
    }
}

void w_scr_dish_main::todetail()
{
    int row = tableView->currentIndex().row();
    if(row < 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("没有行被选中"));
        return;
    }

    w_scr_dish_num_cook_Dialog dialog(tablemodel, row, this);
    if(QDialog::Accepted != lds_roundeddialog_rect_align(&dialog).exec()) {
        return;
    }

    float off = dialog.get_cur_dish_num() - tablemodel->model_data_NIBOLAN(row, "num_num-num_back");
    QString msg;
    //子套菜会自动跳过
    if(fexpandmain_2::guqing_try_check_P(tablemodel, msg, row, off)) {
        if(!msg.isEmpty()) {
            lds_messagebox::information(this, MESSAGE_TITLE_VOID, msg);
        }
        dialog.commitModel(tablemodel, row);
    } else {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, msg + tr("操作失败"));
    }
}

void w_scr_dish_main::topresent()
{
    int row = tableView->currentIndex().row();
    if(row < 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("没有菜品被选中"));
        return;
    }
    if(row >= 0) {
        if(tablemodel->can_present(row)) {
            w_scr_dish_num_cook_Dialog::static_to_present
                    (this,
                     tablemodel,
                     row,
                     tablemodel->model_data_NIBOLAN(row, "num_num-num_back"));
        }
    }
}

void w_scr_dish_main::tomhangpay()
{
    w_scr_dish_m_hang_pay_Dialog dialog(this);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    lds_roundeddialog_rect_align(&dialog).exec();
}

void w_scr_dish_main::toTakebill()
{
    QString errstring;
    int int_flowid;
    if(tablemodel->rowCount() > 0){///挂单
        lds_query::tran_saction();
        if(fast_qudan::pend(errstring, tablemodel,int_flowid)){
            lds_query::com_mit();
            //释放model的折叠信息，与折叠map
            tablemodel->row_fold_map_RELEASE(tableView);
            lds_messagebox::information(this, MESSAGE_TITLE_VOID,tr("挂单成功,挂单号为:") + QString::number(int_flowid));
            goto end;
        }
        lds_query::roll_back();
        lds_messagebox::information(this, MESSAGE_TITLE_VOID,tr("操作失败"));
        goto end;
    } else {///取单
        fast_qudan dialog(this);
        dialog.setWindowTitle(tr("快餐取单表"));
        if(QDialog::Accepted == lds_roundeddialog_rect_align(&dialog).exec() ){
            int_flowid = dialog.int_flowid;
            ///估清判断
            lds_query query;
            QString msg;
            bool guqing_flag = true;
            ///
            query.execSelect(QString("SELECT ch_dishno, num_num - num_back as dish_num from cey_u_hungbill where int_flowID = %1 group by ch_dishno; ")
                             .arg(int_flowid));
            while(query.next()) {
                if(!fexpandmain_2::guqing_check(msg, query.recordValue("ch_dishno").toString(), query.recordValue("dish_num").toFloat())){
                    guqing_flag = false;
                }
            }
            if(!msg.isEmpty()){//设置了沽清
                if(guqing_flag){//沽清成功
                    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, msg);
                } else {//沽清失败
                    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, msg+tr("操作失败"));
                    return;
                }
            }
            /// ～估清判断
            lds_query::tran_saction();
            if(fast_qudan::take(errstring, tablemodel,int_flowid)){
                lds_query::com_mit();
                //自动折叠
                if("1" == tablemodel->get_cey_sys_parameter_value("qt_Y_dish_fold")) {
                    tablemodel->keepdata_Y_dish_fold(tableView, 0);
                }

                lds_messagebox::information(this, MESSAGE_TITLE_VOID, tr("操作成功"));
                goto end;
            }
            lds_query::roll_back();
            lds_messagebox::information(this, MESSAGE_TITLE_VOID,tr("操作失败"));
            goto end;
        }
    }
end:
    updateTakebillState();
}

void w_scr_dish_main::toTakeweight()
{
    int row = tableView->currentIndex().row();
    //没有选中行
    if(row >= 0){
        float dish_num_old = tablemodel->model_data_NIBOLAN(row, "num_num - num_back");
        QString ch_dishno = tablemodel->model_data(row, "ch_dishno").toString();
        take_weight dialog(tablemodel->model_data(row, "ch_dishno").toString(),
                           tablemodel->model_data(row, "num_price").toFloat(),
                           tablemodel->model_data_NIBOLAN(row, "num_num - num_back"),
                           tablemodel->model_data(row, "int_discount").toInt(),
                           this);
        dialog.setWindowTitle(tr("取重"));
        if(QDialog::Accepted == lds_roundeddialog_rect_align(&dialog).exec()){
            QString msg;
            if(fexpandmain_2::guqing_check(msg, ch_dishno, dialog._weight - dish_num_old)) {
                if(!msg.isEmpty()) {
                    lds_messagebox::information(this, MESSAGE_TITLE_VOID, msg);
                }
                tablemodel->model_data_set(row, "num_num", tablemodel->model_data_NIBOLAN(row, QString("%1 + num_back").arg(dialog._weight)));
                return;
            }
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, msg+tr("操作失败"));
        }
    }
}

void w_scr_dish_main::topaybill()
{
    //2
    /** -----------------------------权限-----------------------------begin*/
    QString ls_operid = public_sql::gs_operid;
    if(!n_func::f_check_bus_grant_else_authority(ls_operid, "0214", this, tr("付款"))){
        return  ;
    }
    /** -----------------------------权限-----------------------------end*/

    if(lds::sysconf->value("system_setting/kuaican_mpay_enabled", false).toBool()) {
        if(windowTypeKuaiCan == window_type
                && lds::sysconf->value("w_scr_dish_main/paymode", 1).toInt() == 1) {
            toMemberPayBill();
            return;
        }
    }
    if(paybill()) {
        if(windowTypeKuaiCan == window_type) {

        } else {//restaurant
            this->accept();
        }
    }
}

void w_scr_dish_main::topaybillSelectPayMode()
{
    switch(lds_messagebox::question(this ,MESSAGE_TITLE_VOID, tr("选择模式"), tr("结算"), tr("会员付款"), tr("取消"))) {
    case 0://结算
        lds::sysconf->setValue("w_scr_dish_main/paymode", 0);
        pushButton_pay->setText(tr("结算"));
        break;
    case 1://会员付款
        lds::sysconf->setValue("w_scr_dish_main/paymode", 1);
        pushButton_pay->setText(tr("会员付款"));
        break;
    case 2://取消
        break;
    }

    pushButton_pay->setDown(false);
}

void w_scr_dish_main::toMemberPayBill()
{
    if(tablemodel->rowCount() == 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("没有点菜记录,不能付款"), true);
        return;
    }
    //
    w_scr_dish_pay_widget::operatModes flags;
    flags = w_scr_dish_pay_widget::OPERAT_PAY|w_scr_dish_pay_widget::TYPE_FAST;
    w_scr_dish_pay_widget paywidget;
    paywidget.initGeometry(flags, false);
    paywidget.initdataByM(tablemodel, -1);
    if(QDialog::Accepted == paywidget.mread(true, tr("会员付款"))) {
        paywidget.toScrpay();
        QString old_ch_billno = tablemodel->cur_ch_billno();
        if(paywidget.pay_finished) {
            refreshChBillno();
            tableView_dish->m->refreshcur();
            to_payfor_info_ch_billno(old_ch_billno, w_scr_dish_pay_widget::OPERAT_PAY);
        }
    }
}

void w_scr_dish_main::toTuichu()
{
    menuhide();
    if(windowTypeKuaiCan == window_type) {
        if(tablemodel->rowCount() > 0)
        {
            if(0 == lds_messagebox::question(this, MESSAGE_TITLE_VOID,tr("存在消费记录! 是否整单取消然后退出?"),tr("确认"),tr("取消"))){

                //清空并且还原沽清信息
                removeAllInsertDish();

                this->reject();
            }
        } else {
            if(0 ==lds_messagebox::question(this, MESSAGE_TITLE_VOID,tr("确认退出?"),tr("确认"),tr("取消"))){
                this->reject();
            }
        }
    }
    if(windowTypeKuaiCan == window_type) {

    } else {//中餐
        QString errstring;
        bool data_has_changed = tablemodel->rowCountOp(lds_model_sqltablemodel::OpInsert) > 0 || tablemodel->rowCountOp(lds_model_sqltablemodel::OpUpdate) > 0;
        if(data_has_changed) {//插入
            switch(lds_messagebox::question(this, MESSAGE_TITLE_VOID,tr("点菜信息有更改,是否落单?"),tr("落单"), tr("不落单"), tr("取消"))){
            case 0://落单
            {
                if(!printdish_tran_saction(errstring)) {
                    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
                    return;
                }
            }
                break;
            case 1://不落单
                break;
            case 2://取消
                return;
            }
        } else {
            if(!tablemodel->try_update_data(errstring)) {
                lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
                return;
            }
        }

        //退出
        this->reject();
    }
}

void w_scr_dish_main::toMRead()
{
    if(lds::sysconf->value("system_setting/qt_can_member_sendcard", false).toBool()) {
        switch(lds_messagebox::question(this, MESSAGE_TITLE_VOID, tr("会员管理"), tr("会员资料维护"), tr("会员读卡"), tr("取消"))) {
        case 0://会员资料维护
        {
            w_m_member_list dialog(this);
            if(backheader::operid_right_code(this, "0402")){//会员资料维护【 0402 】
                dialog.setWindowTitle(tr("会员资料维护"));
                dialog.setFixedSize(lds::MAIN_WINDOW_SIZE);
                lds::setwflagFrameless(&dialog);
                dialog.exec();
            }
        }
            return;
        case 1://会员读卡
            break;
        case 2://取消
            return;
        }
    }
    //1
    if(-1 != fexpandmain_2::to_m_read(this, tablemodel)) {
        updatememberchecked();
    }
    //2
}

void w_scr_dish_main::towaimai()
{
    if(tablemodel->rowCount() == 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("您还未进行点菜,请确认"));
        return ;
    }
    for(int row = 0; row < tablemodel->rowCount(); row ++) {
        if(tablemodel->opera(row) != lds_model_sqltablemodel::OpInsert) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("有菜品已经落单") + "," + tr("操作失败"));
            return;
        }
    }
    takeout_cook dialog(tablemodel, ch_tableno, this);
    dialog.ui->lineEdit_orderman->setText(tablemodel->tablename_kvmap_value("t_m_member", tablemodel->vch_membernod.vch_memberno, "vch_memberno", "vch_member").toString());
    dialog.ui->lineEdit_tel->setText(tablemodel->tablename_kvmap_value("t_m_member", tablemodel->vch_membernod.vch_memberno, "vch_memberno", "vch_tel").toString());

    dialog.setWindowTitle(tr("普通外卖"));
    if(QDialog::Accepted == lds_roundeddialog_rect_align(&dialog).exec()){
        if(windowTypeKuaiCan == window_type) {
            refreshChBillno();
            tableView_dish->m->refreshcur();
        } else {
            this->accept();
        }
        return ;
    }
    return ;
}

void w_scr_dish_main::towaimaipay()
{
    take_cashier cashierdialog(this);
    cashierdialog.setWindowTitle(tr("外卖收银结算"));
    lds_roundeddialog_rect_align(&cashierdialog).exec();
}

void w_scr_dish_main::toweixinwaimai()
{
    w_scr_dish_weixinwaimai dialog(this);
    dialog.setWindowTitle(tr("微信外卖"));
    lds_roundeddialog_rect_align(&dialog).exec();
}

void w_scr_dish_main::toweixinquitbill()
{
    w_scr_dish_weixin_quitbill dialog(this);
    dialog.setWindowTitle(tr("退单查询"));
    lds_roundeddialog_rect_align(&dialog).exec();
}

void w_scr_dish_main::toweixinbill_import()
{
    w_scr_dish_weixinbill_import dialog(this);
    dialog.setWindowTitle(tr("微信订单"));
    if(QDialog::Accepted == lds_roundeddialog_rect_align(&dialog).exec()){
        QDateTime dt = n_func::f_get_sysdatetime();
        lds_query query;

        tablemodel->keepdata_disabled_begin_rowcount();
        query.execSelect(QString("select b.*, (case b.ch_suitflag when 'Y' then 'P' else 'N' end) as ch_suitflag2, (case b.ch_suitflag when 'Y' then b.ch_dishno else '' end) as ch_suitno, a.number as num_default, c.vch_unitname FROM cey_web_order_dish a, cey_bt_dish b, cey_bt_unit c where  a.dishNo = b.ch_dishno and b.ch_unitno = c.ch_unitno and b.ch_stopflag = 'N' and a.orderSn = '%1' ; ")
                         .arg(dialog.wdata.sn));
        while(query.next()){
            todish_fill_data(query, dt);
        }
        tablemodel->keepdata_submit();
    }
}

void w_scr_dish_main::tosysyingyedanju()
{
    //切换到中餐模式
    business_documents_query dialog(this);
    dialog.show_login_data_by_sql();
    dialog.setWindowTitle(tr("营业单据查询"));
    dialog.resize(lds::MAIN_WINDOW_SIZE);
    lds_roundeddialog_rect_align(&dialog, false).exec();

    //若是执行了反结帐
    QString ch_billno = w_scr_dish_restaurant_div_pop::getMaxChbillnoAdd1_cey_u_master("Q");
    if(this->ch_billno == ch_billno) {
        return;
    }
    this->ch_billno = ch_billno;
    tablemodel->update_ch_billno(ch_billno);
}

void w_scr_dish_main::toLanguageSwitch()
{
    lds_Dialog::clearCache();
    retranslateView();
    initLogo(label_logo, label_text);
    tablemodel->emit_num_total_change_label(tablemodel->getKeepdata().dish_num(), tablemodel->getKeepdata().dish_total());
}

void w_scr_dish_main::toLanguageSet()
{
    if(!saas_pay_detail::SaaSFuncCheck(this, "0512"/*多语言*/)) {
        return ;
    }

    btnlanguageswicth *b = qobject_cast<btnlanguageswicth *>(this->sender());
    w_scr_dish_language_switch_set dialog(this);
    dialog.setWindowTitle(tr("切换语言"));
    if(QDialog::Accepted ==  lds_roundeddialog_rect_align(&dialog).exec()) {
        if(b) {
            b->initKeys_conf();
            QString key = b->firstKey();
            lds_Dialog::retranslateSystem(key);
            toLanguageSwitch();
        }
    }
    if(b) {
        b->setDown(false);
    }
}

void w_scr_dish_main::tosysyingyeqingkuang()
{
    w_rpt_report_frame_business_summary dialog(this);
    dialog.show_login_data_by_sql();
    dialog.resize(lds::MAIN_WINDOW_SIZE);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    lds_roundeddialog_rect_align(&dialog, false).exec();
}

void w_scr_dish_main::tosysjiaoban()
{
    w_scr_dish_switch_detail dialog(this);
    if(dialog.rightChecked(this)){
        dialog.initData();
        if(QDialog::Accepted==lds_roundeddialog_rect_align(&dialog).exec()){
            lds_messagebox::question(this,MESSAGE_TITLE_VOID,  tr("将会重启"));
            this->reject();//退出
            public_sql::save_login->hideall_and_back_login(dialog.new_operid, dialog.new_operpwd);
        }
    }
}

void w_scr_dish_main::tosyschongzhi()
{
    w_m_member_list::statictorecharge(this, 0, 0,"");
}

void w_scr_dish_main::tosysqukuan()
{
    w_m_member_list::statictorecharge(this, 0, 0,"C");
}

void w_scr_dish_main::tosysxiugaimima()
{
    modify_password dialog(this);
    dialog.setWindowTitle(tr("密码修改"));
    lds_roundeddialog_rect_align(&dialog).exec();
}

void w_scr_dish_main::tosysswitchback()
{
    menuhide();

    if(false == backheader::operid_right_code(this, "0244")) {
        return;
    }

    QString title;
    if(tablemodel && tablemodel->rowCount() > 0){
        title+=tr("点菜信息将会清空")+",";
        if(0 == lds_messagebox::question(this,MESSAGE_TITLE_VOID, title+tr("是否切换到后台?"), tr("确定"), tr("取消"))){
            toAllbillcancel();
            this->accept();
        }
    } else{
        this->accept();
    }
}

void w_scr_dish_main::tosyskaiqianxiang()
{
    if(backheader::operid_right_code(this, "0209")){//钱箱【 0209 】
        QString instructionedit = lds::sysconf->value("system_setting/qianxiangzhiling").toString();
        blockIns bins;
        bins.append(pieceIns(instructionedit.toLocal8Bit(),""));
        backheader::asyn_dataprint(bins);
    }
}

void w_scr_dish_main::tochangyongcai()
{
    ftableview_standmodel_sql *model_dish = static_cast<ftableview_standmodel_sql *>(tableView_dish->m);
    model_dish->toinitBySqlother4();
}

void w_scr_dish_main::tosyssuoping()
{
    lock_screen dialog("1", this);
    dialog.setWindowTitle(tr("前台锁屏"));
    lds_roundeddialog_rect_align(&dialog).exec();
}

void w_scr_dish_main::tosysguqing()
{
    if(!saas_pay_detail::SaaSFuncCheck(this, "0406"/*菜品沽清*/)) {
        return ;
    }

    w_scr_dish_guqing_report dialog(this);
    dialog.resize(lds::MAIN_WINDOW_SIZE);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    lds_roundeddialog_rect_align(&dialog).exec();
}

void w_scr_dish_main::toRestaurant()
{
    menuhide();
    if(tablemodel->rowCount() > 0)
    {
        if(0 == lds_messagebox::question(this, MESSAGE_TITLE_VOID,tr("存在消费记录! 是否整单取消然后切换到酒楼模式"),tr("确认"),tr("取消"))){

            //清空并且还原沽清信息
            removeAllInsertDish();

            this->done(public_sql::GoRestaurant);
        }
    } else {
        this->done(public_sql::GoRestaurant);
    }
}

void w_scr_dish_main::toScanbar()
{
    if(add_dish_by_scanbar(lineEdit_scan->text())) {
    } else {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("操作失败"));
    }
    lineEdit_scan->clear    ();
}

void w_scr_dish_main::tosub()
{
    if(window_type == windowTypeKuaiCan) {//快餐
        dish_add(-1, true);
    }
    if(window_type == windowTypeZhongCan) {//中餐
        int row = tableView->currentIndex().row();
        if(tablemodel->opera(row) == lds_model_sqltablemodel::OpInsert) {//未落单
            dish_add(-1, true);
        } else {//落单
            toquitdish();
        }
    }
}

void w_scr_dish_main::toadd()
{
    dish_add(1, true);
}

void w_scr_dish_main::tochangenum()
{
    int row = tableView->currentIndex().row();
    if(row < 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("没有数据被选中"));
        return;
    }
    int num = tablemodel->model_data_NIBOLAN(row, "num_num-num_back");
    lds_dialog_input_double dialog(this);
    dialog.ui->lineEdit->setValue(num);
    dialog.ui->lineEdit->setDecimals(0);
    dialog.ui->label->setText(tr("数量"));
    dialog.disableDot();
    if(QDialog::Accepted == lds_roundeddialog_rect_align(&dialog).exec()) {
        dish_add((int)dialog.ui->lineEdit->value() - num, false);
    }
}

void w_scr_dish_main::todishup()
{
    int row = tableView->currentIndex().row() - 1;
    if(row < 0) return;

    tableView->scaleHeight(tableView->currentIndex().row(), row);
    tableView->selectRow(row);
}

void w_scr_dish_main::todishdown()
{
    int row = tableView->currentIndex().row() + 1;
    if(row >= tableView->verticalHeader()->count()) return;

    tableView->scaleHeight(tableView->currentIndex().row(), row);
    tableView->selectRow(row);
}

void w_scr_dish_main::showBianMa()
{
    QMenu *m = qobject_cast<QMenu *>(this->sender());
    if(m->actions().count() == 0) {
        QWidgetAction *widgetaction=new QWidgetAction(m);
        bianma_cook *b = new bianma_cook(this);
        b->setFixedSize(500,300);
        widgetaction->setDefaultWidget(b);
        m->addAction(widgetaction);
        ftableview_standmodel_sql *model_dish = static_cast<ftableview_standmodel_sql *>(tableView_dish->m);
        connect(b,SIGNAL(edit_text(QString)), model_dish,SLOT(toinitBySqlother3(QString)));
    }
}

void w_scr_dish_main::toLongPressUpdateDishImage(const QModelIndex &index)
{
    lds_query query;
    QString ch_sub_typeno;


    //        connect(tableView_dish,SIGNAL(timeout_pre()),this,SLOT(toLongPressUpdateDishImageAdd()));
    //        connect(tableView_dish,SIGNAL(timeout_next()),this,SLOT(toLongPressUpdateDishImageAdd()));

    if(n_func::f_get_sysparm("qt_forbide_change_new","0") == "1")
        return;
    QString key = index.data(Qt::UserRole + 1).toString();
    query.execSelect(QString(" select ch_dishno from cey_bt_dish where ch_dishno = '%1' ")
                     .arg(key));
    if(!query.next()) {
        toLongPressUpdateDishImageAdd();
        return;
    }

    QModelIndex subtypeindex = tableView_subtype->currentIndex();
    QModelIndex typeindex = tableView_type->currentIndex();
    if(subtypeindex.isValid()) {
        ch_sub_typeno = subtypeindex.data(Qt::UserRole + 1).toString();
    } else if(typeindex.isValid()) {
        QString ch_dish_typeno = typeindex.data(Qt::UserRole + 1).toString();
        query.execSelect(QString("SELECT ch_sub_typeno from cey_bt_dish_subtype where ch_dish_typeno = '%1'  ")
                         .arg(ch_dish_typeno));
        query.next();
        ch_sub_typeno = query.recordValue(0).toString();
    }
    ///
    fexpandmain_2::updateDishImage(this,index, tableView_dish->m, ch_sub_typeno);
    tablemodel->tablename_kvmap_clear_for_select_and_autoselect();
}

void w_scr_dish_main::toLongPressUpdateDishImageAdd()
{
    if(tableView_dish == 0
            || n_func::f_get_sysparm("qt_forbide_change_new","0") == "1")
        return;

    QString ch_sub_typeno;
    QModelIndex subtypeindex = tableView_subtype->currentIndex();
    QModelIndex typeindex = tableView_type->currentIndex();
    if(subtypeindex.isValid()) {
        ch_sub_typeno = subtypeindex.data(Qt::UserRole + 1).toString();
    } else if(typeindex.isValid()) {
        QString ch_dish_typeno = typeindex.data(Qt::UserRole + 1).toString();
        lds_query query;
        query.execSelect(QString("SELECT ch_sub_typeno from cey_bt_dish_subtype where ch_dish_typeno = '%1'  ")
                         .arg(ch_dish_typeno));
        query.next();
        ch_sub_typeno = query.recordValue(0).toString();
    }
    ///
    fexpandmain_2::updateDishImage_add(this, tableView_dish->m, ch_sub_typeno);
    tablemodel->tablename_kvmap_clear_for_select_and_autoselect();
}

void w_scr_dish_main::toLongPressTypeSet()
{
    if(n_func::f_get_sysparm("qt_forbide_change_new","0") == "1") {
        return;
    }

    w_bt_dish_base_typeset dialog(this);
    dialog.setWindowTitle(tr("品类设置"));
    lds_roundeddialog_rect_align(&dialog).exec();

    update_type_subtype_view();
    torefresh_dish();
}

void w_scr_dish_main::to_suit_fold(const QModelIndex &index)
{
    tablemodel->row_fold_map_FOLD(tableView, index);
}

void w_scr_dish_main::menuhide()
{
}

void w_scr_dish_main::updateTakebillState()
{
}

void w_scr_dish_main::updateTakebillDesc()
{
}

void w_scr_dish_main::retranslateView()
{
}

void w_scr_dish_main::toadddish(const QString &ch_dishno, const QString &vch_print_memo, float num_price, int  int_discount, const bool time_promotion)
{
    lds_query query;
    QString guqing_check_msg;
    bool guqing_check_flag = false;
    int gooddegree = 0;
    if(public_sql::discount_off_mode() == "1") {
        int_discount = 100 - int_discount;//最后必须转出折扣
    }

    //1 默认数量 2 菜品编码
    QString ch_suitflag = tablemodel->get_cey_bt_dish_value(ch_dishno, "ch_suitflag").toString();
    QString vch_dishname = tablemodel->get_cey_bt_dish_value(ch_dishno, "vch_dishname").toString();
    QString ch_weight = tablemodel->get_cey_bt_dish_value(ch_dishno, "ch_weight").toString();
    QString ch_unitno = tablemodel->get_cey_bt_dish_value(ch_dishno, "ch_unitno").toString();
    double num_default = tablemodel->get_cey_bt_unit_value(ch_unitno, "num_default").toDouble();
    QString ch_specialflag = "N";
    if(time_promotion
            && "1" == tablemodel->get_cey_bt_dish_value(ch_dishno, "ch_special_type2").toString()) {
        num_default = 1;//第二份半价，默认数量固定为1
        ch_weight = "N";//第二份半价，无法称重
        ch_specialflag = "Y";
        gooddegree++;
    }
    QString sql0 =
            " SELECT b.ch_dishno, (case ch_suitflag when 'Y' then 'P' else 'N' end) as ch_suitflag2, (case ch_suitflag when 'Y' then ch_dishno else '' end) as ch_suitno,  1 * %1 as num_default  FROM cey_bt_dish b where b.ch_dishno = '%2' and b.ch_stopflag = 'N' ";

    if(ch_suitflag == "Y" ) {
        sql0 += " union "
                "(select b.ch_dishno, 'Y' as ch_suitflag2, ch_suitno, a.num_num as num_default  from cey_bt_dish_suit a left join cey_bt_dish b  on a.ch_dishno = b.ch_dishno  where a.ch_suitno = '%2' and b.ch_stopflag = 'N')  ;";
    }

    QDateTime dt = n_func::f_get_sysdatetime();
    const int const_row = tablemodel->rowCount();
    int row = const_row;
    ///-------------------开始预插入
    tablemodel->keepdata_disabled_begin_rowcount(const_row);
    query.execSelect(sql0.arg(num_default).arg(ch_dishno));//只记录，点菜编码、主子套菜的标记、主套菜编码
    while(query.next()){
        todish_fill_data(query, dt, vch_print_memo);
    }
    tablemodel->model_data_set(const_row, "num_price", num_price);
    tablemodel->model_data_set(const_row, "ch_specialflag", ch_specialflag);
    if(tablemodel->get_ch_discount_by_map(ch_dishno) == "Y") {//
        tablemodel->model_data_set(const_row, "int_discount", int_discount);
    } else {
        tablemodel->model_data_set(const_row, "int_discount", 100);
    }

    if(num_price != tablemodel->model_data(row, "num_price_org").toFloat()) {
        gooddegree ++;
    }
    //判断是否是称重商品
    if(ch_weight == "Y") {
        ///称重时初始数量为0
        ///称重后无法进入菜品详细界面
        take_weight dialog(tablemodel->model_data(const_row, "ch_dishno").toString(),
                           tablemodel->model_data(const_row, "num_price").toFloat(),
                           tablemodel->model_data_NIBOLAN(const_row, "num_num - num_back"),
                           tablemodel->model_data(const_row, "int_discount").toInt(),
                           this);
        dialog.setWindowTitle(vch_dishname + "  "+ tr("取重"));
        if(QDialog::Rejected == lds_roundeddialog_rect_align(&dialog).exec()){
            goto CANCEL;
        }
        //获取重量，等待提交
        float num_num_old = tablemodel->model_data_NIBOLAN(const_row, "num_num-num_back");
        tablemodel->model_data_set(const_row, "num_num", dialog._weight);
        tablemodel->update_num_num_Y(num_num_old, row);
        gooddegree ++;
    }
    //!菜品详细------------------------------begin
    if(gooddegree == 0
            && "1" == n_func::f_get_sysparm("ordertonum")){
        w_scr_dish_num_cook_Dialog dialog(tablemodel, const_row, this);
        if(QDialog::Accepted != lds_roundeddialog_rect_align(&dialog).exec()) {
            goto CANCEL;
        }
        dialog.commitModel(tablemodel, const_row);
    }
    //!菜品详细------------------------------end
    //做法
    if(!tablemodel->model_data(const_row, "vch_print_memo").toString().isEmpty()) {
        gooddegree ++;
    }
    //折扣
    if(tablemodel->model_data(const_row, "int_discount").toInt() != 100){
        gooddegree ++;
    }
    //点菜数量
    if(tablemodel->model_data(const_row, "num_num").toFloat()  <= 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("点菜数量不能小于等于0"), true);
        goto CANCEL;
    }

    //估清check
    guqing_check_msg.clear();
    guqing_check_flag = true;
    for(int row = const_row; row < tablemodel->rowCount(); row ++) {
        QString ch_dishno = tablemodel->model_data(row, "ch_dishno").toString();
        float num_cur_num = tablemodel->model_data_NIBOLAN(row, "num_num-num_back");
        if(!fexpandmain_2::guqing_check(guqing_check_msg, ch_dishno, num_cur_num)) {
            guqing_check_flag = false;
        }
    }
    if(!guqing_check_msg.isEmpty()) {
        if(!guqing_check_flag) {
            guqing_check_msg += "\n" + tr("操作失败");
        }
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, guqing_check_msg);
        if(!guqing_check_flag) {
            goto CANCEL;
        }
    }

    //累计
    if(gooddegree == 0
            &&false == lds::sysconf->value("system_setting/samedishNotadd_order", false).toBool()) {
        QSet<int> rows = tablemodel->ch_dishno_set_row_map_GET(ch_dishno);//由于在select ch_dishno的时候，不在插入ch_dishno_set_row_map中已落单的内容
        foreach(int r, rows) {
            if(r != const_row){
                float addnum = tablemodel->model_data(const_row, "num_num").toFloat();
                float num_num_old = tablemodel->model_data(r, "num_num").toFloat();
                tablemodel->removeRows(const_row, tablemodel->rowCount() - const_row);
                tablemodel->keepdata_enabled();
                tablemodel->model_data_set(r, "num_num", tablemodel->model_data(r, "num_num").toFloat() + addnum);
                if(tablemodel->model_data(r, "ch_suitflag").toString() == "P") {
                    tablemodel->update_num_num_Y(num_num_old, r);
                }
                row = r;
                tableView->update(tablemodel->model_index(r, 0));
                frontViewDialog_virtual::static_display_price(QString().sprintf("%0.2f", tablemodel->model_data(row, "num_price").toFloat()), vch_dishname);
                goto SELECTROW;
            }
        }
    }
    /////////////////
    tablemodel->keepdata_submit();
    if("1" == tablemodel->get_cey_sys_parameter_value("qt_Y_dish_fold")){
        tablemodel->keepdata_Y_dish_fold(tableView, const_row);
    }
    frontViewDialog_virtual::static_display_price(QString().sprintf("%0.2f", tablemodel->model_data(const_row, "num_price").toFloat()), vch_dishname);
    goto SELECTROW;
CANCEL:
    tablemodel->removeRows(const_row, tablemodel->rowCount() - const_row);
SELECTROW:
    tablemodel->keepdata_enabled();
    if(row>=0 && row < tablemodel->rowCount()) {
        tableView->scaleHeight(tableView->currentIndex().row(), row);
        tableView->selectRow(row);
    }
}

void w_scr_dish_main::todishclick(const QString &ch_dishno)
{
    if(ch_dishno == "-3") {
        toLongPressUpdateDishImageAdd();
        return;
    }
    if(ch_dishno.isEmpty()) {
        return;
    }

    int int_discount = 100;
    if(public_sql::discount_off_mode() == "1") {
        int_discount = 0;
    }
    QString dishno = ch_dishno;
    QString vch_print_memo;
    float num_price;
    lds_query query;
    //分组
    if(w_scr_dish_group_dialog::check_group(dishno)) {
        w_scr_dish_group_dialog dialog(dishno, this);
        if(QDialog::Accepted != lds_roundeddialog_rect_align(&dialog).exec()) {
            return;
        }
        dishno = dialog.ret_ch_dishno;
    }

    //负库存不允许消费
    if(n_func::f_get_sysparm_q(query,"inventory_negative_cannot_pay","0") == "1") {
        query.execSelect(QString(" select num_num from cey_st_material_num where ch_materialno = '%1'; ")
                         .arg(ch_dishno));
        if(query.next()) {
            if(query.recordValue("num_num").toDouble() <= 0) {
                lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("负库存不允许消费"));
                return;
            }
        }
    }
    //做法
    if(tablemodel->can_cook_pop(dishno)) {
        w_scr_dish_cook dialog(dishno, this);
        if(QDialog::Accepted != lds_roundeddialog_rect_align(&dialog).exec()) {
            return;
        }
        vch_print_memo = dialog.vch_print_memo;
    }

    //促销
    bool time_promotion = can_promotion();
    if(!get_num_price_when_dishclick(dishno, num_price, int_discount, time_promotion)) {
        return;
    }

    //点菜
    toadddish(dishno, vch_print_memo, num_price, int_discount, time_promotion);
}

bool w_scr_dish_main::to_dish_del(const int row, QString *errstring)
{
    if(tablemodel->model_data(row, "ch_suitflag").toString() == "Y") {
        if(errstring)*errstring = tr("套菜明细不能单独删除");
        return false;
    }
    int row0 = row + 1;
    if(tablemodel->model_data(row, "ch_suitflag").toString() == "P") {
        while(row0 < tablemodel->rowCount() && tablemodel->model_data(row0, "ch_suitflag").toString() == "Y"){
            row0++;
        }
    }

    tablemodel->removeRows(row, row0-row);
    int restore_row = qMax(0, qMin(row, tablemodel->rowCount()-1));//删除为空时，0， 删除最后一个时，为最后一个，
    for(;restore_row >= 0 && restore_row < tablemodel->rowCount(); restore_row --) {
        if(tablemodel->model_data(restore_row, "ch_suitflag").toString() == "Y"){//删除后选中行为子套菜时，自动上移至主套菜
            continue;
        }
        break;
    }

    tableView->scaleHeight(-1, restore_row);
    tableView->selectRow(restore_row);

    frontViewDialog_virtual::static_display_cash(QString().sprintf("%0.2f", tablemodel->getKeepdata().dish_total() ));
    //删除后，int_id重新排布
    for(int rowBegin = row ; rowBegin <tablemodel->rowCount(); rowBegin ++) {
        int int_id = rowBegin;
        if(rowBegin > 0) {
            int_id = tablemodel->model_data(rowBegin - 1, "int_id").toInt();
        }
        int_id++;
        tablemodel->model_data_set(rowBegin, "int_id", int_id);
    }

    if(tablemodel->rowCount() == 0) {
        to_payfor_info("", w_scr_dish_pay_widget::OPERAT_NONE);
    }

    return true;
}

bool w_scr_dish_main::to_dish_del(const QModelIndex &index)
{
    return to_dish_del(index.row(), 0);
}

bool w_scr_dish_main::add_dish_by_scanbar(const QString &pay_barcode)
{
    QString _price;
    QString _weight;
    QString _amount;
    lds_query query;
    QString ch_dishno;
    QDateTime dt = n_func::f_get_sysdatetime();
    QString  vch_barcode;

    //条码秤码
    if(CommonHeader::barcode_scale_resolve(vch_barcode,_price,_weight,_amount,pay_barcode)){
        query.execSelect(QString("select * from cey_bt_dish a  where a.ch_stopflag <> 'Y' and a.vch_barcode='%1' ")
                         .arg(vch_barcode));
        if(query.next()) {
            if(!_weight.isEmpty()&&!_amount.isEmpty()){
                float f  = 0;
                if(_weight.toFloat() != 0) f = _amount.toFloat() /_weight.toFloat();
                _price=QString::number(f);
            }

            if(_price.isEmpty()){
                _price=QString::number(query.recordValue("num_price").toFloat());
            } else {
                _price=QString::number(_price.toFloat());
            }

            if(_weight.isEmpty()&&!_amount.isEmpty()){
                float f  = 0;
                if(_price.toFloat() != 0) f = _amount.toFloat() /_price.toFloat();
                _weight=QString::number(f,  'f', tablemodel->get_cey_sys_parameter_value("bp_fmt_num_dec", 3).toInt());
            }

            if(!_weight.isEmpty()&&_amount.isEmpty()){
                _amount=QString::number(_weight.toFloat()*_price.toFloat(), 'f', tablemodel->get_cey_sys_parameter_value("bp_fmt_amount_dec", 3).toInt());
            }

            if(_weight.isEmpty()&&_amount.isEmpty()){
                _weight="1";
                _amount=QString::number(_weight.toFloat()*_price.toFloat(), 'f', tablemodel->get_cey_sys_parameter_value("bp_fmt_amount_dec", 3).toInt());;
            }
            ch_dishno = query.recordValue("ch_dishno").toString();

            tablemodel->keepdata_disabled_begin_rowcount();
            todish_fill_data(query, dt);
            tablemodel->model_data_set(tablemodel->rowCount() - 1, "num_num", _weight);
            tablemodel->model_data_set(tablemodel->rowCount() - 1, "num_price", _price);
            tablemodel->model_data_set(tablemodel->rowCount() - 1, "ch_suitflag",  "N");
            tablemodel->keepdata_submit();

            return true;
        }
    }

    vch_barcode = pay_barcode;
    //条码
    query.execSelect(QString("select ch_dishno from cey_bt_dish  a where a.ch_stopflag <> 'Y' and  vch_barcode = '%1'  ")
                     .arg(vch_barcode));
    if(query.next()){
        ch_dishno = query.recordValue("ch_dishno").toString();
        todishclick(ch_dishno);
        return true;
    }
    //编码
    query.execSelect(QString("select ch_dishno from cey_bt_dish  a where a.ch_stopflag <> 'Y' and ch_dishno = '%1' ")
                     .arg(vch_barcode));
    if(query.next()){
        ch_dishno = query.recordValue("ch_dishno").toString();
        todishclick(ch_dishno);
        return true;
    }
    return false;
}

void w_scr_dish_main::toprint()
{
    QString errstring;
    bool reject = false;
    if(tablemodel->rowCount() == 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("没有点菜记录"));
        return;
    }
    switch(lds_messagebox::question(this, MESSAGE_TITLE_VOID, tr("点菜信息有更改"), tr("落单并返回桌台"), tr("落单"), tr("取消"))){
    case 0://落单并返回桌台
        reject = true;
        break;
    case 1://仅落单
        reject = false;
        break;
    case 2://取消
        return;
    }
    if(!printdish_tran_saction(errstring)) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
        return ;
    }
    //退出
    if(reject)
        this->reject();
}

void w_scr_dish_main::tocuicaipiece()
{
    w_scr_dish_cuicaipiece_choose dialog(tablemodel->cur_ch_billno(), w_bt_dish::KITCHEN_CUICAI,  this);
    dialog.setWindowTitle(tr("单品催菜"));
    if(QDialog::Accepted == lds_roundeddialog_rect_align(&dialog).exec()){

    }
}

void w_scr_dish_main::tocuicaitable()
{
    lds_query query;

    if(tablemodel->rowCountOp(lds_model_sqltablemodel::OpInsert) > 0) {
        if(1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, tr("未落单将不会执行本操作")+","+("是否整桌催菜") ,tr("是"), tr("否"))) {
            return;
        }
    } else {
        if(1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, tr("是否整桌催菜") ,tr("是"), tr("否"))) {
            return;
        }
    }
    lds_query::tran_saction();
    if(query.execUpdate("cey_u_orderdish_print_tr",
                        "vch_plan_k3o3_state", QString(6, w_bt_dish::k2o3_state2hex_char(w_bt_dish::KITCHEN_CUICAI)),
                        qrtEqual("ch_billno", ch_billno))) {

        lds_query::com_mit();
        lds_messagebox::information(this,MESSAGE_TITLE_VOID, tr("操作成功"));
        return;
    }
    lds_query::roll_back();
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("操作失败"));
}

void w_scr_dish_main::tocuicaitype()
{
    lds_query query;
    QList< QPair<QString, QString> > datamap;
    query.execSelect(QString("select b.ch_dish_typeno, c.vch_dish_typename , sum((num_num-num_back)) as dish_num"
                             " from cey_u_orderdish a, cey_bt_dish b, cey_bt_dish_type c where a.ch_dishno = b.ch_dishno and b.ch_dish_typeno = c.ch_dish_typeno and ch_billno = '%1' group by ch_dish_typeno")
                     .arg(ch_billno));
    while(query.next()){
        datamap.append(QPair<QString, QString>(query.recordValue("ch_dish_typeno").toString(),
                                               query.recordValue("vch_dish_typename").toString() + "\n" + query.recordValue("dish_num").toString())
                       );
    }
    if(datamap.isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("没有菜品被选中"));
        return;
    }
    //2
    w_scr_dish_cuicai_choose dialog(this);
    dialog.ui->label_return->hide();
    dialog.ui->label_state->setText(tr("未落单将不会执行本操作"));
    dialog.reloaddata(datamap);
    dialog.setWindowTitle(tr("类别催菜"));
    if(QDialog::Accepted == lds_roundeddialog_rect_align(&dialog).exec()){

        lds_query::tran_saction();
        QStringList ch_dish_typeno_list = dialog.hasSelected();
        foreach(const QString & ch_dish_typeno, ch_dish_typeno_list) {
            if(!query.execUpdate("cey_u_orderdish_print_tr",
                                 QStringList() << "int_orderflow",
                                 QString("select a.int_orderflow, '%1' as `vch_plan_k3o3_state` from cey_u_orderdish_print_tr a, cey_bt_dish b where "
                                         " a.ch_dishno = b.ch_dishno and b.ch_dish_typeno = '%2' and a.ch_billno = '%3'  ")
                                 .arg(QString(6, w_bt_dish::k2o3_state2hex_char(w_bt_dish::KITCHEN_CUICAI)))
                                 .arg(ch_dish_typeno)
                                 .arg(ch_billno))) {
                goto rollback;
            }
        }
        lds_query::com_mit();
        lds_messagebox::information(this,MESSAGE_TITLE_VOID, tr("操作成功"));
        return;
    }
    return;
rollback:
    lds_query::roll_back();
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("操作失败"));
    return;
}

void w_scr_dish_main::toguaqi()
{
    QString errstring;
    lds_query query;
    int choice = -1;
    query.execSelect(QString("select max(dt_operdate) from cey_u_orderdish where ch_billno = '%1' ").arg(tablemodel->cur_ch_billno()));
    query.next();
    QDateTime dt_operdate_max = query.recordValue(0).toDateTime();


    //
    int print_int_flowid_max;
    query.execSelect(QString("select max(int_flowid) from cey_u_orderdish where ch_billno = '%1' ")
                     .arg(ch_billno));
    query.next();
    print_int_flowid_max = query.recordValue(0).toInt();
    //
    if(tablemodel->rowCountOp(lds_model_sqltablemodel::OpInsert)) {
        switch(lds_messagebox::question(this, MESSAGE_TITLE_VOID, tr("有未落单菜品"), tr("只挂起未落单的菜品"), tr("全部挂起"), tr("取消"))){
        case 0://只挂起未落单的菜品
            choice = 0;
            break;
        case 1://全部挂起
            choice = 1;
            break;
        case 2://取消
            choice = 2;
            return;
        }
        if(!tablemodel->try_commit_data(errstring)){
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("操作失败")+"\n"+errstring);
            return;
        }
    } else {
        if(1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, tr("是否整桌挂起"), tr("是"), tr("否"))) {
            return;
        }
        choice = 1;
    }

    lds_query::tran_saction();
    switch(choice) {
    case 0://只挂起未落单的菜品
    {
        if(!query.execUpdate("cey_u_orderdish_print_tr",
                             "vch_plan_k3o3_state", QString(6, w_bt_dish::k2o3_state2hex_char(w_bt_dish::KITCHEN_GUAQI)),
                             QString("  ch_billno = '%1' and dt_operdate > '%2'  ")
                             .arg(ch_billno)
                             .arg(dt_operdate_max.toString("yyyy-MM-dd hh:mm:ss"))
                             )) {
            goto rollback;
        }
        lds_messagebox::information(this,MESSAGE_TITLE_VOID, tr("操作成功"));
    }
        break;
    case 1://全部挂起
    {
        if(!query.execUpdate("cey_u_orderdish_print_tr",
                             "vch_plan_k3o3_state", QString(6, w_bt_dish::k2o3_state2hex_char(w_bt_dish::KITCHEN_GUAQI)),
                             QString(" ch_billno = '%1' ").arg(ch_billno))) {
            goto rollback;
        }
        lds_messagebox::information(this,MESSAGE_TITLE_VOID, tr("操作成功"));
    }
        break;
    }

    lds_query::com_mit();
    public_printby_ch_billno::print_label(ch_billno, print_int_flowid_max);
    tablemodel->select(tablemodel->cur_ch_billno(), this);
    return;
rollback:
    lds_query::roll_back();
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("操作失败"));
    return;
}

void w_scr_dish_main::tojiaodanpiece()
{
    w_scr_dish_cuicaipiece_choose dialog(tablemodel->cur_ch_billno(), w_bt_dish::KITCHEN_JIAOQI,  this);
    dialog.setWindowTitle(tr("单品叫起"));
    if(QDialog::Accepted == lds_roundeddialog_rect_align(&dialog).exec()){

    }
}

void w_scr_dish_main::tojiaodantable()
{
    lds_query query;

    if(tablemodel->rowCountOp(lds_model_sqltablemodel::OpInsert) > 0) {
        if(1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, tr("未落单将不会执行本操作")+","+("是否整桌叫起") ,tr("是"), tr("否"))) {
            return;
        }
    } else {
        if(1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, tr("是否整桌叫起") ,tr("是"), tr("否"))) {
            return;
        }
    }
    lds_query::tran_saction();
    if(query.execUpdate("cey_u_orderdish_print_tr",
                        "vch_plan_k3o3_state", QString(6, w_bt_dish::k2o3_state2hex_char(w_bt_dish::KITCHEN_JIAOQI)),
                        qrtEqual("ch_billno", ch_billno))) {

        lds_query::com_mit();
        lds_messagebox::information(this,MESSAGE_TITLE_VOID, tr("操作成功"));
        return;
    }
    lds_query::roll_back();
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("操作失败"));
}

void w_scr_dish_main::tojiaodantype()
{
    lds_query query;
    QList< QPair<QString, QString> > datamap;
    query.execSelect(QString("select b.ch_dish_typeno, c.vch_dish_typename , sum((num_num-num_back)) as dish_num"
                             " from cey_u_orderdish a, cey_bt_dish b, cey_bt_dish_type c where a.ch_dishno = b.ch_dishno and b.ch_dish_typeno = c.ch_dish_typeno and ch_billno = '%1' group by ch_dish_typeno")
                     .arg(ch_billno));
    while(query.next()){
        datamap.append(QPair<QString, QString>(query.recordValue("ch_dish_typeno").toString(),
                                               query.recordValue("vch_dish_typename").toString() + "\n" + query.recordValue("dish_num").toString())
                       );
    }
    if(datamap.isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("没有菜品被选中"));
        return;
    }

    //2
    w_scr_dish_cuicai_choose dialog(this);
    dialog.ui->label_return->hide();
    dialog.ui->label_state->setText(tr("未落单将不会执行本操作"));
    dialog.reloaddata(datamap);
    dialog.setWindowTitle(tr("类别叫起"));
    if(QDialog::Accepted == lds_roundeddialog_rect_align(&dialog).exec()){

        lds_query::tran_saction();
        QStringList ch_dish_typeno_list = dialog.hasSelected();
        foreach(const QString & ch_dish_typeno, ch_dish_typeno_list) {
            if(!query.execUpdate("cey_u_orderdish_print_tr",
                                 QStringList() << "int_orderflow",
                                 QString(" select a.int_orderflow, '%1' as `vch_plan_k3o3_state`  from cey_u_orderdish_print_tr a, cey_bt_dish b where a.ch_dishno = b.ch_dishno and b.ch_dish_typeno = '%2' and a.ch_billno = '%3'  ")
                                 .arg(QString(6, w_bt_dish::k2o3_state2hex_char(w_bt_dish::KITCHEN_JIAOQI)))
                                 .arg(ch_dish_typeno)
                                 .arg(ch_billno))) {
                goto rollback;
            }
        }
        lds_query::com_mit();
        lds_messagebox::information(this,MESSAGE_TITLE_VOID, tr("操作成功"));
        return;
    }
    return;
rollback:
    lds_query::roll_back();
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("操作失败"));
    return;
}

void w_scr_dish_main::toDishTmp()
{
    linshi_cook dialog(this);
    dialog.setWindowTitle(tr("临时菜品"));
    if(QDialog::Accepted == lds_roundeddialog_rect_align(&dialog).exec()){
        todishclick(dialog.ch_dishno);
    }
    tablemodel->tablename_kvmap_clear_for_select_and_autoselect();
}

void w_scr_dish_main::toPaySomeDish()
{
    //rowListAsc
    QList<int> rowListAsc;
    w_scr_dish_main_auto_exit_multiSelection d(tableView);
    if(QDialog::Rejected == w_scr_dish_main::paysomedishGetRows(this, tablemodel, rowListAsc, true)) {
        return;
    }
    //
    tableView->clearSelection();
    foreach(const int &row, rowListAsc) {
        tableView->selectRow(row);
    }
    //
    QString errstring;
    lds_query::tran_saction();
    if(tablemodel->try_update_data(errstring)) {
        lds_query::com_mit();
    } else {
        lds_query::roll_back();
    }
    //
    if(w_scr_dish_main::separatePaybill(this, tablemodel, rowListAsc, false)) {
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, tr("结算成功"), true);
    }
}

void w_scr_dish_main::toWaiDai()
{
    int row = tableView->currentIndex().row();
    if(tablemodel->rowCountOp(lds_model_sqltablemodel::OpInsert) == 0 ) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("所有菜品已落单") + "," + tr("操作失败"));
        return;
    }

    if( row < 0) {
        switch(lds_messagebox::question(this, MESSAGE_TITLE_VOID, tr("外带"), tr("全部外带"), tr("全部取消"), tr("取消"))){
        case 0:
            goto all_waidai;
        case 1:
            goto all_waidai_cancel;
        }
    }

    if( row >= 0) {
        if(tablemodel->model_data(row, "ch_waidai_flag").toString() == "Y") {
            switch(lds_messagebox::question(this, MESSAGE_TITLE_VOID,
                                            tablemodel->tablename_kvmap_value("cey_bt_dish", tablemodel->model_data(row, "ch_dishno").toString(),  "ch_dishno", "vch_dishname").toString()
                                            + "\n" +  tr("只能操作未落单的菜品"), tr("单品取消"), tr("全部外带"), tr("全部取消"), tr("取消"))) {
            case 0:
                goto row_waidai_cancel;
            case 1:
                goto all_waidai;
            case 2:
                goto all_waidai_cancel;
            }
        } else {
            switch(lds_messagebox::question(this, MESSAGE_TITLE_VOID,
                                            tablemodel->tablename_kvmap_value("cey_bt_dish", tablemodel->model_data(row, "ch_dishno").toString(),  "ch_dishno", "vch_dishname").toString()
                                            + "\n" + tr("只能操作未落单的菜品"), tr("单品外带"), tr("全部外带"), tr("全部取消"), tr("取消"))){
            case 0:
                goto row_waidai;
            case 1:
                goto all_waidai;
            case 2:
                goto all_waidai_cancel;
            }
        }
    }
    return;

all_waidai:
    for(int row = 0; row < tablemodel->rowCount(); row++) {
        if(tablemodel->opera(row) == lds_model_sqltablemodel::OpInsert) {
            tablemodel->model_data_set(row, "ch_waidai_flag", "Y");
        }
    }
    return;

all_waidai_cancel:
    for(int row = 0; row < tablemodel->rowCount(); row++) {
        if(tablemodel->opera(row) == lds_model_sqltablemodel::OpInsert) {
            tablemodel->model_data_set(row, "ch_waidai_flag", "N");
        }
    }
    return;

row_waidai:
    if(tablemodel->opera(row) != lds_model_sqltablemodel::OpInsert) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("菜品已落单") + "," + tr("操作失败"));
        return;
    }
    tablemodel->model_data_set(row, "ch_waidai_flag", "Y");
    return;

row_waidai_cancel:
    if(tablemodel->opera(row) != lds_model_sqltablemodel::OpInsert) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("菜品已落单") + "," + tr("操作失败"));
        return;
    }
    tablemodel->model_data_set(row, "ch_waidai_flag", "N");
    return;
}

void w_scr_dish_main::toTelComing(const QString &telNo)
{
    if(false == lds::sysconf->value("system_setting/read_tel_enable", false).toBool()){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("请在软件后台启用此功能"));
        return;
    }
    QWidget *aw = qApp->activeWindow();
    if(aw && QString("w_scr_dish_main_view_800x600") == aw->metaObject()->className()) {
        static bool isRunning = false;
        if(isRunning) return;
        isRunning = true;

        w_m_member_telComing dialog(true, telNo, this);
        if(QDialog::Accepted == lds_roundeddialog_rect_align(&dialog).exec()) {
            if(1 == fexpandmain_2::to_m_read(this, tablemodel, dialog.ret_vch_memberno)) {
                if(lds::sysconf->value("w_m_member_telComing/delRecord", true).toBool()) {
                    dialog.delTelFileTelNo(dialog.ret_tel);
                }
            }
            updatememberchecked();
        }

        isRunning = false;
    }
}

int w_scr_dish_main::paysomedishGetRows(QWidget *parent, fexpandmain_model_sqltablemodel_data *orderdish_mode,  QList<int> &rowListAsc, bool preprint)
{

    QMap<QString/*key*/, QVariant/*value*/> map;
    map.insert( "P", tr("主套菜"));
    map.insert( "Y", tr("子套菜"));

    w_scr_dish_restaurant_paysomedish dialog(parent);
    dialog.ui->label_desc->setMaximumHeight(30);
    dialog.ui->label_desc->setText(tr("菜品左边*号表示未落单菜品"));
    dialog.ui->widget->setup(QStringList() << tr("全选") << tr("全不选") << tr("上一页") << tr("下一页") << tr("预打单")<< tr("结算") << tr("取消"), Qt::AlignRight);

    dialog.ui->tableView->setTransferHheader();
    dialog.ui->tableView->setModel(orderdish_mode);
    dialog.ui->tableView->justShowColumns(QList<int>() << orderdish_mode->fieldIndex("ch_dishno")
                                          << orderdish_mode->fieldIndex("num_num")
                                          << orderdish_mode->fieldIndex("num_back")
                                          << orderdish_mode->fieldIndex("ch_suitflag"));
    dialog.ui->tableView->setColumnWidth(orderdish_mode->fieldIndex("ch_dishno"), 300);
    dialog.ui->tableView->horizontalHeader()->setStretchLastSection(true);
    dialog.ui->tableView->setSelectionMode(QTableView::MultiSelection);
    dialog.ui->tableView->setItemDelegateForColumn(orderdish_mode->fieldIndex("ch_dishno"), new lds_model_sqltablemodel_delegate_com(
                                                       &dialog, orderdish_mode, "cey_bt_dish","ch_dishno","vch_dishname"
                                                       ));
    dialog.ui->tableView->setItemDelegateForColumn(orderdish_mode->fieldIndex("ch_suitflag"), new lds_model_sqltablemodel_delegate_com(
                                                       &dialog, &map
                                                       ));

    connect(dialog.ui->widget->index_widget(tr("结算")), SIGNAL(clicked()),&dialog,SLOT(accept()));
    connect(dialog.ui->widget->index_widget(tr("取消")), SIGNAL(clicked()),&dialog,SLOT(reject()));
    connect(dialog.ui->widget->index_widget(tr("上一页")), SIGNAL(clicked()),dialog.ui->tableView,SLOT(toPageUp()));
    connect(dialog.ui->widget->index_widget(tr("下一页")), SIGNAL(clicked()),dialog.ui->tableView,SLOT(toPageDown()));
    connect(dialog.ui->widget->index_widget(tr("全选")), SIGNAL(clicked()),dialog.ui->tableView,SLOT(toSelectAll()));
    connect(dialog.ui->widget->index_widget(tr("全不选")), SIGNAL(clicked()),dialog.ui->tableView,SLOT(toClearAll()));
    connect(dialog.ui->widget->index_widget(tr("预打单")), SIGNAL(clicked()),&dialog,SLOT(topaysomedish_preprint()));
    if(!preprint) {
        dialog.ui->widget->index_widget(tr("预打单"))->hide();
    }
    dialog.setWindowTitle(tr("分开结算"));

    int ret = lds_roundeddialog_rect_align(&dialog).exec();
    rowListAsc = dialog.ui->tableView->getSelectedRowListAsc();
    return ret;
}

void w_scr_dish_main::initLogo(QLabel *label_logo, QLabel *label_text)
{
    label_logo->setPixmap(back_view_dialog::get_fastfdbacklogo());
    QStringList ret_list = back_view_dialog::get_fastfdbacktext();
    label_text->setText(ret_list.value(0).trimmed());
    label_text->setAlignment(Qt::AlignBottom|Qt::AlignLeft);
    label_text->setStyleSheet("font-size:14pt;");
    if(lds::MAIN_WINDOW_SIZE.width() == 800) {
        label_text->setStyleSheet("font-size:12pt;");
    }
}

QString w_scr_dish_main::cur_ch_billno()
{
    return ch_billno;
}

bool w_scr_dish_main::paybill()
{
    //1
    w_scr_dish_main_auto_exit_multiSelection d(tableView);

    lds_query query;
    if(tablemodel->rowCount() == 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("没有点菜记录,不能付款"), true);
        return false;
    }
    //print_int_flowid_max
    int print_int_flowid_max = -1;
    query.execSelect(QString("select max(int_flowid) from cey_u_orderdish where ch_billno = '%1' ")
                     .arg(tablemodel->cur_ch_billno()));
    if(query.next())
        print_int_flowid_max = query.recordValue(0).toInt();
    //

    w_scr_dish_pay_widget::operatModes flags;
    if(windowTypeKuaiCan == window_type) {
        flags = w_scr_dish_pay_widget::OPERAT_PAY|w_scr_dish_pay_widget::TYPE_FAST;
    } else {
        flags = w_scr_dish_pay_widget::OPERAT_PAY|w_scr_dish_pay_widget::TYPE_RESTAURANT;
    }

    tablemodel->getKeepdata().print();

    w_scr_dish_pay_dialog dialog(flags, this, tablemodel, print_int_flowid_max);
    if(QDialog::Accepted == dialog.exec()){//付款成功
        QString old_ch_billno = tablemodel->cur_ch_billno();
        if(windowTypeKuaiCan == window_type) {
            refreshChBillno();
            tableView_dish->m->refreshcur();
            to_payfor_info_ch_billno(old_ch_billno, w_scr_dish_pay_widget::OPERAT_PAY);
        } else {
            tablemodel->select("1=2", this);//1=2必定为空
            to_payfor_info_ch_billno(old_ch_billno, w_scr_dish_pay_widget::OPERAT_PAY);
            lds_messagebox::information(this, MESSAGE_TITLE_VOID, tr("结算成功"), true);
        }
        return true;
    }
    return false;
}

void w_scr_dish_main::removeAllInsertDish()
{
    for(int row = tablemodel->rowCount() - 1; row >= 0; row --) {
        if(tablemodel->opera(row) == lds_model_sqltablemodel::OpInsert) {
            tablemodel->removeRows(row, 1);
        }
    }
}

bool w_scr_dish_main::separatePaybill(QWidget *parent, fexpandmain_model_sqltablemodel_data *tablemodel, const QList<int> &rowListAsc, bool table_pay)
{
    //ch_billno_new
    QString ch_billno = tablemodel->cur_ch_billno();
    QString ch_tableno = tablemodel->get_cey_u_table_value(tablemodel->cur_ch_billno(), "ch_tableno");

    lds_query::tran_saction();
    lds_query query;
    QString ch_billno_new;
    if(false == w_scr_dish_restaurant_div_pop::getMaxChbillnoAdd1_insert_cey_u_master("B", ch_billno_new)) {
        lds_query::roll_back();
        lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, tr("操作失败"));
        return false ;
    }
    if(!query.execInsert("cey_u_table",qrtVariantPairList()

                         << qrtVariantPair("ch_billno", ch_billno_new)<< qrtVariantPair("ch_tableno", ch_tableno)<< qrtVariantPair("int_person", 1)<< qrtVariantPair("ch_state", 1)<< qrtVariantPair("vch_operID", public_sql::gs_operid)
                         << qrtVariantPair("dt_operdate", n_func::f_get_sysdatetime())<< qrtVariantPair("ch_order_state", 0)<< qrtVariantPair("vch_waiter", "")<< qrtVariantPair("vch_memo", tr("分开结算")))) {

        lds_query::roll_back();
        lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, tr("操作失败"));
        return false ;
    }
    lds_query::com_mit();

    //print_int_flowid_max
    int print_int_flowid_max = -1;
    query.execSelect(QString("select max(int_flowid) from cey_u_orderdish where ch_billno = '%1' ")
                     .arg(ch_billno));
    if(query.next())
        print_int_flowid_max = query.recordValue(0).toInt();

    //keepdata
    tablemodel->update_ch_billno(ch_billno_new, rowListAsc);
    //w_scr_dish_pay
    bool payFlag = false;
    if(table_pay) {
        w_scr_dish_main_table_pay dialog(tablemodel, parent, rowListAsc);
        dialog.move(0, 0);
        if(QDialog::Accepted == dialog.exec()) {
            payFlag = true;

            //新增的菜品付款部分
            for(int count = rowListAsc.count() - 1; count >= 0; count --) {
                if(tablemodel->opera(rowListAsc[count]) == lds_model_sqltablemodel::OpInsert) {
                    tablemodel->removeRow(rowListAsc[count]);
                }
            }
        }
    } else {
        w_scr_dish_pay_dialog dialog(w_scr_dish_pay_widget::OPERAT_PAY|w_scr_dish_pay_widget::TYPE_RESTAURANT, parent, tablemodel, print_int_flowid_max);
        if(QDialog::Accepted == dialog.exec()){//付款成功
            payFlag = true;
        }
    }
    if(payFlag) {
        //新增的菜品付款部分
        for(int count = rowListAsc.count() - 1; count >= 0; count --) {
            if(tablemodel->opera(rowListAsc[count]) == lds_model_sqltablemodel::OpInsert) {
                tablemodel->removeRow(rowListAsc[count]);
            }
        }
    }

    //新增的菜品未付款部分
    QList<QVariantList> vlist_list;
    for(int rowcount = tablemodel->rowCount() - 1; rowcount >= 0; rowcount--) {
        if(tablemodel->opera(rowcount) == lds_model_sqltablemodel::OpInsert){
            QVariantList vlist;
            for(int c = 0, ccount = tablemodel->columnCount(); c< ccount; c++) {
                vlist.append(tablemodel->model_data(rowcount, c));
            }
            vlist_list.push_front(vlist);
            tablemodel->removeRow(rowcount);
            continue;
        }
        break;
    }
    //
    tablemodel->select(ch_billno, parent);
    tablemodel->keepdata_disabled_begin_rowcount();
    for(int k = 0, kcount = vlist_list.count();  k < kcount; k++) {
        tablemodel->insertRow(tablemodel->rowCount());
        for(int c = 0 ,ccount = vlist_list[k].count(); c < ccount; c++) {
            tablemodel->model_data_set(tablemodel->rowCount() - 1, c, vlist_list[k][c]);
        }
        tablemodel->model_data_set(tablemodel->rowCount() - 1, "ch_billno", ch_billno);
    }
    tablemodel->keepdata_submit();

    //
    if(tablemodel->rowCount() == 0) {
        QString errstring;
        if(!w_scr_dish_pay_widget::cey_u_orderdish_direct_pay_transaction(ch_billno, &errstring)) {
            qDebug() << errstring;
        }
    }
    return payFlag;
}

void w_scr_dish_main::todish_fill_data(lds_query &query, const QDateTime &dt, const QString &vch_print_memo)
{
    QString vch_plan_k3o3_state;
    QString vch_plan_k3o3_id;
    QString ch_suitflag2 = query.recordValue("ch_suitflag2").toString();
    QString ch_dishno = query.recordValue("ch_dishno").toString();
    //int_id
    w_bt_dish::kitchenState x = w_bt_dish::KITCHEN_MIN_DEF;
    if(tablemodel->rowCount() != tablemodel->rowCountOp(lds_model_sqltablemodel::OpInsert)) {
        x = w_bt_dish::KITCHEN_JiACAI;
    }
    w_bt_dish::get_vch_plan_k3o3(vch_plan_k3o3_state, vch_plan_k3o3_id, ch_dishno, x);

    int row = tablemodel->rowCount();

    //int_id
    int int_id = 0;
    if(row > 0) {
        int_id = tablemodel->model_data(row - 1, "int_id").toInt();
    }
    int_id++;

    tablemodel->insertRow(row);

    tablemodel->model_data_set(row, "int_id", int_id);
    tablemodel->model_data_set(row, "ch_billno", this->ch_billno);
    tablemodel->model_data_set(row, "ch_tableno", ch_tableno);
    tablemodel->model_data_set(row, "ch_dishno", ch_dishno);
    tablemodel->model_data_set(row, "num_price", tablemodel->get_cey_bt_dish_value (ch_dishno, "num_price") );

    tablemodel->model_data_set(row, "num_price_org", tablemodel->get_cey_bt_dish_value(ch_dishno, "num_price") );
    tablemodel->model_data_set(row, "num_price_add", 0);
    tablemodel->model_data_set(row, "num_num", query.recordValue("num_default"));
    tablemodel->model_data_set(row, "num_back", 0);
    tablemodel->model_data_set(row, "vch_back_operID", "");

    tablemodel->model_data_set(row, "dt_back_operdate", lds::dtifnull());
    tablemodel->model_data_set(row, "int_discount", 100);
    tablemodel->model_data_set(row, "vch_dis_operID", "");
    tablemodel->model_data_set(row, "dt_dis_operdate", lds::dtifnull());
    tablemodel->model_data_set(row, "vch_memberno", tablemodel->vch_membernod.vch_memberno);
    tablemodel->model_data_set(row, "ch_member_belong_state", tablemodel->vch_membernod.ch_member_belong_state);

    tablemodel->model_data_set(row, "ch_consult", "");
    tablemodel->model_data_set(row, "vch_print_memo", "");
    tablemodel->model_data_set(row, "ch_suitflag", ch_suitflag2);///
    tablemodel->model_data_set(row, "ch_suitno", query.recordValue("ch_suitno"));///
    tablemodel->model_data_set(row, "ch_specialflag", "N");

    tablemodel->model_data_set(row, "ch_presentflag", "N");
    tablemodel->model_data_set(row, "vch_pre_operID", "");
    tablemodel->model_data_set(row, "dt_pre_operdate", lds::dtifnull());
    tablemodel->model_data_set(row, "vch_operID", public_sql::gs_operid);
    tablemodel->model_data_set(row, "dt_operdate", dt);

    tablemodel->model_data_set(row, "ch_togono", "");
    tablemodel->model_data_set(row, "ch_payno",  "");
    tablemodel->model_data_set(row, "vch_plan_k3o3_id", vch_plan_k3o3_id);
    tablemodel->model_data_set(row, "vch_plan_k3o3_state", vch_plan_k3o3_state);

    tablemodel->model_data_set(row, "vch_plan_desc", "");
    tablemodel->model_data_set(row, "int_rate", tablemodel->get_cey_bt_dish_value(ch_dishno, "int_rate") );//query.recordValue("int_rate"));///

    //优先级
    if(ch_suitflag2 == "Y") {
        tablemodel->model_data_set(row, "num_price", 0);
    }
    //非子套菜
    if(ch_suitflag2 != "Y" && vch_print_memo.length() > 0) {
        tablemodel->model_data_set(row, "vch_print_memo", vch_print_memo);
        tablemodel->model_data_set(row, "num_price_add", tablemodel->get_num_price_add_vch_print_memo_by_memo(vch_print_memo));
    }
}

bool w_scr_dish_main::get_num_price_when_dishclick(const QString &ch_dishno, float &num_price, int &int_discount, bool &time_promotion)
{
    //判断促销
    const float num_price_org = tablemodel->get_cey_bt_dish_value(ch_dishno, "num_price").toFloat();
    const float num_m_price = tablemodel->get_cey_bt_dish_value(ch_dishno, "num_m_price").toFloat();
    const float num_special_price = tablemodel->get_cey_bt_dish_value(ch_dishno, "num_special_price").toFloat();

    QString discount_desc;
    int int_discount_value;
    if(1 == autoDiscount(discount_desc, int_discount_value)) {
        int_discount = int_discount_value;
    }
    //
    num_price = num_price_org;
    if(tablemodel->vch_membernod.is_m_price()) {
        num_price = qMin(num_m_price, num_price);
    }
    if(tablemodel->vch_membernod.is_m_discount()) {
        int_discount = tablemodel->vch_membernod.int_discount;
    }
    //
    if(time_promotion //当前正在促销
            && tablemodel->tablename_kvmap_value_direct("cey_sys_parameter", "promotion_setting_target").toInt() == 1//会员
            && tablemodel->vch_membernod.vch_memberno.isEmpty() //未读会员
            ){
        time_promotion = false;
    }
    //
    if(time_promotion //当前正在促销
            && tablemodel->tablename_kvmap_value_direct("cey_sys_parameter", "promotion_setting_target").toInt() == 2//非会员
            && tablemodel->vch_membernod.vch_memberno.length() > 0 //已读会员
            ){
        time_promotion = false;
    }

    //时间符合促销
    if(time_promotion) {
        ///1 策略1 选择价格
        if("1" == tablemodel->get_cey_bt_dish_value(ch_dishno, "ch_special_type").toString()) {
            num_price = qMin(num_special_price, num_price);
        }
        if("2" == tablemodel->get_cey_bt_dish_value(ch_dishno, "ch_special_type").toString()) {//可选特价
            switch(lds_messagebox::question(this, MESSAGE_TITLE_VOID, tr("选择特价"),
                                            tr("特价")+":"+QString::number(num_special_price, 'f', 2), tr("现价")+":"+QString::number(num_price, 'f', 2), tr("取消"))) {
            case 0://特价
                num_price = num_special_price;
                break;
            case 1://原价
                break;
            case 2://取消
                return false;
            }
        }
    }


    ///3策略2 第二份半价、
    /**
                                 *          第二份半价，直接用字面意思，第二杯价格半价
                                 *          软件中，原价不等于现价不允许折扣，故：第二份半价不能折扣
                                 *
                                 *         商品A为第二份半价，商品B的子套餐包含A, 点商品B时，A不参与第二份半价的优惠
                                 *
                                 *         先判断会员价、然后特价、然后第二份半价、然后时价
                                 *         原件 10，会员8实际8=min(10,8)，特价9,实际8=min(9, 8)，半价4=8/2， 时价
                                 *         第二份半价的商品，不能叠加，默认数量强制为1
                                 *         第二份半价的商品，不能称重
                                 *         第二份半价的商品，不能修改数量
                                      */
    //时间符合促销
    if(time_promotion) {
        if("1" == tablemodel->get_cey_bt_dish_value(ch_dishno, "ch_special_type2").toString()) {
            if(1 == int(tablemodel->ch_dishno_dish_num_map_GET().value(ch_dishno))%2) {
                lds_messagebox::question(this, MESSAGE_TITLE_VOID,
                                         tablemodel->get_cey_bt_dish_value(ch_dishno, "vch_dishname").toString() +"  X" + QString::number(1 + tablemodel->ch_dishno_dish_num_map_GET().value(ch_dishno)) + "\n"
                                         + tr("第二份半价"));
                num_price = num_price/2;
            }
        }
    }
    ///~3策略2 第二份半价、

    ///实价菜
    if(tablemodel->get_cey_bt_dish_value(ch_dishno, "ch_curprice").toString() == "Y"){
        lds_dialog_input inputdialog(this);
        inputdialog.ui->label->setText(tr("时价菜"));
        inputdialog.ui->lineEdit->setText(QString::number(num_price));
        inputdialog.ui->label_2->setText(tr("数量"));
        if(QDialog::Accepted != lds_roundeddialog_rect_align(&inputdialog).exec()){
            return false;
        }
        num_price = inputdialog.ui->lineEdit->text().toFloat();
    }
    ///～实价菜
    return true;
}

bool w_scr_dish_main::can_promotion()
{
    //
    QVector<int> promotion_week_days;
    bool promotion_enabled = false;
    QDate promotion_setting_date =   tablemodel->tablename_kvmap_value_direct("cey_sys_parameter", "promotion_setting_date").toDate();
    QDate promotion_setting_date_2 =   tablemodel->tablename_kvmap_value_direct("cey_sys_parameter", "promotion_setting_date_2").toDate();

    QTime promotion_setting_am =   tablemodel->tablename_kvmap_value_direct("cey_sys_parameter", "promotion_setting_am").toTime();
    QTime promotion_setting_am_2 =   tablemodel->tablename_kvmap_value_direct("cey_sys_parameter", "promotion_setting_am_2").toTime();
    QTime promotion_setting_pm =   tablemodel->tablename_kvmap_value_direct("cey_sys_parameter", "promotion_setting_pm").toTime();
    QTime promotion_setting_pm_2 =   tablemodel->tablename_kvmap_value_direct("cey_sys_parameter", "promotion_setting_pm_2").toTime();

    if(tablemodel->tablename_kvmap_value_direct("cey_sys_parameter", "promotion_setting_Fri").toBool())promotion_week_days.append(5);
    if(tablemodel->tablename_kvmap_value_direct("cey_sys_parameter", "promotion_setting_Mon").toBool())promotion_week_days.append(1);
    if(tablemodel->tablename_kvmap_value_direct("cey_sys_parameter", "promotion_setting_Sat").toBool())promotion_week_days.append(6);
    if(tablemodel->tablename_kvmap_value_direct("cey_sys_parameter", "promotion_setting_Sun").toBool())promotion_week_days.append(7);

    if(tablemodel->tablename_kvmap_value_direct("cey_sys_parameter", "promotion_setting_Thur").toBool())promotion_week_days.append(4);
    if(tablemodel->tablename_kvmap_value_direct("cey_sys_parameter", "promotion_setting_Tues").toBool())promotion_week_days.append(2);
    if(tablemodel->tablename_kvmap_value_direct("cey_sys_parameter", "promotion_setting_Wed").toBool())promotion_week_days.append(3);
    QDateTime sqldt = n_func::f_get_sysdatetime();
    QDate curdate = sqldt.date();
    QTime curtime = sqldt.time();
    if(curdate >= promotion_setting_date && curdate <= promotion_setting_date_2) {
        if(promotion_week_days.contains(curdate.dayOfWeek())){
            if(promotion_setting_am <= curtime && curtime <= promotion_setting_am_2) {
                promotion_enabled = true;
            }
            if(promotion_setting_pm <= curtime && curtime <= promotion_setting_pm_2) {
                promotion_enabled = true;
            }
        }
    }

    return promotion_enabled;
}

void w_scr_dish_main::dish_add(int f, bool autoScale)
{
    int row = tableView->currentIndex().row();
    if(row < 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("没有行被选中"));
        return;
    }
    QString errstring;
    if(false == tablemodel->can_change_num(row, &errstring)) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
        return;
    }
    //
    QString ch_dishno = tablemodel->model_data(row, "ch_dishno").toString();
    QString ch_unitno = tablemodel->get_cey_bt_dish_value(ch_dishno, "ch_unitno").toString();
    double num_add = f;
    if(autoScale){
        num_add = num_add * tablemodel->get_cey_bt_unit_value(ch_unitno, "num_default").toDouble();
    }

    //guqing
    QString msg;
    if(fexpandmain_2::guqing_try_check_P(tablemodel, msg, row,  num_add)) {
        if(!msg.isEmpty()) {
            lds_messagebox::information(this, MESSAGE_TITLE_VOID, msg);
        }
    } else {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, msg + tr("操作失败"));
        return;
    }
    //~guqing

    //num_num
    double num_num_old = tablemodel->model_data_NIBOLAN(row, "num_num-num_back");
    tablemodel->model_data_set(row, "num_num", tablemodel->model_data(row, "num_num").toDouble() + num_add);

    if(tablemodel->model_data(row, "ch_suitflag").toString() == "P") {
        //num_num
        tablemodel->update_num_num_Y(num_num_old, row);
        //vch_print_memo
        tablemodel->update_vch_print_memo_Y(num_num_old, row);
        //num_price_add
        tablemodel->update_num_price_add_2_P(row);
    }
    if(tablemodel->model_data(row, "ch_suitflag").toString() == "N") {
        //vch_print_memo
        tablemodel->update_vch_print_memo_N(num_num_old, row);
        //num_price_add
        tablemodel->model_data_set(row, "num_price_add", tablemodel->get_num_price_add_vch_print_memo( tablemodel->model_data(row, "vch_print_memo").toString()));
    }

    //paint
    //Y
    int paint_row_p = row;
    if(tablemodel->model_data(paint_row_p, "ch_suitflag").toString() == "Y") {
        while(-- paint_row_p >= 0) {
            if(tablemodel->model_data(paint_row_p, "ch_suitflag").toString() == "P") {
                break;
            }
        }
        tableView->update(tablemodel->model_index(row, 0));
        tableView->update(tablemodel->model_index(paint_row_p, 0));
    }
    //P
    if(tablemodel->model_data(paint_row_p, "ch_suitflag").toString() == "P") {
        tableView->update(tablemodel->model_index(paint_row_p, 0));
        while(++ paint_row_p >= 0) {
            if(tablemodel->model_data(paint_row_p, "ch_suitflag").toString() != "Y") {
                break;
            }
            tableView->update(tablemodel->model_index(paint_row_p, 0));
        }
    }
    //N
    if(tablemodel->model_data(paint_row_p, "ch_suitflag").toString() == "N") {
        tableView->update(tablemodel->model_index(row, 0));
    }

    //if 0 del
    if(tablemodel->model_data_NIBOLAN(row, "num_num - num_back") <= 0) {
        to_dish_del(row);
    }
}

void w_scr_dish_main::toSwitchSkin()
{
    //skyblue obsidian
    QString key;
    if("obsidian" == lds::sysconf->value("system_setting/skin_name", "obsidian").toString()) {
        key = "skyblue";
    } else {
        key = "obsidian";
    }

    QFile file(QString("userdata/settings/skin/fastfd_%1.qss")
               .arg(key));
    file.open(QFile::ReadOnly);
    qApp->setStyleSheet(file.readAll());
    lds::sysconf->setValue("system_setting/skin_name", key);
}

void w_scr_dish_main::to_update_num_total(const QString &n, const QString &t)
{
    update_pay_info_set_empty();
    label_num->setText(n);
    label_total->setText(t);
}

void w_scr_dish_main::update_pay_info_set_empty()
{
    if(pay_info_widget) {
        hideWidget(pay_info_widget);
    }
}

void w_scr_dish_main::to_payfor_info(const QString &ch_payno, int paytype)
{
    ///促销读会员卡
    if(tablemodel->rowCount() == 0) {
        bool ispromotion = false;
        if(can_promotion()
                && tablemodel->tablename_kvmap_value_direct("cey_sys_parameter", "promotion_setting_target").toInt() == 1) {//会员
            ispromotion = true;
        }
        //
        QString discount_desc;
        int int_discount_value;
        autoDiscount(discount_desc, int_discount_value);
        //
        if(0 == pay_info_widget) {
            pay_info_widget =new  w_scr_dish_main_pay_info(tableView);
        }
        fexpandmain_2::showPayinfo(tableView, pay_info_widget, ch_payno, paytype, ispromotion, discount_desc);
    }
}

void w_scr_dish_main::to_payfor_info_ch_billno(const QString &ch_billno, int paytype)
{
    lds_query query;
    query.execSelect(QString("SELECT ch_payno FROM cey_u_checkout_master where ch_billno = '%1' ; ")
                     .arg(ch_billno));
    query.next();

    to_payfor_info(query.recordValue("ch_payno").toString(), paytype);
}

void w_scr_dish_main::updatememberchecked()
{
    if(pay_info_widget != 0) {
        if(lds::widgetIsShow(pay_info_widget->ui->label_discount)) {
            QString discount_desc;
            int int_discount_value;
            if(-1 != autoDiscount(discount_desc, int_discount_value))
                pay_info_widget->ui->label_discount->setText(discount_desc);
        }
    }
}

void w_scr_dish_main::update_type_subtype_view()
{
    ftableview_standmodel_sql *model_subtype = static_cast<ftableview_standmodel_sql *>(tableView_subtype->m);

    if(lds::sysconf->value("system_setting/dish_type_enabled", false).toBool()) {
        tableView_subtype->fillData_addAnimation(1, model_subtype->columnCount());
        showWidget(tableView_type);
        showWidget(tableView_subtype);

        tableView_type->setFixedHeight(lds::QT_FIXED_HEIGHT_BOTTOM);
        tableView_subtype->setFixedHeight(lds::QT_FIXED_HEIGHT_BOTTOM);
        tableView_type->move(lds::PAY_LEFT_SPACE, 0);
        tableView_subtype->move(lds::PAY_LEFT_SPACE, lds::QT_FIXED_HEIGHT_BOTTOM);
    } else {
        tableView_subtype->fillData_addAnimation(2, model_subtype->columnCount());
        hideWidget(tableView_type);
        showWidget(tableView_subtype);

        tableView_subtype->setFixedHeight(2 * lds::QT_FIXED_HEIGHT_BOTTOM);
        tableView_subtype->move(lds::PAY_LEFT_SPACE, 0);
    }
}

void w_scr_dish_main::showWidget(QWidget *w)
{
    if(w->isHidden())
        w->show();
}

void w_scr_dish_main::hideWidget(QWidget *w)
{
    if(!w->isHidden())
        w->hide();
}

bool w_scr_dish_main::printdish_tran_saction(QString &errstring)
{
    lds_query::tran_saction();
    lds_query query;
    QDateTime begin_dt;
    bool is_need_insert = tablemodel->rowCountOp(lds_model_sqltablemodel::OpInsert) > 0;
    int print_int_flowid_max;
    if(is_need_insert) {
        query.execSelect(QString("select max(int_flowid) from cey_u_orderdish where ch_billno = '%1' ")
                         .arg(ch_billno));
        query.next();
        print_int_flowid_max = query.recordValue(0).toInt();

        query.execSelect(QString("SELECT dt_operdate from cey_u_orderdish where int_flowid = '%1' ")
                         .arg(print_int_flowid_max));
        query.next();
        begin_dt = query.recordValue(0).toDateTime();
    }

    if(!try_commit_data_update_num_cost(errstring)) {
        lds_query::roll_back();
        return false;
    }
    lds_query::com_mit();
    //打印
    if(is_need_insert) {
        //消费单
        if(lds::sysconf->value("system_setting/luodanzongdan").toString() == "1"){
            public_printby_ch_billno printer(this->ch_billno);
            printer.toprint_preprint(lds::sysconf->value("system_setting/printerpath").toString(), tr("消费单"), begin_dt);
        }//success
        //标签
        public_printby_ch_billno::print_label(this->ch_billno, print_int_flowid_max);
        tablemodel->select(tablemodel->cur_ch_billno(), this);
        //厨打
        public_sql::save_login->to_everytime_to_print_failed_kitchen_bill();
    }
    return true;
}

bool w_scr_dish_main::try_commit_data_update_num_cost(QString &errstring)
{
    //try_commit_data
    if(!tablemodel->try_commit_data(errstring, false, false)) {
        return false;
    }
    //update_num_cost
    if(!update_num_cost(errstring)) {
        return false;
    }
    return true;
}

void w_scr_dish_main::loadSn(const QString &orderSn)
{
    if(!orderSn.isEmpty()) {
        QDateTime dt = n_func::f_get_sysdatetime();
        lds_query query;
        query.execSelect(QString("select b.*, (case b.ch_suitflag when 'Y' then 'P' else 'N' end) as ch_suitflag2, (case b.ch_suitflag when 'Y' then b.ch_dishno else '' end) as ch_suitno, a.number as num_default, c.vch_unitname FROM cey_web_order_dish a, cey_bt_dish b, cey_bt_unit c where  a.dishNo = b.ch_dishno and b.ch_unitno = c.ch_unitno and b.ch_stopflag = 'N' and a.orderSn = '%1' ; ")
                         .arg(orderSn));

        tablemodel->keepdata_disabled_begin_rowcount();
        while(query.next()){
            todish_fill_data(query, dt);
        }
        tablemodel->keepdata_submit();
    }
}

bool w_scr_dish_main::update_num_cost(QString &errstring)
{
    lds_query query;
    if(!query.execUpdate("cey_u_table",qrtVariantPairList()
                         << qrtVariantPair("num_cost", tablemodel->getKeepdata().dish_total()),
                         QString(" ch_billno = '%1' ").arg(this->ch_billno))) {
        errstring = query.recordError();
        return false;
    }
    return true;
}


void w_scr_dish_main::keyPressEvent(QKeyEvent *e)
{
    if(false == public_short_cut_map::keyPressQt(this, e)) return;
    if(this->lineEdit_scan)
        this->lineEdit_scan->tokey(e->key());
    QDialog::keyPressEvent(e);
}

void w_scr_dish_main::loadControl(ftableview *tableView_dish, ftableview *tableView_type, ftableview *tableView_subtype, fexpandmain_tableview *tableView, QLabel *label_logo, QLabel *label_text, QLabel *label_num, QPushButton *label_total, lds_lineedit *lineEdit_scan, public_pushbutton_pay *pushButton_pay)
{
    this->tableView_dish = tableView_dish;
    this->tableView_type = tableView_type;
    this->tableView_subtype = tableView_subtype;
    this->tableView = tableView;

    this->label_logo = label_logo;
    this->label_text = label_text;
    this->label_num = label_num;
    this->label_total = label_total;
    this->lineEdit_scan = lineEdit_scan;
    this->pushButton_pay = pushButton_pay;

    this->lineEdit_scan->setReadOnly(true);
}
