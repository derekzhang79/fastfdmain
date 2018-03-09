#include "w_scr_dish_pay_widget.h"
#include "ui_w_scr_dish_pay_widget.h"
#include "lds_query.h"
#include "btnlanguageswicth.h"
#include <QSqlError>
#include "lds.h"
#include "n_func.h"
#include "n_func.h"
#include <QKeyEvent>
#include "lds_dialog_input.h"
#include <QProcess>
#include <QHostInfo>
#include "w_bt_dish.h"
#include "free_view.h"
#include "bank_card.h"
#include "w_m_member_list.h"
#include "w_bt_dish_paymode.h"
#include "lds_menu.h"
#include <QWidgetAction>
#include "w_scr_balance_service.h"
#include "w_scr_dish_m_hang_dialog.h"
#include "w_scr_dish_m_hang_pay_dialog.h"
#include "power_oper.h"
#include "w_scr_dish_member.h"
#include "ui_w_scr_dish_member.h"
#include "w_scr_dish.h"
#include "public_printby_ch_billno.h"
#include "fexpandmain_model_sqltablemodel.h"
#include "w_scr_dish_restaurant_div_pop.h"
#include "w_sys_manage_cloudsync_with_time.h"
#include "ftableview_delegate.h"
#include "ftableview_standmodel_sql_none.h"
#include "public_short_cut_map.h"
#include "w_scr_dish_pay_set.h"
#include "mobile_pay_help.h"
#include "business_data_upload_isoft.h"
#include "public_logindialog.h"
#include "w_scr_dish_pay_module_select.h"
#include "lds_model_sqltablemodel_delegate_com.h"
#include "lds.h"
#include "w_scr_dish_pay_select_language.h"
#include "w_scr_dish_main.h"
#include "w_bt_dish_kitchenprinter_printerset.h"
#include "quick_pay_help.h"
#include "lds_dialog_input_double.h"
#include "ui_lds_dialog_input_dialog_double.h"
#include "fastfd_update.h"
#include "w_scr_dish_phone_pay.h"
#include "w_scr_dish_pay_set_ogg.h"
//cey_u_master
//cey_u_checkout_master
//cey_u_service_low
//cey_u_table
//cey_u_orderdish
//cey_u_checkout_detail
//会员卡：t_m_curamount， t_m_point
//cey_u_checkout_master
//会员挂账：t_m_curamount，t_m_hang_balance


static void showWidget(QWidget *w)
{
    if(w->isHidden())
        w->show();
}

static void hideWidget(QWidget *w)
{
    if(!w->isHidden())
        w->hide();
}

double w_scr_dish_pay_widget::valuerountbysys(double value)
{
    //1
    QString str;
    QString type = n_func::f_get_sysparm("blotout_round");
    if(type == "1")//抹零
    {
        str = n_func::f_get_sysparm("blotout_mode");
        if(str == "2")//到元
        {
            return(int(value));//2.63 -> 2
        }
        else if(str == "3")//到十元
        {
            return(int(value)/10*10);//26.36->20
        }
        else if(str == "4")//到百元
        {
            return(int(value)/100*100);//26.36
        }
    } else {//四舍五入
        str = n_func::f_get_sysparm("round_mode");
        if(str == "2")//到角12.36 -> 12.4
        {
            return(qRound(value*10)/10.0);//
        }
        else if(str == "3")//到元
        {
            return(qRound(value));
        }
        else if(str == "4")//到十元
        {
            return(qRound(value/10)*10);
        }
        else if(str == "5")//到五厘
            /*
q
        0.00--------0.00
        0.01--------0.00
        0.02--------0.00
        0.03--------0.05
        0.04--------0.05
        0.05--------0.05
        0.06--------0.05
        0.07--------0.05
        0.08--------0.10
        0.09--------0.10
*/
        {
            int v = qRound(value * 100);//value:2.69999, v:270
            switch(v % 10) {
            case 0:
            case 1:
            case 2:
                return (int(v / 10.0) + 0.0)/10.0;//2.70

            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
                return (int(v / 10.0) + 0.5)/10.0;//

            case 8:
            case 9:
                return (int(v / 10.0) + 1.0)/10.0;
            }
        }
    }
    return(value);
}

bool w_scr_dish_pay_widget::cey_u_orderdish_direct_pay_transaction(const QString &ch_billno, QString *errstring)
{
    lds_query query;
    query.execSelect(QString("SELECT sum(num_num - num_back) from cey_u_orderdish where ch_billno = '%1' ")
                     .arg(ch_billno));
    query.next();
    if(query.recordValue(0).toInt() > 0) {
        if(errstring) *errstring = tr("本单数量不为0")+tr("操作失败");
        return false;
    }

    QString ch_tableno;
    query.execSelect(QString("SELECT ch_tableno from cey_u_table where ch_billno = '%1' ")
                     .arg(ch_billno));
    query.next();
    ch_tableno = query.recordValue(0).toString();

    lds_query::tran_saction();
    QString ch_payno = w_scr_dish_restaurant_div_pop::getMaxChpaynoAdd1_cey_u_checkout_master();
    //付款
    if(query.execInsert("cey_u_checkout_master",qrtVariantPairList()

                        << qrtVariantPair("ch_billno", ch_billno)
                        << qrtVariantPair("ch_tableno", ch_tableno)
                        << qrtVariantPair("num_cost", 0)
                        << qrtVariantPair("num_discount", 0)
                        << qrtVariantPair("num_present", 0)

                        << qrtVariantPair("num_service", 0)
                        //                        << qrtVariantPair("vch_service_desc", "")
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

                        << qrtVariantPair("num_cash", 0)

                        )){
        //餐桌
        if(query.execUpdate("cey_u_table" ,"ch_payno", ch_payno,qrtEqual("ch_billno",ch_billno))) {
            //菜品
            if(query.execUpdate("cey_u_orderdish","ch_payno", ch_payno,qrtEqual("ch_billno",ch_billno))) {

                goto commit;
            }
        }
    }
    if(errstring) *errstring = query.recordError();

    lds_query::roll_back();
    return false;
commit:
    lds_query::com_mit();
    return true;
}

w_scr_dish_pay_widget::w_scr_dish_pay_widget(QWidget *parent)
    : QFrame(parent)
{
}

w_scr_dish_pay_widget::w_scr_dish_pay_widget(w_scr_dish_pay_widget::operatModes m, QWidget *parent, const fexpandmain_model_sqltablemodel_data *tablemodel, int print_int_flowid_max)
    : QFrame(parent)
{
    initGeometry(m);
    initdataByM(tablemodel, print_int_flowid_max);
}


w_scr_dish_pay_widget::~w_scr_dish_pay_widget()
{
    delete ui;
}

void w_scr_dish_pay_widget::initdataByM(const fexpandmain_model_sqltablemodel_data *tablemodel, int print_int_flowid_max)
{
    this->print_int_flowid_max =print_int_flowid_max;
    _tablemodel = tablemodel;

    //
    ui->lineEdit_consum->setDecimals(tablemodel->get_cey_sys_parameter_value("bp_fmt_amount_dec", 2).toInt());
    ui->lineEdit_present->setDecimals(tablemodel->get_cey_sys_parameter_value("bp_fmt_amount_dec", 2).toInt());ui->lineEdit_present->enableShowNegativeValue();
    ui->lineEdit_discount->setDecimals(tablemodel->get_cey_sys_parameter_value("bp_fmt_amount_dec", 2).toInt());ui->lineEdit_discount->enableShowNegativeValue();
    ui->lineEdit_int_rate->setDecimals(tablemodel->get_cey_sys_parameter_value("bp_fmt_amount_dec", 2).toInt());
    ui->lineEdit_blotout->setDecimals(tablemodel->get_cey_sys_parameter_value("bp_fmt_amount_dec", 2).toInt());ui->lineEdit_blotout->enableShowNegativeValue();


    ui->lineEdit_lowcost->setDecimals(tablemodel->get_cey_sys_parameter_value("bp_fmt_amount_dec", 2).toInt());
    ui->lineEdit_service->setDecimals(tablemodel->get_cey_sys_parameter_value("bp_fmt_amount_dec", 2).toInt());
    //    ui->lineEdit_int_person->setDecimals(tablemodel->get_cey_sys_parameter_value("bp_fmt_amount_dec", 2).toInt());

    ui->lineEdit_realreceive->setDecimals(tablemodel->get_cey_sys_parameter_value("bp_fmt_amount_dec", 2).toInt());
    ui->lineEdit_change->setDecimals(tablemodel->get_cey_sys_parameter_value("bp_fmt_amount_dec", 2).toInt());
    ui->lineEdit_moneypay->setDecimals(tablemodel->get_cey_sys_parameter_value("bp_fmt_amount_dec", 2).toInt());

    first_go_in();

    if(pay_operate_mode & OPERAT_QUITBILL) {
        ui->pushButton_pay->setText(tr("退单"));
        refreshQuitbillControl();
    }
}

void w_scr_dish_pay_widget::initGeometry(w_scr_dish_pay_widget::operatModes m, bool strechLayout)
{
    ui = new Ui::w_scr_dish_pay_widget;
    ui->setupUi(this);

    pay_operate_mode = m;
    pay_all_discount = -1;
    pay_finished = false;
    //3
    table_model = 0;
    _tablemodel = 0;
    ui->radioButton_payinfo->setChecked1(true);
    ui->radioButton_payinfo->enableLongPress(true);

    if(strechLayout) {
        tostackinfo0(true);
        resize(lds::MAIN_WINDOW_SIZE.width() - lds::PAY_LEFT_SPACE, lds::MAIN_WINDOW_SIZE.height());

        ui->frame_top->setGeometry(0, 0, this->width(), 30);

        ui->frame_stack0->setGeometry(QRect(0, ui->frame_top->geometry().bottom(), this->width()/2, (this->height() - ui->frame_top->height()) * 2 / 5));
        ui->frame_stack1->setGeometry(QRect(0, ui->frame_top->geometry().bottom(), this->width()/2, (this->height() - ui->frame_top->height()) * 2 / 5));

        ui->frame_summary->setGeometry(ui->frame_stack0->geometry().right(), ui->frame_stack0->geometry().top(), ui->frame_stack0->width(), ui->frame_stack0->height());
        ui->tableView->setGeometry(ui->frame_summary->geometry().left(), ui->frame_summary->geometry().bottom(), ui->frame_summary->width(), ui->frame_summary->height()/2);

        ui->frame_num_key->setGeometry(0, ui->tableView->geometry().bottom(), ui->frame_summary->width(), ui->frame_summary->height());
        ui->frame_pay_key->setGeometry(ui->frame_num_key->geometry().right(), ui->frame_num_key->geometry().top(), ui->frame_num_key->geometry().width(), ui->frame_num_key->geometry().height());

        ui->lineEdit_consum->setLabelText(tr("消费:"));
        ui->lineEdit_present->setLabelText(tr("赠送:"));
        ui->lineEdit_discount->setLabelText(tr("折扣:"));
        ui->lineEdit_int_rate->setLabelText(tr("税收:"));
        ui->lineEdit_blotout->setLabelText(tr("抹零:"));

        ui->lineEdit_ch_tableno->setLabelText(tr("餐桌:"));
        ui->lineEdit_service->setLabelText(tr("服务费:"));
        ui->lineEdit_lowcost->setLabelText(tr("低消:"));
        ui->lineEdit_vch_memo->setLabelText(tr("备注:"));
        ui->lineEdit_int_person->setLabelText(tr("人数:"));

        ui->lineEdit_change->setLabelText(tr("找零:"));
        ui->lineEdit_change->setBgColor(QColor("#ff650b"));
        ui->lineEdit_realreceive->setLabelText(tr("应收:"));
        ui->lineEdit_realreceive->setBgColor(QColor("#ff650b"));
        ui->lineEdit_moneypay->setLabelText(tr("现金:"));
        ui->lineEdit_moneypay->setBgColor(QColor("green"));
        ui->lineEdit_moneypay->enableSelectAll();


        connect(ui->lineEdit_moneypay,SIGNAL(valueChanged(double)),this,SLOT(updateChange()));
        connect(ui->lineEdit_realreceive, SIGNAL(valueChanged(double)), this,SLOT(updateDD(double)));
        connect(ui->pushButton,SIGNAL(clicked()),this,SLOT(updateAmount()));
        connect(ui->pushButton_2,SIGNAL(clicked()),this,SLOT(updateAmount()));
        connect(ui->pushButton_3,SIGNAL(clicked()),this,SLOT(updateAmount()));
        connect(ui->pushButton_6,SIGNAL(clicked()),this,SLOT(updateAmount()));
        connect(ui->pushButton_8,SIGNAL(clicked()),this,SLOT(updateAmount()));
        connect(ui->pushButton_5,SIGNAL(clicked()),this,SLOT(updateAmount()));
        connect(ui->pushButton_10,SIGNAL(clicked()),this,SLOT(updateAmount()));
        connect(ui->pushButton_12,SIGNAL(clicked()),this,SLOT(updateAmount()));
        connect(ui->pushButton_9,SIGNAL(clicked()),this,SLOT(updateAmount()));
        connect(ui->pushButton_14,SIGNAL(clicked()),this,SLOT(updateAmount()));
        connect(ui->pushButton_16,SIGNAL(clicked()),this,SLOT(updateAmount()));
        connect(ui->pushButton_4,SIGNAL(clicked()),this,SLOT(setAmount()));
        connect(ui->pushButton_7,SIGNAL(clicked()),this,SLOT(setAmount()));
        connect(ui->pushButton_clear,SIGNAL(clicked()),this,SLOT(amountClear()));
        connect(ui->radioButton_payinfo,SIGNAL(signalpressed(bool)),this, SLOT(tostackinfo0(bool)));
        connect(ui->radioButton_tableinfo,SIGNAL(signalpressed(bool)),this, SLOT(tostackinfo1(bool)));
        connect(ui->radioButton_payinfo, SIGNAL(timeout()), this, SLOT(toplayloop()));

        //
        if(pay_operate_mode & OPERAT_PAY) {
            connect(ui->pushButton_pay,SIGNAL(clicked()),this,SLOT(toScrpay()));
        }
        if(pay_operate_mode & OPERAT_QUITBILL) {
            connect(ui->pushButton_pay,SIGNAL(clicked()),this,SLOT(toExitbill()));
        }
        connect(ui->pushButton_pay, SIGNAL(released()),ui->lineEdit_moneypay, SLOT(setFocus()));
        if(pay_operate_mode & TYPE_RESTAURANT) {
            showWidget(ui->radioButton_tableinfo);
        }

        if(pay_operate_mode & TYPE_FAST) {
            hideWidget(ui->radioButton_tableinfo);
        }
        retranslateView();
    }
}


