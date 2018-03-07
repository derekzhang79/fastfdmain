#include "public_printby_ch_billno.h"
#include <QtDebug>
#include "n_func.h"
#include "public_mainprinter.h"
#include "lds.h"
#include "w_sys_manage_cloudsync.h"
#include "lds.h"
#include "lds_query.h"
#include "w_scr_dish_pay_widget.h"
#include "w_scr_dish_restaurant_div_pop.h"
#include "fexpandmain_2.h"
#include "fexpandmain_model_sqltablemodel.h"
#include "labelprint_main_dialog.h"
#include <QFile>
#include "lds_dialog.h"
#include "public_sql.h"
#include "printer_normal.h"
#include "backheader.h"

//注意流水号，保存的位置vch_memo from cey_u_master
static QString CONTENT_GROUP = " group by a.ch_dishno, a.num_price, a.int_discount, a.ch_presentflag, a.vch_print_memo, a.num_price_add, a.ch_waidai_flag   ";


public_printby_ch_billno::public_printby_ch_billno(const QString &_ch_billno, const QString &languageKey, QObject *parent) :
    QObject(parent)
{
    print_format.setPrintTranslate(languageKey, this);
    rowno = 0;
    //打印分数
    bp_count = 1;
    ch_billno = _ch_billno;
    paperwidth = 48;

    bp_content_len = "s"+f_get_sysparm("bp_fmt_content_len").toLocal8Bit();
    int bp_content_len_scale_h = 1;
    int s0 = bp_content_len.mid(1,1).toInt();//s00
    if(s0 != 0){
        bp_content_len_scale_h = s0 + 1;
    }
    //本单序号
    bp_rowid = f_get_sysparm("bp_fmt_rowid");
    //赠送标识
    bp_presentflag = f_get_sysparm("bp_fmt_presentflag");
    //特价标识
    bp_fmt_specialflag = f_get_sysparm("bp_fmt_specialflag");
    //超出字符串长度自动截断
    bp_fmt_overstep_cut = f_get_sysparm("bp_fmt_overstep_cut");
    //品名
    bp_dishname = f_get_sysparm("bp_fmt_dishname");
    bp_dishname_len = f_get_sysparm("bp_fmt_dishname_len").toInt() / bp_content_len_scale_h;
    bp_dishname_row = f_get_sysparm("bp_fmt_dishname_row", "1").toInt();
    bp_dishname_left = f_get_sysparm("bp_fmt_dishname_left").toInt();
    //单位
    bp_unit = f_get_sysparm("bp_fmt_unit");
    bp_unit_len = f_get_sysparm("bp_fmt_unit_len").toInt() / bp_content_len_scale_h;
    bp_unit_row = f_get_sysparm("bp_fmt_unit_row", "1").toInt();
    bp_unit_left = f_get_sysparm("bp_fmt_unit_left").toInt();
    //数量
    bp_num = f_get_sysparm("bp_fmt_num");
    bp_num_len = f_get_sysparm("bp_fmt_num_len").toInt() / bp_content_len_scale_h;
    bp_num_row = f_get_sysparm("bp_fmt_num_row", "1").toInt();
    bp_num_left = f_get_sysparm("bp_fmt_num_left").toInt();

    bp_num_dec = f_get_sysparm("bp_fmt_num_dec").toInt();
    //价格
    bp_price = f_get_sysparm("bp_fmt_price");
    bp_price_len = f_get_sysparm("bp_fmt_price_len").toInt() / bp_content_len_scale_h;
    bp_price_row = f_get_sysparm("bp_fmt_price_row", "1").toInt();
    bp_price_left = f_get_sysparm("bp_fmt_price_left").toInt();
    bp_fmt_price_with_org = f_get_sysparm("bp_fmt_price_with_org");

    bp_price_dec = f_get_sysparm("bp_fmt_price_dec").toInt();
    //金额
    bp_amount = f_get_sysparm("bp_fmt_amount");
    bp_amount_len = f_get_sysparm("bp_fmt_amount_len").toInt() / bp_content_len_scale_h;
    bp_amount_row = f_get_sysparm("bp_fmt_amount_row", "1").toInt();
    bp_amount_left = f_get_sysparm("bp_fmt_amount_left").toInt();

    bp_amount_dec = f_get_sysparm("bp_fmt_amount_dec").toInt();
    //折扣
    bp_disdetail = f_get_sysparm("bp_fmt_disdetail");
    bp_disdetail_len = f_get_sysparm("bp_fmt_disdetail_len").toInt() / bp_content_len_scale_h;
    bp_disdetail_row = f_get_sysparm("bp_fmt_disdetail_row", "1").toInt();
    bp_fmt_disdetail_left = f_get_sysparm("bp_fmt_disdetail_left").toInt();
    //合计
    bp_total = f_get_sysparm("bp_fmt_total");
    bp_total_len = f_get_sysparm("bp_fmt_total_len").toInt() / bp_content_len_scale_h;
    bp_total_row = f_get_sysparm("bp_fmt_total_row", "1").toInt();
    bp_fmt_total_left = f_get_sysparm("bp_fmt_total_left").toInt();

    //    //做法
    bp_printmemo = f_get_sysparm("bp_fmt_printmemo");

    bp_fmt_table_fmts =  f_get_sysparm("bp_fmt_table_fmts");
    bp_fmt_serial_no_desc =  f_get_sysparm("bp_fmt_serial_no_desc");
    bp_fmt_operid_fmts =  f_get_sysparm("bp_fmt_operid_fmts", "no");


    bp_offcols  = f_get_sysparm("bp_fmt_offcols", "1").toInt();
    bp_begincols  = f_get_sysparm("bp_fmt_begincols", "1").toInt();

    bp_vch_dishname_No  = f_get_sysparm("bp_vch_dishname_No", "vch_dishname");

    litterblank ="--";
}

public_printby_ch_billno::~public_printby_ch_billno()
{
}

void public_printby_ch_billno::toPayPrint(const QString &bp_path, int bp_count, int label_print_int_flowid_max, payPrintModes m)
{
    if(m & payDrawer) {
        blockIns ins;
        ins += pieceIns(lds::sysconf->value("system_setting/qianxiangzhiling").toByteArray(), "");
        Printer_Normal::toprint(bp_path, ins, 1, 0);
        //防止钱箱太快，影响小票
        lds_thread::msecSleep(lds::sysconf->value("system_setting/drawer_timeout", 1000).toInt());
    }

    if(m & payNormal) {
        toprint(bp_path, bp_count, false);
    }

    if(m & payExit) {
        toprint(bp_path, bp_count, true);
    }

    if(m & payLabel) {
        public_printby_ch_billno::print_label(ch_billno, label_print_int_flowid_max);
    }
}

void public_printby_ch_billno::toprint(const QString &path, int manual_bp_count, bool payexit)
{
    rowno = 0;
    lds_query query;
    print_format.print_path = "/dev/usb/lp0";
    print_format.print_path = path;

    query.execSelect(QString("select int_paperwidth FROM cey_bt_kitchen_print where vch_printip = '%1' ")
                     .arg(print_format.print_path));
    query.next();
    paperwidth = query.recordValue("int_paperwidth").toInt();
    if(paperwidth == 0) {
        qDebug("cey_bt_kitchen_print not exist int_paperwidth, please check !");
        paperwidth = 32;
    }
    print_format.paperwidth = paperwidth;

    //手动输入手牌号
    query.execSelect(QString("select vch_memo from cey_u_master where ch_billno = '%1' ")
                     .arg(ch_billno));
    query.next();
    QString serial_no_cur = query.recordValue("vch_memo").toString();
    if(serial_no_cur.isEmpty()) {//默认
        serial_no_cur = QString().sprintf("%03d", ch_billno.right(5).toInt());
    }
    serial_no_cur = QString().sprintf("%03d", serial_no_cur.toInt());


    //时长
    bp_count = manual_bp_count;
    query.execSelect(QString("select timediff(b.dt_operdate ,a.dt_operdate) , b.dt_operdate  from cey_u_table  a, cey_u_checkout_master b where a.ch_billno = b.ch_billno and b.ch_state = 'Y' and b.ch_billno = '%1' ")
                     .arg(ch_billno));
    query.next();
    QString time_diff = query.recordValue(0).toString();
    QString cey_u_checkout_master_dt = query.recordValue("dt_operdate").toDateTime().toString(yyyyMMddhhmmss);

    print_format.drawText("i");

    //logo
    if(f_get_sysparm("bp_fmt_title_logo_en") == "1"){
        print_format.drawText("a00");
        print_format.drawN();
        print_format.drawLogo("", f_get_sysparm("bp_fmt_title_logo"));
        print_format.drawText("a10");
        print_format.drawN();
    }

    ///第一标题
    if(f_get_sysparm("bp_fmt_title_enable") == "1"){
        print_format.drawText("a00,s11", print_format.printTr(QT_TR_NOOP("付款单")));
        print_format.drawN();
        print_format.drawText("a10,s00");
    }
    //第二标题
    if(f_get_sysparm("bp_fmt_title_sc_enable") == "1"){
        print_format.drawText("a00,s11", f_get_sysparm("bp_fmt_title_second"));
        print_format.drawN();
        print_format.drawText("a10,s00");
    }


    //置顶语
    if(f_get_sysparm("bp_fmt_top_enable") == "1") {
        print_format.drawText("a00", f_get_sysparm("bp_fmt_top_desc"));
        print_format.drawText("a10");
        print_format.drawN();
        print_format.drawText("a10");//在drawN后面容易出现
    }


    qDebug() << query.lastQuery();
    ///---begin--////////////////////////////////////////
    print_format.drawText("a10");
    print_format.begin_column_print(bp_begincols);
    if(f_get_sysparm("bp_fmt_payno") == "1") print_format.drawText("s"+f_get_sysparm("bp_fmt_payno_len", "00").toLocal8Bit(), print_format.printTr(QT_TR_NOOP("单号"))+":"+get_cey_u_table("ch_payno").toString());
    if(f_get_sysparm("bp_fmt_operid") == "1") print_format.drawText("s"+f_get_sysparm("bp_fmt_operid_len",  "00").toLocal8Bit(), print_format.printTr(QT_TR_NOOP("工号"))+":"+get_bp_fmt_operid());


    if(f_get_sysparm("bp_fmt_table") == "1") print_format.drawText("s"+f_get_sysparm("bp_fmt_table_len", "00").toLocal8Bit(), print_format.printTr(QT_TR_NOOP("餐桌"))+":"+get_bp_fmt_table());

    if(f_get_sysparm("bp_fmt_person") == "1") print_format.drawText("s"+f_get_sysparm("bp_fmt_person_len",  "00").toLocal8Bit(), print_format.printTr(QT_TR_NOOP("人数"))+":"+get_cey_u_table("int_person").toString());


    if(f_get_sysparm("bp_fmt_waiter") == "1") print_format.drawText("s"+f_get_sysparm("bp_fmt_waiter_len",  "00").toLocal8Bit(), print_format.printTr(QT_TR_NOOP("服务员"))+":"+get_cey_u_table("vch_waiter").toString());
    if(f_get_sysparm("bp_fmt_waitername") == "1") print_format.drawText("s"+f_get_sysparm("bp_fmt_waitername_len",  "00").toLocal8Bit(), print_format.printTr(QT_TR_NOOP("服务员名称"))+":"+name_of_operid(get_cey_u_table("vch_waiter").toString()));


    if(f_get_sysparm("bp_fmt_serial_no") == "1") print_format.drawText("s"+f_get_sysparm("bp_fmt_serial_no_len",  "00").toLocal8Bit(), bp_fmt_serial_no_desc+":"+serial_no_cur);
    if(f_get_sysparm("bp_fmt_time") == "1") print_format.drawText("s"+f_get_sysparm("bp_fmt_time_len",  "00").toLocal8Bit(), print_format.printTr(QT_TR_NOOP("时间"))+":"+cey_u_checkout_master_dt);
    if(f_get_sysparm("bp_fmt_times") == "1") print_format.drawText("s"+f_get_sysparm("bp_fmt_times_len",  "00").toLocal8Bit(), print_format.printTr(QT_TR_NOOP("时长"))+":"+time_diff);


    if(f_get_sysparm("bp_fmt_memo") == "1") print_format.drawText("s"+f_get_sysparm("bp_fmt_memo_len",  "00").toLocal8Bit(), print_format.printTr(QT_TR_NOOP("开台备注"))+":"+get_cey_u_table("vch_memo").toString());

    print_format.end_column_print();
    ///---end--////////////////////////////////////////

    print_content(QDateTime(), payexit);

    //付款详细
    //应收、实收、找零     query.execSelect(QString("select ifnull(sum(num_realamount), 0), ifnull(sum(num_payamount), 0), ifnull(sum(num_change), 0) from cey_u_checkout_detail where ch_payno = '%1' and ch_paymodeno <> 'ZZ'  and ch_paymodeno <> 'YY' ")//优惠和自动抹零
    //消费：cey_u_checkout_master-num_cost
    //折扣：cey_u_checkout_master-num_discount
    //赠送：cey_u_checkout_master-num_present
    //服务费：cey_u_checkout_master-num_servic
    //低消：cey_u_checkout_master-num_lowcost
    //抹零：cey_u_checkout_master-num_blotout
    //税率：cey_u_checkout_master-num_rate
    //免单：cey_u_checkout_master-num_free

    //人均
    //会员类型
    //会员名称
    //会员打印余额
    //会员积分



    print_format.drawN();
    print_format.drawText("a10,s00");
    print_format.drawText("-");

    //付款详细，付款方式，应收，会员等
    print_pay_detail();

    print_tail();//二维码，票尾预之类的

    print_finish();
}

