#include "fexpandmain_2.h"
#include <QTableView>
#include <QtDebug>
#include <QHeaderView>
#include <QPainter>
#include "public_sql.h"
#include <QDateTime>
#include "n_func.h"
#include "lds_dialog_input.h"
#include "ui_fexpandmain_payover.h"
#include "w_bt_dish.h"
#include <QSqlError>
#include "w_bt_dish_base.h"
#include <QScrollBar>
#include "w_scr_dish_dish_change.h"
#include "w_scr_dish_pay_widget.h"
#include "uo_rw_member.h"
#include "w_m_member_list.h"
#include "w_scr_dish_dish_quit.h"
#include "take_cashier.h"
#include "fexpandmain_model_sqltablemodel.h"
#include "w_scr_dish_num_cook_dialog.h"
#include "w_scr_dish_group_dialog.h"
#include "lds_pushbuttonwithnofocusline.h"
#include "ui_w_bt_dish_new_dialog_2.h"
#include "w_bt_dish_base.h"
#include "w_scr_dish_restaurant_div_pop.h"
#include "ftableview_standmodel_sql_table.h"
#include <QMenu>
#include "w_scr_dish_main_bar_cancel.h"
#include "w_scr_dish_main_pay_info.h"
#include "restauranttableview.h"
#include "w_scr_dish_main_bar_cancel_win.h"


/**
关于套菜数量修改的方案
1.主套菜P， 子套菜Y
2.P的数量和Y的数量独立，例如P:0.5, Y:1,2,5
3.换菜品时，分：可替换菜【不会改变P的价格】、非可替换菜品【改变P的价格】
4.加做法时，做法加价会加到P里
5.P允许修改数量的条件：旗下Y没有做法、旗下Y没有换菜、没有折扣、没有落单、价格等于原价、非特价、非赠送
*/


void fexpandmain_2::showPayinfo(QWidget *tableview, w_scr_dish_main_pay_info *pay_info_widget, const QString &ch_payno, int pay_type, bool ispromotion, const QString &discount_desc)
{
    //    if(pay && pay->isVisible()){pay->hide();}
    //    if(ch_payno.isEmpty()) return;
    //2
    lds_query query;
    QString title;
    switch(pay_type){
    case w_scr_dish_pay_widget::OPERAT_PAY:
        title = tr("结算成功");
        break;
    case w_scr_dish_pay_widget::OPERAT_QUITBILL:
        title = tr("退菜退款");
        break;
    case w_scr_dish_pay_widget::OPERAT_WAIMAI:
        title = tr("外卖结算");
        break;
    }

    lds::DOUBLE_DATA ddata;
    query.execSelect(QString("select ifnull(sum(num_realamount), 0), ifnull(sum(num_payamount), 0), ifnull(sum(num_change), 0), count(0) as count  from cey_u_checkout_detail where ch_payno = '%1' and ch_paymodeno <> 'ZZ'  and ch_paymodeno <> 'YY' ")//优惠和自动抹零
                     .arg(ch_payno));
    query.next();
    if(query.recordValue("count").toInt() > 0) {
        ddata.udpKey = lds::udp_pay_request;
        ddata.p_desc = title;
        ddata.p_pay = lds::septNumber(query.recordValue(0).toDouble());
        ddata.p_receive = lds::septNumber(query.recordValue(1).toDouble());
        ddata.p_change = lds::septNumber(query.recordValue(2).toDouble());
        doublescreensenddataByconf(ddata);
    }
    //w_scr_dish_main_pay_info
    pay_info_widget->ui->label_desc->setText(ddata.p_desc);
    pay_info_widget->ui->label_num_payamount->setText(ddata.p_pay);
    pay_info_widget->ui->label_num_realamount->setText(ddata.p_receive);
    pay_info_widget->ui->label_num_change->setText(ddata.p_change);
    //
    if(ispromotion){
        pay_info_widget->ui->label_promotion->setText(tr("促销对象为会员, 请先读卡"));
        lds::showWidget(pay_info_widget->ui->label_promotion);
    } else  {
        pay_info_widget->ui->label_promotion->setText("");
        lds::hideWidget(pay_info_widget->ui->label_promotion);
    }
    //
    if(discount_desc.length() > 0) {
        pay_info_widget->ui->label_discount->setText(discount_desc);
        lds::showWidget(pay_info_widget->ui->label_discount);
    } else {
        pay_info_widget->ui->label_discount->setText("");
        lds::hideWidget(pay_info_widget->ui->label_discount);
    }
    //
    lds::showWidget(pay_info_widget->ui->label_desc, ddata.p_desc.length() > 0);
    lds::showWidget(pay_info_widget->ui->label_num_payamount, ddata.p_pay.length() > 0);lds::showWidget(pay_info_widget->ui->label, lds::widgetIsShow(pay_info_widget->ui->label_num_payamount));
    lds::showWidget(pay_info_widget->ui->label_num_realamount, ddata.p_pay.length() > 0);lds::showWidget(pay_info_widget->ui->label_2, lds::widgetIsShow(pay_info_widget->ui->label_num_realamount));
    lds::showWidget(pay_info_widget->ui->label_num_change, ddata.p_pay.length() > 0);lds::showWidget(pay_info_widget->ui->label_3, lds::widgetIsShow(pay_info_widget->ui->label_num_change));
    lds::hideWidget(pay_info_widget->ui->frame, lds::widgetIsHide(pay_info_widget->ui->label_num_payamount) && lds::widgetIsHide(pay_info_widget->ui->label_num_realamount) && lds::widgetIsHide(pay_info_widget->ui->label_num_change) );
    lds::hideWidget(pay_info_widget,
                    lds::widgetIsHide(pay_info_widget->ui->label_desc)
                    && lds::widgetIsHide(pay_info_widget->ui->label_promotion)
                    && lds::widgetIsHide(pay_info_widget->ui->label_discount)
                    );

    int width = qMin(tableview->height(), tableview->width())*5/6;
    pay_info_widget->resize(width, pay_info_widget->sizeHint().height());
    //    if(lds::widgetIsShow(pay_info_widget->ui->frame)) {
    //        pay_info_widget->resize(width, width*3/4);
    //    } else {
    //        pay_info_widget->resize(width, width*1/4);
    //    }
    pay_info_widget->move(lds::getParentCenterPopup(tableview, pay_info_widget));
    pay_info_widget->raise();
}