void w_scr_dish_pay_widget::retranslateView()
{
    if(lds::get_soft_curent_language_first() != "CN") {
        lds_pushbutton_wrap *p = paySectionValue("tg");
        if(p) p->setVisible(false);
        lds_query query;
        query.execSelect("select ch_paymodeno from cey_bt_paymode where vch_arg4 = 'T' ");
        while(query.next()) {
            p = paySectionValue(query.recordValue(0).toString());
            if(p) p->setVisible(false);
        }
    }
    //
    resetPaySection();
}

void w_scr_dish_pay_widget::initdataPayModel(const QString &ch_payno)
{
    lds_query query;
    query.execSelect(QString("SELECT ch_paymodeno,num_payamount,num_realamount,vch_voucherno,vch_memo from cey_u_checkout_detail where ch_payno = '%1' ").arg(ch_payno));
    while(query.next()) {
        int row = payModel()->rowCount();
        payModel()->insertRow(row);
        payModel()->model_data_set(row, "ch_paymodeno", query.recordValue("ch_paymodeno"));
        payModel()->model_data_set(row, "num_payamount", query.recordValue("num_payamount"));
        payModel()->model_data_set(row, "num_realamount", query.recordValue("num_realamount"));
        payModel()->model_data_set(row, "vch_voucherno", query.recordValue("vch_voucherno"));
        payModel()->model_data_set(row, "vch_memo", query.recordValue("vch_memo"));

        updatePaylistCashpay();
    }
}

int w_scr_dish_pay_widget::mread(bool b_always_disable_num_pay, const QString &title)
{
    if(ui->lineEdit_realreceive->value()  <= hasPaid())
    {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("已足额付款无需继续付款"));
        return QDialog::Rejected;
    }

    //!保存
    m_save();
    //!～保存
    w_scr_dish_member dialog(this);
    dialog.setWindowTitle(title);
    if(b_always_disable_num_pay)
        dialog.always_disable_num_pay();
    if(QDialog::Accepted == lds_roundeddialog_rect_align(&dialog).exec()) {
        if(dialog.cardinfo.ch_attribute == "2") {//充值卡
            QString ch_paymodeno;
            if(dialog.cardinfo.isyun()) {
                ch_paymodeno = "10";//云会员
            } else {
                ch_paymodeno = "02";
            }

            int row = payModel()->rowCount();
            payModel()->insertRow(row);
            payModel()->model_data_set(row, "ch_paymodeno", ch_paymodeno);
            payModel()->model_data_set(row, "num_payamount", dialog.ui->lineEdit_num_pay->value());
            payModel()->model_data_set(row, "num_realamount", dialog.ui->lineEdit_num_pay->value());
            payModel()->model_data_set(row, "vch_voucherno", dialog.ui->lineEdit_vch_memberno->text());
            payModel()->model_data_set(row, "vch_memo", dialog.ui->lineEdit_vch_memo->text());

            if(ch_paymodeno == "10") {
                payModel()->model_data_set(row, "ch_paymodeno", QColor("green"), Qt::DecorationRole);
            }
            updatePaylistCashpay();
        }
        //充值卡 优惠卡
        return QDialog::Accepted;
    }

    //!还原
    m_restore();
    //!～还原
    refreshControl();
    return QDialog::Rejected;
}

void w_scr_dish_pay_widget::first_go_in()
{
    pay_data_init();
    //
    if(pay_operate_mode & TYPE_FAST) {
        ui->lineEdit_ch_tableno->setText("Q000");
        ui->lineEdit_vch_memo->setText("");
        ui->lineEdit_int_person->setValue(1);
    }

    if(pay_operate_mode & TYPE_RESTAURANT) {
        ui->lineEdit_ch_tableno->setText(_tablemodel->get_cey_u_table_value(_tablemodel->cur_ch_billno(), "ch_tableno"));
        ui->lineEdit_vch_memo->setText(_tablemodel->get_cey_u_table_value(_tablemodel->cur_ch_billno(), "vch_memo"));
        ui->lineEdit_int_person->setText(_tablemodel->get_cey_u_table_value(_tablemodel->cur_ch_billno(), "int_person"));
    }

    origin_pay_low = 0;
    if(pay_operate_mode & TYPE_RESTAURANT) {
        //低消
        QString ch_typeno = _tablemodel->get_cey_bt_table_value(ui->lineEdit_ch_tableno->text(), "ch_typeno");
        double num_lowcost = _tablemodel->get_cey_bt_table_type_value(ch_typeno, "num_lowcost").toDouble();
        int ch_low_mode = _tablemodel->get_cey_bt_table_type_value(ch_typeno, "ch_low_mode").toInt();
        origin_pay_low = 0;
        if(1 == ch_low_mode) {//餐桌低消
            origin_pay_low = num_lowcost;
        }
        if(2 == ch_low_mode) {//人均低消
            origin_pay_low = num_lowcost * ui->lineEdit_int_person->value();
        }
    }
    //初始化
    refreshControl();
}

void w_scr_dish_pay_widget::pay_data_init()
{
    pay_all_discount = -1;
    pay_keep_data = _tablemodel->getKeepdata();

    vch_membernod = _tablemodel->vch_membernod;

    pay_finished = false;
}

void w_scr_dish_pay_widget::refreshLowServiceControl()
{
    ui->lineEdit_lowcost->setValue(qMax(double(0), origin_pay_low - pay_without_low_service() - ui->lineEdit_service->value_if_prefix()));
    double v = pay_without_low_service() +ui->lineEdit_lowcost->value_if_prefix() + ui->lineEdit_service->value_if_prefix();
    ui->lineEdit_realreceive->setValue(valuerountbysys(v));
    ui->lineEdit_blotout->setValue(v - ui->lineEdit_realreceive->value());
    ui->lineEdit_moneypay->setValue(label_paylist_remain());
}

void w_scr_dish_pay_widget::refreshQuitbillControl()
{
    for(int k = 0; k < pay_section.count(); k++) {
        lds_pushbutton_wrap *p = pay_section[k];
        QString key = p->getData(w_scr_dish_pay_module_select::modeule_key).toString();
        if(canQuitbill(key)){
            p->setEnabled(true);
        } else {
            p->setEnabled(false);
        }
    }
}


QString w_scr_dish_pay_widget::getMaxSerialnoAdd1(const QString &ch_billno)
{
    lds_query query;
    query.execSelect(QString(" select (select int_flowID from  cey_u_master_help where ch_billno = '%1') - min(int_flowID) as orderNo from cey_u_master_help where date = '%2' ; ")
                     .arg(ch_billno)
                     .arg(QDateTime::fromString(ch_billno.mid(1, 6), "yyMMdd").toString("20yy-MM-dd"))
                     );
    query.next();

    int serial = query.recordValue("orderNo").toInt() + 1;//排除serial为0的情况
    int begin=n_func::f_get_sysparm("serial_no_begin").toInt();
    int end=n_func::f_get_sysparm("serial_no_end").toInt();

    return QString::number(lds::min_max_value_add1_loop(begin, end, serial));
}

void w_scr_dish_pay_widget::refreshControl()
{
    if( (_tablemodel->vch_membernod.vch_memberno.length() == 0 && vch_membernod.vch_memberno.length() > 0 && vch_membernod.is_m_price())//点单没有读会员卡 && 但是付款时读卡
            || -1 != pay_all_discount
            ) {
        pay_keep_data = fexpandmain_model_sqltablemodel_data::get_keep_data_pay_by_isMPrice_intDiscount(
                    _tablemodel,
                    vch_membernod.is_m_price(),
                    pay_all_discount);
    }
    ui->lineEdit_consum->setValue(pay_keep_data.consum);
    ui->lineEdit_discount->setValue(pay_keep_data.discount);
    ui->lineEdit_present->setValue(pay_keep_data.present);
    ui->lineEdit_int_rate->setValue(pay_keep_data.num_rate);

    pay_keep_data.print();

    //v
    int tax_service_form_value = _tablemodel->get_cey_sys_parameter_value("tax_service_form_value", 0).toInt();
    switch(tax_service_form_value) {
    case 1://含税,不含服务费
        ui->lineEdit_int_rate->setEnabled(false);
        break;
    case 2://不含税,含服务费
        break;
    case 3://含税,含服务费
        ui->lineEdit_int_rate->setEnabled(false);
        break;
    case 0://不含税,不含服务费
    default:
        break;
    }
    ui->lineEdit_lowcost->setPrefix("");ui->lineEdit_lowcost->setValue(0);
    ui->lineEdit_service->setPrefix("");ui->lineEdit_service->setValue(0);

    //低消
    if(pay_operate_mode & TYPE_RESTAURANT) {
        QString ch_typeno = _tablemodel->get_cey_bt_table_value(ui->lineEdit_ch_tableno->text(), "ch_typeno");
        int ch_service_mode = _tablemodel->get_cey_bt_table_type_value(ch_typeno, "ch_service_mode").toInt();
        double num_ration = _tablemodel->get_cey_bt_table_type_value(ch_typeno, "num_ration").toInt();

        //服务费
        ui->lineEdit_service->setValue(0);
        if(4 == ch_service_mode) {//菜品消费服务率
            ui->lineEdit_service->setValue(pay_keep_data.num_service);
        }
        if(3 == ch_service_mode){//餐桌定额
            ui->lineEdit_service->setValue(num_ration);//在设置餐桌定额服务费时,必须比最大服务费小
        }
    }
    refreshLowServiceControl();

    //判断是否已经设置折扣卡
    ui->pushButton_member->set_rt_checked(vch_membernod);
    frontViewDialog_virtual::static_display_cash(ui->lineEdit_realreceive->value());
    if(_tablemodel->get_cey_sys_parameter_value("qt_def_money", "1").toString() == "0") {
        ui->lineEdit_moneypay->setValue(0);
    }
}

void w_scr_dish_pay_widget::updateMuyBillPay(member_up_yun &muy)
{
    muy.clearallList();
    muy.isrun = false;
    if(vch_membernod.ch_member_belong_state == "Y") {
        muy.isrun = true;
        bool ok;
        QString dateyyMMdd = n_func::f_get_sysdatetime().toString("yyMMdd");

        QString ch_billno = w_sys_manage_cloudsync::get_yun_member_maxorderno(this, "maxOrderNo", "K"+dateyyMMdd ,ok);
        muy.ch_billno = "K"+dateyyMMdd  + QString().sprintf("%05d",ch_billno.right(5).toInt()+1);

        QString ch_payno = w_sys_manage_cloudsync::get_yun_member_maxmasterno(this, "maxMasterNo",  "F" +dateyyMMdd  ,ok);
        muy.ch_payno = "F"+dateyyMMdd  + QString().sprintf("%05d",ch_payno.right(5).toInt()+1);
    }
}

void w_scr_dish_pay_widget::accept()
{
    emit accepted();
}

void w_scr_dish_pay_widget::reject()
{
    emit rejected();
}

//只在还原和保存会员信息是有用
static int pay_all_discount_save;
static bool member_not_read;//点菜时没有读卡
void w_scr_dish_pay_widget::m_save()
{
    pay_all_discount_save = pay_all_discount;
    member_not_read = vch_membernod.vch_memberno.isEmpty();
}

void w_scr_dish_pay_widget::m_restore()
{
    if(member_not_read) {//点菜时没有读卡
        pay_data_init();
        pay_all_discount = pay_all_discount_save;
    }
}

void w_scr_dish_pay_widget::tocancel()
{
    //这个地方。。。
    //    if(_tablemodel){
    //        _tablemodel = 0;
    //    }
    this->reject();
}

void w_scr_dish_pay_widget::todiscount()
{
    if(payModel()->rowCount() > 0)
    {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("已有付款记录,不可进行打折处理"));
        return;
    }

    //! 折扣获取
    lds_dialog_input_double inputdialog(this, true);
    inputdialog.ui->label->setText(tr("折扣"));
    inputdialog.ui->lineEdit->setRange(0, 100);
    inputdialog.ui->lineEdit->setDecimals(0);
    inputdialog.ui->lineEdit->setSuffix("%");
    inputdialog.ui->lineEdit->setValue(100);
    inputdialog.ui->label_2->setText(tr("金额"));
    inputdialog.ui->lineEdit_2->setDecimals(_tablemodel->get_cey_sys_parameter_value("bp_fmt_amount_dec", 2).toInt());
    inputdialog.ui->lineEdit_2->setRange(0, ui->lineEdit_realreceive->value());
    inputdialog.ui->lineEdit_2->setValue(ui->lineEdit_realreceive->value());
    inputdialog.connect_discount_amount_transfer(ui->lineEdit_realreceive->value());

    if(public_sql::discount_off_mode() == "1") {//折让
        inputdialog.ui->lineEdit->enableValueMaxMinus();
        inputdialog.ui->lineEdit_2->setValue(0);
    }
    if(QDialog::Accepted != lds_roundeddialog_rect_align(&inputdialog).exec()){
        return;
    }
    QString discount_desc;
    //enableValueMaxMinus， 则trueValue返回的就是折让的值
    if(false == _tablemodel->can_discount_limit(inputdialog.ui->lineEdit->trueValue(), tr("折扣"), discount_desc)) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, discount_desc);
    } else {
        //enableValueMaxMinus， 则value返回的折扣的值
        pay_all_discount = inputdialog.ui->lineEdit->value();
    }
    refreshControl();
}

void w_scr_dish_pay_widget::tomember()
{
    mread(false, _TEXT_SLOT(this));
}

void w_scr_dish_pay_widget::tobank()
{
    QString timer;
    QString fukuanpingzheng;
    double kafujine = 0;
    double label_paylist_remain_amt =  label_paylist_remain();
    if(label_paylist_remain_amt == 0)
    {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("已足额付款无需继续付款"));
        return;
    }
    bank_card dialog(QString::number(label_paylist_remain_amt), this);
    dialog.setWindowTitle(tr("银行卡"));
    lds_roundeddialog_rect_align(&dialog).exec();
    if(dialog.btn_t_f == true)
    {
        kafujine = dialog.shishoujine;
        fukuanpingzheng = dialog.pingzhenghao;
        timer = dialog.time;
    }
    else
    {
        return;
    }

    lds_query model;
    model.execSelect("select ch_paymodeno,ch_incomeflag from cey_bt_paymode where ch_paymodeno = '03' ");
    model.next();
    double t_shifuxianjin = 0;
    double t_fukuanjie = 0;
    if(label_paylist_remain_amt <= kafujine)//付清
    {
        t_shifuxianjin = label_paylist_remain_amt;
        t_fukuanjie = kafujine;
    }
    else//未付清
    {
        t_shifuxianjin = kafujine;
        t_fukuanjie = kafujine;
    }

    int row = payModel()->rowCount();
    payModel()->insertRow(row);
    payModel()->model_data_set(row, "ch_paymodeno", "03");
    payModel()->model_data_set(row, "num_payamount", t_fukuanjie);
    payModel()->model_data_set(row, "num_realamount", t_shifuxianjin);
    payModel()->model_data_set(row, "vch_voucherno", fukuanpingzheng);
    payModel()->model_data_set(row, "vch_memo", dialog.vch_memeo);

    updatePaylistCashpay();
}