void public_printby_ch_billno::toprint_preprint(const QString &path, const QString &title, const QDateTime &dt, const QString &out_sql)
{
    lds_query query;
    print_format.print_path = "/dev/usb/lp0";
    print_format.print_path = path;

    query.execSelect(QString("select int_paperwidth FROM cey_bt_kitchen_print where vch_printip = '%1' ")
                     .arg(print_format.print_path));
    query.next();
    paperwidth = query.recordValue("int_paperwidth").toInt();
    if(paperwidth == 0) {
        qDebug("cey_bt_kitchen_print not exist int_paperwidth, please check !");
        paperwidth = 32;
    }
    print_format.paperwidth = paperwidth;

    print_format.drawText("i");
    //logo
    if(f_get_sysparm("bp_fmt_title_logo_en") == "1"){
        print_format.drawText("a00");
        print_format.drawN();
        print_format.drawLogo("", f_get_sysparm("bp_fmt_title_logo"));
        print_format.drawN();
        print_format.drawText("a10");
    }
    //title
    print_format.drawText("a00,s11", title);
    print_format.drawN();
    print_format.drawText("a10,s00");
    print_format.drawText("a10");//在drawN后面容易出现

    //表头
    ///---begin--////////////////////////////////////////
    print_format.begin_column_print(bp_begincols);
    if(f_get_sysparm("bp_fmt_operid") == "1") print_format.drawText("s"+f_get_sysparm("bp_fmt_operid_len", "00").toLocal8Bit(), print_format.printTr(QT_TR_NOOP("工号"))+":"+get_cey_u_table("vch_operID").toString());

    if(f_get_sysparm("bp_fmt_table") == "1") print_format.drawText("s"+f_get_sysparm("bp_fmt_table_len", "00").toLocal8Bit(), print_format.printTr(QT_TR_NOOP("餐桌"))+":"+get_bp_fmt_table());

    if(f_get_sysparm("bp_fmt_person") == "1") print_format.drawText("s"+f_get_sysparm("bp_fmt_person_len", "00").toLocal8Bit(), print_format.printTr(QT_TR_NOOP("人数"))+":"+get_cey_u_table("int_person").toString());

    if(f_get_sysparm("bp_fmt_waiter") == "1") print_format.drawText("s"+f_get_sysparm("bp_fmt_waiter_len", "00").toLocal8Bit(), print_format.printTr(QT_TR_NOOP("服务员"))+":"+get_cey_u_table("vch_waiter").toString());
    if(f_get_sysparm("bp_fmt_waitername") == "1") print_format.drawText("s"+f_get_sysparm("bp_fmt_waitername_len", "00").toLocal8Bit(), print_format.printTr(QT_TR_NOOP("服务员名称"))+":"+name_of_operid(get_cey_u_table("vch_waiter").toString()));

    if(f_get_sysparm("bp_fmt_time") == "1") print_format.drawText("s"+f_get_sysparm("bp_fmt_time_len", "00").toLocal8Bit(), print_format.printTr(QT_TR_NOOP("时间"))+":"+n_func::f_get_sysdatetime_str());

    print_format.end_column_print();
    ///---end--////////////////////////////////////////

    print_content(dt, false, out_sql);

    print_format.drawN();
    print_format.drawText("a10,s00");
    print_format.drawText("-");
    ///消费情况
    toprint_preprint_summary();
    print_tail();//二维码，票尾预之类的    //表尾走纸
    print_finish();
}

void public_printby_ch_billno::toprint_dish_return(const QString &path, const QDateTime &dt)
{
    rowno=  0;

    lds_query query;
    print_format.print_path = "/dev/usb/lp0";
    print_format.print_path = path;

    query.execSelect(QString("select int_paperwidth FROM cey_bt_kitchen_print where vch_printip = '%1' ")
                     .arg(print_format.print_path));
    query.next();
    paperwidth = query.recordValue("int_paperwidth").toInt();
    if(paperwidth == 0) {
        qDebug("cey_bt_kitchen_print not exist int_paperwidth, please check !");
        paperwidth = 32;
    }
    print_format.paperwidth = paperwidth;
    print_format.drawText("a00,s11", print_format.printTr(QT_TR_NOOP("退菜单")));
    print_format.drawN();
    print_format.drawText("a10,s00");
    print_format.drawText("a10");//在drawN后面容易出现

    //表头
    ///---begin--////////////////////////////////////////
    print_format.begin_column_print(bp_begincols);
    if(f_get_sysparm("bp_fmt_operid") == "1") print_format.drawText("s"+f_get_sysparm("bp_fmt_operid_len", "00").toLocal8Bit(), print_format.printTr(QT_TR_NOOP("工号"))+":"+get_cey_u_table("vch_operID").toString());

    if(f_get_sysparm("bp_fmt_table") == "1") print_format.drawText("s"+f_get_sysparm("bp_fmt_table_len", "00").toLocal8Bit(), print_format.printTr(QT_TR_NOOP("餐桌"))+":"+get_bp_fmt_table());

    if(f_get_sysparm("bp_fmt_person") == "1") print_format.drawText("s"+f_get_sysparm("bp_fmt_person_len", "00").toLocal8Bit(), print_format.printTr(QT_TR_NOOP("人数"))+":"+QString::number(get_cey_u_table("int_person").toInt()));

    if(f_get_sysparm("bp_fmt_waiter") == "1") print_format.drawText("s"+f_get_sysparm("bp_fmt_waiter_len", "00").toLocal8Bit(), print_format.printTr(QT_TR_NOOP("服务员"))+":"+get_cey_u_table("vch_waiter").toString());
    if(f_get_sysparm("bp_fmt_waitername") == "1") print_format.drawText("s"+f_get_sysparm("bp_fmt_waitername_len", "00").toLocal8Bit(), print_format.printTr(QT_TR_NOOP("服务员名称"))+":"+name_of_operid(get_cey_u_table("vch_waiter").toString()));

    if(f_get_sysparm("bp_fmt_time") == "1") print_format.drawText("s"+f_get_sysparm("bp_fmt_time_len", "00").toLocal8Bit(), print_format.printTr(QT_TR_NOOP("时间"))+":"+n_func::f_get_sysdatetime_str());

    print_format.end_column_print();
    ///---end--////////////////////////////////////////
    print_content_dishreturn(dt);

    print_finish();
}