fexpandmain_2::fexpandmain_2(QObject *parent) :
    frontViewDialog_virtual(parent)
{

    //    tablemodel = 0;
    //    model_type = new ftableview_standmodel_sql;
    //    model_subtype = new ftableview_standmodel_sql;
    //    model_dish = new ftableview_standmodel_sql;
    /////////
    //    tablemodel = new fexpandmain_model_sqltablemodel_dd(this);//点菜部分
    //    tablemodel->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    //    tablemodel->setTable("cey_u_orderdish");
    //    ///抓取促销信息  vch_parameter, vch_value
    //    tablemodel->tablename_kvmap_insert("cey_sys_parameter", "vch_parameter", "vch_value", " vch_parameter like 'promotion_setting%' ", false,
    //                                       lds_model_sqltablemodel::SELECT_AUTO_REFRESH_KEY);

    //    //////////////////
    //    //4 大类
    //    {
    //        model_type = new ftableview_standmodel_sql(this);
    //    }
    //    //3 小类
    //    {
    //        model_subtype = new ftableview_standmodel_sql(this);
    //    }
    //    //2 菜品
    //    {
    //        model_dish = new ftableview_standmodel_sql(this, ftableview_standmodel_sql::TYPE_icon);
    //    }
}

fexpandmain_2::~fexpandmain_2()
{


    //    delete model_type;
    //    delete model_subtype;
    //    delete model_dish;


}



void fexpandmain_2::updateDishImage(QWidget *parent, const QModelIndex &index, ftableview_standmodel_virtual *model_dish, const QString &ch_sub_typeno, const QString &ch_groupno)
{
    lds_query query;
    if(index.isValid()){//点击非菜品出
        if(index.data(Qt::UserRole + 1).toString() == "-3") goto tonew;

        if((model_dish->rowCount() - 1) == index.row() && (model_dish->columnCount() - 1) == index.column()) {
            switch(lds_messagebox::question(parent, MESSAGE_TITLE_VOID, tr("菜品操作"),  tr("新增"), tr("修改"), tr("取消"))){
            case 0:
                goto tonew;
                return;
            case 1:
                goto tochange;
                return;
            case 2:
                return;
            }
        }
        goto tochange;
    }
    goto tonew;
    return;
tochange:
    {
        QString ch_dishno = index.data(Qt::UserRole+1).toString();
        w_bt_dish_base_change_by_ch_dishno dialog(parent, ch_dishno);
        if(ch_groupno.length() > 0)
            dialog.ui->lineEdit_ch_groupno->setEnabled(false);
        lds_roundeddialog_rect_align(&dialog, false).exec();
        query.execSelect(QString("SELECT ch_dishno, CONCAT(vch_dishname,'\n',num_price) from cey_bt_dish where ch_stopflag <> 'Y' and ch_tempflag <> 'Y' and ch_dishno = '%1' ")
                         .arg(ch_dishno));
        if(query.next()){
            model_dish->setData(index, query.recordValue(1), Qt::UserRole);
            QPixmap pixmap(model_dish->imagedishdir+"/"+ch_dishno);
            if(pixmap.isNull()) {
                model_dish->setData(index, ch_dishno, Qt::UserRole+2);
            } else {
                model_dish->setData(index, pixmap, Qt::UserRole+2);
            }
        } else {
            qDebug("not ch_dishno match filter!");
        }
        model_dish->refreshcur();
        return;
    }
tonew:
    {
        updateDishImage_add(parent, model_dish, ch_sub_typeno, ch_groupno);
        model_dish->refreshcur();
    }
}

void fexpandmain_2::updateDishImage_add(QWidget *parent, ftableview_standmodel_virtual *model_dish, const QString &ch_sub_typeno, const QString &ch_groupno)
{
    lds_query query;
    QString ch_dish_typeno;
    query.execSelect(QString("select ch_dish_typeno FROM hddpos.cey_bt_dish_subtype where ch_sub_typeno = '%1' ")
                     .arg(ch_sub_typeno));
    query.next();
    ch_dish_typeno = query.recordValue("ch_dish_typeno").toString();

    w_bt_dish_base_new dialog(parent, ch_dish_typeno, ch_sub_typeno, ch_groupno, "");
    dialog.setWindowTitle(tr("新增"));
    lds_roundeddialog_rect_align(&dialog, false).exec();
    model_dish->refreshcur();
}