void w_scr_dish_pay_widget::tovouchers()
{
    QString fukuanpingzheng;
    double kafujine = 0;
    double label_paylist_remain_amt =  label_paylist_remain();
    if(label_paylist_remain_amt == 0)
    {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("已足额付款无需继续付款"));
        return;
    }
    QString timer;
    bank_card dialog(QString::number(label_paylist_remain_amt), this);
    dialog.setWindowTitle(tr("代金券"));
    lds_roundeddialog_rect_align(&dialog).exec();

    if(dialog.btn_t_f == true)
    {
        kafujine = dialog.shishoujine;
        fukuanpingzheng = dialog.pingzhenghao;
        timer = dialog.time;
    }
    else
    {
        return;
    }

    lds_query model;
    model.execSelect("select ch_paymodeno,ch_incomeflag from cey_bt_paymode where ch_paymodeno = '04' ");
    model.next();
    double t_shifuxianjin = 0;
    double t_fukuanjie = 0;
    if(label_paylist_remain_amt <= kafujine)//付清
    {
        t_shifuxianjin = label_paylist_remain_amt;
        t_fukuanjie = kafujine;
    }
    else//未付清
    {
        t_shifuxianjin = kafujine;
        t_fukuanjie = kafujine;
    }


    int row = payModel()->rowCount();
    payModel()->insertRow(row);
    payModel()->model_data_set(row, "ch_paymodeno", "04");
    payModel()->model_data_set(row, "num_payamount", t_fukuanjie);
    payModel()->model_data_set(row, "num_realamount", t_shifuxianjin);
    payModel()->model_data_set(row, "vch_voucherno", fukuanpingzheng);
    payModel()->model_data_set(row, "vch_memo", dialog.vch_memeo);

    updatePaylistCashpay();
}

void w_scr_dish_pay_widget::torecharge()
{
    w_m_member_list::statictorecharge(this, 0, 0,"");
}

void w_scr_dish_pay_widget::amountClear()
{
    ui->lineEdit_moneypay->setValue(0);
}

void w_scr_dish_pay_widget::updateAmount()
{
    QPushButton *b = qobject_cast<QPushButton *>(this->sender());
    if(b){
        ui->lineEdit_moneypay->setValue(b->text());
        /*
        QKeyEvent keyPress(QEvent::KeyPress, b->text().data()->unicode(), Qt::NoModifier, b->text());
        ui->lineEdit_moneypay->setReadOnly(false);
        qApp->sendEvent(ui->lineEdit_moneypay, &keyPress);
        ui->lineEdit_moneypay->setReadOnly(true);*/
    }
}

void w_scr_dish_pay_widget::setAmount()
{
    QPushButton *b = qobject_cast<QPushButton *>(this->sender());
    if(b){
        ui->lineEdit_moneypay->clear();
        //        QKeyEvent keyPress(QEvent::KeyPress, b->text().data()->unicode(), Qt::NoModifier, b->text());

        ui->lineEdit_moneypay->setValue(b->text().toDouble());
        //        ui->lineEdit_moneypay->setReadOnly(false);
        //        qApp->sendEvent(ui->lineEdit_moneypay, &keyPress);
        //        ui->lineEdit_moneypay->setReadOnly(true);
    }
}

void w_scr_dish_pay_widget::updateChange()
{
    double change = ui->lineEdit_moneypay->value() - label_paylist_remain();
    ui->lineEdit_change->setValue(change);
}

void w_scr_dish_pay_widget::updateDD(double v)
{
    _tablemodel->emit_num_total_change_label_pay(pay_keep_data.dish_num(), v);
}

void w_scr_dish_pay_widget::toChooseSpecialDispose(const QString &arg1)
{
    lds_query query;
    if(arg1 == "SETTINGS"){
        toPaysettings();
        return;
    }
    double paylist_remain =  label_paylist_remain();
    if(paylist_remain == 0)
    {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("已足额付款无需继续付款"));
        return;
    }

    if(arg1 == "LOWCOST"
            ||arg1 == "SERVICE"
            //            ||arg1 == "ZZ"
            ||arg1 == "YY"
            ){
        if(payModel()->rowCount() > 0)
        {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("已有付款记录,不可进行特殊处理"));
            return;
        }
    }

    if(arg1 == "LOWCOST"){//免低消差
        toLowcostFree();
    } else if(arg1 == "SERVICE"){//免服务费
        toServiceFree();
    } else if(arg1 == "ZZ"){//免单
        toBillFree();
    }/* else if(arg1 == "ZZ"){//优惠
        tp_manual_blotout();
    } */else if(arg1 == "11"){
        toMemberHang();
        //        //挂账/还款
        //        switch(lds_messagebox::question(this, tr("挂账/还款"), tr("请选择")+tr("挂账/还款"), tr("挂账"), tr("还款"),tr("退出"))){
        //        case 0:
        //            to_member_hang();
        //            break;
        //        case 1:
        //            to_bill_pay();
        //            break;
        //        case 2:
        //            break;
        //        }
    } else if(arg1 == "AA"){
        tomobilepay();
    } else if(arg1 == "BB") {
        toquickpay();
    } else if(arg1 == "05"){
        towxpay();
    } else if(arg1 == "03"){
        tobank();

    } else if(arg1 == "04"){
        tovouchers();

    } else if(arg1 == "06"){
        toalipay();

    } else {//其他
        if(otherModePay(arg1)) return;
        if(towaimaipaymodeno(arg1)) return;
        query.execSelect(QString("select  ch_paymodeno from cey_bt_paymode where  ch_paymodeno = '%1' ")
                         .arg(arg1));
        if(!query.next()) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("未找到该支付方式"));
        }
    }
}

void w_scr_dish_pay_widget::toLowcostFree()
{
    //ue_free_lowcost (none) returns (none)
    //1.5
    /** -----------------------------权限-----------------------------begin*/
    QString ls_operid = public_sql::gs_operid;
    if(!n_func::f_check_bus_grant_else_authority(ls_operid, "0220", this, tr("低消差"))){
        return;
    }
    /** -----------------------------权限-----------------------------end*/
    //2
    if(ui->lineEdit_lowcost->prefix().isEmpty()) {
        if(0==lds_messagebox::question(this, MESSAGE_TITLE_VOID, tr("是否确定取消低消?"), tr("是") , tr("否"))){
            ui->lineEdit_lowcost->setPrefix(tr("免"));
        }
    } else {
        if(0==lds_messagebox::question(this, MESSAGE_TITLE_VOID, tr("是否确定恢复低消?"), tr("是") , tr("否"))){
            ui->lineEdit_lowcost->setPrefix("");
        }
    }
    refreshLowServiceControl();
}

void w_scr_dish_pay_widget::toPaysettings()
{
    w_scr_dish_pay_set dialog(this);
    dialog.setWindowTitle(tr("结算设置"));
    dialog.resize(lds::MAIN_WINDOW_SIZE);
    dialog.move(0,0);
    dialog.exec();
}

void w_scr_dish_pay_widget::toServiceFree()
{
    //免服务费，只有中餐有的
    //ue_free_service (none) returns (none)
    //1
    QString ls_ret;
    double ldec_service;
    double ldec_rate;
    double ldec_amount;
    if(payModel()->rowCount() > 0){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("已有付款，操作禁止"));
        return;
    }
    //1.5
    /** -----------------------------权限-----------------------------begin*/
    QString ls_operid = public_sql::gs_operid;
    if(!n_func::f_check_bus_grant_else_authority(ls_operid, "0219", this, tr("免服务费"))){
        return;
    }
    /** -----------------------------权限-----------------------------end*/
    //2
    if(n_func::f_get_sysparm("service_handwork") == "1"){//!手动处理服务费0
        w_scr_balance_service dialog(this);
        dialog.setWindowTitle(tr("手动处理服务费"));
        if(QDialog::Accepted != lds_roundeddialog_rect_align(&dialog).exec()) {
            return;
        }
        ls_ret = dialog.get_returnvalue();
        if(ls_ret.isEmpty()){
            return;
        }

        if(ls_ret.left(1) == "N"){//指定
            ldec_service = ls_ret.mid(2).toDouble();
        } else if(ls_ret.left(1) == "R"){//费率
            ldec_amount = pay_keep_data.num_service;
            ldec_rate = ls_ret.mid(2).toDouble() * 0.01;
            ldec_service = QString::number(ldec_rate * ldec_amount, 'f', 2).toDouble();
        } else {//系统
            ldec_service = pay_keep_data.num_service;
        }

        ui->lineEdit_service->setValue(ldec_service);
    } else {//!手动处理服务费1
        if(ui->lineEdit_service->value() <= 0){
            lds_messagebox::warning(this ,MESSAGE_TITLE_VOID ,tr("服务费等于零，不必免服务费"));
            return;
        }
        if(ui->lineEdit_service->prefix().isEmpty()) {
            if(0==lds_messagebox::question(this, MESSAGE_TITLE_VOID, tr("是否确定取消服务费?"), tr("是") , tr("否"))){
                ui->lineEdit_service->setPrefix(tr("免"));
            }
        } else {
            if(0==lds_messagebox::question(this, MESSAGE_TITLE_VOID, tr("是否确定恢复服务费?"), tr("是") , tr("否"))){
                ui->lineEdit_service->setPrefix("");
            }
        }
    }//!手动处理服务费2
    //3
    refreshLowServiceControl();
}

void w_scr_dish_pay_widget::toMemberHang()
{
    QString ch_paymodeno = "11";
    QString vch_memberno;
    w_scr_dish_m_hang_Dialog dialog(this);
    dialog.setWindowTitle(tr("选择会员"));
    if(QDialog::Accepted != lds_roundeddialog_rect_align(&dialog).exec()){
        return;
    }
    vch_memberno = dialog.m;
    //1
    //2
    free_view freeview(label_paylist_remain_str(), ch_paymodeno, this);
    freeview.setWindowTitle(tr("挂账")+"-"+vch_memberno);
    freeview.m=vch_memberno;
    lds_roundeddialog_rect_align(&freeview).exec();
    if(freeview.btn_t_f == true)
    {
        int row = payModel()->rowCount();
        payModel()->insertRow(row);
        payModel()->model_data_set(row, "ch_paymodeno", ch_paymodeno);
        payModel()->model_data_set(row, "num_payamount", freeview.line2_num);
        payModel()->model_data_set(row, "num_realamount", freeview.line2_num);
        payModel()->model_data_set(row, "vch_voucherno", vch_memberno);
        payModel()->model_data_set(row, "vch_memo", freeview.beizhu);
        /**/
        if(dialog.pno == "10")
            payModel()->model_data_set(row, "vch_memo", QColor("green"), Qt::DecorationRole);

        updatePaylistCashpay();
    }
}

void w_scr_dish_pay_widget::toBillPay()
{
    w_scr_dish_m_hang_pay_Dialog dialog(this);
    dialog.setWindowTitle(tr("挂账付款"));
    lds_roundeddialog_rect_align(&dialog).exec();
}

void w_scr_dish_pay_widget::toBillFree()
{
    toOtherPay("ZZ");
}

void w_scr_dish_pay_widget::toOtherPay(const QString &ch_paymodeno)
{
    otherModePay(ch_paymodeno);
}

void w_scr_dish_pay_widget::towxpay()
{
    lds_query query;
    QString timestamp_not_null= n_func::f_get_sysdatetime().toString("yyyy-MM-dd hh:mm:ss");
    QString out_trade_no;
    QString ch_incomeflag;
    QString ch_paymodeno = "05";
    double payamount;
    query.execSelect(QString("select ch_incomeflag, vch_arg6, vch_paymodename from cey_bt_paymode where ch_paymodeno = '%1' ")
                     .arg(ch_paymodeno));
    query.next();
    ch_incomeflag = query.recordValue("ch_incomeflag").toString();
    QString vch_arg6 = query.recordValue("vch_arg6").toString();
    QString vch_paymodename = query.recordValue("vch_paymodename").toString();
    if(vch_arg6 == "Y") {
        //记账
        otherModePay(ch_paymodeno, 0, vch_paymodename+"("+tr("仅记账")+")");
        return;
    }


    if(false == w_scr_dish_phone_pay::getInputPayFee(payamount, ch_paymodeno, label_paylist_remain(), this)) {
        return;
    }

    w_scr_dish_phone_pay dialog(ch_paymodeno, payamount, this);
    if(QDialog::Accepted != lds_roundeddialog_rect_align(&dialog).exec()) {
        return;
    }

    int row = payModel()->rowCount();
    payModel()->insertRow(row);
    payModel()->model_data_set(row, "ch_paymodeno", ch_paymodeno);
    payModel()->model_data_set(row, "num_payamount", payamount);
    payModel()->model_data_set(row, "num_realamount", payamount);
    payModel()->model_data_set(row, "vch_voucherno", dialog.ret_out_trade_no);
    payModel()->model_data_set(row, "vch_memo", OnlinePayTypeDesc(dialog.ret_pay_type));
    updatePaylistCashpay();
}

void w_scr_dish_pay_widget::toalipay()
{
    QString ch_paymodeno = "06";
    double payamount;
    //记账
    if(w_scr_dish_phone_pay::isPayRecord(ch_paymodeno)) {
        otherModePay(ch_paymodeno, 0, tr("支付宝支付")+"("+tr("仅记账")+")");
        return;
    }

    if(false == w_scr_dish_phone_pay::getInputPayFee(payamount, ch_paymodeno, label_paylist_remain(), this)) {
        return;
    }

    w_scr_dish_phone_pay dialog(ch_paymodeno, payamount, this);
    if(QDialog::Accepted != lds_roundeddialog_rect_align(&dialog).exec()) {
        return;
    }

    int row = payModel()->rowCount();
    payModel()->insertRow(row);
    payModel()->model_data_set(row, "ch_paymodeno", ch_paymodeno);
    payModel()->model_data_set(row, "num_payamount", payamount);
    payModel()->model_data_set(row, "num_realamount", payamount);
    payModel()->model_data_set(row, "vch_voucherno", dialog.ret_out_trade_no);
    payModel()->model_data_set(row, "vch_memo", OnlinePayTypeDesc(dialog.ret_pay_type));
    updatePaylistCashpay();
}