void public_printby_ch_billno::toprintwaimai(const QString &path, const QDateTime &dt)
{
    lds_query query;
    print_format.print_path = "/dev/usb/lp0";
    print_format.print_path = path;

    query.execSelect(QString("select int_paperwidth FROM cey_bt_kitchen_print where vch_printip = '%1' ")
                     .arg(print_format.print_path));
    query.next();
    paperwidth = query.recordValue("int_paperwidth").toInt();
    if(paperwidth == 0) {
        qDebug("cey_bt_kitchen_print not exist int_paperwidth, please check !");
        paperwidth = 32;
    }
    print_format.paperwidth = paperwidth;

    print_format.drawText("a00,s11", print_format.printTr(QT_TR_NOOP("外卖单")));
    print_format.drawN();
    print_format.drawText("a10,s00");
    print_format.drawText("a10");//在drawN后面容易出现
    //表头
    ///---begin--////////////////////////////////////////
    print_format.begin_column_print(bp_begincols);
    if(f_get_sysparm("bp_fmt_operid") == "1") print_format.drawText("s"+f_get_sysparm("bp_fmt_operid_len", "00").toLocal8Bit(), print_format.printTr(QT_TR_NOOP("工号"))+":"+get_cey_u_table("vch_operID").toString());

    if(f_get_sysparm("bp_fmt_table") == "1") print_format.drawText("s"+f_get_sysparm("bp_fmt_table_len", "00").toLocal8Bit(), print_format.printTr(QT_TR_NOOP("餐桌"))+":"+get_bp_fmt_table());

    if(f_get_sysparm("bp_fmt_person") == "1") print_format.drawText("s"+f_get_sysparm("bp_fmt_person_len", "00").toLocal8Bit(), print_format.printTr(QT_TR_NOOP("人数"))+":"+get_cey_u_table("int_person").toString());

    if(f_get_sysparm("bp_fmt_time") == "1") print_format.drawText("s"+f_get_sysparm("bp_fmt_time_len", "00").toLocal8Bit(), print_format.printTr(QT_TR_NOOP("时间"))+":"+n_func::f_get_sysdatetime_str());


    query.execSelect(QString("select vch_linkman, vch_waiter, vch_tel, vch_address FROM cey_u_togo where ch_billno = '%1' ").arg(ch_billno));
    query.next();
    
    if(f_get_sysparm("bp_fmt_address") == "1") print_format.drawText("s"+f_get_sysparm("bp_fmt_address_len", "00").toLocal8Bit(), print_format.printTr(QT_TR_NOOP("外卖地址"))+":"+query.recordValue("vch_address").toString());

    if(f_get_sysparm("bp_fmt_linkman") == "1") print_format.drawText("s"+f_get_sysparm("bp_fmt_linkman_len", "00").toLocal8Bit(), print_format.printTr(QT_TR_NOOP("外卖人"))+":"+query.recordValue("vch_linkman").toString());

    if(f_get_sysparm("bp_fmt_tel") == "1") print_format.drawText("s"+f_get_sysparm("bp_fmt_tel_len", "00").toLocal8Bit(), print_format.printTr(QT_TR_NOOP("外卖电话"))+":"+query.recordValue("vch_tel").toString());


    print_format.end_column_print();
    ///---end--////////////////////////////////////////

    print_content(dt);

    print_format.drawN();
    print_format.drawText("a10,s00");
    print_format.drawText("-");
    ///消费情况
    query.execSelect(sumsql);//预打单没有汇总的消费信息，通过求和得出
    query.next();
    QByteArray bp_tail_len;
    ///---begin--////////////////////////////////////////
    print_format.begin_column_print(bp_offcols);

    if(f_get_sysparm("bp_fmt_cost") == "1"){
        bp_tail_len  = "s" + f_get_sysparm("bp_fmt_cost_len").toLocal8Bit();
        float subtotal = query.recordValue("amount").toFloat();
        subtotal = w_scr_dish_pay_widget::valuerountbysys(subtotal);//四舍五入
        print_format.drawText(bp_tail_len, key_value_com(print_format.printTr(QT_TR_NOOP("总金额"))+":",  format_amount(subtotal, "subtotal"), print_format.column_width()/ scale_h_by(bp_tail_len)));

    }


    print_format.end_column_print();
    ///---end--////////////////////////////////////////
    print_tail();//二维码，票尾预之类的    //表尾走纸
    print_finish();
}

void public_printby_ch_billno::toprintwaimai_pay(const QString &path, int manual_bp_count)
{
    rowno = 0;
    lds_query query;
    print_format.print_path = path;

    query.execSelect(QString("select int_paperwidth FROM cey_bt_kitchen_print where vch_printip = '%1' ")
                     .arg(print_format.print_path));
    query.next();
    paperwidth = query.recordValue("int_paperwidth").toInt();
    if(paperwidth == 0) {
        qDebug("cey_bt_kitchen_print not exist int_paperwidth, please check !");
        paperwidth = 32;
    }
    print_format.paperwidth = paperwidth;

    //手动输入手牌号
    query.execSelect(QString("select vch_memo from cey_u_master where ch_billno = '%1' ")
                     .arg(ch_billno));
    query.next();
    QString serial_no_cur = query.recordValue("vch_memo").toString();
    if(serial_no_cur.isEmpty()) {//默认
        serial_no_cur = QString().sprintf("%03d", ch_billno.right(5).toInt());
    }
    serial_no_cur = QString().sprintf("%03d", serial_no_cur.toInt());

    ///第一标题
    if(f_get_sysparm("bp_fmt_title_enable") == "1"){
        print_format.drawText("a00,s11", print_format.printTr(QT_TR_NOOP("付款单"))+"/"+print_format.printTr(QT_TR_NOOP("外卖")));
        print_format.drawN();
        print_format.drawText("a10,s00");
    }
    //第二标题
    if(f_get_sysparm("bp_fmt_title_sc_enable") == "1"){
        print_format.drawText("a00,s11", f_get_sysparm("bp_fmt_title_second")+"/"+print_format.printTr(QT_TR_NOOP("外卖")));
        print_format.drawN();
        print_format.drawText("a10,s00");
    }
    print_format.drawText("a10");//在drawN后面容易出现

    //时间
    bp_count = manual_bp_count;
    query.execSelect(QString("select  dt_operdate  from cey_u_checkout_master where ch_billno = '%1' and ch_state = 'Y' ")
                     .arg(ch_billno));
    query.next();
    QString cey_u_checkout_master_dt = query.recordValue("dt_operdate").toDateTime().toString(yyyyMMddhhmmss);
    //表头
    ///---begin--////////////////////////////////////////
    print_format.begin_column_print(bp_begincols);
    if(f_get_sysparm("bp_fmt_payno") == "1") print_format.drawText("s"+f_get_sysparm("bp_fmt_payno_len", "00").toLocal8Bit(), print_format.printTr(QT_TR_NOOP("单号"))+":"+get_cey_u_table("ch_payno").toString());
    if(f_get_sysparm("bp_fmt_operid") == "1") print_format.drawText("s"+f_get_sysparm("bp_fmt_operid_len",  "00").toLocal8Bit(), print_format.printTr(QT_TR_NOOP("工号"))+":"+get_cey_u_table("vch_operID").toString());
    if(f_get_sysparm("bp_fmt_table") == "1") print_format.drawText("s"+f_get_sysparm("bp_fmt_table_len", "00").toLocal8Bit(), print_format.printTr(QT_TR_NOOP("餐桌"))+":"+get_bp_fmt_table());
    if(f_get_sysparm("bp_fmt_person") == "1") print_format.drawText("s"+f_get_sysparm("bp_fmt_person_len",  "00").toLocal8Bit(), print_format.printTr(QT_TR_NOOP("人数"))+":"+get_cey_u_table("int_person").toString());
    if(f_get_sysparm("bp_fmt_time") == "1") print_format.drawText("s"+f_get_sysparm("bp_fmt_time_len",  "00").toLocal8Bit(), print_format.printTr(QT_TR_NOOP("时间"))+":"+cey_u_checkout_master_dt);

    query.execSelect(QString("select vch_linkman, vch_waiter, vch_tel, vch_address FROM cey_u_togo where ch_billno = '%1' ").arg(ch_billno));
    query.next();
    
    if(f_get_sysparm("bp_fmt_address") == "1") print_format.drawText("s"+f_get_sysparm("bp_fmt_address_len", "00").toLocal8Bit(), print_format.printTr(QT_TR_NOOP("外卖地址"))+":"+query.recordValue("vch_address").toString());

    if(f_get_sysparm("bp_fmt_linkman") == "1") print_format.drawText("s"+f_get_sysparm("bp_fmt_linkman_len", "00").toLocal8Bit(), print_format.printTr(QT_TR_NOOP("外卖人"))+":"+query.recordValue("vch_linkman").toString());

    if(f_get_sysparm("bp_fmt_tel") == "1") print_format.drawText("s"+f_get_sysparm("bp_fmt_tel_len", "00").toLocal8Bit(), print_format.printTr(QT_TR_NOOP("外卖电话"))+":"+query.recordValue("vch_tel").toString());

    if(f_get_sysparm("bp_fmt_waiter") == "1") print_format.drawText("s"+f_get_sysparm("bp_fmt_waiter_len",  "00").toLocal8Bit(), print_format.printTr(QT_TR_NOOP("服务员"))+":"+query.recordValue("vch_waiter").toString());
    if(f_get_sysparm("bp_fmt_waitername") == "1") print_format.drawText("s"+f_get_sysparm("bp_fmt_waitername_len",  "00").toLocal8Bit(), print_format.printTr(QT_TR_NOOP("服务员名称"))+":"+name_of_operid(query.recordValue("vch_waiter").toString()));
    if(f_get_sysparm("bp_fmt_serial_no") == "1") print_format.drawText("s"+f_get_sysparm("bp_fmt_serial_no_len",  "00").toLocal8Bit(), bp_fmt_serial_no_desc+":"+serial_no_cur);


    print_format.end_column_print();
    ///---end--////////////////////////////////////////

    print_content(QDateTime());
    ///
    print_format.drawN();
    print_format.drawText("a10,s00");
    print_format.drawText("-");
    //付款详细，付款方式，应收，会员等
    print_pay_detail();
    //单号
    //工号
    //餐桌
    //人数
    //时间
    //外卖地址
    //外卖人
    //外卖电话
    //服务员
    //服务员名称
    //流水号

    //。。。


    //签名
    //尾语
    //图片打印
    //打印一维码
    //打印二维码
    //是否开钱箱
    print_tail();//二维码，票尾预之类的


    print_finish();



}

double public_printby_ch_billno::toprint_preprint_div(const QString &path, int sqr, const QDateTime &dt)
{
    rowno=  0;

    lds_query query;
    print_format.print_path = "/dev/usb/lp0";
    print_format.print_path = path;

    query.execSelect(QString("select int_paperwidth FROM cey_bt_kitchen_print where vch_printip = '%1' ")
                     .arg(print_format.print_path));
    query.next();
    paperwidth = query.recordValue("int_paperwidth").toInt();
    if(paperwidth == 0) {
        qDebug("cey_bt_kitchen_print not exist int_paperwidth, please check !");
        paperwidth = 32;
    }
    print_format.paperwidth = paperwidth;

    if(sqr != -1) {
        print_format.drawText("a00,s11", print_format.printTr(QT_TR_NOOP("分单单号"))+":"+QString::number(sqr));
        print_format.drawN();
    }
    print_content(dt);

    print_format.drawN();
    print_format.drawText("a10,s00");
    print_format.drawText("-");
    ///消费情况
    double subtotal = toprint_preprint_summary();
    ///
    print_format.synchPrint();

    return subtotal;
}

void public_printby_ch_billno::toprint_preprint_div_title(const QString &path, const QString &title)
{
    rowno=  0;

    lds_query query;
    print_format.print_path = "/dev/usb/lp0";
    print_format.print_path = path;

    query.execSelect(QString("select int_paperwidth FROM cey_bt_kitchen_print where vch_printip = '%1' ")
                     .arg(print_format.print_path));
    query.next();
    paperwidth = query.recordValue("int_paperwidth").toInt();
    if(paperwidth == 0) {
        qDebug("cey_bt_kitchen_print not exist int_paperwidth, please check !");
        paperwidth = 32;
    }
    print_format.paperwidth = paperwidth;
    print_format.drawText("a00,s11", title);
    print_format.drawN();
    print_format.drawText("a10,s00");
    print_format.drawText("a10");//在drawN后面容易出现

    //表头
    ///---begin--////////////////////////////////////////
    print_format.begin_column_print(bp_begincols);
    if(f_get_sysparm("bp_fmt_operid") == "1") print_format.drawText("s"+f_get_sysparm("bp_fmt_operid_len", "00").toLocal8Bit(), print_format.printTr(QT_TR_NOOP("工号"))+":"+get_bp_fmt_operid());

    if(f_get_sysparm("bp_fmt_table") == "1") print_format.drawText("s"+f_get_sysparm("bp_fmt_table_len", "00").toLocal8Bit(), print_format.printTr(QT_TR_NOOP("餐桌"))+":"+get_bp_fmt_table());

    if(f_get_sysparm("bp_fmt_person") == "1") print_format.drawText("s"+f_get_sysparm("bp_fmt_person_len", "00").toLocal8Bit(), print_format.printTr(QT_TR_NOOP("人数"))+":"+get_cey_u_table("int_person").toString());

    if(f_get_sysparm("bp_fmt_waiter") == "1") print_format.drawText("s"+f_get_sysparm("bp_fmt_waiter_len", "00").toLocal8Bit(), print_format.printTr(QT_TR_NOOP("服务员"))+":"+get_cey_u_table("vch_waiter").toString());
    if(f_get_sysparm("bp_fmt_waitername") == "1") print_format.drawText("s"+f_get_sysparm("bp_fmt_waitername_len", "00").toLocal8Bit(), print_format.printTr(QT_TR_NOOP("服务员名称"))+":"+name_of_operid(get_cey_u_table("vch_waiter").toString()));

    if(f_get_sysparm("bp_fmt_time") == "1") print_format.drawText("s"+f_get_sysparm("bp_fmt_time_len", "00").toLocal8Bit(), print_format.printTr(QT_TR_NOOP("时间"))+":"+n_func::f_get_sysdatetime_str());

    print_format.end_column_print();
    ///---end--////////////////////////////////////////

    print_format.drawN();

    print_format.synchPrint();
}