//-1no effect 0clear 1read
int fexpandmain_2::to_m_read(QWidget *parent, fexpandmain_model_sqltablemodel_data *tablemodel, const QString &vch_memberno)
{
    bool reReadCard = false;
    QString mtitle;
    QString errstring;

    if(tablemodel->vch_membernod.vch_memberno.length() > 0) {
        switch(lds_messagebox::question(parent, MESSAGE_TITLE_VOID, tr("已读会员卡") + "\n" + tablemodel->vch_membernod.desc(), tr("清除读卡信息") , tr("重新读卡"), tr("取消"))) {
        case 0://清除读卡信息
            tablemodel->vch_membernod.init();
            tablemodel->vch_membernod_m_update(errstring);
            return 0;
        case 1://重新读卡
            tablemodel->vch_membernod.init();
            tablemodel->vch_membernod_m_update(errstring);
            reReadCard = true;
            break;
        default://取消
            return -1;
        }
    }
    errstring = "";
    //读卡
    uo_rw_member uo_member;
    QString cardno;
    n_func::READCARDINFO cardinfo;
    if(vch_memberno.isEmpty()) {
        if(!w_m_member_list::static_to_memeber_readcard(parent,cardno, uo_member, true, true)){//读卡未完成
            goto end;
        }
    } else {
        cardno = vch_memberno;
    }
    //判断卡是否存在
    if(cardno.isEmpty()){
        lds_messagebox::warning(parent, MESSAGE_TITLE_VOID,tr("无可查询的卡"));
        goto end;//卡信息不满足要求
    }
    //读卡信息
    if(!n_func::qtcardinfo_readcard(cardinfo, cardno)) {
        lds_messagebox::warning(parent, MESSAGE_TITLE_VOID,cardinfo.errstring);
        goto end;//卡信息不满足要求
    }
    //判断密码
    if(!cardinfo.vch_password.isEmpty()){
        lds_dialog_input inputdialog(parent);
        inputdialog.hideKey();
        inputdialog.ui->label->setText(tr("密码"));
        inputdialog.ui->lineEdit->setEchoMode(QLineEdit::Password);
        inputdialog.setWindowTitle(tr("请输入密码"));
        if(QDialog::Accepted != lds_roundeddialog_rect_align(&inputdialog).exec()){
            goto end;//取消
        }
        if(cardinfo.vch_password!=inputdialog.ui->lineEdit->text()){
            lds_messagebox::warning(parent, MESSAGE_TITLE_VOID,tr("密码错误"));
            goto end;//卡信息不满足要求
        }
    }
    //判断卡的状态
    if(cardinfo.vch_memberno.isEmpty()){
        lds_messagebox::warning(parent, MESSAGE_TITLE_VOID,tr("有误,还未发卡或不存在,请核实"));
        goto end;//卡信息不满足要求
    }
    if(cardinfo.ch_state == "4"){
        lds_messagebox::warning(parent, MESSAGE_TITLE_VOID,tr("此卡已经作废,不能付款"));
        goto end;//卡信息不满足要求
    }
    if(cardinfo.ch_state == "3"){
        lds_messagebox::warning(parent, MESSAGE_TITLE_VOID,tr("此卡已经挂失,不能付款"));
        goto end;//卡信息不满足要求
    }
    if(cardinfo.ch_state == "2"){
        lds_messagebox::warning(parent, MESSAGE_TITLE_VOID,tr("此卡已经止付,不能付款"));
        goto end;//卡信息不满足要求
    }
    if(cardinfo.ch_state.isEmpty()){
        lds_messagebox::warning(parent, MESSAGE_TITLE_VOID,tr("非本公司卡或无效卡"));
        goto end;//卡信息不满足要求
    }
    if(cardinfo.dt_limit <= n_func::f_get_sysdatetime()){
        lds_messagebox::warning(parent, MESSAGE_TITLE_VOID,tr("此卡已经超过有效期"));
        goto end;//卡信息不满足要求
    }
    if(cardinfo.ch_cardflag != "Y"){
        lds_messagebox::warning(parent, MESSAGE_TITLE_VOID,tr("会员卡")+cardinfo.vch_memberno+tr("还未发卡"));
        goto end;//卡信息不满足要求
    }

    //
    if(w_m_member_list::is_yun_member) mtitle = tr("云") + tr("会员卡") + cardinfo.vch_memberno;
    else mtitle = tr("会员卡") + cardinfo.vch_memberno;

    //根据会员类别信息，更新信息--begin
    if(cardinfo.ch_favourable == "1"){//会员价
        if(cardinfo.ch_pricekind.isEmpty()){
            lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, mtitle + "\n"+ tr("没有设置会员价")+","+tr("请到会员类型里修改"));
            goto end;//卡信息不满足要求
        }
        if(1 == lds_messagebox::information(parent, MESSAGE_TITLE_VOID, mtitle + "\n"+  tr("当前读卡信息:") + tr("会员特价"), tr("确定"), tr("取消"))){
            goto end;//卡信息不满足要求
        }
        //会员特价
    } else if(cardinfo.ch_favourable == "2"){//固定折扣
        if(cardinfo.int_discount.isEmpty()){
            lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, mtitle + "\n"+ tr("没有设置会员折扣")+","+tr("请到会员类型里修改"));
            goto end;//卡信息不满足要求
        }
        if(1 == lds_messagebox::information(parent, MESSAGE_TITLE_VOID, mtitle + "\n"+  tr("当前读卡信息:") + tr("折扣(%1)").arg(cardinfo.int_discount), tr("确定"), tr("取消"))){
            goto end;//卡信息不满足要求
        }
    } else {
        lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, mtitle + "\n"+  tr("没有找到该会员优惠信息")+","+tr("请到会员类型里修改"));
        goto end;//卡信息不满足要求
    }

    tablemodel->vch_membernod.init0(cardinfo.vch_memberno,
                                    cardinfo.vch_member,
                                    cardinfo.ch_member_belong_state,
                                    cardinfo.ch_favourable,
                                    cardinfo.int_discount.toInt(),
                                    cardinfo.ch_pricekind,
                                    cardinfo.ch_attribute,
                                    cardinfo.vch_typename
                                    );
    tablemodel->vch_membernod_m_update(errstring);
    return 1;