void w_scr_dish_pay_widget::tomobilepay()
{
    QString ch_paymodeno = "AA";
    double payamount;

    if(false == w_scr_dish_phone_pay::getInputPayFee(payamount, ch_paymodeno, label_paylist_remain(), this)) {
        return;
    }

    w_scr_dish_phone_pay dialog(ch_paymodeno, payamount, this);
    if(QDialog::Accepted != lds_roundeddialog_rect_align(&dialog).exec()) {
        return;
    }

    int row = payModel()->rowCount();
    payModel()->insertRow(row);
    payModel()->model_data_set(row, "ch_paymodeno", ch_paymodeno);
    payModel()->model_data_set(row, "num_payamount", payamount);
    payModel()->model_data_set(row, "num_realamount", payamount);
    payModel()->model_data_set(row, "vch_voucherno", dialog.ret_out_trade_no);
    payModel()->model_data_set(row, "vch_memo", OnlinePayTypeDesc(dialog.ret_pay_type));

    updatePaylistCashpay();
}

void w_scr_dish_pay_widget::toquickpay()
{
    //旧系统 且发布版
#if !defined(QT_NEW_SYSTEM) && defined(QT_NO_DEBUG)
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, fastfd_update::gs_soft_version + "\n"+tr("软件不支持该功能,请升级4.0或4.0以上版本"));
    return;
#endif
    lds_query query;
    QString ch_incomeflag;
    QString ch_paymodeno = "BB";
    double payamount;
    query.execSelect(QString("select ch_incomeflag, vch_arg6, vch_paymodename from cey_bt_paymode where ch_paymodeno = '%1' ")
                     .arg(ch_paymodeno));
    query.next();
    ch_incomeflag = query.recordValue("ch_incomeflag").toString();
    QString vch_arg6 = query.recordValue("vch_arg6").toString();
    QString vch_paymodename = query.recordValue("vch_paymodename").toString();
    if(vch_arg6 == "Y") {
        //记账
        otherModePay(ch_paymodeno, 0, vch_paymodename+"("+tr("仅记账")+")");
        return;
    }

    if(false == w_scr_dish_phone_pay::getInputPayFee(payamount, ch_paymodeno, label_paylist_remain(), this)) {
        return;
    }
    w_scr_dish_phone_pay dialog(ch_paymodeno, payamount, this);
    if(QDialog::Accepted != lds_roundeddialog_rect_align(&dialog).exec()) {
        return;
    }

    int row = payModel()->rowCount();
    payModel()->insertRow(row);
    payModel()->model_data_set(row, "ch_paymodeno", ch_paymodeno);
    payModel()->model_data_set(row, "num_payamount", payamount);
    payModel()->model_data_set(row, "num_realamount", payamount);
    payModel()->model_data_set(row, "vch_voucherno", dialog.ret_out_trade_no);
    payModel()->model_data_set(row, "vch_memo", OnlinePayTypeDesc(dialog.ret_pay_type));
    updatePaylistCashpay();
}

void w_scr_dish_pay_widget::tonuomi()
{
    towaimaipaymodeno("07");
}

void w_scr_dish_pay_widget::todianping()
{
    towaimaipaymodeno("08");
}

void w_scr_dish_pay_widget::tomeituan()
{
    towaimaipaymodeno("09");
}

bool w_scr_dish_pay_widget::towaimaipaymodeno(const QString &ch_paymodeno)
{
    QStandardItemModel model;
    model.setItem(0, 0, new QStandardItem);
    lds_query query;
    query.execSelect(QString("select ch_paymodeno, vch_paymodename,vch_arg2,vch_arg4 from cey_bt_paymode where vch_arg4 = 'T' and ch_paymodeno = '%1' ")
                     .arg(ch_paymodeno));
    if(query.next()) {

        model.setData(model.index(0, 0), query.recordValue("vch_paymodename"), Qt::UserRole);
        model.setData(model.index(0, 0), query.recordValue("ch_paymodeno"), Qt::UserRole + 1);
        model.setData(model.index(0, 0), query.recordValue("vch_arg2"), Qt::UserRole + 2);

        towaimaicheck(model.index(0,0));
        return true;
    }

    return false;
}

void w_scr_dish_pay_widget::towaimaicheck(const QModelIndex &index)
{
    //
    if(index.data(Qt::UserRole + 1).toString() == "-1"
            || index.data(Qt::UserRole + 1).toString() == "-2"
            || index.data(Qt::UserRole + 1).toString().isEmpty()
            ) {
        return;
    }
    //
    if(label_paylist_remain() <= 0)
    {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("已足额付款无需继续付款"));
        return;
    }

    QString vch_paymodename = index.data(Qt::UserRole).toString();
    QString ch_paymodeno = index.data(Qt::UserRole + 1).toString();
    QString vch_paymodeaddress = index.data(Qt::UserRole + 2).toString();

    lds_dialog_input inputdialog(this);
    inputdialog.setWindowTitle(vch_paymodename);
    inputdialog.showSecondLineEdt();
    inputdialog.ui->label->setText(tr("付款金额:"));
    inputdialog.ui->label_2->setText(tr("优惠金额:"));
    inputdialog.ui->lineEdit->setText(QString::number(label_paylist_remain()));
    inputdialog.ui->lineEdit_2->setText(QString::number(label_paylist_remain()));
    inputdialog.ui->lineEdit_2->total = label_paylist_remain();
    inputdialog.ui->lineEdit_2->setRemain_sub(inputdialog.ui->lineEdit->text());

    connect(inputdialog.ui->lineEdit,SIGNAL(textChanged(QString)), inputdialog.ui->lineEdit_2,SLOT(setRemain_sub(QString)));
    if(QDialog::Accepted != lds_roundeddialog_rect_align(&inputdialog).exec()){
        return;
    }
    if(inputdialog.ui->lineEdit->text().toDouble() > label_paylist_remain()){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("实付金额不能大于应收金额"));
        return;
    }
    if(inputdialog.ui->lineEdit_2->text().toDouble() < 0){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("优惠金额不能小于零"));
        return;
    }
    if((inputdialog.ui->lineEdit_2->text().toDouble() + inputdialog.ui->lineEdit->text().toDouble()) > label_paylist_remain()){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("优惠金额") +"+"+ tr("实付金额不能大于应收金额"));
    }
    //优惠金额将会做优惠处理
    bool  fav_as_manuldis = false;
    if(inputdialog.ui->lineEdit_2->text().toDouble() > 0){
        switch(lds_messagebox::question(this, MESSAGE_TITLE_VOID, tr("优惠金额将会做优惠处理"), tr("是") , tr("否"),  tr("取消"))){
        case 2://取消
            return;
        case 0:
            fav_as_manuldis = true;
            break;
        case 1:
            fav_as_manuldis = false;
            break;
        }
    }
    QProcess p;
    p.start(("/usr/desk/ie/QtWeb"" " + vch_paymodeaddress));
#ifndef QT_DEBUG
    if(p.waitForFinished(-1)) {
#endif
        if(0 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, tr("是否验证成功"), tr("是"), tr("否"))) {
            if(fav_as_manuldis){//1优惠
                double favorite = inputdialog.ui->lineEdit_2->text().toDouble();
                if(favorite > 0 && !otherModePay("ZZ", favorite)) return;
            }
            //2
            QString payamount = inputdialog.ui->lineEdit->text();
            int row = payModel()->rowCount();
            payModel()->insertRow(row);
            payModel()->model_data_set(row, "ch_paymodeno", ch_paymodeno);
            payModel()->model_data_set(row, "num_payamount", payamount);
            payModel()->model_data_set(row, "num_realamount", payamount);
            payModel()->model_data_set(row, "vch_voucherno", "");
            payModel()->model_data_set(row, "vch_memo", "");

            updatePaylistCashpay();
        }
#ifndef QT_DEBUG
    } else {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("浏览器打开超时"));
    }
#endif
}

void w_scr_dish_pay_widget::tospecialcheck(const QModelIndex &index)
{
    //
    if(index.data(Qt::UserRole + 1).toString() == "-1"
            || index.data(Qt::UserRole + 1).toString() == "-2"
            || index.data(Qt::UserRole + 1).toString().isEmpty()
            ) {
        return;
    }
    //usr usr_1 usr2 : name, 内部key， 外部key可重复
    toChooseSpecialDispose(index.data(Qt::UserRole + 2).toString());
}

void w_scr_dish_pay_widget::toMenuTg(QPushButton *btn_parent)
{
    QDialog *dialog = new QDialog;

    ftableview *tableview = new ftableview(dialog);
    ftableview_delegate *d = new ftableview_delegate(dialog);
    d->keepConnect(tableview);
    d->setMargin(1);
    d->indexisEmptystillDraw = true;

    ftableview_standmodel_sql_none *T_teampay_model = new ftableview_standmodel_sql_none(dialog);
    tableview->setModel(T_teampay_model);
    tableview->setProperty("outer_stylesheet", "tableView_type");
    tableview->setItemDelegate(d);
    tableview->setSelectionModel(new QItemSelectionModel(T_teampay_model));
    tableview->updateFixed_fillData_addAnimation(2, 3, 80, 80);
    connect(tableview,SIGNAL(clicked(QModelIndex)),this,SLOT(towaimaicheck(QModelIndex)));

    T_teampay_model->selectionmodel = tableview->selectionModel();
    QList<ftableview_standmodel_sql_none::noneData> listdata;
    lds_query query;
    QString ch_paymodeno;
    QString vch_paymodename;
    QString vch_arg2;
    QString vch_arg4;
    query.execSelect("select vch_paymodename, ch_paymodeno, vch_arg2,vch_arg4 from cey_bt_paymode where vch_arg4 = 'T' ; ");
    while(query.next()){
        ch_paymodeno = query.recordValue("ch_paymodeno").toString();
        vch_paymodename = query.recordValue("vch_paymodename").toString();
        vch_arg4 = query.recordValue("vch_arg4").toString();
        vch_arg2 = query.recordValue("vch_arg2").toString();
        ftableview_standmodel_sql_none::noneData d;
        d.setData(Qt::UserRole , vch_paymodename);
        d.setData(Qt::UserRole + 1, ch_paymodeno);
        d.setData(Qt::UserRole + 2, vch_arg2);
        listdata << d;
    }
    T_teampay_model->add_list_data(listdata);
    T_teampay_model->toinit();


    dialog->resize(tableview->size()+QSize(4, 4));
    dialog->setStyleSheet("QDialog{border:2px solid #ff650b;}");
    tableview->move(2, 2);

    //统一删除所有的指针
    lds_roundeddialog_rect_pop p(dialog, this);
    p.updatePos(btn_parent);
    p.exec();
}

void w_scr_dish_pay_widget::toMenuOther(QPushButton *btn_parent)
{
    QDialog *dialog = new QDialog;

    ftableview *tableview = new ftableview(dialog);
    ftableview_delegate *d = new ftableview_delegate(dialog);
    d->keepConnect(tableview);
    d->setMargin(1);
    d->indexisEmptystillDraw = true;

    ftableview_standmodel_sql_none *S_paymode_model = new ftableview_standmodel_sql_none(dialog);
    tableview->setModel(S_paymode_model);
    tableview->setProperty("outer_stylesheet", "tableView_type");
    tableview->setItemDelegate(d);
    tableview->setSelectionModel(new QItemSelectionModel(S_paymode_model));
    tableview->updateFixed_fillData_addAnimation(2, 3, 80, 80);
    connect(tableview,SIGNAL(clicked(QModelIndex)),this,SLOT(tospecialcheck(QModelIndex)));



    S_paymode_model->selectionmodel = tableview->selectionModel();
    QList<ftableview_standmodel_sql_none::noneData> listdata;
    lds_query query;
    QString ch_paymodeno;
    QString vch_paymodename;
    QString vch_arg2;
    QString vch_arg4;

    if(pay_operate_mode & TYPE_RESTAURANT) {
        listdata << ftableview_standmodel_sql_none::noneData(tr("免低消差"), "LOWCOST");
        listdata << ftableview_standmodel_sql_none::noneData((n_func::f_get_sysparm("service_handwork","0") == "1"?tr("服务费"):tr("免服务费")),
                                                             "SERVICE");
    }

    query.execSelect("select ch_paymodeno, vch_paymodename,vch_arg2,vch_arg4 from cey_bt_paymode where vch_arg4 = 'S' order by ch_paymodeno desc");
    while(query.next()){
        ch_paymodeno = query.recordValue("ch_paymodeno").toString();
        vch_paymodename = query.recordValue("vch_paymodename").toString();
        vch_arg4 = query.recordValue("vch_arg4").toString();
        vch_arg2 = query.recordValue("vch_arg2").toString();
        if(ch_paymodeno == "YY"){//自动抹零
            continue;
        }
        //若是退单模式,则没有会员挂账
        if(pay_operate_mode & OPERAT_QUITBILL){
            if(ch_paymodeno == "11"){//会员挂账
                continue;
            }
        }
        listdata << ftableview_standmodel_sql_none::noneData(vch_paymodename, ch_paymodeno);
    }
    listdata << ftableview_standmodel_sql_none::noneData(tr("结算设置"), "SETTINGS");
    listdata << ftableview_standmodel_sql_none::noneData(tr("优惠"), "ZZ");

    S_paymode_model->add_list_data(listdata);
    S_paymode_model->toinit();


    dialog->resize(tableview->size()+QSize(4, 4));
    dialog->setStyleSheet("QDialog{border:2px solid #ff650b;}");
    tableview->move(2, 2);

    //统一删除所有的指针
    lds_roundeddialog_rect_pop p(dialog, this);
    p.updatePos(btn_parent);
    p.exec();
}

void w_scr_dish_pay_widget::toplayloop()
{
    playloop();
    //    QTimer::singleShot(500, this ,SLOT(playloop()));
}

void w_scr_dish_pay_widget::playloop()
{
    w_scr_dish_pay_set_ogg dialog(this);
    dialog.setWindowTitle("支付语言轮询播放");
    lds_roundeddialog_rect_align(&dialog).exec();
}