void public_printby_ch_billno::toprint_preprint_div_tail(const QString &path, double subtotal, int idvcount)
{
    if(f_get_sysparm("bp_fmt_cost") == "1" && idvcount > 1) {
        print_format.drawText("-");
        QByteArray bp_tail_len  = "s" + f_get_sysparm("bp_fmt_cost_len").toLocal8Bit();
        print_format.drawText(bp_tail_len,
                              print_format.printTr(QT_TR_NOOP("总单数")) + ":" + QString::number(idvcount) + "  "+
                              print_format.printTr(QT_TR_NOOP("总金额")) + ":" + format_amount(subtotal, "subtotal"));
    }

    print_format.print_path = "/dev/usb/lp0";
    print_format.print_path = path;
    print_tail();//二维码，票尾预之类的    //表尾走纸
    print_finish();
}

/*

*/

double public_printby_ch_billno::toprint_preprint_summary()
{
    ///消费情况
    fexpandmain_model_sqltablemodel_data tablemodel;
    tablemodel.setTable("cey_u_orderdish");
    tablemodel.select(ch_billno, 0, true);

    int tax_service_form_value = model_cache.get_cey_sys_parameter_value("tax_service_form_value", 0).toInt();
    //pay_receive
    double pay_receive = tablemodel.getKeepdata().dish_total();
    double subtotal = w_scr_dish_pay_widget::valuerountbysys(pay_receive);//四舍五入
    double num_blotout = pay_receive - subtotal;
    //    QString ch_tableno =  model_cache.get_cey_u_table_value(ch_billno, "ch_tableno");
    //    QString ch_typeno =  model_cache.get_cey_bt_table_type_value(ch_tableno, "ch_typeno");
    //    double num_max_service = model_cache.get_cey_bt_table_type_value(ch_typeno, "num_max_service").toInt();
    ///
    print_format.begin_column_print(bp_begincols);
    QByteArray s00;
    if(tablemodel.getKeepdata().num_service != 0 ||  tablemodel.getKeepdata().num_rate != 0) {//服务费 != 0 或 税率 != 0
        QString tax_desc;
        switch(tax_service_form_value) {
        case 1://含税,不含服务费
            //pay_receive
            tax_desc = "("+print_format.printTr(QT_TR_NOOP("不计")) + ")";
            pay_receive += tablemodel.getKeepdata().num_service;
            subtotal = w_scr_dish_pay_widget::valuerountbysys(pay_receive);//四舍五入
            num_blotout = pay_receive - subtotal;
            break;
        case 2://不含税,含服务费
            //pay_receive
            pay_receive += tablemodel.getKeepdata().num_service + tablemodel.getKeepdata().num_rate;
            subtotal = w_scr_dish_pay_widget::valuerountbysys(pay_receive);//四舍五入
            num_blotout = pay_receive - subtotal;
            break;
        case 3://含税,含服务费
            //pay_receive
            tax_desc = "("+print_format.printTr(QT_TR_NOOP("不计")) + ")";
            pay_receive += tablemodel.getKeepdata().num_service;
            subtotal = w_scr_dish_pay_widget::valuerountbysys(pay_receive);//四舍五入
            num_blotout = pay_receive - subtotal;
            break;
        default://不含税,不含服务费
            //pay_receive
            pay_receive += tablemodel.getKeepdata().num_service + tablemodel.getKeepdata().num_rate;
            subtotal = w_scr_dish_pay_widget::valuerountbysys(pay_receive);//四舍五入
            num_blotout = pay_receive - subtotal;
            break;
        }

        s00 = "s" + f_get_sysparm("bp_fmt_cost_len").toLocal8Bit();
        if(pay_receive != 0) //消费 != 0
            print_format.drawText(
                        s00,
                        key_value_com(print_format.printTr(QT_TR_NOOP("消费"))+":",   format_amount(/*消费*/tablemodel.getKeepdata().dish_total(), "num_cost"), print_format.column_width()/ scale_h_by(s00))
                        );
        s00 = "s" + f_get_sysparm("bp_fmt_service_len").toLocal8Bit();
        qDebug() << __LINE__ << "服务费:" << tablemodel.getKeepdata().num_service;
        if(tablemodel.getKeepdata().num_service != 0)//服务费 != 0
            print_format.drawText(
                        s00,
                        key_value_com(print_format.printTr(QT_TR_NOOP("服务费")) +pay_fixed_service_suffix()+":",   format_amount(tablemodel.getKeepdata().num_service, "num_service"), print_format.column_width()/ scale_h_by(s00))
                        );
        s00 = "s" + f_get_sysparm("bp_fmt_tax_len").toLocal8Bit();
        if(tablemodel.getKeepdata().num_rate != 0)//税率 != 0
            print_format.drawText(
                        s00,
                        key_value_com(print_format.printTr(QT_TR_NOOP("税率"))+pay_fixed_tax_suffix()+tax_desc+":",   format_amount(tablemodel.getKeepdata().num_rate, "num_rate"), print_format.column_width()/ scale_h_by(s00))
                        );
        s00 = "s" + f_get_sysparm("bp_fmt_blotout_len").toLocal8Bit();
        if(num_blotout != 0) //服务费+税率 != 0
            print_format.drawText(
                        s00,
                        key_value_com(print_format.printTr(QT_TR_NOOP("抹零"))+":",   format_amount(num_blotout, "num_blotout"), print_format.column_width()/ scale_h_by(s00))
                        );
    }
    s00 = "s" + f_get_sysparm("bp_fmt_cost_len").toLocal8Bit();//金额 != 0
    print_format.drawText(
                s00,
                key_value_com(print_format.printTr(QT_TR_NOOP("金额"))+":",   format_amount(subtotal, "subtotal"), print_format.column_width()/ scale_h_by(s00))
                );
    print_format.end_column_print();
    print_format.drawN();

    return subtotal;
}

void public_printby_ch_billno::clear_ins()
{
    print_format.print_ins.clear();
}

QString public_printby_ch_billno::defpath()
{
    return lds::sysconf->value("system_setting/printerpath").toString();
}