end:
    w_m_member_list::yun_member_close();//! 关闭云会员
    if(!errstring.isEmpty()) {
        lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, errstring);
    }
    if(reReadCard)
        return 0;//重新读卡但是没有读到卡
    return -1;
}

bool fexpandmain_2::static_toquit_1_only(QString &errstring, const num_backData &d)
{
    lds_query query;
    QString ch_dishno;
    QString ch_suitflag;
    double num_price_add;
    double num_price_add_P;
    QString vch_plan_k3o3_id;
    QString state;
    query.execSelect(QString("select ch_dishno, ch_suitflag, num_price_add, vch_plan_k3o3_id  from cey_u_orderdish where int_flowid = '%1' ")
                     .arg(d.int_flowid));
    query.next();
    ch_dishno = query.recordValue("ch_dishno").toString();
    ch_suitflag = query.recordValue("ch_suitflag").toString();
    vch_plan_k3o3_id = query.recordValue("vch_plan_k3o3_id").toString();

    num_price_add = fexpandmain_model_sqltablemodel::get_num_price_add_vch_print_memo(d.vch_print_memo_remain);
    if(ch_suitflag == "Y") num_price_add = 0;
    if(ch_suitflag == "P") num_price_add_P = query.recordValue("num_price_add").toDouble();


    for(int k = 0; k < 6; k++) {
        if(vch_plan_k3o3_id.mid(k*2, 2) == "00") {
            state += "1";
        } else {
            state += w_bt_dish::k2o3_stae2hex_str(w_bt_dish::KITCHEN_QUIT);
        }
    }
    if(query.execUpdate("cey_u_orderdish", qrtVariantPairList()
                        << qrtVariantPair("num_back", qrtFunc("num_back + "+QString::number(d.num_back_add)))
                        << qrtVariantPair("vch_back_operid", d.vch_back_operid)
                        << qrtVariantPair("dt_back_operdate", d.dt_back_operdate)
                        << qrtVariantPair("vch_print_memo", d.vch_print_memo_remain)
                        << qrtVariantPair("num_price_add", num_price_add),
                        qrtEqual("int_flowid", d.int_flowid))) {
        //更新log
        num_price_add = fexpandmain_model_sqltablemodel::get_num_price_add_vch_print_memo(d.vch_print_memo);
        if(ch_suitflag == "Y") num_price_add = 0;
        if(ch_suitflag == "P") num_price_add = num_price_add_P;
        if(query.execInsert("cey_u_orderdish_log", qrtVariantPairList()
                            << qrtVariantPair("int_orderflow", d.int_flowid)
                            << qrtVariantPair("ch_type", 1)
                            << qrtVariantPair("vch_operID", public_sql::gs_operid)
                            << qrtVariantPair("dt_operdate", d.dt_back_operdate)
                            << qrtVariantPair("vch_reason", d.vch_reason)

                            << qrtVariantPair("num_back_cur", d.num_back_add)
                            << qrtVariantPair("vch_plan_k3o3_state", state)
                            << qrtVariantPair("vch_print_memo", d.vch_print_memo)
                            << qrtVariantPair("num_price_add", num_price_add))) {
            query.execSelect(QString("SELECT num_num, num_back from cey_u_orderdish where int_flowid = %1 ")
                             .arg(d.int_flowid));
            query.next();
            double num_num = query.recordValue("num_num").toDouble();
            double num_back = query.recordValue("num_back").toDouble();
            if(query.execUpdate("material_u_orderdish",
                                "num_num", QString::number(num_num)+" * num_rate",
                                "num_back", QString::number(num_back)+" * num_rate",
                                QString(" int_order_flowID = %1  ").arg(d.int_flowid))) {
                return true;
            }
        }
    }
    errstring = query.recordError();
    return false;
}