void w_scr_dish_pay_widget::tostackinfo0(bool f)
{
    if(f == false) {
        return;
    }
    hideWidget(ui->frame_stack0);
    showWidget(ui->frame_stack1);
}

void w_scr_dish_pay_widget::tostackinfo1(bool f)
{
    if(f == false) {
        return;
    }
    hideWidget(ui->frame_stack1);
    showWidget(ui->frame_stack0);
}


void w_scr_dish_pay_widget::toScrpay()
{
    //云会员支付异常
    if(table_model != 0) {
        for(int row = 0; row < table_model->rowCount(); row ++) {
            if("10" == table_model->model_data(row, "ch_paymodeno").toString()) {//云会员
                if(false == vch_membernod.isyun()
                        || vch_membernod.vch_memberno.isEmpty()) {//折扣卡异常
                    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("云会员支付异常") + "\n" +
                                            "vch_membernod.isyun:" + QString::number(vch_membernod.isyun()) + "\n" +
                                            "vch_membernod.vch_memberno:" + vch_membernod.vch_memberno + "\n" +
                                            "vch_voucherno:" +  table_model->model_data(row, "vch_voucherno").toString()
                                            );
                    return;
                }
                break;
            }
        }
    }
    //

    //置灰
    pushbuttonAutoEnable b(ui->pushButton_pay);
    //已经付款
    if(pay_finished) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("本单已付款"));
        return;
    }
    //~已经付款
    if(ui->lineEdit_change->value() < 0){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("付款金额不足"));
        return ;
    }
    QString errstring;
    QString ch_payno;
    QDateTime dt = n_func::f_get_sysdatetime();
    //开始付款
    /**
*/
    ///手牌号
    QString serial_no;
    if(lds::sysconf->value("system_setting/fukuanshishurushoupai").toBool()){
        lds_dialog_input inputdialog(this);
        inputdialog.ui->label->setText(tr("手牌号:"));
        inputdialog.ui->lineEdit->setText(serial_no);
        inputdialog.setWindowTitle(tr("请输入手牌号"));
        if(QDialog::Accepted==lds_roundeddialog_rect_align(&inputdialog).exec()){
            serial_no=inputdialog.ui->lineEdit->text();
            if(serial_no.trimmed().isEmpty()) {
                lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("手牌号不能为空"));
                return;
            }
        } else {
            errstring = tr("操作取消");
            return;
        }
    }
    ///~手牌号

    //前台埋单打印账单
    if(lds::sysconf->value("system_setting/maidanzhangdan").toString() == "1") {
        if(lds::sysconf->value("system_setting/pay_select_language", false).toBool()) {
            w_scr_dish_pay_select_language dialog(this);
            dialog.setWindowTitle(tr("选择语言"));
            if(QDialog::Accepted != lds_roundeddialog_rect_align(&dialog).exec()) {
                errstring = tr("操作取消");
                return;
            }
        }
    }
    lds_query::tran_saction();
    toMPay(dt, errstring, ch_payno, serial_no);
    if(pay_finished){//付款成功
        /**
*/lds_query::com_mit();
        //云端上传流水
        toYunPaydataUploadWithTransaction(this, ch_payno, dt);
        //厨打
        public_sql::save_login->to_everytime_to_print_failed_kitchen_bill();
        //客显
        frontViewDialog_virtual::static_display_change(ui->lineEdit_change->value());
        //营业数据上传和打印
        dataupload(ch_payno, dt);
        //预打单状态更新
        if(pay_operate_mode & TYPE_RESTAURANT) {
            public_sql::preprinttablemap.insert(_tablemodel->cur_ch_billno(), false);
        }
        //同步数据
        if(lds::sysconf->value("w_sys_manage_cloudsync_with_time/cloud_yun_is_run", false).toBool()) {
            public_sql::save_login->keepdata_time_basedata_polling->emitsignalTrigger();
        }
        public_printby_ch_billno::payPrintModes pay_print_mode = public_printby_ch_billno::payNone;
        //开钱箱
        if(ui->lineEdit_moneypay->value() != 0//现金不为0
                && lds::sysconf->value("system_setting/qianxiang", true).toBool()){
            pay_print_mode |= public_printby_ch_billno::payDrawer;
        }
        //~开钱箱
        //选择打印结算单
        if(lds::sysconf->value("system_setting/maidanzhangdan").toString() == "1") {
            if( pay_operate_mode & OPERAT_QUITBILL) {
                pay_print_mode |= public_printby_ch_billno::payExit;
            }
            if( pay_operate_mode & OPERAT_PAY) {
                pay_print_mode |= public_printby_ch_billno::payNormal;
            }
        }
        // ~选择打印结算单
        //打印标签
        pay_print_mode |= public_printby_ch_billno::payLabel;
        //        printLabel(_tablemodel->cur_ch_billno());
        //~打印标签
        //        while(1 )
        {
            QString bp_languageKey = lds::sysconf->value("system_setting/maidan_language").toString();
            public_sql::save_login->qt_recipt_print_polling->emitsignalTrigger(_tablemodel->cur_ch_billno(), bp_languageKey, print_int_flowid_max, pay_print_mode);
        }

        this->accept();
        return;
    }
    /**
*/lds_query::roll_back();
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
}

void w_scr_dish_pay_widget::toExitbill()
{
    disconnect(ui->lineEdit_moneypay,SIGNAL(valueChanged(double)),this,SLOT(updateChange()));

    //
    negativeallControl();//正负翻转
    toScrpay();
    if(pay_finished) {
        goto end;
    }
    negativeallControl();//正负翻转
    //
end:
    connect(ui->lineEdit_moneypay,SIGNAL(valueChanged(double)),this,SLOT(updateChange()));
}

void w_scr_dish_pay_widget::toYunPaydataUploadWithTransaction(QWidget *parent, const QString &ch_payno, const QDateTime &pay_dt)
{
    if(!(lds::sysconf->value("w_sys_manage_cloudsync_with_time/cloud_yun_is_run", false).toBool())) return;

    //!等待
    lds_messagebox_loading_show loading(parent, "WAITING...", "WAITING...");
    loading.show();
    //!~等待
    //!
    lds_query::tran_saction();
    //同步本次会员流水
    QString errstring;
    if(!w_sys_manage_cloudsync_with_time_running::flow_pay_data_upload(ch_payno, pay_dt, n_func::f_get_sysparm("orgCode"), &errstring)) {
        lds_query::roll_back();
        lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, tr("流水上传失败")+":"+errstring);
        return ;
    }
    //～同步本次会员流水
    lds_query::com_mit();
}

void w_scr_dish_pay_widget::toModuleSelect()
{
    if(public_sql::gs_operid != "0000") {
        if(1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, tr("非管理员账号，不允许本操作"), tr("管理员授权"), tr("退出"))) {
            return;
        }
        QString password;
        if(!public_sql::get_password(this, tr("管理员密码"), password)) {
            return;
        }
        if(password != n_func::f_get_godpassword()
                && password != n_func::f_get_sysparm("system_pass", "0000")) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("密码错误"));
            return;
        }
    }
    lds_pushbutton_wrap *cur_p = 0;
    QStringList select_items;
    foreach(lds_pushbutton_wrap *p, pay_section) {
        QString k = p->getData(w_scr_dish_pay_module_select::modeule_key).toString();
        select_items << k;
        if(p->isDown()) {
            cur_p = p;
        }
    }
    w_scr_dish_pay_module_select dialog(
                select_items,
                pay_operate_mode,
                this);
    dialog.setWindowTitle(tr("结算模块自定义"));
    int ret = lds_roundeddialog_rect_align(&dialog).exec();
    if(QDialog::Accepted == ret) {
        QString pay_module1 = dialog.get_select_module_key();
        QString pay_module0 = cur_p->getData(w_scr_dish_pay_module_select::modeule_key).toString();

        //直接替换
        cur_p->setData(w_scr_dish_pay_module_select::modeule_key, pay_module1);
        cur_p->setText(w_scr_dish_pay_module_select::pay_module_list_value(pay_module1, pay_operate_mode));
        updatePayModuleStylesheet(cur_p);
        cur_p->enableLongPress(!cur_p->menu());

        //若是替换前AB，替换后BB，则第二B应该替换为A
        if(pay_module1.isEmpty()) return;
        foreach(lds_pushbutton_wrap *p, pay_section) {
            if(p != cur_p) {
                if(p->getData(w_scr_dish_pay_module_select::modeule_key).toString() == pay_module1) {
                    p->setData(w_scr_dish_pay_module_select::modeule_key, pay_module0);
                    p->setText(w_scr_dish_pay_module_select::pay_module_list_value(pay_module0, pay_operate_mode));
                    updatePayModuleStylesheet(p);
                    p->enableLongPress(!p->menu());
                }
            }
        }
    }
    if(w_scr_dish_pay_module_select::dialog_remove == ret) {
        cur_p->setData(w_scr_dish_pay_module_select::modeule_key, "");
        cur_p->setText("");
        updatePayModuleStylesheet(cur_p);
        cur_p->enableLongPress(!cur_p->menu());
    }

    if(w_scr_dish_pay_module_select::dialog_default == ret) {
        QFile::remove("w_scr_dish_pay_module_select_result_save_file");
        pay_section.clear();
        retranslateView();
    }

    cur_p->setDown(false);
    savePayModuleFile();

    if(pay_operate_mode & OPERAT_QUITBILL) {
        refreshQuitbillControl();
    }
}

void w_scr_dish_pay_widget::toPaySectionModule()
{
    lds_pushbutton_wrap *b = qobject_cast<lds_pushbutton_wrap *>(this->sender());
    QString k = b->getData(w_scr_dish_pay_module_select::modeule_key).toString();


    //    if("ydzf" == k) tomobilepay();
    //    if("zfb" == k) toalipay();
    //    if("yhk" == k)tobank();
    if("hyk" == k){tomember();return;}
    if("cz" == k){torecharge();return;}

    //    if("djq" == k)tovouchers();
    //    if("mt" == k)tomeituan();
    //    if("dzdp" == k) todianping();
    //    if("tg" == k) {}
    if("zk" == k){todiscount();return;}

    //    if("qt" == k){}
    if("qx" == k){tocancel();return;}
    if("jssz" == k){toPaysettings();return;}
    //    if("yh" == k)to_bill_free();
    //    if("wxzf" == k)towxpay();

    //    if("hygz" == k)to_member_hang();
    if("mdxc" == k){toLowcostFree();return;}
    if("mfwf" == k){toServiceFree();return;}
    if("tg" == k) {
        toMenuTg(b);
        b->setChecked(false);
        return;
    }
    if("qt" == k) {
        toMenuOther(b);
        b->setChecked(false);
        return;
    }

    toChooseSpecialDispose(k);
    return;
}

void w_scr_dish_pay_widget::toMPay(const QDateTime &dt, QString &errstring, QString &ch_payno, const QString &serial_no)
{
    ///预付款插入
    lds_query query;
    bool gonext = true;
    if(gonext) {
        //处理快餐退菜
        bool kuaican_quit = pay_operate_mode & OPERAT_QUITBILL
                //&& pay_operate_mode & TYPE_FAST
                ;
        gonext = _tablemodel->try_commit_data(errstring, kuaican_quit);
        if(gonext) {
            if(kuaican_quit) {
                gonext = query.execUpdate("cey_u_orderdish_print_tr",
                                          "vch_plan_k3o3_state", QString(6, w_bt_dish::k2o3_state2hex_char(w_bt_dish::KITCHEN_QUIT_KUAICAN)),
                                          qrtEqual("ch_billno", _tablemodel->cur_ch_billno()));
            }
        }
        if(gonext) {
            errstring = tr("付款失败");
            //1
            if(_tablemodel->vch_membernod.vch_memberno.isEmpty()//点菜没有读会员卡
                    && !vch_membernod.vch_memberno.isEmpty()) {// 付款时读会员卡
                if(vch_membernod.is_m_price()) {
                    //修正会员特价
                    query.execUpdate("cey_u_orderdish",
                                     QStringList() << "int_flowid",
                                     QString("select a.int_flowid, least(b.num_m_price, a.num_price) as `num_price`, '%1' as `vch_memberno`, '%2' as `ch_member_belong_state` from "
                                             " cey_u_orderdish a , cey_bt_dish b where "
                                             " a.ch_dishno = b.ch_dishno and a.ch_billno = '%3'  and "
                                             /// 会员价：～赠送、～子套菜,   ~ 特价
                                             " a.ch_presentflag <> 'Y'  and a.ch_suitflag <> 'Y'   and a.ch_specialflag <> 'Y'  ")
                                     .arg(vch_membernod.vch_memberno)
                                     .arg(vch_membernod.ch_member_belong_state)
                                     .arg(_tablemodel->cur_ch_billno()));
                }
            }
            //3 全场折扣
            if(gonext) {
                if(pay_all_discount != -1) {
                    gonext = query.execUpdate("cey_u_orderdish",
                                              QStringList() << "int_flowid",
                                              QString("select a.int_flowid , %1 as int_discount from cey_u_orderdish a , cey_bt_dish b where "
                                                      " a.ch_dishno = b.ch_dishno and a.ch_billno = '%2'   and "
                                                      /// 折扣：折扣菜品、~赠送，～子套菜,   ~ 特价
                                                      " b.ch_discount = 'Y'  and a.ch_presentflag <> 'Y'   and a.ch_suitflag <> 'Y'  and a.ch_specialflag <> 'Y' ")
                                              .arg(pay_all_discount)
                                              .arg(_tablemodel->cur_ch_billno()));
                    if(!gonext) {
                    }
                }
            }
            //4
        }
    }
    if(!gonext) {
        return;
    }
    ///～预付款插入
    ///
    ///～退款更新u_orderdish_log
    payAndYunCommit(dt, errstring, ch_payno, serial_no);
}