void public_printby_ch_billno::print_label(const QString &ch_billno, int print_int_flowid_max)
{
    lds_query query;
    query.execSelect("select count(0) from cey_bt_dish_subtype where ch_labelprint_flag = 'Y' ");
    query.next();
    if(0 == query.record().value(0).toInt()) {
        return;
    }
    //手动输入手牌号
    query.execSelect(QString("select vch_memo from cey_u_master where ch_billno = '%1' ")
                     .arg(ch_billno));
    query.next();
    QString serial_no_cur = query.recordValue("vch_memo").toString();
    if(serial_no_cur.isEmpty()) {//默认
        serial_no_cur = QString().sprintf("%03d", ch_billno.right(5).toInt());
    }
    serial_no_cur = QString().sprintf("%03d", serial_no_cur.toInt());

    QString vch_company;
    query.execSelect("select vch_company FROM cey_sys_company;");
    query.next();
    vch_company = query.recordValue("vch_company").toString();

    QStandardItemModel printmodel;
    printmodel.setHorizontalHeaderLabels(QStringList()
                                         << tr("打印次数")<< tr("单号")<< tr("价格")
                                         << tr("品名") <<  tr("品码") <<tr("时间") << tr("做法") << tr("店名")
                                         << tr("数量")
                                         );
    QString check_int_flowid;
    if(print_int_flowid_max != -1) {
        check_int_flowid = QString(" and a.int_flowid > '%1' ").arg(print_int_flowid_max);
    }
    /// 金额：赠送0， 非赠送((点菜-退菜)*价格)*0.01*折扣+加价
    query.execSelect(QString(
                         "select  if(a.ch_presentflag = 'Y', 0,   (a.num_num - a.num_back)*a.num_price*0.01*a.int_discount)  + a.num_price_add  as num_cost , b.vch_dishname, a.ch_dishno, a.dt_operdate, vch_print_memo, "
                         " b.ch_weight, num_num-num_back as dish_num  from cey_u_orderdish a, cey_bt_dish b ,cey_bt_dish_subtype c "
                         " where a.ch_dishno = b.ch_dishno and b.ch_sub_typeno = c.ch_sub_typeno and c.ch_labelprint_flag = 'Y' and ch_billno = '%1' and (num_num-num_back) > 0 "
                         )
                     .arg(ch_billno)
                     + check_int_flowid);
    if(query.next()) {
        query.previous();
        while(query.next()) {
            double dish_num = qMax(query.recordValue("dish_num").toDouble(), 1.0);
            double num_cost_per = 0;

            if(query.recordValue("ch_weight").toString() == "Y") {//称重商品
                dish_num = query.recordValue("dish_num").toDouble();
                num_cost_per = query.recordValue("num_cost").toDouble();

                printmodel.appendRow(QList<QStandardItem*>()
                                     <<new QStandardItem("1")
                                     <<new QStandardItem((serial_no_cur))
                                     <<new QStandardItem(QString::number(num_cost_per))
                                     <<new QStandardItem(query.recordValue("vch_dishname").toString())
                                     <<new QStandardItem(query.recordValue("ch_dishno").toString())
                                     <<new QStandardItem(query.recordValue("dt_operdate").toDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                                     <<new QStandardItem(fexpandmain_model_sqltablemodel::get_desc_vch_print_memo(query.recordValue("vch_print_memo").toString()))
                                     <<new QStandardItem(vch_company)
                                     <<new QStandardItem(QString().sprintf("%.02f", dish_num))
                                     );
            } else {
                //sql dish_num > 0
                num_cost_per = query.recordValue("num_cost").toDouble() / dish_num;
                QMap<QString, int> map = fexpandmain_model_sqltablemodel::get_map_vch_print_memo(query.recordValue("vch_print_memo").toString());
                for(QMap<QString, int>::iterator k = map.begin(); k != map.end(); k ++) {
                    k.value() = k.value() / dish_num;
                }
                QString vch_print_memo_per = fexpandmain_model_sqltablemodel::get_desc_vch_print_memo(fexpandmain_model_sqltablemodel::get_value_vch_print_memo(map));

                for(int k = 0; k < dish_num; k ++) {//非承重商品
                    float k_num = 1;
                    if(dish_num - k < 1) {
                        k_num = dish_num - k;
                    }
                    printmodel.appendRow(QList<QStandardItem*>()
                                         <<new QStandardItem("1")
                                         <<new QStandardItem((serial_no_cur))
                                         <<new QStandardItem(QString::number(num_cost_per))
                                         <<new QStandardItem(query.recordValue("vch_dishname").toString())
                                         <<new QStandardItem(query.recordValue("ch_dishno").toString())
                                         <<new QStandardItem(query.recordValue("dt_operdate").toDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                                         <<new QStandardItem(vch_print_memo_per)
                                         <<new QStandardItem(vch_company)
                                         <<new QStandardItem(QString::number(k_num))
                                         );
                }
            }
        }
        for(int row = 0; row < printmodel.rowCount(); row++) {
            printmodel.item(row, 1)->setText(printmodel.item(row, 1)->text() + QString("(%1_%2)").arg(printmodel.rowCount()).arg(row + 1));
        }

        QString errstring;
        labelprint_main_dialog_object print_help;
        if(!print_help.printLabel(&printmodel, false, &errstring)) {
            QMetaObject::invokeMethod(public_sql::save_login_object(),
                                      "showprinterr",
                                      Qt::QueuedConnection,
                                      Q_ARG(QString, errstring),
                                      Q_ARG(QString, tr("标签打印机"))
                                      );
        }
    }
}

void public_printby_ch_billno::print_label_test(QWidget *parent)
{
    labelprint_main_dialog dialog(parent);
    dialog.printmodel->setHorizontalHeaderLabels(QStringList()
                                                 << tr("打印次数")
                                                 //                                                 << tr("抬头")
                                                 << tr("单号")

                                                 << tr("价格")
                                                 //                                                 << tr("电话")
                                                 //                                                 << tr("地址")

                                                 << tr("品名")
                                                 << tr("品码")
                                                 << tr("时间")
                                                 << tr("做法")
                                                 << tr("店名")
                                                 << tr("数量")
                                                 );
    dialog.printmodel->appendRow(QList<QStandardItem*>()
                                 <<new QStandardItem("1")
                                 //                                 <<new QStandardItem(tr("抬头")
                                 <<new QStandardItem(tr("单号"))
                                 <<new QStandardItem(tr("价格"))
                                 //                                 <<new QStandardItem(tr("电话")
                                 //                                 <<new QStandardItem(tr("地址")

                                 <<new QStandardItem(tr("品名"))
                                 <<new QStandardItem(tr("品码"))
                                 <<new QStandardItem(tr("时间"))
                                 <<new QStandardItem(tr("做法"))
                                 <<new QStandardItem(tr("店名"))
                                 <<new QStandardItem(tr("数量"))
                                 );
    dialog.set_label_enabled();
    dialog.inputfinished(true);
    dialog.toprint(parent);
}

blockIns public_printby_ch_billno::getPintIns() const
{
    return print_format.print_ins;
}

void public_printby_ch_billno::print_content(const QDateTime &dt, bool payexit, const QString & out_sql)
{
    lds_query query;
    //打印明细
    //打印本单序号、赠送标识、特价标识、超出长度截断、大类分组
    //品名、单位、数量、价格、金额、折扣、做法
    //特价菜品打印原价：特价/原价 ： 12.00/15.00
    //折扣：1元
    //大类分组时，套菜不参与分组

    QString order_sql;
    QString bp_series_group = f_get_sysparm("bp_fmt_series_group");
    QString ch_dish_typeno;
    QString ch_sub_typeno;
    if(bp_series_group == "0") order_sql += "order by a.int_flowid asc ";
    if(bp_series_group == "1") order_sql += " order by b.ch_dish_typeno asc";
    if(bp_series_group == "2") order_sql += " order by b.ch_sub_typeno asc";


    QString dt_filter;
    if(dt.isValid()) {
        dt_filter = QString(" and a.dt_operdate > '%1' ").arg(dt.toString(yyyyMMddhhmmss));
    }

    QString out_sql_filter;
    if(!out_sql.isEmpty()) {
        out_sql_filter = " and " + out_sql;
    }
    //
    sumsql =
            /// 金额：赠送0， 非赠送((点菜-退菜)*价格)*0.01*折扣+加价
            QString("select sum(   if(a.ch_presentflag = 'Y', 0,   (%2 - a.num_back)*a.num_price*0.01*a.int_discount  )   + a.num_price_add    ) as amount from cey_u_orderdish a, cey_bt_dish b  where a.ch_billno = '%1' and a.ch_dishno = b.ch_dishno  ")
            .arg(ch_billno)
            .arg(payexit?"0":"a.num_num")
            + dt_filter
            + out_sql_filter;
    //
    intersql =
            QString(
                " SELECT "
                "  concat(%3(case a.ch_suitflag when 'P' then '%1' when 'Y' then '%2' else '' end),b."
                +bp_vch_dishname_No
                +") as dishname"
                " , c.vch_unitname as unit"
                " , sum(%4 - a.num_back) as num"
                " , a.num_price as price"
                " , sum( if(ch_presentflag = 'Y'  ,  0  ,  (%4 - a.num_back)*a.num_price*0.01*a.int_discount )   ) as amount"
                " , sum(a.num_price*(%4 - a.num_back)*(1-a.int_discount*0.01) ) as disdetail"
                " , vch_print_memo"
                " , (select vch_reason from cey_u_orderdish_log aa where aa.int_orderflow = a.int_flowID limit 0, 1) as vch_reason"
                " , b.ch_dish_typeno"
                " , b.ch_sub_typeno"
                " , a.ch_suitflag"
                " , a.int_flowID"
                " , a.ch_specialflag"
                " , a.ch_presentflag"
                " , a.ch_waidai_flag"

                "  ,a.num_price_org"
                "  ,a.num_price_add"
                "   FROM cey_u_orderdish a, cey_bt_dish b, cey_bt_unit c"
                )
            .arg(print_format.printTr(QT_TR_NOOP("套"))+")")
            .arg(/*print_format.printTr(QT_TR_NOOP("细"))*/"   ")
            .arg("")//bp_rowid == "1" ? "a.int_id+1, '.' ," :
            .arg(payexit?"0":"a.num_num")
            +"  where a.ch_billno = '%1' "+(payexit?" and a.num_back > 0 " :"and (a.num_num - a.num_back) <> 0 ")+"and a.ch_dishno = b.ch_dishno and b.ch_unitno = c.ch_unitno"
            + dt_filter
            + out_sql_filter;

    print_format.drawN();
    print_format.drawText("a10,s00");
    print_format.drawText("-");
    print_format.drawN();


    print_format.drawText(bp_content_len);
    //header
    query.execSelect(" select "// 序号默认3个长度
                     +QString(" '%1' as dishname"
                              ", '%2' as unit"
                              ", '%3' as num"
                              ", '%4' as price"
                              ", '%5' as amount"
                              ", '%6' as disdetail"
                              ", '' as vch_print_memo"
                              ", '' as vch_reason "
                              ", '' as ch_dish_typeno"
                              ", '' as ch_sub_typeno"
                              ", '' as ch_suitflag"
                              ", '' as int_flowID"
                              ", '' as ch_specialflag"
                              ", '' as ch_presentflag "
                              ", '' as ch_waidai_flag "

                              ", '' as num_price_org"
                              ", '' as num_price_add" )
                     .arg(print_format.printTr(QT_TR_NOOP("品名")))
                     .arg(print_format.printTr(QT_TR_NOOP("单位")))
                     .arg(print_format.printTr(QT_TR_NOOP("数量")))
                     .arg(print_format.printTr(QT_TR_NOOP("单价")))
                     .arg(print_format.printTr(QT_TR_NOOP("小计")))
                     .arg(print_format.printTr(QT_TR_NOOP("折扣")))
                     );
    query.next();
    piecedishprint(query, bp_content_len, true);
    //content
    query.execSelect(intersql.arg(ch_billno)
                     +QString(" and a.ch_suitflag <> 'Y' ")
                     +group_sql_samedishNotadd_qtprint()
                     +order_sql);
    while(query.next()){
        //1
        if(bp_series_group == "1") {
            if(ch_dish_typeno != query.recordValue("ch_dish_typeno").toString()){
                ch_dish_typeno = query.recordValue("ch_dish_typeno").toString();
                print_format.drawText(bp_content_len, litterblank+get_dish_typename(ch_dish_typeno)+litterblank+get_dish_typeno_sum(ch_dish_typeno), print_format.paperwidth, Qt::AlignCenter);
                print_format.drawN();
            }
        }
        if(bp_series_group == "2") {
            if(ch_sub_typeno != query.recordValue("ch_sub_typeno").toString()){
                ch_sub_typeno = query.recordValue("ch_sub_typeno").toString();
                print_format.drawText(bp_content_len, litterblank+get_sub_typename(ch_sub_typeno)+litterblank+get_sub_typeno_sum(ch_sub_typeno), print_format.paperwidth, Qt::AlignCenter);
                print_format.drawN();
            }
        }
        piecedishprint(query, bp_content_len, false);
    }
}

void public_printby_ch_billno::print_content_dishreturn(const QDateTime &dt)
{
    lds_query query;
    //打印明细
    //打印本单序号、赠送标识、特价标识、超出长度截断、大类分组
    //品名、单位、数量、价格、金额、折扣、做法
    //特价菜品打印原价：特价/原价 ： 12.00/15.00
    //折扣：1元
    //大类分组时，套菜不参与分组

    QString appsql;
    QString bp_series_group = f_get_sysparm("bp_fmt_series_group");
    QString ch_dish_typeno;
    QString ch_sub_typeno;
    if(bp_series_group == "1") appsql += " order by b.ch_dish_typeno asc";
    if(bp_series_group == "2") appsql += " order by b.ch_sub_typeno asc";

    sumsql =
            QString("select sum(      (0 - d.num_back_cur)*a.num_price*0.01*a.int_discount   - d.num_price_add     ) as amount from cey_u_orderdish a, cey_bt_dish b, cey_u_orderdish_log  d where a.ch_billno = '%1' and a.ch_dishno = b.ch_dishno and a.int_flowID = d.int_orderflow  ")
            .arg(ch_billno)
            +QString("   and d.dt_operdate = '%1' ")
            .arg(dt.toString(yyyyMMddhhmmss))
            ;

    //
    intersql =
            QString(
                " SELECT   "
                "   concat((case a.ch_suitflag when 'P' then '%1' when 'Y' then '%2' else '' end),b."
                +bp_vch_dishname_No
                +") as dishname,"
                "   c.vch_unitname as unit ,"
                "   -1*d.num_back_cur as num ,"
                "   a.num_price as price ,"
                "    ((0 - d.num_back_cur)*a.num_price*0.01*a.int_discount  )  as amount , "
                "   a.num_price*-1*d.num_back_cur*(1-a.int_discount*0.01)  as disdetail , "
                "   d.vch_print_memo , "
                "   d.vch_reason ,"
                "   b.ch_dish_typeno , "
                "   b.ch_sub_typeno , "
                "   a.ch_suitflag , "
                "   a.int_flowID , "
                "   a.ch_specialflag , "
                "   a.ch_presentflag,   "
                "   a.ch_waidai_flag,   "
                "   a.num_price_org, "
                "   -d.num_price_add as num_price_add"
                "   FROM cey_u_orderdish a, cey_bt_dish b, cey_bt_unit c  "
                ", cey_u_orderdish_log d where d.ch_type = 1 "
                )
            .arg(print_format.printTr(QT_TR_NOOP("套"))+")")
            .arg(/*print_format.printTr(QT_TR_NOOP("细"))*/"   ")
            +"    and a.ch_billno = '%1' and num_back_cur <> 0 and a.ch_dishno = b.ch_dishno and b.ch_unitno = c.ch_unitno and a.int_flowID = d.int_orderflow "
            +QString("   and d.dt_operdate = '%1' ")
            .arg(dt.toString(yyyyMMddhhmmss))
            ;
    print_format.drawN();
    print_format.drawText("a10,s00");
    print_format.drawText("-");
    print_format.drawN();
    print_format.drawText(bp_content_len);

    //header
    query.execSelect(" select "// 序号默认3个长度
                     +QString(" '%1' as dishname"
                              ", '%2' as unit"
                              ", '%3' as num"
                              ", '%4' as price"
                              ", '%5' as amount"
                              ", '%6' as disdetail"
                              ", '' as vch_print_memo"
                              ", '' as vch_reason "
                              ", '' as ch_dish_typeno"
                              ", '' as ch_sub_typeno"
                              ", '' as ch_suitflag"
                              ", '' as int_flowID"
                              ", '' as ch_specialflag"
                              ", '' as ch_presentflag "
                              ", '' as ch_waidai_flag "

                              ", '' as num_price_org"
                              ", '' as num_price_add" )
                     .arg(print_format.printTr(QT_TR_NOOP("品名")))
                     .arg(print_format.printTr(QT_TR_NOOP("单位")))
                     .arg(print_format.printTr(QT_TR_NOOP("数量")))
                     .arg(print_format.printTr(QT_TR_NOOP("单价")))
                     .arg(print_format.printTr(QT_TR_NOOP("小计")))
                     .arg(print_format.printTr(QT_TR_NOOP("折扣")))
                     );

    query.next();
    piecedishprint(query, bp_content_len, true, -1);
    //~header
    //content
    query.execSelect(intersql.arg(ch_billno)
                     + " and a.ch_suitflag <> 'Y' "
                     +appsql);
    while(query.next()){
        //1
        if(bp_series_group == "1") {
            if(ch_dish_typeno != query.recordValue("ch_dish_typeno").toString()){
                ch_dish_typeno = query.recordValue("ch_dish_typeno").toString();
                print_format.drawText(bp_content_len, litterblank+get_dish_typename(ch_dish_typeno)+litterblank+get_dish_typeno_sum(ch_dish_typeno), Qt::AlignCenter);
                print_format.drawN();
            }
        }
        if(bp_series_group == "2") {
            if(ch_sub_typeno != query.recordValue("ch_sub_typeno").toString()){
                ch_sub_typeno = query.recordValue("ch_sub_typeno").toString();
                print_format.drawText(bp_content_len);
                print_format.drawText(bp_content_len, litterblank+get_sub_typename(ch_sub_typeno)+litterblank+get_sub_typeno_sum(ch_sub_typeno), Qt::AlignCenter);
                print_format.drawN();
            }
        }
        piecedishprint(query, bp_content_len, false, -1);
    }
    //~content
}

void public_printby_ch_billno::print_tail()
{
    //票尾
    print_format.drawN();
    print_format.drawText("s00");
    int tailRow = print_format.currentIndex();
    //签名
    bool hasTail = false;
    if(f_get_sysparm("bp_fmt_underwrite") == "1"){
        hasTail = true;
        print_format.drawText("a10", print_format.printTr(QT_TR_NOOP("签名:")));
        print_format.drawN();
    }
    //票尾语
    QString bp_fmt_end_desc = f_get_sysparm("bp_fmt_end_desc");
    if(bp_fmt_end_desc.length() > 0) {
        hasTail = true;
        print_format.drawText("a00", bp_fmt_end_desc);
        print_format.drawText("a10");
        print_format.drawN();
    }
    //一维码
    if(f_get_sysparm("bp_fmt_end_bc_en") == "1"){
        hasTail = true;
        print_format.drawText("a00");
        print_format.drawoneBarcode("", f_get_sysparm("bp_fmt_end_bc"));
    }
    //二维码
    if(f_get_sysparm("bp_fmt_end_rc_en") == "1"){
        hasTail = true;
        print_format.drawQR_Code("", f_get_sysparm("bp_fmt_end_rc"));
    }

    if(hasTail) {
        print_format.insertText(tailRow,"-");
    }
}

void public_printby_ch_billno::print_finish()
{
    //表尾走纸
    int count = f_get_sysparm("bp_fmt_tail_block").toInt();
    print_format.drawText("", QString(count, '\n'));

    //打印最后
    print_format.drawTextEnd("1B6405 1D5601");//走纸5x， 切纸

    //    print_format.setPrintCount(bp_count);
    for(int k = 0; k < bp_count; k++) {
        print_format.synchPrint();
        if(k < bp_count) {
            ///打印间隔
            lds_thread::msecSleep(1000);
            ///~打印间隔
        }
    }
}

void public_printby_ch_billno::checkpaperwidth()
{

}

void public_printby_ch_billno::print_pay_detail()
{
    lds_query query;
    ///消费情况
    query.execSelect(QString("select  *  from cey_u_checkout_master where ch_billno = '%1'  and ch_state = 'Y'  ")//cey_u_checkout_master 存储的会员的优惠信息
                     .arg(ch_billno));
    query.next();
    QString vch_memberno = query.recordValue("vch_memberno").toString();
    QString ch_member_belong_state = query.recordValue("ch_member_belong_state").toString();
    QMap<QString/*会员号*/, QString/*state*/> m_state_map;
    if(!vch_memberno.isEmpty()) {
        m_state_map.insert(vch_memberno, ch_member_belong_state);
    }
    QByteArray bp_tail_len;

    ///---begin--////////////////////////////////////////
    print_format.begin_column_print(bp_offcols);

    if(f_get_sysparm("bp_fmt_cost") == "1"){
        bp_tail_len  = "s" + f_get_sysparm("bp_fmt_cost_len").toLocal8Bit();
        print_format.drawText(bp_tail_len, key_value_com(print_format.printTr(QT_TR_NOOP("消费"))+":",  format_amount( query.recordValue("num_cost").toFloat(), "num_cost"), print_format.column_width()/ scale_h_by(bp_tail_len)));
    }
    //    if(f_get_sysparm("bp_fmt_present") == "1")
    {
        if( query.recordValue("num_present").toFloat() != 0) {
            bp_tail_len  = "s" + f_get_sysparm("bp_fmt_present_len").toLocal8Bit();
            print_format.drawText(bp_tail_len, key_value_com(print_format.printTr(QT_TR_NOOP("赠送"))+":",  format_amount(query.recordValue("num_present").toFloat(), "num_present"), print_format.column_width()/ scale_h_by(bp_tail_len)));
        }
    }
    //    if(f_get_sysparm("bp_fmt_discount") == "1")
    {
        if( query.recordValue("num_discount").toFloat() != 0) {
            bp_tail_len  = "s" + f_get_sysparm("bp_fmt_discount_len").toLocal8Bit();
            print_format.drawText(bp_tail_len, key_value_com(print_format.printTr(QT_TR_NOOP("折扣"))+":",  format_amount( query.recordValue("num_discount").toFloat(), "num_discount"), print_format.column_width()/ scale_h_by(bp_tail_len)));
        }
    }
    if(f_get_sysparm("bp_fmt_service") == "1"){
        bp_tail_len  = "s" + f_get_sysparm("bp_fmt_service_len").toLocal8Bit();

        print_format.drawText(bp_tail_len, key_value_com(print_format.printTr(QT_TR_NOOP("服务费"))+pay_fixed_service_suffix()+":",  format_amount(query.recordValue("num_service").toFloat(), "num_service"), print_format.column_width()/ scale_h_by(bp_tail_len)));
    }
    if(f_get_sysparm("bp_fmt_tax") == "1") {
        bp_tail_len  = "s" + f_get_sysparm("bp_fmt_tax_len").toLocal8Bit();
        print_format.drawText(bp_tail_len, key_value_com(print_format.printTr(QT_TR_NOOP("税率"))+pay_fixed_tax_suffix() + ":",  format_amount(query.recordValue("num_rate").toFloat(), "num_rate"), print_format.column_width()/ scale_h_by(bp_tail_len)));
    }

    //    if(f_get_sysparm("bp_fmt_lowcost") == "1")
    {
        float num_lowcost = query.recordValue("num_lowcost").toFloat();
        if(num_lowcost != 0) {
            bp_tail_len  = "s" + f_get_sysparm("bp_fmt_lowcost_len").toLocal8Bit();
            print_format.drawText(bp_tail_len, key_value_com(print_format.printTr(QT_TR_NOOP("低消"))+":",  format_amount(num_lowcost, "num_lowcost"), print_format.column_width()/ scale_h_by(bp_tail_len)));
        }
    }
    if(query.recordValue("num_free").toFloat() != 0){//优惠不和其他一样，不等于0时，打印
        bp_tail_len  = "s" + f_get_sysparm("bp_fmt_free_len").toLocal8Bit();
        print_format.drawText(bp_tail_len, key_value_com(print_format.printTr(QT_TR_NOOP("优惠"))+":",  format_amount(query.recordValue("num_free").toFloat(), "num_free"), print_format.column_width()/ scale_h_by(bp_tail_len)));
    }
    //    if(f_get_sysparm("bp_fmt_blotout") == "1") {
    if( query.recordValue("num_blotout").toFloat() != 0) {
        bp_tail_len  = "s" + f_get_sysparm("bp_fmt_blotout_len").toLocal8Bit();
        print_format.drawText(bp_tail_len, key_value_com(print_format.printTr(QT_TR_NOOP("抹零"))+":",  format_amount(query.recordValue("num_blotout").toFloat(), "num_blotout"), print_format.column_width()/ scale_h_by(bp_tail_len)));
    }
    //    }
    ///
    ///付款方式详细
    print_format.drawN();
    query.execSelect(QString("select ifnull(sum(num_realamount), 0) as num_realamount, ifnull(sum(num_payamount), 0) as num_amount, ifnull(sum(num_change), 0)  as num_change from cey_u_checkout_detail where ch_payno = '%1' and ch_paymodeno <> 'ZZ'  and ch_paymodeno <> 'YY' ")//优惠和自动抹零
                     .arg(get_cey_u_table("ch_payno").toString()));
    query.next();
    if(f_get_sysparm("bp_fmt_receive") == "1"){
        bp_tail_len  = "s" + f_get_sysparm("bp_fmt_receive_len").toLocal8Bit();
        print_format.drawText(bp_tail_len, key_value_com(print_format.printTr(QT_TR_NOOP("应收"))+":",  format_amount(query.recordValue("num_realamount").toFloat(), "num_realamount"), print_format.column_width()/ scale_h_by(bp_tail_len)));
    }
    if(f_get_sysparm("bp_fmt_cash") == "1"){
        bp_tail_len  = "s" + f_get_sysparm("bp_fmt_cash_len").toLocal8Bit();
        print_format.drawText(bp_tail_len, key_value_com(print_format.printTr(QT_TR_NOOP("实收"))+":",  format_amount(query.recordValue("num_amount").toFloat(), "num_amount"), print_format.column_width()/ scale_h_by(bp_tail_len)));
    }

    if(f_get_sysparm("bp_fmt_change") == "1"){
        bp_tail_len  = "s" + f_get_sysparm("bp_fmt_change_len").toLocal8Bit();
        print_format.drawText(bp_tail_len, key_value_com(print_format.printTr(QT_TR_NOOP("找零"))+":",  format_amount(query.recordValue("num_change").toFloat(), "num_change"), print_format.column_width()/ scale_h_by(bp_tail_len)));
    }
    if(get_cey_u_table("int_person").toInt() != 0){
        if(f_get_sysparm("bp_fmt_avg") == "1"){
            bp_tail_len  = "s" + f_get_sysparm("bp_fmt_avg_len").toLocal8Bit();
            print_format.drawText(bp_tail_len, key_value_com(print_format.printTr(QT_TR_NOOP("人均"))+":",  format_amount(query.recordValue("num_realamount").toFloat() / get_cey_u_table("int_person").toInt(), "num_realamount_per_person"), print_format.column_width()/ scale_h_by(bp_tail_len)));
        }
    }

    ///
    ///付款方式情况
    ///
    print_format.drawN();
    if(f_get_sysparm("bp_fmt_pay_detail") == "1"){
        print_format.drawText("", print_format.printTr(QT_TR_NOOP("付款方式")));
        print_format.drawN();
        bp_tail_len  = "s" + f_get_sysparm("bp_fmt_pay_detail_len").toLocal8Bit();
        query.execSelect(QString(" SELECT b.ch_paymodeno, b.vch_paymodename, a.num_payamount-a.num_change as real_payamount, a.vch_voucherno FROM cey_u_checkout_detail a, cey_bt_paymode b where a.ch_paymodeno = b.ch_paymodeno and a.ch_payno = '%1' ")
                         .arg(get_cey_u_table("ch_payno").toString()));
        while(query.next()){
            if(query.recordValue("ch_paymodeno").toString() == "ZZ"//优惠跳过
                    || query.recordValue("ch_paymodeno").toString() == "YY"//自动抹零
                    ) {
                continue;
            }
            print_format.drawText(bp_tail_len, key_value_com(query.recordValue("vch_paymodename").toString()+":",  format_amount(query.recordValue("real_payamount").toFloat(), "real_payamount"), print_format.column_width()/ scale_h_by(bp_tail_len)));
        }
    }
    ///



    ///会员情况
    //会员类型
    //会员名称
    //会员打印余额
    //会员积分
    //付款方式：02、10
    print_format.drawN();
    if(f_get_sysparm("bp_fmt_m_detail") == "1"){

        QString vch_member, num_amount,num_point,cardno ;
        bp_tail_len  = "s" + f_get_sysparm("bp_fmt_m_detail_len").toLocal8Bit();
        query.execSelect(QString("select a.vch_voucherno ,if(a.ch_paymodeno='10', 'Y', 'N')  from  cey_u_checkout_detail a where  ( a.ch_paymodeno = '02' or a.ch_paymodeno = '10' ) and a.ch_payno = '%1' ")
                         .arg(get_cey_u_table("ch_payno").toString()));
        
        while(query.next()) {
            m_state_map.insert(query.recordValue(0).toString(), query.recordValue(1).toString());
        }
        QMapIterator<QString/*会员号*/, QString/*state*/> mapitr(m_state_map);
        while(mapitr.hasNext()) {
            mapitr.next();
            cardno =  mapitr.key();
            if(mapitr.value() == "Y") {
                bool ok;
                vch_member=w_sys_manage_cloudsync::get_yun_member_readcard(0, cardno, n_func::f_get_sysparm("cloud_user"),
                                                                           "vch_member", ok);if(!ok) continue;
                num_amount=w_sys_manage_cloudsync::get_yun_member_readcard(0, cardno, n_func::f_get_sysparm("cloud_user"),
                                                                           "num_amount", ok);if(!ok) continue;
                num_point=w_sys_manage_cloudsync::get_yun_member_readcard(0, cardno, n_func::f_get_sysparm("cloud_user"),
                                                                          "num_point", ok);if(!ok) continue;
            } else {
                query.execSelect(QString("select b.vch_member, c.num_amount, c.num_point FROM  t_m_member b, t_m_curamount c where b.vch_memberno = c.vch_memberno and b.vch_memberno = '%1' ")
                                 .arg(mapitr.key()));
                query.next();
                vch_member = query.recordValue("vch_member").toString();
                num_amount = query.recordValue("num_amount").toString();
                num_point = query.recordValue("num_point").toString();
            }

            print_format.drawText(bp_tail_len, key_value_com((mapitr.value() == "Y" ?print_format.printTr(QT_TR_NOOP("云")):"")+print_format.printTr(QT_TR_NOOP("会员号"))+":",  cardno, print_format.column_width()/ scale_h_by(bp_tail_len)));
            print_format.drawText(bp_tail_len, key_value_com((mapitr.value() == "Y" ?print_format.printTr(QT_TR_NOOP("云")):"")+print_format.printTr(QT_TR_NOOP("会员"))+":",  vch_member, print_format.column_width()/ scale_h_by(bp_tail_len)));
            print_format.drawText(bp_tail_len, key_value_com(print_format.printTr(QT_TR_NOOP("余额"))+":",  format_amount(num_amount.toFloat(), "num_amount"), print_format.column_width()/ scale_h_by(bp_tail_len)));
            print_format.drawText(bp_tail_len, key_value_com(print_format.printTr(QT_TR_NOOP("积分"))+":",  format_amount(num_point.toFloat(), "num_point"), print_format.column_width()/ scale_h_by(bp_tail_len)));
        }
    }

    print_format.end_column_print();
    ///---end--////////////////////////////////////////
}

QString public_printby_ch_billno::get_dish_typename(const QString &ch_dish_typeno)
{
    lds_query query;
    //select vch_dish_typename from cey_bt_dish_type where ch_dish_typeno = '10';
    query.execSelect(QString("select vch_dish_typename from cey_bt_dish_type where ch_dish_typeno = '%1' ")
                     .arg(ch_dish_typeno));
    query.next();
    return query.recordValue("vch_dish_typename").toString();
}

QString public_printby_ch_billno::get_sub_typename(const QString &ch_sub_typeno)
{
    lds_query query;
    query.execSelect(QString("select vch_sub_typename from cey_bt_dish_subtype where ch_sub_typeno = '%1' ")
                     .arg(ch_sub_typeno));
    query.next();
    
    return query.recordValue("vch_sub_typename").toString();
}

QString public_printby_ch_billno::get_dish_typeno_sum(const QString &ch_dish_typeno)
{
    lds_query query;
    query.execSelect(sumsql + QString(" and b.ch_dish_typeno = '%1' ")
                     .arg(ch_dish_typeno));
    query.next();
    return print_format.printTr(QT_TR_NOOP("小计"))+":"+format_amount(query.recordValue(0).toFloat(), "subtotal");
}

QString public_printby_ch_billno::get_sub_typeno_sum(const QString &ch_sub_typeno)
{
    lds_query query;
    query.execSelect(sumsql + QString(" and b.ch_sub_typeno = '%1' ")
                     .arg(ch_sub_typeno));
    query.next();
    return print_format.printTr(QT_TR_NOOP("小计"))+":"+format_amount(query.recordValue(0).toFloat(), "subtotal");
}

void public_printby_ch_billno::suitdishprint(int P_int_flowid, const QByteArray &pre, int print_memo_num_x)
{
    //主套菜切刀子套菜
    lds_query query;
    query.execSelect(QString("SELECT ch_dishno from cey_u_orderdish where int_flowid = %1 ").arg(P_int_flowid));
    query.next();
    QString ch_suitno = query.record().value(0).toString();
    query.execSelect(intersql.arg(ch_billno) +QString(" and a.ch_suitflag = 'Y' and ch_suitno = '%1' "
                                                      +group_sql_samedishNotadd_qtprint()
                                                      +" order by a.int_flowid asc").arg(ch_suitno));
    while(query.next()){
        if(query.recordValue("ch_suitflag") != "Y") break;
        piecedishprint( query, pre, false, print_memo_num_x);
    }
}

void public_printby_ch_billno::piecedishprint(lds_query &query, const QByteArray &pre, bool isheader, int print_memo_num_x)
{
    print_format.drawText("a10");
    //2
    QByteArray vch_print_memo = query.recordValue("vch_print_memo").toByteArray();
    QString vch_reason = query.recordValue("vch_reason").toString();
    vch_reason = "";
    QString ch_suitflag = query.recordValue("ch_suitflag").toString();

    QString price;
    QString amount;
    QString dish_num;
    QString disdetail;
    QString total;
    int amount_left = -1;
    QString num_price_add;
    QString preapp;
    if(!isheader && bp_rowid == "1"){
        rowno++;
        //        query2.execSelect(QString("select int_id from cey_u_orderdish where int_flowid = '%1' ")
        //                    .arg(query.recordValue("int_flowID").toString()));
        //        query2.next();
        //        preapp += QString("%1.").arg(query2.recordValue("int_id").toString());
        preapp += QString("%1.").arg(rowno);
    }
    if(bp_presentflag == "1") {
        if(query.recordValue("ch_presentflag").toString() == "Y"){
            preapp += print_format.printTr(QT_TR_NOOP("赠"))+")";
        }
    }

    if(bp_fmt_specialflag == "1") {
        if(query.recordValue("ch_specialflag").toString() == "Y"){
            preapp += print_format.printTr(QT_TR_NOOP("特"))+")";
        }
    }
    if(query.recordValue("ch_waidai_flag").toString() == "Y"){
        preapp += print_format.printTr(QT_TR_NOOP("外带"))+")";
    }
    if(isheader) {
        price = query.recordValue("price").toString();
        amount = query.recordValue("amount").toString();
        dish_num = query.recordValue("num").toString();
        disdetail = query.recordValue("disdetail").toString();
        total = print_format.printTr(QT_TR_NOOP("合计"));
    } else {
        price = format_price(query.recordValue("price").toFloat(), "price");
        if(bp_fmt_price_with_org == "1"
                &&ch_suitflag != "Y") {//非子套菜
            if(query.recordValue("price").toFloat() != query.recordValue("num_price_org").toFloat()) {
                price = price+"/"+format_price(query.recordValue("num_price_org").toFloat(), "num_price_org");
            }
        }
        amount = format_amount(query.recordValue("amount").toFloat(), "amount");
        dish_num = format_num(query.recordValue("num").toFloat(), "num");
        if(!print_format.languageIsCN()) {
            dish_num = "x"+dish_num;
        }
        disdetail ="";
        if(query.recordValue("disdetail").toFloat() != 0) {
            disdetail = format_amount(query.recordValue("disdetail").toFloat(), "disdetail");
        }
        total = format_amount(query.recordValue("price").toFloat() * query.recordValue("num").toFloat(), "total");
        num_price_add = format_amount(query.recordValue("num_price_add").toFloat(), "num_price_add");
    }

    //第一行
    if(bp_dishname_row == 1 && bp_dishname == "1")print_format.drawText(pre, QString(bp_dishname_left, ' ')+getfixedlen(preapp+query.recordValue("dishname").toString(), bp_dishname_len), bp_dishname_len, Qt::AlignLeft);
    if(bp_num_row == 1 && bp_num == "1")print_format.drawText(pre, QString(bp_num_left, ' ')+dish_num, bp_num_len, Qt::AlignRight);
    if(bp_unit_row == 1 && bp_unit == "1")print_format.drawText(pre, QString(bp_unit_left, ' ')+query.recordValue("unit").toString(), bp_unit_len, Qt::AlignLeft);
    if(bp_price_row == 1 && bp_price == "1")print_format.drawText(pre, QString(bp_price_left, ' ')+price, bp_price_len, Qt::AlignRight);
    if(bp_total_row == 1 && bp_total == "1")print_format.drawText(pre, QString(bp_fmt_total_left, ' ')+total, bp_total_len, Qt::AlignRight);
    if(bp_disdetail_row == 1 && bp_disdetail == "1")print_format.drawText(pre, QString(bp_fmt_disdetail_left, ' ')+disdetail, bp_disdetail_len, Qt::AlignRight);
    if(bp_amount_row == 1 && bp_amount == "1") {   amount_left = print_format.left(); print_format.drawText(pre, QString(bp_amount_left, ' ')+amount, bp_amount_len, Qt::AlignRight);}

    print_format.drawN();

    //第二行
    if(bp_dishname_row == 2 && bp_dishname == "1")print_format.drawText(pre, QString(bp_dishname_left, ' ')+getfixedlen(preapp+query.recordValue("dishname").toString(), bp_dishname_len), bp_dishname_len, Qt::AlignLeft);
    if(bp_num_row == 2 && bp_num == "1")print_format.drawText(pre, QString(bp_num_left, ' ')+dish_num, bp_num_len, Qt::AlignRight);
    if(bp_unit_row == 2 && bp_unit == "1")print_format.drawText(pre, QString(bp_unit_left, ' ')+query.recordValue("unit").toString(), bp_unit_len, Qt::AlignLeft);
    if(bp_price_row == 2 && bp_price == "1")print_format.drawText(pre, QString(bp_price_left, ' ')+price, bp_price_len, Qt::AlignRight);
    if(bp_total_row == 2 && bp_total == "1")print_format.drawText(pre, QString(bp_fmt_total_left, ' ')+total, bp_total_len, Qt::AlignRight);
    if(bp_disdetail_row == 2 && bp_disdetail == "1")print_format.drawText(pre, QString(bp_fmt_disdetail_left, ' ')+disdetail, bp_disdetail_len, Qt::AlignRight);
    if(bp_amount_row == 2 && bp_amount == "1") {   amount_left = print_format.left(); print_format.drawText(pre, QString(bp_amount_left, ' ')+amount, bp_amount_len, Qt::AlignRight);}

    if(!vch_print_memo.trimmed().isEmpty()
            &&bp_printmemo == "1"
            ){
        print_format.drawN();
        QString vch_print_memo_desc = print_format.printTr(QT_TR_NOOP("做法"))+":"+fexpandmain_model_sqltablemodel::get_desc_vch_print_memo(vch_print_memo, print_memo_num_x);
        print_format.drawText("", vch_print_memo_desc);

        ///打印总包含金额、则打印做法加价
        if(amount_left > 0) {
            int amount_left0 = amount_left - lds::size_g_of_u(vch_print_memo_desc);
            if( amount_left0 < 0) {
                print_format.drawN();
            }  else {
                amount_left = amount_left0;
            }

            print_format.drawText(pre, QString(amount_left, ' '));
            print_format.drawText(pre, QString(bp_amount_left, ' ')+num_price_add, bp_amount_len, Qt::AlignRight);
        }
        ///～打印总包含金额、则打印做法加价
    }
    if(!vch_reason.trimmed().isEmpty()){
        print_format.drawN();
        print_format.drawText("", "#"+vch_reason);
        print_format.drawN();
    }

    if(isheader) {
        print_format.drawN();
        print_format.drawText("a10,s00");
        print_format.drawText("-");
    }

    print_format.drawN();//最后添加
    //处理套菜明细
    if(ch_suitflag == "P"){
        suitdishprint(query.recordValue("int_flowID").toInt(), pre, print_memo_num_x);
    }
}

int public_printby_ch_billno::format_Negative(const QString &field_name)
{
    //num_cost num_prensent num_discount num_rate num_blutout num_service num_lowcost num_cash
    if(field_name == "disdetail"
            || field_name == "num_blotout"
            || field_name == "num_present"
            || field_name == "num_discount"
            ) {
        return -1;
    }
    return 1;
}

QString public_printby_ch_billno::format_amount(float value, const QString &field_name)
{
    return f_get_sysparm("pay_unit_prefix") + lds::septNumber(format_Negative(field_name) * value, bp_amount_dec);
}

QString public_printby_ch_billno::format_num(float value, const QString &field_name)
{
    return lds::septNumber(format_Negative(field_name) * value, bp_num_dec);
}

QString public_printby_ch_billno::format_price(float value, const QString &field_name)
{
    return f_get_sysparm("pay_unit_prefix") + lds::septNumber(format_Negative(field_name) * value, bp_price_dec);
}

QString public_printby_ch_billno::name_of_operid(const QString &operid)
{
    if(operid == "0000") return "system";

    lds_query query;
    query.execSelect(QString("select vch_operator FROM cey_sys_operator where vch_operID = '%1' and ch_flag = 'Y';")
                     .arg(operid));
    query.next();
    return query.recordValue("vch_operator").toString();
}

QString public_printby_ch_billno::getfixedlen(const QString &value, int len, const QString & isCut)
{
    len = len;
    int slen = lds::size_g_of_u(value);
    if(isCut != "1" || slen < len){
        return value;
    } else {
        int sum = 0;
        QString retstr;
        foreach(const QChar &c,value){
            if(QByteArray().append(c).size() == 1) {
                sum +=1;
            } else {
                sum += 2;
            }
            if(sum > len) break;//若是len 5， value 碧蓝鱿鱼 -> 碧蓝鱿，不够最多补齐一个汉子
            retstr += c;
            if(sum == len) break;//若是len 5， value 碧蓝鱿鱼 -> 碧蓝鱿，不够最多补齐一个汉子
        }
        return retstr;
    }
}

QString public_printby_ch_billno::getfixedlen(const QString &value, int len)
{
    return getfixedlen(value, len, bp_fmt_overstep_cut);
}

QString public_printby_ch_billno::key_value_com(const QString &k, const QString &v, int len)
{
    int klen = lds::size_g_of_u(k);
    int vlen = lds::size_g_of_u(v);
    return k + QString(len - klen - vlen, ' ')+v;
}

int public_printby_ch_billno::scale_h_by(const QByteArray &s00)
{
    int scale = 1;
    int s0 = s00.mid(1,1).toInt();//s00
    if(s0 != 0){
        scale = s0 + 1;
    }
    return scale;
}

QString public_printby_ch_billno::get_bp_fmt_table(const QString &vch_tablename, const QString &ch_tableno)
{
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

QString public_printby_ch_billno::get_bp_fmt_table()
{
    QString ch_tableno = model_cache.tablename_kvmap_value("cey_u_table", ch_billno, "ch_billno", "ch_tableno").toString();
    QString vch_tablename = model_cache.tablename_kvmap_value("cey_bt_table", ch_tableno, "ch_tableno", "vch_tablename").toString();
    return get_bp_fmt_table(vch_tablename, ch_tableno);
}

QString public_printby_ch_billno::get_bp_fmt_operid(const QString &vch_operator, const QString &vch_operid)
{
    if(bp_fmt_operid_fmts == "name(no)") {
        return vch_operator+"("+vch_operid+")";
    }
    if(bp_fmt_operid_fmts == "name") {
        return vch_operator;
    }
    if(bp_fmt_operid_fmts == "no") {
        return vch_operid;
    }
    return vch_operator+"("+vch_operid+")";
}

QString public_printby_ch_billno::get_bp_fmt_operid()
{
    QString vch_operid = model_cache.tablename_kvmap_value("cey_u_table", ch_billno, "ch_billno", "vch_operid").toString();
    QString vch_operator = model_cache.tablename_kvmap_value("("+backheader::SELECT_OPERID_NAME +")  b", vch_operid, "vch_operid", "vch_operator").toString();
    return get_bp_fmt_operid(vch_operator, vch_operid);
}

QVariant public_printby_ch_billno::get_cey_u_table(const QString &sql_value_name)
{
    return model_cache.tablename_kvmap_value("cey_u_table", ch_billno, "ch_billno", sql_value_name);
}

QString public_printby_ch_billno::group_sql_samedishNotadd_qtprint()
{
    if(lds::sysconf->value("system_setting/samedishNotadd_qtprint", false).toBool()) {
        return " group by a.int_flowid ";
    }
    return CONTENT_GROUP;
}

QString public_printby_ch_billno::f_get_sysparm(const QString &as_parameter, const QVariant &as_default)
{
    QString ret =  model_cache.tablename_kvmap_value("cey_sys_parameter", as_parameter, "vch_parameter", "vch_value", "", as_default).toString();
    if(ret.trimmed().isEmpty()) {
        ret = as_default.toString();
    }
    return ret;
}

QString public_printby_ch_billno::pay_fixed_service_suffix()
{
    if(n_func::f_get_sysparm("pay_fixed_service", "0") == "1") {
        lds_query query;
        query.execSelect(" select int_rate from cey_bt_table_type limit 1");
        query.next();
        return "("+query.recordValue("int_rate").toString() + "%)";
    }
    return "";
}

QString public_printby_ch_billno::pay_fixed_tax_suffix()
{
    if(n_func::f_get_sysparm("pay_fixed_service", "0") == "1") {
        lds_query query;
        query.execSelect(" select int_rate from cey_bt_dish limit 1");
        query.next();
        return "("+query.recordValue("int_rate").toString() + "%)";
    }
    return "";
}