//dishnum_back_total、d_map 同步更新
bool fexpandmain_2::static_toquit_1_suit(QString &errstring, qlonglong int_flowid, double &dishnum_back_total, const QString &vch_reason, const QString &operid, const QDateTime &dt, QList<num_backData> *d_list)
{
    lds_query query;
    lds_query query2;
    QString ch_suitflag;
    QList<qlonglong> int_flowid_list;
    QString ch_dishno;
    QString vch_print_memo;
    double back_num_P=dishnum_back_total;
    double dish_num;
    double back_num_P_org;

    if(dishnum_back_total <= 0) {
        errstring = tr("退菜数量为0");
        goto failure;
    }
    query.execSelect(QString("select int_flowid, ch_suitflag, ch_dishno, num_num-num_back as dish_num from cey_u_orderdish where int_flowid = %1 ").arg(int_flowid));
    if(query.next()) {
        int_flowid_list.clear();
        int_flowid_list.append(query.recordValue("int_flowid").toLongLong());
        ch_suitflag = query.recordValue("ch_suitflag").toString();
        ch_dishno = query.recordValue("ch_suitflag").toString();
        back_num_P_org = query.recordValue("dish_num").toDouble();

        if(ch_suitflag == "P") {
            while(query2.execSelect(QString("select int_flowid, ch_suitflag from cey_u_orderdish where int_flowid = %1 + 1 and ch_suitflag = 'Y' ")
                                    .arg(int_flowid_list.last()))) {
                if(query2.next()) {
                    int_flowid_list.append(query2.recordValue("int_flowid").toLongLong());
                } else {
                    break;
                }
            }
        }

        foreach(int id , int_flowid_list){
            query2.execSelect(QString("select int_flowid, num_num - num_back, ch_suitflag, ch_dishno, vch_print_memo from cey_u_orderdish where int_flowid = %1 ")
                              .arg(id));
            query2.next();
            num_backData d;
            ch_suitflag = query2.recordValue("ch_suitflag").toString();
            vch_print_memo = query2.recordValue("vch_print_memo").toString();
            dish_num = query2.recordValue(1).toDouble();

            d.int_flowid = id;
            d.num_back_add = dish_num;
            d.dt_back_operdate = dt;
            d.vch_back_operid = operid;
            d.vch_reason = vch_reason;
            d.vch_print_memo = vch_print_memo;//套菜全退

            //主套菜或普通菜品
            if(d.num_back_add <= dishnum_back_total) {
            } else {
                d.num_back_add = dishnum_back_total;
            }
            //子套菜
            if(ch_suitflag == "Y") {
                if(back_num_P_org == 0) goto failure;
                d.num_back_add = back_num_P/back_num_P_org * dish_num;
                //                d.vch_print_memo = "";
            }


            if(d_list)d_list->append(d);
            if(!fexpandmain_2::static_toquit_1_only(errstring, d)){
                goto failure;
            }

            //主套菜或普通菜品
            dishnum_back_total -= d.num_back_add;
            //子套菜
            if(ch_suitflag == "Y") {
                dishnum_back_total += d.num_back_add;
            }
        }
        return true;
    }
    errstring = tr("菜品没有找到");
failure:
    return false;
}

QString fexpandmain_2::get_sub_vch_print_memo(const QString &org, const QString &sub)
{
    QString org0 = org;
    org0.remove(org0.indexOf(sub), sub.length());
    return org0;
}

bool fexpandmain_2::guqing_check(QString &msg, const QString &ch_dishno, double off)
{
    double num_guqing_num = 0;

    off = qRound(100*off)/100.0;
    lds_query query;

    //菜品估清清除
    guqing_try_clear();

    query.execSelect(QString("SELECT num_guqing_num, vch_dishname from cey_bt_dish where ch_dishno = '%1' and ifnull(ch_guqing_flag, '') = 'Y'  ")
                     .arg(ch_dishno));
    if(query.next()) {
        num_guqing_num = query.recordValue("num_guqing_num").toDouble();
        bool num_guqing_flag = (off) > num_guqing_num;
        if(num_guqing_flag) {
            msg +=  QString("%1\t").arg(tr("估清不足")).arg(num_guqing_num-off);
        } else {
            msg +=  QString("%1:%2\t").arg(tr("估清剩余")).arg(num_guqing_num-off);
        }
        msg += query.recordValue("vch_dishname").toString() + "\n";

        if(num_guqing_flag) return false;
    }
    return  true;
}

void fexpandmain_2::guqing_try_clear()
{
    lds_query query;
    QString sql_dt_yyyyMMdd = n_func::f_get_sysdatetime().toString("yyyy-MM-dd");
    QString guqing_clear_day = n_func::f_get_sysparm("guqing_clear_day", "");

    //第一次进入
    if(guqing_clear_day.isEmpty()) {
        guqing_clear_day =  sql_dt_yyyyMMdd;
        n_func::f_set_sysparm("guqing_clear_day", sql_dt_yyyyMMdd);
    }
    if(guqing_clear_day != sql_dt_yyyyMMdd) {
        query.execUpdate("cey_bt_dish", "ch_guqing_flag", "N", "num_guqing_num", 0 , "");//初始化菜品估清
        n_func::f_set_sysparm("guqing_clear_day", sql_dt_yyyyMMdd);
    }
}

bool fexpandmain_2::guqing_update(const QString &ch_dishno, double off)
{
    if(off == 0) return true;

    lds_query query;
    query.execSelect(QString("select ch_guqing_flag from cey_bt_dish where ch_dishno = '%1' ").arg(ch_dishno));
    query.next();
    QString ch_guqing_flag = query.recordValue("ch_guqing_flag").toString();
    if("Y" == ch_guqing_flag) {
        if(!query.execUpdate("cey_bt_dish", "num_guqing_num", qrtFunc("num_guqing_num - "+QString::number(off)), qrtEqual("ch_dishno", ch_dishno))){
            return false;
        }
    }
    return true;
}