bool w_scr_dish_pay_widget::otherModePay(const QString &ch_paymodeno, double amount, const QString &wtitle)
{
    lds_query query;
    QString vch_paymodename;
    //1
    query.execSelect(QString("select ch_paymodeno, vch_paymodename,vch_arg2,vch_arg4 from cey_bt_paymode where  ch_paymodeno = '%1'  and (vch_arg4 = 'N' or vch_arg4 = 'S') ")
                     .arg(ch_paymodeno));
    if(query.next()){
        vch_paymodename = query.recordValue("vch_paymodename").toString();
        //2
        free_view freeview(label_paylist_remain_str(), ch_paymodeno, this);
        freeview.setWindowTitle(vch_paymodename);
        if(!wtitle.isEmpty()) {
            freeview.setWindowTitle(wtitle);
        }
        if(amount > 0){
            freeview.on_pushButton_ok_clicked();//确定
            freeview.line2_num = amount;
        } else {
            lds_roundeddialog_rect_align(&freeview).exec();
            if(freeview.btn_t_f == false) return false;
        }

        if(ch_paymodeno == "ZZ" && !checkNumFree(freeview.oper_id, freeview.line2_num)) {
            return false;
        }

        int row = payModel()->rowCount();
        payModel()->insertRow(row);
        payModel()->model_data_set(row, "ch_paymodeno", ch_paymodeno);
        payModel()->model_data_set(row, "num_payamount", freeview.line2_num);
        payModel()->model_data_set(row, "num_realamount", freeview.line2_num);
        payModel()->model_data_set(row, "vch_voucherno", "");
        payModel()->model_data_set(row, "vch_memo", freeview.beizhu);

        updatePaylistCashpay();
        return true;
    }

    return false;
}

void w_scr_dish_pay_widget:: payAndYunCommit(const QDateTime &dt, QString &errstring, QString &ch_payno, QString serial_no)
{
    lds_query query;
    QString pay_datetime_str;
    member_up_yun muy;
    //    QString ch_payno;
    //    QString vch_service_desc;
    pay_finished=false;
    pay_datetime_str = dt.toString("yyyy-MM-dd hh:mm:ss");
    const QString ch_billno = _tablemodel->cur_ch_billno();
    //ch_payno
    ch_payno = w_scr_dish_restaurant_div_pop::getMaxChpaynoAdd1_cey_u_checkout_master(dt);
    updateMuyBillPay(muy);

    //付款明细
    //////
    query.execSelect(QString("SELECT ch_billno from cey_u_table where ch_billno = '%1' ")
                     .arg(ch_billno));
    if(!query.next()) {
        if(!query.execInsert("cey_u_table",qrtVariantPairList()

                             << qrtVariantPair("ch_billno", ch_billno)
                             << qrtVariantPair("ch_tableno", "Q000")
                             << qrtVariantPair("int_person", 1)
                             << qrtVariantPair("ch_state", 1)
                             << qrtVariantPair("vch_operID", public_sql::gs_operid)

                             << qrtVariantPair("dt_operdate", dt)
                             << qrtVariantPair("ch_order_state", 0)
                             << qrtVariantPair("vch_waiter", "")
                             << qrtVariantPair("vch_memo", ui->lineEdit_vch_memo->text())
                             )) {
            errstring = (tr("数据库错误")+ QString::number(__LINE__));
            goto fastpayend;
        }
    }

    //快餐的需要insert
    query.execSelect(QString("SELECT ch_billno from cey_u_master where ch_billno = '%1' ")
                     .arg(ch_billno));
    if(!query.next()) {
        if(!w_scr_dish_restaurant_div_pop::cey_u_master_insert(ch_billno)) {
            errstring = (tr("数据库错误")+ QString::number(__LINE__));
            goto fastpayend;
        }
    }
    ///手牌号
    if(serial_no.isEmpty()) {
        serial_no = getMaxSerialnoAdd1(ch_billno);
    }
    if(!query.execUpdate("cey_u_master" ,"vch_memo", serial_no, qrtEqual("ch_billno", ch_billno))) {
        errstring = (tr("数据库错误")+ QString::number(__LINE__));
        return;
    }
    ///~手牌号
    //
    //    query.execSelect(QString("select  a.ch_service_mode,  a.int_rate, a.ch_serviceflag from cey_bt_table_type a, cey_bt_table b where a.ch_typeno = b.ch_typeno and b.ch_tableno = '%1'")
    //                     .arg(ui->lineEdit_ch_tableno->text()));
    //    query.next();
    //    vch_service_desc += query.recordValue("int_rate").toString()+"%";
    if(!query.execInsert("cey_u_checkout_master",qrtVariantPairList()
                         << qrtVariantPair("ch_billno", ch_billno)
                         << qrtVariantPair("ch_tableno", ui->lineEdit_ch_tableno->text())
                         << qrtVariantPair("num_cost", ui->lineEdit_consum->value())// - (ui->lineEdit_int_rate->isEnabled() ? 0 : ui->lineEdit_int_rate->value()) )
                         << qrtVariantPair("num_discount", ui->lineEdit_discount->value())
                         << qrtVariantPair("num_present", ui->lineEdit_present->value())

                         << qrtVariantPair("num_service", ui->lineEdit_service->value_if_prefix())
                         //                         << qrtVariantPair("vch_service_desc", vch_service_desc)
                         << qrtVariantPair("num_lowcost", ui->lineEdit_lowcost->value_if_prefix())
                         << qrtVariantPair("num_blotout", ui->lineEdit_blotout->value())
                         << qrtVariantPair("num_run", 0)

                         << qrtVariantPair("vch_run_operID", "")
                         << qrtVariantPair("ch_state", "Y")
                         << qrtVariantPair("vch_operID", public_sql::gs_operid)
                         << qrtVariantPair("dt_operdate", pay_datetime_str)
                         << qrtVariantPair("vch_operID2", "")

                         << qrtVariantPair("num_rate", ui->lineEdit_int_rate->value())
                         << qrtVariantPair("ch_payno", ch_payno)
                         << qrtVariantPair("vch_memberno", vch_membernod.vch_memberno)
                         << qrtVariantPair("ch_member_belong_state", vch_membernod.ch_member_belong_state)
                         << qrtVariantPair("ch_yun_payno", muy.ch_payno)

                         << qrtVariantPair("num_cash", ui->lineEdit_realreceive->value()))){
        errstring = (tr("数据库错误")+ QString::number(__LINE__));
        goto fastpayend;
    }
    //服务费
    if(ui->lineEdit_service->value() != 0) {
        if(!query.execInsert("cey_u_service_low",qrtVariantPairList()
                             << qrtVariantPair("ch_payno", ch_payno)
                             << qrtVariantPair("ch_type", 1)
                             << qrtVariantPair("num_amount", ui->lineEdit_service->value())
                             << qrtVariantPair("vch_operID", public_sql::gs_operid)
                             << qrtVariantPair("dt_operdate", pay_datetime_str)

                             << qrtVariantPair("ch_state", ui->lineEdit_service->prefix().length() == 0 ? 1 : 0) //为空表示正常
                             ,

                             &errstring)) {

            errstring = (tr("数据库错误")+ QString::number(__LINE__));
            goto fastpayend;
        }
    }
    //低消
    if(ui->lineEdit_lowcost->value() != 0) {
        if(!query.execInsert("cey_u_service_low",qrtVariantPairList()
                             << qrtVariantPair("ch_payno", ch_payno)
                             << qrtVariantPair("ch_type", 2)
                             << qrtVariantPair("num_amount", ui->lineEdit_lowcost->value())
                             << qrtVariantPair("vch_operID", public_sql::gs_operid)

                             << qrtVariantPair("dt_operdate", pay_datetime_str)
                             << qrtVariantPair("ch_state", ui->lineEdit_lowcost->prefix().length() == 0 ? 1 : 0) //为空表示正常
                             , &errstring)) {

            errstring = (tr("数据库错误")+ QString::number(__LINE__));
            goto fastpayend;
        }
    }

    //餐桌
    if(!query.execUpdate("cey_u_table","ch_payno", ch_payno, "ch_order_state", 4, qrtEqual("ch_billno",ch_billno))) {
        errstring = (tr("数据库错误")+ QString::number(__LINE__));
        goto fastpayend;
    }

    //原料
    if(!query.execUpdate("cey_u_orderdish","ch_payno", ch_payno, qrtEqual("ch_billno",ch_billno))) {

        errstring = (tr("数据库错误")+ QString::number(__LINE__) + query.recordError());
        goto fastpayend;
    }

    //
    if(!query.execUpdate("cey_u_orderdish_log",
                         QStringList() << "int_orderflow",
                         QString(" select a.int_orderflow , b.ch_payno from cey_u_orderdish_log a, cey_u_orderdish b where a.int_orderflow = b.int_flowID and b.ch_payno = '%1' ; ").arg(ch_payno)
                         )) {
        errstring = (tr("数据库错误")+ QString::number(__LINE__));
        goto fastpayend;
    }
    //==============================================================================
    //现金付款
    if(ui->lineEdit_moneypay->value() != 0)
    {
        double aa = ui->lineEdit_moneypay->value() - ui->lineEdit_change->value();
        if(!query.execInsert("cey_u_checkout_detail",qrtVariantPairList()

                             << qrtVariantPair("ch_payno", ch_payno)
                             << qrtVariantPair("ch_paymodeno", "00")
                             << qrtVariantPair("num_payamount", ui->lineEdit_moneypay->value())
                             << qrtVariantPair("num_change", ui->lineEdit_change->value())
                             << qrtVariantPair("num_realamount", aa)

                             << qrtVariantPair("num_face", 0)
                             << qrtVariantPair("vch_operID", public_sql::gs_operid)
                             << qrtVariantPair("dt_operdate", pay_datetime_str)
                             << qrtVariantPair("num_card_remain", 0)
                             << qrtVariantPair("vch_voucherno", "")

                             << qrtVariantPair("num_face_Num", 0))) {

            errstring = (tr("数据库错误")+ QString::number(__LINE__));
            goto fastpayend;
        }
    }

    //自动抹零
    if(ui->lineEdit_blotout->value() != 0)//存在自动抹零
    {
        if(!query.execInsert("cey_u_checkout_detail",qrtVariantPairList()

                             << qrtVariantPair("ch_payno", ch_payno)
                             << qrtVariantPair("ch_paymodeno", "YY")
                             << qrtVariantPair("num_payamount", ui->lineEdit_blotout->value())
                             << qrtVariantPair("num_change", 0)
                             << qrtVariantPair("num_realamount", ui->lineEdit_blotout->value())

                             << qrtVariantPair("num_face", 0)
                             << qrtVariantPair("vch_operID", public_sql::gs_operid)
                             << qrtVariantPair("dt_operdate", pay_datetime_str)
                             << qrtVariantPair("num_card_remain", 0)
                             << qrtVariantPair("vch_voucherno", "")

                             << qrtVariantPair("num_face_Num", 0))) {

            errstring = (tr("数据库错误")+ QString::number(__LINE__));
            goto fastpayend;
        }
    }

    ///付款方式------------begin
    for(int i = 0; i < payModel()->rowCount(); i++)
    {
        //会员卡付款不再计算积分
        QString paymodeno = payModel()->model_data(i, "ch_paymodeno").toString();
        bool ok;
        QString ch_member_belong_state;
        QString vch_voucherno = payModel()->model_data(i,"vch_voucherno").toString();
        ///云会员或本地会员付款---------begin
        if(paymodeno == "02"
                || paymodeno == "10") {//云会员卡付款

            QString num_amount;
            int li_backcash_rate = 0;
            double ldec_payamt;
            double ldec_remain_amt ;
            double ldec_backcash;
            double num_pay = payModel()->model_data(i,"num_realamount").toDouble();

            if(paymodeno == "10") {//云会员卡付款

                ch_member_belong_state = "Y";
            }
            //m_pay
            if(paymodeno == "10") {//云会员卡付款

                memberPay o;
                o.mpVchMemberNo = vch_voucherno;
                o.mpChPayNo = muy.ch_payno;
                o.mpChPayModeNo = "10";
                o.mpNumPay = num_pay;
                o.mpVchOperId = public_sql::gs_operid;
                o.mpDtOperDate = pay_datetime_str;
                muy.appitem(o);
            }
            if(paymodeno == "02"){ //本地会员付款{
                if(!query.execInsert("t_m_pay",qrtVariantPairList()

                                     << qrtVariantPair("vch_memberno", vch_voucherno)
                                     << qrtVariantPair("ch_payno", ch_payno)
                                     << qrtVariantPair("ch_paymodeno", paymodeno)
                                     << qrtVariantPair("num_pay", num_pay)
                                     << qrtVariantPair("vch_operID", public_sql::gs_operid)

                                     << qrtVariantPair("dt_operdate", pay_datetime_str)
                                     << qrtVariantPair("int_checkflow", 0)

                                     )) {

                    errstring = (tr("数据库错误")+ QString::number(__LINE__));
                    goto fastpayend;
                }
            }
            //m_deposit
            if(paymodeno == "10") {//云会员卡付款

                num_amount=w_sys_manage_cloudsync::get_yun_member_readcard(this, vch_voucherno, n_func::f_get_sysparm("cloud_user"),
                                                                           "num_amount", ok);
                if(!ok) goto fastpayend;
                li_backcash_rate=w_sys_manage_cloudsync::get_yun_member_readcard(this, vch_voucherno, n_func::f_get_sysparm("cloud_user"),
                                                                                 "int_backcash_rate", ok).toInt();
                if(!ok) goto fastpayend;
            }
            if(paymodeno == "02") {//本地会员付款

                query.execSelect(QString("select num_amount  from t_m_curamount where vch_memberno = '%1' ").arg(vch_voucherno));
                query.next();
                num_amount=query.recordValue("num_amount").toString();

                query.execSelect(QString("SELECT b.int_backcash_rate from t_m_member a , t_m_member_type b  where a.ch_typeno =b.ch_typeno and a.vch_memberno = '%1'; ")
                                 .arg(vch_voucherno));
                query.next();
                li_backcash_rate=query.recordValue("int_backcash_rate").toInt();
            }

            ldec_payamt = payModel()->model_data(i,"num_realamount").toDouble();
            ldec_remain_amt = num_amount.toDouble();
            ldec_remain_amt -= ldec_payamt;
            ldec_backcash = ldec_payamt * (li_backcash_rate * 0.01);
            if(ldec_backcash != 0){//付款返现 本地，并更新余额和积分
                ldec_remain_amt += ldec_backcash;
                if(paymodeno == "02"){//本地会员付款

                    if(!query.execInsert("t_m_deposit",qrtVariantPairList()

                                         << qrtVariantPair("vch_memberno", vch_voucherno)
                                         << qrtVariantPair("num_deposit", ldec_backcash)
                                         << qrtVariantPair("num_realamount", 0)
                                         << qrtVariantPair("ch_deposit_mode", 3)
                                         << qrtVariantPair("ch_pay_mode", 5)

                                         << qrtVariantPair("vch_operID", public_sql::gs_operid)
                                         << qrtVariantPair("dt_operdate", pay_datetime_str)
                                         << qrtVariantPair("vch_memo", "")
                                         << qrtVariantPair("remain_amt", ldec_remain_amt))) {

                        errstring = (tr("数据库错误")+ QString::number(__LINE__));
                        goto fastpayend;
                    }
                }
                if(paymodeno == "10"){ //云会员卡付款
                    memberDeposit o;

                    o.mdVchMemberNo=vch_voucherno;
                    o.mdNumDeposit=ldec_backcash;
                    o.mdNumRealaMount = 0;//新增的积分
                    o.mdChDepositMode="3";
                    o.mdChPayMode="5";

                    o.mdVchOperId=public_sql::gs_operid;
                    o.dtOperDate=pay_datetime_str;
                    o.mdVchMemo="";
                    o.mdRemainAmt=ldec_remain_amt;
                    o.comChPayNo=muy.ch_payno;

                    muy.appitem(o);
                }
            }
            //t_m_curamount
            if(paymodeno == "10"){ //云会员卡付款
                memberCuramount o;
                o.vchMemberno=vch_voucherno;
                o.numAmount=ldec_remain_amt;
                //                o.numPoint;//新增的积分
                o.vchOperId=public_sql::gs_operid;
                o.dtOperdate=pay_datetime_str;
                muy.appitem(o);
            }
            if(paymodeno == "02"){ //云会员卡付款
                if(!query.execInsertDuplicateUpdate("t_m_curamount",
                                                    "vch_memberno",
                                                    qrtVariantPairList()
                                                    << qrtVariantPair("vch_memberno", vch_voucherno)
                                                    << qrtVariantPair("num_amount", ldec_remain_amt)
                                                    << qrtVariantPair("vch_operID", public_sql::gs_operid)
                                                    << qrtVariantPair("dt_operdate", pay_datetime_str)
                                                    )) {
                    errstring = ( tr("数据库错误")+ QString::number(__LINE__));
                    goto fastpayend;
                }
            }
        }
        ///云会员或本地会员付款---------end

        //会员优惠
        if(paymodeno == "ZZ"){
            if(!query.execUpdate("cey_u_checkout_master", qrtVariantPairList()

                                 << qrtVariantPair("num_free", payModel()->model_data(i,"num_realamount")),
                                 QString(" ch_payno = '%1' ").arg(ch_payno))) {

                errstring = ( tr("数据库错误")+ QString::number(__LINE__));
                goto fastpayend;
            }
        }
        //会员挂账
        if(paymodeno == "11"){
            double num_hangamount_cur;
            //1
            if(vch_voucherno.isEmpty()){
                errstring = (tr("会员挂账")+tr("会员号不能为空"));
                goto fastpayend;
            }
            //2
            num_hangamount_cur = payModel()->model_data(i, "num_realamount").toDouble();

            //3 num_hangamount
            if(!query.execInsertDuplicateUpdate("t_m_curamount",
                                                "vch_memberno",
                                                qrtVariantPairList()
                                                << qrtVariantPair("vch_memberno", vch_voucherno)
                                                << qrtVariantPair("vch_operID", public_sql::gs_operid)
                                                << qrtVariantPair("dt_operdate", pay_datetime_str)
                                                << qrtVariantPair("num_hangamount", qrtFunc("ifnull(num_hangamount, 0) + " + QString::number(num_hangamount_cur)))
                                                )) {
                errstring = ( tr("数据库错误")+ QString::number(__LINE__));
                goto fastpayend;
            }

            //4
            if(!query.execInsert("t_m_hang_balance",qrtVariantPairList()

                                 << qrtVariantPair("ch_balanceno", ch_payno)
                                 << qrtVariantPair("vch_memberno", vch_voucherno)
                                 << qrtVariantPair("num_remain", payModel()->model_data(i, "num_payamount").toString())
                                 << qrtVariantPair("num_pay", payModel()->model_data(i, "num_payamount").toString())
                                 << qrtVariantPair("num_amount", payModel()->model_data(i, "num_realamount").toString())

                                 << qrtVariantPair("vch_operID", public_sql::gs_operid)
                                 << qrtVariantPair("dt_operdate", pay_datetime_str)
                                 << qrtVariantPair("vch_memo", tr("会员挂账付款"))
                                 << qrtVariantPair("ch_state", "N")
                                 << qrtVariantPair("ch_paymodeno", "02")

                                 << qrtVariantPair("ch_type", ""))){

                errstring = ( tr("数据库错误")+ QString::number(__LINE__));
                goto fastpayend;
            }
        }

        if(!query.execInsert("cey_u_checkout_detail",qrtVariantPairList()

                             << qrtVariantPair("ch_payno", ch_payno)
                             << qrtVariantPair("ch_paymodeno", paymodeno)
                             << qrtVariantPair("num_payamount", payModel()->model_data(i,"num_payamount").toString())
                             << qrtVariantPair("num_change", 0)
                             << qrtVariantPair("num_realamount", payModel()->model_data(i,"num_realamount").toString())

                             << qrtVariantPair("num_face", 0)
                             << qrtVariantPair("vch_operID", public_sql::gs_operid)
                             << qrtVariantPair("dt_operdate", dt)
                             << qrtVariantPair("num_card_remain", 0)
                             << qrtVariantPair("vch_voucherno", vch_voucherno)

                             << qrtVariantPair("num_face_Num", 0)
                             << qrtVariantPair("vch_memo", payModel()->model_data(i,"vch_memo").toString()))){

            errstring = tr("数据库错误")+ QString::number(__LINE__);
            goto fastpayend;
        }
    }


    ///若是使用了折扣卡，则会员积分归该折扣卡---------------------begin
    /// 本地不在保留云会员的积分信息，都有云端自己处理
    /// 付款会员的积分默认=0
    if(!vch_membernod.vch_memberno.isEmpty()) {
        double num_point;
        num_point = getMNumPoint(vch_membernod);
        //t_m_curamount
        if(vch_membernod.isyun()) {
            double num_amount;
            bool ok;
            num_amount=w_sys_manage_cloudsync::get_yun_member_readcard(this, vch_membernod.vch_memberno, n_func::f_get_sysparm("cloud_user"),
                                                                       "num_amount", ok).toDouble();if(!ok) goto fastpayend;
            {memberCuramount o;
                int k;
                o.vchMemberno=vch_membernod.vch_memberno;
                o.numAmount=num_amount;
                o.numPoint=num_point;//新增的积分
                o.vchOperId=public_sql::gs_operid;
                o.dtOperdate=pay_datetime_str;
                if((k = muy.finditem(o)) >= 0) {
                    muy.memberCuramountList[k].numPoint = o.numPoint;
                } else {
                    muy.appitem(o);
                }
            }
            {
                memberPointList o;
                o.mpChPayNo = muy.ch_payno;
                o.mpChType = "3";
                o.mpDtOperDate =pay_datetime_str;
                o.mpIntFlowId = 0;
                o.mpNumPoint = num_point;
                o.mpVchMemberNo = vch_membernod.vch_memberno;
                o.mpVchMemo = "";
                o.mpVchOperId = public_sql::gs_operid;
                o.orgCode = n_func::f_get_sysparm("orgCode");
                muy.appitem(o);
            }
        } else {
            if(!query.execInsertDuplicateUpdate("t_m_curamount",
                                                "vch_memberno",
                                                qrtVariantPairList()
                                                << qrtVariantPair("vch_memberno", vch_membernod.vch_memberno)
                                                << qrtVariantPair("vch_operID", public_sql::gs_operid)
                                                << qrtVariantPair("dt_operdate", pay_datetime_str)
                                                << qrtVariantPair("num_point", qrtFunc("ifnull(num_point, 0) + " + QString::number(num_point)))
                                                )) {
                errstring = ( tr("数据库错误")+ QString::number(__LINE__));
                goto fastpayend;
            }

            //t_m_point
            if(num_point != 0) {
                if(!query.execInsert("t_m_point",qrtVariantPairList()

                                     << qrtVariantPair("vch_memberno", vch_membernod.vch_memberno)
                                     << qrtVariantPair("ch_type", 3)
                                     << qrtVariantPair("ch_payno", ch_payno)
                                     << qrtVariantPair("num_point", num_point)
                                     << qrtVariantPair("vch_memo", "")

                                     << qrtVariantPair("vch_operID", public_sql::gs_operid)
                                     << qrtVariantPair("dt_operdate", pay_datetime_str))) {
                    errstring = ( tr("数据库错误")+ QString::number(__LINE__));
                    goto fastpayend;
                }
            }
        }
    }
    ///若是使用了折扣卡，则会员积分归该折扣卡---------------------end

    ///付款方式------------end
    ///
    if(muy.isrun){
        yunpayData d = w_sys_manage_cloudsync::upload_yun_pay_info(this, muy, ch_payno);
        if("isOk" == d.message) {
            lds_messagebox::question(this, MESSAGE_TITLE_VOID, tr("本地已经支付")+":" + ch_payno);
            goto fastpayend;
        }
        if("ok" != d.message) {
            lds_messagebox::question(this, MESSAGE_TITLE_VOID, tr("付款失败"));
            goto fastpayend;
        }
        //云会员的话，默认本次上传成功
        //        if(!w_sys_manage_cloudsync_with_time_running::q_u_put_by(ch_payno)) {
        //            goto fastpayend;
        //        }
    }
    pay_finished = true;
    goto fastpayend;
fastpayend:
    w_m_member_list::yun_member_close();
    return;
}

void w_scr_dish_pay_widget::dataupload(const QString &ch_payno, const QDateTime &dt)
{
    ///dataupload
    double dish_num = pay_keep_data.num;
    QString ch_billno = _tablemodel->cur_ch_billno();

    //tabwidget选中了科传接口
    //checkbox勾选了启用
    if(n_func::f_get_sysparm("njgj_use") == "1"){///采集数据
        //现金付款
        QList<w_scr_dish::TECH_TRANS_NJGJ_tendercode> tendercodes;
        if(ui->lineEdit_moneypay->value() != 0){
            tendercodes.append(w_scr_dish::TECH_TRANS_NJGJ_tendercode("CH", ui->lineEdit_moneypay->value()));
        }
        //银行卡
        double bank_pay = 0;
        for(int row=0,rowcount=payModel()->rowCount();row<rowcount;row++){
            if(payModel()->model_data(row, "ch_paymodeno").toString()=="03"){
                bank_pay += payModel()->model_data(row, "num_payamount").toDouble();
            }
        }
        if(bank_pay != 0) {
            tendercodes.append(w_scr_dish::TECH_TRANS_NJGJ_tendercode("CI", (bank_pay)));
        }

        double other_pay = ui->lineEdit_realreceive->value() - ui->lineEdit_moneypay->value() - bank_pay;

        //其他付款
        if(other_pay != 0) {
            tendercodes.append(w_scr_dish::TECH_TRANS_NJGJ_tendercode("OT",(other_pay)));
        }
        //上传
        w_scr_dish::TECH_TRANS_NJGJ_sql(
                    dt.toString("yyyyMMdd"),
                    dt.toString("hhmmss"),
                    "01",
                    ch_billno,
                    public_sql::gs_operid,
                    public_sql::gs_opername,
                    QString::number(dish_num),
                    ui->lineEdit_realreceive->value(),
                    tendercodes
                    );

    }///～采集数据
    //星摩尔
    if(n_func::f_get_sysparm("isoft_use") == "1") {
        business_data_upload_isoft::genrateBill(ch_payno);
        public_sql::save_login->isoft_upload_polling->emitsignalTrigger();
    }
    //~星摩尔

    //辽宁顶有
    if(n_func::f_get_sysparm("liaoning_inventory_keep") == "1") {
        w_scr_dish_liaoning_inventory_keep_thread_event::genrateBill(ch_payno);
        public_sql::save_login->liaoning_inventory_keep_polling->emitsignalTrigger();
    }
    //~辽宁顶有
}

void w_scr_dish_pay_widget::dataprintBill(const QString &languageKey)
{
    //根据单号
    if(lds::sysconf->value("system_setting/maidanzhangdan").toString() == "1"){
        public_sql::save_login->qt_recipt_print_polling->emitsignalTrigger(_tablemodel->cur_ch_billno(),
                                                                           languageKey,
                                                                           n_func::f_get_sysparm("bp_count", "1").toInt(),
                                                                           pay_operate_mode & OPERAT_QUITBILL);
    }
}


double w_scr_dish_pay_widget::getMNumPoint(const vch_membernoData &md)
{
    QString ch_typeno;
    QString ch_calc;
    QString ch_calc_type;
    int int_calc_uint;
    QString ch_area_flag;
    double ret_num_point = 0;
    double dec_membercost = 0;
    double dec_sub_income = 0;
    lds_query query;
    if(md.isyun()) {
        bool ok;
        ch_typeno=w_sys_manage_cloudsync::get_yun_member_readcard(this, md.vch_memberno, n_func::f_get_sysparm("cloud_user"),
                                                                  "ch_typeno", ok);if(!ok) goto end;
        ch_calc=w_sys_manage_cloudsync::get_yun_member_membertype(this, ch_typeno,
                                                                  "chCalc", ok);if(!ok) goto end;
        ch_calc_type=w_sys_manage_cloudsync::get_yun_member_membertype(this, ch_typeno,
                                                                       "chCalcType", ok);if(!ok) goto end;
        int_calc_uint=w_sys_manage_cloudsync::get_yun_member_membertype(this, ch_typeno,
                                                                        "intCalcUnit", ok).toInt();if(!ok) goto end;
        ch_area_flag=w_sys_manage_cloudsync::get_yun_member_membertype(this, ch_typeno,
                                                                       "chAreaFlag", ok);if(!ok) goto end;
    } else {
        query.execSelect(QString("select ch_typeno from t_m_member where vch_memberno = '%1' ").arg(md.vch_memberno));
        query.next();
        ch_typeno = query.recordValue(0).toString();//得到类型编号
        query.execSelect(QString("select ch_calc,ch_calc_type,int_calc_unit,ch_area_flag from t_m_member_type where ch_typeno = '%1' ").arg(ch_typeno));
        query.next();
        ch_calc=query.recordValue("ch_calc").toString();
        ch_calc_type=query.recordValue("ch_calc_type").toString();
        int_calc_uint=query.recordValue("int_calc_unit").toInt();
        ch_area_flag=query.recordValue("ch_area_flag").toString();
    }

    if(ch_calc == "Y"){
        switch(ch_area_flag.toInt()){
        case 0://消费额
            dec_membercost=ui->lineEdit_consum->value();
            break;
        case 2://非营业收入的积分
            for(int row = 0; row < payModel()->rowCount(); row++){
                if(_tablemodel->get_cey_bt_paymode_value(payModel()->model_data(row, "ch_paymodeno").toString(), "ch_incomeflag")!="Y"){
                    dec_sub_income += payModel()->model_data(row, "num_realamount").toDouble();
                }
            }
        case 1://折后额或卡付款
            dec_membercost = ui->lineEdit_realreceive->value();
            dec_membercost -= dec_sub_income;
            break;
        }
        if(int_calc_uint == 0){
            ret_num_point = 0;
        } else {
            if(ch_calc_type == "0"){//小数
                ret_num_point = dec_membercost/int_calc_uint;
            } else if(ch_calc_type == "1"){//取整
                ret_num_point = qRound(dec_membercost)/int_calc_uint;
            }
        }
    }

end:
    return ret_num_point;
}