//若rowP是主套菜，则依次替换子套菜
//若rowp是子套菜，则子套菜本来不能修改数量
bool fexpandmain_2::guqing_try_check_P(lds_model_sqltablemodel *tablemodel, QString &msg, int rowP, double off)
{
    if(off == 0) return true;//0表示没有改变

    double dish_num = tablemodel->model_data_NIBOLAN(rowP, "num_num-num_back");
    QString ch_dishno = tablemodel->model_data(rowP, "ch_dishno").toString();

    if(dish_num == 0) {
        qDebug() << __FILE__<<__LINE__ << "dish_num is 0";
    }

    bool f = true;

    double off_rate_P_new_old = off / dish_num;
    if(false == guqing_check(msg, ch_dishno, off)) {
        f = false;
    }

    for(int row = rowP+1; row < tablemodel->rowCount(); row++) {
        if(tablemodel->model_data(row, "ch_suitflag").toString() == "Y") {
            ch_dishno = tablemodel->model_data(row, "ch_dishno").toString();
            dish_num = tablemodel->model_data_NIBOLAN(row, "num_num-num_back");
            if(!guqing_check(msg, ch_dishno, dish_num * off_rate_P_new_old)) {
                f = false;
            }
            continue;
        }
        break;
    }
    return f;
}

bool fexpandmain_2::material_insert(lds_query &query, const QString &ch_billno, int int_id)
{
    bool f = true;//默认是没有设置原料的
    //原料----点菜流水号
    query.execSelect(QString("SELECT int_flowid, num_num, num_back, ch_dishno from cey_u_orderdish where ch_billno = '%1' and int_id = %2 ")
                     .arg(ch_billno)
                     .arg(int_id));
    if(query.next()) {
        QString int_flowid = query.recordValue("int_flowid").toString();
        QString ch_dishno = query.recordValue("ch_dishno").toString();
        double num_num = query.recordValue("num_num").toDouble();
        double num_back = query.recordValue("num_back").toDouble();

        query.execSelect(QString("SELECT int_order_flowID from material_u_orderdish where int_order_flowID = %1 ")
                         .arg(int_flowid));
        if(query.next()) {
            f = query.execUpdate("material_u_orderdish",
                                 "num_num", qrtFunc(QString::number(num_num)+" * num_rate"),
                                 "num_back", qrtFunc(QString::number(num_back)+" * num_rate"),
                                 QString(" int_order_flowID = %1  ").arg(int_flowid));
            
        } else {

            f = query.execInsert("material_u_orderdish",
                                 QString("select %1 as `int_order_flowID`, "
                                         "ch_itemno as `ch_dishno`, "
                                         "num_num as `num_rate` ,"
                                         "num_num * %2 as `num_num`, "
                                         "num_num * %3 as `num_back` from material_bt_dish where ch_dishno = '%4' ")
                                 .arg(int_flowid)
                                 .arg(num_num)
                                 .arg(num_back)
                                 .arg(ch_dishno)
                                 );
        }
    }

    return f;
}

bool fexpandmain_2::cancelTable_transaction(const QString &ch_tableno, int sqr, QString &errstring)
{
    if(false == cancelTableCheck(ch_tableno, sqr, errstring)) {
        return false;
    }
    //
    lds_query query;
    lds_query::tran_saction();
    if(cancelTable(ch_tableno, sqr, errstring)) {
        lds_query::com_mit();
        return true;
    }
    lds_query::roll_back();
    errstring = query.recordError();
    return false;
}

void fexpandmain_2::wineTake(QWidget *parent)
{
    lds_query query;
    QString vch_guest;
    QString vch_handle;
    int ret;
    switch(lds_messagebox::question(parent, MESSAGE_TITLE_VOID, tr("存取酒"), tr("存酒"), tr("取酒"), tr("取消"))) {
    case 0:
    {
        //vch_handle
        lds_dialog_input inputdialog(parent);
        inputdialog.setWindowTitle(tr("录入手机号"));
        inputdialog.ui->label->setText(tr("手机号"));
        if(QDialog::Accepted != lds_roundeddialog_rect_align(&inputdialog).exec()) {
            return;
        }
        vch_handle = inputdialog.ui->lineEdit->text().trimmed();
        if(vch_handle.isEmpty()) {
            lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, tr("手机号为空"));
            return;
        }
        //vch_guest
        query.execSelect(QString("SELECT  vch_guest from cey_bt_table_bar_wine  where vch_handle = '%1' limit 1")
                         .arg(vch_handle));
        if(query.next()) {
            vch_guest = query.recordValue("vch_guest").toString();
        } else {
            if(1 == lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, tr("没找到该手机记录"), tr("新增"), tr("取消"))) {
                return;
            }
            inputdialog.setWindowTitle(tr("录入姓名"));
            inputdialog.ui->label->setText(tr("姓名"));
            inputdialog.ui->lineEdit->clear();
            inputdialog.delay_set_focus();

            while(QDialog::Accepted == (ret = lds_roundeddialog_rect_align(&inputdialog).exec())) {
                vch_guest = inputdialog.ui->lineEdit->text().trimmed();
                if(vch_guest.isEmpty()) {
                    lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, tr("姓名为空"));
                    inputdialog.delay_set_focus();
                    continue;
                }
                break;
            }
            if(QDialog::Accepted != ret) {
                return;
            }
        }
        //w_scr_dish_main_bar_cancel_win_pend
        w_scr_dish_main_bar_cancel_win_pend dialog(vch_handle, vch_guest, parent);
        lds_roundeddialog_rect_align(&dialog).exec();
    }
        break;
    case 1:
    {
        //vch_handle
        lds_dialog_input inputdialog(parent);
        inputdialog.setWindowTitle(tr("录入手机号"));
        inputdialog.ui->label->setText(tr("手机号"));
        if(QDialog::Accepted != lds_roundeddialog_rect_align(&inputdialog).exec()) {
            return;
        }
        vch_handle = inputdialog.ui->lineEdit->text().trimmed();
        if(vch_handle.isEmpty()) {
            lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, tr("手机号为空"));
            inputdialog.delay_set_focus();
            return;
        }
        query.execSelect(QString("SELECT  vch_guest from cey_bt_table_bar_wine  where vch_handle = '%1' limit 1")
                         .arg(vch_handle));
        if(!query.next()) {
            lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, tr("手机号不存在"));
            inputdialog.delay_set_focus();
            return;
        }

        //w_scr_dish_main_bar_cancel_win_take
        w_scr_dish_main_bar_cancel_win_take dialog(vch_handle, parent);
        lds_roundeddialog_rect_align(&dialog).exec();

    }
        break;
    }
}