void w_scr_dish_pay_widget::printLabel(const QString &ch_billno)
{
    //    if(pay_operate_mode & TYPE_FAST)
    {
        public_printby_ch_billno::print_label(ch_billno, print_int_flowid_max);
    }
}

void w_scr_dish_pay_widget::negativeallControl()
{
    //付款方式
    for(int row = 0, rowcount = payModel()->rowCount(); row < rowcount; row++){
        payModel()->model_data_set(row, "num_realamount", -1*payModel()->model_data(row, "num_realamount").toDouble());
        payModel()->model_data_set(row, "num_payamount", -1*payModel()->model_data(row, "num_payamount").toDouble());
    }
    //各种消费详细
    negativeallControl(ui->lineEdit_blotout);
    negativeallControl(ui->lineEdit_change);
    //    ui->lineEdit_ch_billno;
    //    ui->lineEdit_ch_tableno;

    negativeallControl(ui->lineEdit_consum);
    negativeallControl(ui->lineEdit_discount);
    //    ui->lineEdit_int_person;
    negativeallControl(ui->lineEdit_int_rate);
    negativeallControl(ui->lineEdit_lowcost);

    negativeallControl(ui->lineEdit_moneypay);
    negativeallControl(ui->lineEdit_present);
    negativeallControl(ui->lineEdit_realreceive);
    negativeallControl(ui->lineEdit_service);
    //    ui->lineEdit_vch_memo;

}
void w_scr_dish_pay_widget::negativeallControl(lds_label_doublespinbox *l)
{
    l->setValue(-1*l->value());
}

bool w_scr_dish_pay_widget::get_pay_finished()
{
    return pay_finished;
}

void w_scr_dish_pay_widget::updatePaylistCashpay()
{
    ui->lineEdit_moneypay->setValue(label_paylist_remain());
}

void w_scr_dish_pay_widget::keyPressEvent(QKeyEvent *e)
{
    const int kvalue = e->key();
    if( (kvalue >= Qt::Key_0 && kvalue <= Qt::Key_9)
            || kvalue == Qt::Key_Period) {
        ui->lineEdit_moneypay->setValue(e->text());

    } else  if(kvalue == Qt::Key_Delete
               || kvalue == Qt::Key_Backspace) {
        amountClear();
    } else if(kvalue == Qt::Key_F1) {
        public_short_cut_map dialog(this);
        dialog.showpay();
        dialog.exec();
    } else if(kvalue == Qt::Key_Slash) {
        todiscount();
    } else {
        toShortCutFunction(public_short_cut_map::get_desc_by_keyvalue(kvalue, public_short_cut_map::PAY_BEGIN, public_short_cut_map::PAY_END));
    }

    QWidget::keyPressEvent(e);
}

void w_scr_dish_pay_widget::toShortCutFunction(const QString &k)
{
    if(k == "WX") {
        tomobilepay();
    } else if(k == "ZFB") {
        toalipay();
    } else if(k == "YHK") {
        tobank();
    } else if(k == "HYK") {
        tomember();
    }

    else if(k == "CZ") {
        torecharge();
    } else if(k == "DJQ") {
        tovouchers();
    } else if(k == "MT") {
        tomeituan();
    } else if(k == "DZDP") {
        todianping();
    }

    else if(k == "QCZK") {
        todiscount();
    } else if(k == "YH") {
        toBillFree();
    } else if(k == "HYGZ") {
        toMemberHang();
    }
}

void w_scr_dish_pay_widget::resetPaySection()
{
    /////////////////////////////////////////////////////
    if(pay_section.isEmpty()) {
        pay_section.append(ui->pushButton_mobile_pay);//pay_section.last()->setData(w_scr_dish_pay_module_select::modeule_key, "AA");
        pay_section.append(ui->pushButton_alipay);//pay_section.last()->setData(w_scr_dish_pay_module_select::modeule_key, "06");
        pay_section.append(ui->pushButton_bank);//pay_section.last()->setData(w_scr_dish_pay_module_select::modeule_key, "03");

        pay_section.append(ui->pushButton_member);//pay_section.last()->setData(w_scr_dish_pay_module_select::modeule_key, "hyk");
        pay_section.append(ui->pushButton_memberrecharge);//pay_section.last()->setData(w_scr_dish_pay_module_select::modeule_key, "cz");
        pay_section.append(ui->pushButton_vouchers);//pay_section.last()->setData(w_scr_dish_pay_module_select::modeule_key, "04");

        pay_section.append(ui->pushButton_meituan);//pay_section.last()->setData(w_scr_dish_pay_module_select::modeule_key, "09");
        pay_section.append(ui->pushButton_youhui);//pay_section.last()->setData(w_scr_dish_pay_module_select::modeule_key, "ZZ");
        pay_section.append(ui->pushButton_waimaiteam);//pay_section.last()->setData(w_scr_dish_pay_module_select::modeule_key, "tg");

        pay_section.append(ui->pushButton_discount);//pay_section.last()->setData(w_scr_dish_pay_module_select::modeule_key, "zk");
        pay_section.append(ui->pushButton_other);//pay_section.last()->setData(w_scr_dish_pay_module_select::modeule_key, "qt");
        pay_section.append(ui->pushButton_cancel);//pay_section.last()->setData(w_scr_dish_pay_module_select::modeule_key, "qx");

        for(int k = 0; k < pay_section.count(); k++) {
            connect(pay_section[k], SIGNAL(timeout()),this,SLOT(toModuleSelect()));
            connect(pay_section[k], SIGNAL(clicked()),this,SLOT(toPaySectionModule()));
            pay_section[k]->enableLongPress(true);
        }
    }
    QStringList pay_section_record = lds::sysconf->value("w_scr_dish_pay_widget/pay_section", QStringList() << "AA"<< "06"<< "03"<< "hyk"<< "cz"<< "04"<< "09"<< "ZZ"<< "tg"<< "zk"<< "qt"<< "qx").toStringList();
    for(int k = 0; k < pay_section_record.count(); k++) {
        QString p = pay_section_record[k];
        pay_section[k]->setData(w_scr_dish_pay_module_select::modeule_key, p);
        pay_section[k]->setText(w_scr_dish_pay_module_select::pay_module_list_value(p, pay_operate_mode));
        updatePayModuleStylesheet(pay_section[k]);
    }
    //    }
}

lds_pushbutton_wrap *w_scr_dish_pay_widget::paySectionValue(const QString &key)
{
    for(int k = 0; k < pay_section.count(); k++) {
        lds_pushbutton_wrap *p = pay_section[k];
        if(p->getData(w_scr_dish_pay_module_select::modeule_key).toString() == key) {
            return p;
        }
    }
    return 0;
}

void w_scr_dish_pay_widget::savePayModuleFile()
{
    QStringList list;
    foreach(lds_pushbutton_wrap *p, pay_section) {
        QString k = p->getData(w_scr_dish_pay_module_select::modeule_key).toString();
        list << k;
    }
    lds::sysconf->setValue("w_scr_dish_pay_widget/pay_section", list);
}

void w_scr_dish_pay_widget::updatePayModuleStylesheet(lds_pushbutton_wrap *b)
{
    QString pay_module1 = b->getData(w_scr_dish_pay_module_select::modeule_key).toString();
    if("tg" == pay_module1
            || "qt" == pay_module1) {
        b->setProperty("outer_stylesheet", "pushbutton_bottom_menu");
        b->setCheckable(true);
    } else {
        b->setProperty("outer_stylesheet", "pushbutton_bottom");
        b->setCheckable(false);
    }
    //    b->setStyleSheet(b->styleSheet());
    b->style()->polish(b);
}

bool w_scr_dish_pay_widget::checkNumFree(const QString &id, double num_free)
{
    lds_query query;
    QString errstring;
    if(id == "0000") return true;
    query.execSelect(QString("select num_free from cey_sys_operator where vch_operID = '%1' ").arg(id));
    query.next();
    double num_free_min = query.recordValue("num_free").toDouble();
    if(num_free <= num_free_min) return true;

    switch(lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("优惠金额")+QString("(%1)").arg(num_free)+("超出了优惠限额")+QString("(%1)").arg(num_free_min), tr("指定新的操作员"), tr("取消"))){
    case 0:
    {
        QString newid;
        if(getNewOperid(errstring, newid)) {
            if(newid == "0000") return true;
            query.execSelect(QString("select num_free from cey_sys_operator where vch_operID = '%1' ").arg(newid));
            query.next();
            num_free_min = query.recordValue("num_free").toDouble();
            if(num_free <= num_free_min) {
                return true;
            }
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("优惠金额")+QString("(%1)").arg(num_free)+("超出了优惠限额")+QString("(%1)").arg(num_free_min));
        }
        if(!errstring.isEmpty()) {
            lds_messagebox::warning(this , MESSAGE_TITLE_VOID, errstring);
        }
    }
        break;
    case 1:
        break;
    }
    return false;
}

bool w_scr_dish_pay_widget::getNewOperid(QString &errstring, QString &id)
{
    //匹配登录账号失败后，需要输入人工密码
    lds_dialog_input inputdialog(this);
    inputdialog.showSecondLineEdt();
    inputdialog.ui->label->setText(tr("操作员代码:"));
    inputdialog.ui->label_2->setText(tr("操作密码:"));
    inputdialog.ui->lineEdit_2->setEchoMode(QLineEdit::Password);
#ifdef QT_DEBUG
    inputdialog.ui->lineEdit->setText(0000);
    inputdialog.ui->lineEdit_2->setText(n_func::f_get_godpassword());
#endif
    inputdialog.setWindowTitle(tr("录入新的操作员"));
    if(QDialog::Accepted ==lds_roundeddialog_rect_align(&inputdialog).exec()) {
        QString  ls_operid_new = inputdialog.ui->lineEdit->text();
        QString  ls_operpassword = inputdialog.ui->lineEdit_2->text();
        errstring = n_func::f_confirm_user_pwd(ls_operid_new, ls_operpassword);
        if(errstring.isEmpty()) {
            id = ls_operid_new;
            errstring = "";
            return true;
        }
    }
    return false;
}

lds_model_sqltablemodel *w_scr_dish_pay_widget::payModel()
{
    if(table_model == 0) {
        QList<int> show_colums;
        table_model = new lds_model_sqltablemodel(this);
        table_model->removeRows(0, table_model->rowCount());
        table_model->setTable("cey_u_checkout_detail");
        table_model->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
        table_model->save_set_header("ch_paymodeno", tr("付款类型"));show_colums << table_model->fieldIndex("ch_paymodeno");
        //        table_model->save_set_header("num_payamount", tr("付款金额"));show_colums << table_model->fieldIndex("num_payamount");
        table_model->save_set_header("num_realamount", tr("实付金额"));show_colums << table_model->fieldIndex("num_realamount");
        table_model->save_set_header("vch_voucherno", tr("凭证号"));show_colums << table_model->fieldIndex("vch_voucherno");
        //    table_model->save_set_header("vch_memo", tr("备注"));

        ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
        ui->tableView->setEditTriggers(QTableView::NoEditTriggers);
        ui->tableView->setModel(table_model);
        ui->tableView->justShowColumns(show_colums);
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
        ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
#else
        ui->tableView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
#endif
        ui->tableView->setItemDelegateForColumn(table_model->fieldIndex("ch_paymodeno"), new lds_model_sqltablemodel_delegate_com(
                                                    this, table_model, "cey_bt_paymode", "ch_paymodeno", "vch_paymodename"));
    }
    return table_model;
}

double w_scr_dish_pay_widget::pay_without_low_service()
{
    int tax_service_form_value = n_func::f_get_sysparm("tax_service_form_value",0).toInt();
    switch(tax_service_form_value) {
    case 1://含税,不含服务费
        return ui->lineEdit_consum->value() - ui->lineEdit_present->value()  - ui->lineEdit_discount->value();
    case 2://不含税,含服务费
        return ui->lineEdit_consum->value() - ui->lineEdit_present->value()  - ui->lineEdit_discount->value() + ui->lineEdit_int_rate->value();
    case 3://含税,含服务费
        return ui->lineEdit_consum->value() - ui->lineEdit_present->value()  - ui->lineEdit_discount->value();
    case 0://不含税,不含服务费
    default:
        return ui->lineEdit_consum->value() - ui->lineEdit_present->value()  - ui->lineEdit_discount->value() + ui->lineEdit_int_rate->value();
    }

    return 0;
}

double w_scr_dish_pay_widget::hasPaid()
{
    double paid = 0;
    if(0 != table_model) {
        for(int row = 0, rowcount = payModel()->rowCount();row < rowcount; row ++) {
            paid+=payModel()->model_data(row, "num_realamount").toDouble();
        }
    }
    return paid;
}

QString w_scr_dish_pay_widget::label_paylist_remain_str()
{
    return QString::number(label_paylist_remain());
}

double w_scr_dish_pay_widget::label_paylist_remain()
{
    return ui->lineEdit_realreceive->value() - hasPaid();
}

bool w_scr_dish_pay_widget::canQuitbill(QString key)
{
    lds_query query;
    QStringList paymode_S;
    //自定义的付款方式
    query.execSelect("select ch_paymodeno from cey_bt_paymode where vch_arg4 = 'S' ");
    while(query.next()) {
        paymode_S.append( query.recordValue(0).toString() );
    }

    //
    if(     "hyk" == key//会员卡
            ||"cz" == key//充值
            ||"zk" == key//折扣
            ||"jssz" == key//结算设置
            ||"mdxc" == key//免低消差
            ||"mfwf" == key//免服务费
            ||"qx" == key//取消
            ||"qt" == key//其他

            ||"00" == key//现金
            ||"02" == key//会员卡
            ||"10" == key//充值卡(云)
            ||"04" == key//代金券
            ||"03" == key//银行卡
            ||"ZZ" == key//银行卡
            ||"YY" == key//抹零
            ||paymode_S.contains(key)
            ){
        return true;
    } else {
        return false;
    }
}