bool fexpandmain_2::cancelTable(const QString &ch_tableno, int sqr, QString &errstring)
{
    //
    QString ch_billno;
    QString div = QString::number(sqr);
    if(sqr == 0) {
        div = "";
    } else {
        div = "_"+div;
    }
    lds_query query;
    if(query.execUpdate("cey_bt_table", "ch_billno"+div, "", qrtEqual("ch_tableno", ch_tableno))) {
        return true;
    }

    //    query.execSelect(QString("SELECT ch_billno%1 from cey_bt_table where ch_tableno = '%2' ")
    //                     .arg(div)
    //                     .arg(ch_tableno));
    //    query.next();
    //    ch_billno = query.recordValue(0).toString();
    //
    //    if(query.execUpdate("cey_u_table", "ch_order_state", 3 , QString("ch_billno = '%1' ").arg(ch_billno))) {
    //        return true;
    //    }
    errstring = query.recordError();
    return false;
}

bool fexpandmain_2::cancelTableCheck(const QString &ch_tableno, int sqr, QString &errstring)
{
    if(w_scr_dish_restaurant_div_pop::cey_bt_table_has_locked(ch_tableno, sqr)) {
        errstring = tr("该餐桌已经被锁定,请确定");
        return false;
    }
    //
    QString ch_billno = w_scr_dish_restaurant_div_pop::cey_bt_table_ch_billno_get(ch_tableno, sqr);
    if(w_scr_dish_restaurant_div_pop::cey_u_orderdish_dish_existed(ch_billno)) {
        errstring = tr("当前桌位存在菜品消费,不能取消开台");
        return false;
    }
    return true;
}

vch_membernoData::vch_membernoData()
{
    init();
}

void vch_membernoData::init()
{
    //    ch_attribute = "";//1：优惠卡、2充值卡
    //    ch_favourable = "";//1会员价, 2固定折扣
    //    ch_pricekind = "";
    //    int_discount = -1;
    //    ch_member_belong_state = "";
    //    vch_memberno = "";
    //    vch_typename = "";
    init0("", "", "","", -1, "", "", "");
}

void vch_membernoData::init0(const QString &vch_memberno, const QString &vch_member, const QString &ch_member_belong_state, const QString &ch_favourable, int int_discount, const QString &ch_pricekind, const QString &ch_attribute, const QString &vch_typename)
{
    this->ch_attribute = ch_attribute;//1：优惠卡、2充值卡
    this->ch_favourable = ch_favourable;//1会员价, 2固定折扣
    this->ch_pricekind = ch_pricekind;
    this->int_discount = int_discount;
    this->ch_member_belong_state = ch_member_belong_state;
    this->vch_memberno = vch_memberno;
    this->vch_member = vch_member;
    this->vch_typename = vch_typename;
}

//                  福

bool vch_membernoData::init_by_memberno(QWidget *parent, const QString &memberno, const QString &ch_member_belong_state)
{
    lds_query query;
    QString ch_typeno;
    bool ok;
    this->vch_memberno = memberno;
    this->ch_member_belong_state = ch_member_belong_state;

    if(!vch_memberno.isEmpty()) {
        if("Y" == ch_member_belong_state) {//云会员
            ch_typeno=w_sys_manage_cloudsync::get_yun_member_readcard(parent, vch_memberno, n_func::f_get_sysparm("cloud_user"),
                                                                      "ch_typeno", ok);if(!ok) goto failure;
            ch_attribute=w_sys_manage_cloudsync::get_yun_member_readcard(parent, vch_memberno, n_func::f_get_sysparm("cloud_user"),
                                                                         "ch_attribute", ok);if(!ok) goto failure;
            ch_favourable=w_sys_manage_cloudsync::get_yun_member_membertype(parent, ch_typeno,
                                                                            "chFavourable", ok);if(!ok) goto failure;
            ch_pricekind=w_sys_manage_cloudsync::get_yun_member_membertype(parent, ch_typeno,
                                                                           "chPricekind", ok);if(!ok) goto failure;
            int_discount = w_sys_manage_cloudsync::get_yun_member_membertype(parent, ch_typeno,
                                                                             "intDiscount", ok).toInt();if(!ok) goto failure;
            vch_typename=w_sys_manage_cloudsync::get_yun_member_readcard(parent, vch_memberno, n_func::f_get_sysparm("cloud_user"),
                                                                         "vch_typename", ok);if(!ok) goto failure;
            vch_member=w_sys_manage_cloudsync::get_yun_member_readcard(parent, vch_memberno, n_func::f_get_sysparm("cloud_user"),
                                                                         "vch_member", ok);if(!ok) goto failure;

            return true;
        } else  {//本地会员
            query.execSelect(QString(
                                 " SELECT a.vch_memberno,a.vch_member,a.ch_typeno,a.vch_tel, a.vch_handtel,  a.dt_birthday , a.ch_state ,a.vch_password ,a.dt_limit,a.ch_cardflag ,b.int_card_baseamount,"
                                 " b.ch_attribute, b.ch_favourable, b.ch_pricekind, b.int_discount,b.vch_typename ,c.num_amount, c.num_point"
                                 " FROM t_m_member a "
                                 " left join t_m_member_type b on a.ch_typeno = b.ch_typeno "
                                 " left join t_m_curamount c on a.vch_memberno = c.vch_memberno"
                                 " where a.ch_typeno = b.ch_typeno and b.ch_attribute like '%'  and a.vch_memberno = '%1' "
                                 )
                             .arg(vch_memberno));
            query.next();
            ch_attribute=query.recordValue("ch_attribute").toString();
            ch_favourable=query.recordValue("ch_favourable").toString();
            ch_pricekind=query.recordValue("ch_pricekind").toString();
            int_discount=query.recordValue("int_discount").toInt();
            vch_typename=query.recordValue("vch_typename").toString();
            vch_member=query.recordValue("vch_member").toString();
            return true;
        }
    }
    //会员号为空
    init();
    return true;
failure:
    init();
    return false;
}

QString vch_membernoData::desc()
{
    QString retv;
    retv += QObject::tr("会员类型")+":"+vch_typename+",";

    if(!vch_memberno.isEmpty()) {
        if(isyun()) {
            retv = QObject::tr("云");
        }
        if(ch_attribute == "1") {
            retv += QObject::tr("优惠卡")+":" + vch_memberno+",";
        }
        if(ch_attribute == "2") {
            retv += QObject::tr("充值卡")+":" + vch_memberno;
        }

        //        if(ch_favourable == "1") {
        //            retv += QObject::tr("会员特价");
        //        }
        //        if(ch_favourable == "2"){
        //            retv += QObject::tr("固定折扣")+QString("(%1)").arg(int_discount);
        //        }
    }
    return retv;
}

QString vch_membernoData::color()
{
    if(isyun()) {
        return "green";
    }
    return "#ff650b";
}

QString vch_membernoData::paymodeno()
{
    if(isyun()) {
        return "10";
    }
    return "02";
}

vch_membernoData::chooseFlag vch_membernoData::mutil_choose()
{
    /*第一次读卡，且没有开启云会员*/if(vch_memberno.isEmpty() && !(lds::sysconf->value("w_sys_manage_cloudsync_with_time/cloud_yun_member", false).toBool()))
        return  vch_membernoData::choose_local;
    /*第一次读卡，且开启云会员，且禁止本地会员*/if(vch_memberno.isEmpty()
                               && lds::sysconf->value("w_sys_manage_cloudsync_with_time/cloud_yun_member", false).toBool()
                               && lds::sysconf->value("w_sys_manage_cloudsync_with_time/cloud_no_use_local_member", false).toBool()
                               ) return  vch_membernoData::choose_yun;
    /*已经读过卡，且之前是云会员*/if(!vch_memberno.isEmpty() && "Y" == ch_member_belong_state)
        return  vch_membernoData::choose_yun;
    /*已经读过卡，且之前是非会员*/if(!vch_memberno.isEmpty() && "Y" != ch_member_belong_state)
        return  vch_membernoData::choose_local;

    return  vch_membernoData::choose_switch;
}

void vch_membernoData::print()
{
    qDebug() << "vch_memberno" << vch_memberno;
    qDebug() << "ch_attribute" << ch_attribute;
    qDebug() << "ch_favourable" << ch_favourable;
    qDebug() << "ch_pricekind" << ch_pricekind;
    qDebug() << "int_discount" << int_discount;
    qDebug() << "ch_member_belong_state" << ch_member_belong_state;
}

bool vch_membernoData::isyun() const
{
    return "Y" == ch_member_belong_state;
}

bool vch_membernoData::is_m_price() const
{
    if(!vch_memberno.isEmpty() && ch_favourable == "1" && ch_pricekind == "1" ) {
        return true;
    }
    return false;
}

bool vch_membernoData::is_m_discount() const
{
    if(!vch_memberno.isEmpty() && ch_favourable == "2") {
        return true;
    }
    return false;
}

double vch_membernoData::mprice(double org_price, double m_price) const
{
    if(is_m_price()) {
        return m_price;
    }
    return org_price;
}
