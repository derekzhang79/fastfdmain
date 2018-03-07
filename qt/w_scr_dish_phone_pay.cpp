#include "w_scr_dish_phone_pay.h"
#include "ui_w_scr_dish_phone_pay.h"
#include "frontviewdialog_virtual.h"
#include "lds_dialog_input_double.h"
#include "ui_lds_dialog_input_dialog_double.h"
#include "lds_roundeddialog_rect_align.h"
#include "lds_messagebox.h"
#include "quick_pay_help.h"
#include "mobile_pay_help.h"
#include "lds_query.h"
#include "public_sql.h"
#include "n_func.h"
#include "lds.h"
#include <QMovie>
#include "backheader.h"
#include "lds_ogg_play.h"
#include "public_logindialog.h"

w_scr_dish_phone_pay::w_scr_dish_phone_pay(const QString &ch_paymodeno, double fee, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_scr_dish_phone_pay)
{
    ui->setupUi(this);
    this->ch_paymodeno = ch_paymodeno;
    this->fee = fee;
    timer = new QTimer(this);
    ret_pay_type = OnlinePayTypeNone;
#ifdef QT_DEBUG
    this->fee = 0.01;
#endif

    ui->label_alipay->setMovie(new QMovie(":/image/loading.gif", "", this));
    ui->label_wechat->setMovie(new QMovie(":/image/loading.gif", "", this));
    if("05" == ch_paymodeno) {//微信
        ui->label_alipay->hide();
        ui->label_wechat_desc->hide();
        ui->label_alipay_desc->hide();
        ui->lineEdit_payno->hide();
        ui->label_payno->hide();
    }
    if("06" == ch_paymodeno) {//支付宝
        ui->label_wechat->hide();
        ui->label_alipay_desc->hide();
        ui->label_wechat_desc->hide();
    }
    //    if("BB" == ch_paymodeno) {//快捷支付可以和支付宝类型的方法
    //        ui->label_wechat->hide();
    //        ui->label_wechat_desc->hide();
    //        ui->label_alipay_desc->setText(tr("微信") + "、" + ("支付宝"));
    //    }

    QDateTime cur_dt = n_func::f_get_sysdatetime();
    out_trade_no_alipay
            = lds::terminalCode  + lds::dt_str(cur_dt.addSecs(0), "yyyyMMddhhmmss");
    out_trade_no_wechat
            = lds::terminalCode  + lds::dt_str(cur_dt.addSecs(1), "yyyyMMddhhmmss");
    out_trade_no_barcode
            = lds::terminalCode  + lds::dt_str(cur_dt.addSecs(2), "yyyyMMddhhmmss");
    //
    //connect
    connect(ui->pushButton_exit, SIGNAL(clicked()), this, SLOT(toDDexit()));
    connect(timer, SIGNAL(timeout()), this, SLOT(topolling()));
    connect(ui->lineEdit_payno, SIGNAL(returnPressed()), this, SLOT(tocodePay()));
    connect(ui->lineEdit_payno, SIGNAL(textChanged(QString)), timer, SLOT(start()));//时间器会立刻stop and start
    connect(ui->pushButton_print, SIGNAL(clicked()), this, SLOT(toprint()));
    connect(ui->checkBox, SIGNAL(clicked(bool)), this, SLOT(to_save_audio(bool)));

    ui->checkBox->setChecked(lds::sysconf->value("w_scr_dish_phone_pay_" + ch_paymodeno, false).toBool());

    setWindowTitle(title(ch_paymodeno) + ":" + QString::number(fee, 'f', 2));
    timer->setInterval(2000);
    QTimer::singleShot(10, this, SLOT(makeUrl()));
}

w_scr_dish_phone_pay::~w_scr_dish_phone_pay()
{
    delete ui;
}

bool w_scr_dish_phone_pay::getInputPayFee(double &ret_fee, const QString &ch_paymodeno, double num_pay, QWidget *parent)
{
    lds_dialog_input_double inputdialog(parent);
    inputdialog.ui->label->setText(tr("支付金额:"));
    inputdialog.setWindowTitle(title(ch_paymodeno));
    inputdialog.ui->lineEdit->setValue(num_pay);
    if(QDialog::Accepted == lds_roundeddialog_rect_align(&inputdialog).exec()) {
        if(inputdialog.ui->lineEdit->value() > num_pay) {
            lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, tr("输入金额不能大于应收金额"));
            return false;
        }
        ret_fee = inputdialog.ui->lineEdit->value();
        return true;
    }
    return false;
}

QString w_scr_dish_phone_pay::title(const QString &ch_paymodeno)
{
    if(ch_paymodeno == "BB"){//快捷支付
        return tr("快捷支付");
    }
    if(ch_paymodeno == "AA") {//移动支付
        return tr("移动支付");
    }
    if(ch_paymodeno == "05") {//微信支付
        return tr("微信支付");
    }
    if(ch_paymodeno == "06") {//支付宝支付
        return tr("支付宝支付");
    }
    return"";
}

bool w_scr_dish_phone_pay::isPayRecord(const QString &ch_paymodeno)
{
    lds_query query;
    query.execSelect(QString("select  vch_arg6  from cey_bt_paymode where ch_paymodeno = '%1'  ")
                     .arg(ch_paymodeno));
    query.next();
    return "Y" == query.recordValue("vch_arg6").toString();
}

void w_scr_dish_phone_pay::makeUrl()
{
    ui->label_alipay->movie()->start();
    ui->label_wechat->movie()->start();

    QByteArray url_wechat;
    QByteArray url_alipay;
    lds_query query_has_exe_next;
    query_has_exe_next.execSelect(QString("select  *  from cey_bt_paymode where ch_paymodeno = '%1' ")
                                  .arg(ch_paymodeno));
    if(false == query_has_exe_next.next()) {
        return;
    }
    //
    if(ch_paymodeno == "BB") {//快捷支付
        QString body = title(ch_paymodeno) +"," + tr("机器号") + lds::terminalCode;
        QString terminal_code = lds::terminalCode;
#ifdef QT_DEBUG
        terminal_code = "AA81013090150008B";
#endif
        QString device_info = terminal_code;
        double total_amount = fee;
        //支付宝
        QString errstring;
        if(false == quick_pay_help::getPayUrl(
                    url_alipay,
                    terminal_code,
                    total_amount,
                    "1",//支付宝
                    out_trade_no_alipay,
                    body,
                    device_info,
                    errstring)) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
        }
        ui->label_alipay->setPixmap(testwx_help::get_rbcode(url_alipay.data(), ui->label_alipay->width()));
        //保留
        d_quick_alipay.walletType = "1";
        d_quick_alipay.body = body;
        d_quick_alipay.terminal_code = terminal_code;
        d_quick_alipay.device_info = device_info;
        d_quick_alipay.total_amount = total_amount;
        d_quick_alipay.out_trade_no = out_trade_no_alipay;
        //微信
        if(false == quick_pay_help::getPayUrl(
                    url_wechat,
                    terminal_code,
                    total_amount,
                    "2",//支付宝
                    out_trade_no_wechat,
                    body,
                    device_info,
                    errstring)) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
        }
        ui->label_wechat->setPixmap(testwx_help::get_rbcode(url_wechat.data(), ui->label_wechat->width()));
        //保留
        d_quick_wechat.walletType = "2";
        d_quick_wechat.body = body;
        d_quick_wechat.terminal_code = terminal_code;
        d_quick_wechat.device_info = device_info;
        d_quick_wechat.total_amount = total_amount;
        d_quick_wechat.out_trade_no = out_trade_no_wechat;
    }

    if(ch_paymodeno == "AA") {//移动支付
        QString sub_mch_id = query_has_exe_next.recordValue("vch_arg1").toString();//arg1
        QString appid = query_has_exe_next.recordValue("vch_arg2").toString();//arg2
        QString kbstore_id = query_has_exe_next.recordValue("vch_arg3").toString();//arg3
        QString total_amount_x100 = QString::number(int(fee*100));
        QString body = tr("移动支付") +"," + tr("机器号") + lds::terminalCode;
        QString device_info;
        //
        if(false == mobile_pay_help::getzfbpayurl(
                    n_func::f_get_sysparm("mobile_alipay_sign_type", "RSA2"),
                    url_alipay,
                    appid,
                    total_amount_x100,
                    out_trade_no_alipay,
                    body,
                    kbstore_id)) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, mobile_pay_help::errstring);
        }
        ui->label_alipay->setPixmap(testwx_help::get_rbcode(url_alipay.data(), ui->label_alipay->width()));
        //保留
        d_mobile_alipay.appid = appid;
        d_mobile_alipay.body = body;
        d_mobile_alipay.kbstore_id = kbstore_id;
        d_mobile_alipay.out_trade_no = out_trade_no_alipay;
        d_mobile_alipay.total_amount = total_amount_x100;
        d_mobile_alipay.device_info = device_info;
        //
        if(false == mobile_pay_help::getwxpayurl(url_wechat,
                                                 sub_mch_id,
                                                 total_amount_x100,
                                                 out_trade_no_wechat,
                                                 body)) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, mobile_pay_help::errstring);
        }
        ui->label_wechat->setPixmap(testwx_help::get_rbcode(url_wechat.data(), ui->label_wechat->width()));
        //保留
        d_mobile_wechat.sub_mch_id = sub_mch_id;
        d_mobile_wechat.body = body;
        d_mobile_wechat.out_trade_no = out_trade_no_wechat;
        d_mobile_wechat.total_amount = total_amount_x100;
        d_mobile_wechat.device_info = device_info;
    }
    if(ch_paymodeno == "05") {//微信
        QString appid = query_has_exe_next.recordValue("vch_arg1").toString();//微信APPID
        QString mch_id = query_has_exe_next.recordValue("vch_arg2").toString();//商户号
        QString paykey = query_has_exe_next.recordValue("vch_arg3").toString();//API密钥
        int pay_x100 = fee * 100;
        if(false == testwx_help::AmountQRcodeRequest(appid,
                                                     tr("微信支付"),
                                                     pay_x100,
                                                     paykey,
                                                     mch_id,
                                                     out_trade_no_wechat,
                                                     url_wechat)) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID,  tr("操作失败"));
        }
        ui->label_wechat->setPixmap(testwx_help::get_rbcode(url_wechat.data(), ui->label_wechat->width()));
        //保留
        d_wechat.appid = appid;
        d_wechat.mch_id = mch_id;
        d_wechat.paykey = paykey;
        d_wechat.out_trade_no = out_trade_no_wechat;
    }
    if(ch_paymodeno == "06") {//支付宝
        QString app_id = query_has_exe_next.recordValue("vch_arg1").toString();
        QString store_id = query_has_exe_next.recordValue("vch_arg2").toString();
        QString rsa_private_key_path = query_has_exe_next.recordValue("vch_arg3").toString();

        alipay_help::alipay_public_request_argument arg0;
        arg0.app_id_not_null = app_id;
        arg0.charset_not_null="utf-8";
        arg0.sign_type=lds::sysconf->value("alipay_help/" + app_id).toString(); //alipay_help::sign_type_SHA1;
        arg0.timestamp_not_null= lds::cur_machine_dt_str();
        arg0.version_not_null="1.0";
        arg0.rsa_private_key_data_not_null = alipay_help::alipay_private_key_add_start_line(rsa_private_key_path);
        //http://www.yqspos.com/CateSystem/market/receiveAppAuthCode.mobile?org_code=123456001
        QString orgCode = n_func::f_get_sysparm("orgCode");
        if(!orgCode.isEmpty())
            arg0.notify_url = "http://www.yqspos.com/CateSystem/market/receiveAppAuthCode.mobile?org_code="+orgCode;

        alipay_help::alipay_trade_precreate_argument arg1;
        arg1.out_trade_no_not_null = out_trade_no_alipay;
        arg1.subject_not_null = tr("餐饮消费") + "," + tr("机器号") + lds::terminalCode;
        arg1.total_amount_not_null=QString::number(fee, 'f', 2);
        arg1.store_id = store_id;
        arg1.extend_params.sys_service_provider_id = public_sql::alipay_sys_service_provider_id;//public_sql::alipay_sys_service_provider_id;
        //保留
        d_alipay.out_trade_no = arg1.out_trade_no_not_null;
        d_alipay.arg0 = arg0;
        //
        alipay_help::alipay_trade_precreate_return_argument arg2;
        if(false == alipay_help::alipay_trade_precreate(arg0,arg1,arg2)) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, alipay_help::errstring);
        } else {
            lds::sysconf->setValue("alipay_help/" + app_id, arg2.sign_type_not_null);
            url_alipay = arg2.qr_code_not_null.toLocal8Bit();
        }
        ui->label_alipay->setPixmap(testwx_help::get_rbcode(url_alipay.data(), ui->label_alipay->width()));
    }
    //副屏
    rb_ddata.udpKey = lds::udp_rb_code_request_begin;
    rb_ddata.rb_title = this->windowTitle();
    rb_ddata.rb_title_alipay = ui->label_alipay_desc->text();
    rb_ddata.rb_title_wechat = ui->label_wechat_desc->text();
    rb_ddata.rb_alipay = url_alipay;
    rb_ddata.rb_wechat = url_wechat;
    rb_ddata.rb_ch_paymodeno = ch_paymodeno;

    frontViewDialog_virtual::doublescreensenddataByconf(rb_ddata);

    //开始循环
    timer->start();
}

void w_scr_dish_phone_pay::topolling()
{
    //上锁
    lds_mutex_auto_unlock d(&mutex);
    qDebug() << __FUNCTION__ << "will lock";
    if(false == d.tryLock()) {
        qDebug() << __FUNCTION__ << "error : has locked. return";
        return;
    } else {
        qDebug() << __FUNCTION__ << "locked";
    }

    if("AA" == ch_paymodeno) {//移动
        QString transaction_id;
        if(mobile_pay_help::wxOrderQuery(transaction_id,
                                         d_mobile_wechat.sub_mch_id,
                                         d_mobile_wechat.out_trade_no,
                                         d_mobile_wechat.sub_appid)) {
            ret_out_trade_no = d_mobile_wechat.out_trade_no;
            ret_pay_type = OnlinePayTypeWeChat;
            toDDaccept();
            return;
        }
        if(mobile_pay_help::zfbOrderQuery(
                    n_func::f_get_sysparm("mobile_alipay_sign_type", "RSA2"),
                    transaction_id,
                    d_mobile_alipay.appid,
                    d_mobile_alipay.out_trade_no)) {
            ret_out_trade_no = d_mobile_alipay.out_trade_no;
            ret_pay_type = OnlinePayTypeAliPay;
            toDDaccept();
            return;
        }
    }

    if("BB" == ch_paymodeno) {//快捷
        QString errstring;
        OnlinePayType pay_type;
        if(quick_pay_help::query(d_quick_alipay.terminal_code, d_quick_alipay.out_trade_no, errstring, pay_type)) {
            ret_out_trade_no = d_quick_alipay.out_trade_no;
            ret_pay_type = pay_type;
            toDDaccept();
            return;
        }

        if(quick_pay_help::query(d_quick_wechat.terminal_code, d_quick_wechat.out_trade_no, errstring, pay_type)) {
            ret_out_trade_no = d_quick_wechat.out_trade_no;
            ret_pay_type = pay_type;
            toDDaccept();
            return;
        }
    }

    if("05" == ch_paymodeno) {//微信
        testwx_help::retDataStruct ret = testwx_help::getPayBillInfo(d_wechat.out_trade_no,
                                                                     d_wechat.appid,
                                                                     d_wechat.paykey,
                                                                     d_wechat.mch_id);
        if(!ret.transaction_id.trimmed().isEmpty()
                &&ret.trade_state.trimmed()=="SUCCESS"){
            ret_out_trade_no = d_wechat.out_trade_no;
            ret_pay_type = OnlinePayTypeWeChat;
            this->toDDaccept();
            return;
        }
        qDebug() << "result_code:" << ret.result_code;
        qDebug() << "return_msg:" << ret.return_msg;
    }

    if("06" == ch_paymodeno) {//支付宝
        alipay_help::alipay_trade_query_argument arg1;
        alipay_help::alipay_trade_query_return_argument arg2;
        arg1.out_trade_no = d_alipay.out_trade_no;
        if(alipay_help::alipay_trade_query(d_alipay.arg0, arg1,arg2)){
            if(!arg2.trade_no_not_null.isEmpty()
                    &&arg2.trade_status_not_null.trimmed()=="TRADE_SUCCESS"){
                ret_out_trade_no = d_alipay.out_trade_no;
                ret_pay_type = OnlinePayTypeAliPay;
                this->toDDaccept();
                return;
            }
        }
    }
}

void w_scr_dish_phone_pay::tocodePay()
{
    timer_auto_start d(timer);
    d.stop();

    QString errstring;
    QString auth_code = ui->lineEdit_payno->text().replace(" ", "");
    lds_query query_has_exe_next;
    query_has_exe_next.execSelect(QString("select  vch_arg1, vch_arg2, vch_arg3  from cey_bt_paymode where ch_paymodeno = '%1' ")
                                  .arg(ch_paymodeno));
    query_has_exe_next.next();
    if("BB" == ch_paymodeno) {//快捷
        OnlinePayType walletType;
        if(quick_pay_help::barCodePay(
                    d_quick_alipay.terminal_code,
                    d_quick_alipay.total_amount,
                    auth_code,
                    out_trade_no_barcode,
                    d_quick_alipay.body,
                    d_quick_alipay.device_info,
                    errstring,
                    walletType)) {
            ret_out_trade_no = out_trade_no_barcode;
            ret_pay_type =walletType;
            toDDaccept();
            return;
        }
    }
    if("AA" == ch_paymodeno) {//移动
        QString transaction_id;
        if(mobile_pay_help::codepay(
                    n_func::f_get_sysparm("mobile_alipay_sign_type", "RSA2"),
                    transaction_id,
                    d_mobile_alipay.appid,
                    d_mobile_wechat.sub_mch_id,
                    d_mobile_alipay.kbstore_id,
                    auth_code,
                    d_mobile_alipay.total_amount,

                    out_trade_no_barcode,
                    d_mobile_alipay.body
                    )) {
            ret_out_trade_no = transaction_id;
            ret_pay_type = mobile_pay_help::pay_type;
            toDDaccept();
            return;
        }
        errstring = mobile_pay_help::errstring;
    }
    if("05" == ch_paymodeno) {//微信暂时没有条码支付

    }
    if("06" == ch_paymodeno) {//支付宝
        QString store_id = query_has_exe_next.recordValue("vch_arg2").toString();

        alipay_help::alipay_trade_pay_argument arg1;
        alipay_help::alipay_trade_pay_return_argument arg2;
        arg1.auth_code_not_null = auth_code;
        arg1.out_trade_no_not_null = out_trade_no_barcode;
        arg1.subject_not_null = tr("餐饮消费") + "," + tr("机器号") + lds::terminalCode;
        arg1.total_amount_not_null = QString::number(fee, 'f', 2);
        arg1.store_id = store_id;
        arg1.extend_params.sys_service_provider_id = public_sql::alipay_sys_service_provider_id;//public_sql::alipay_sys_service_provider_id;

        //本质上， 条码支付和二维码支付是独立的，这个同用一个arg0
        if(alipay_help::alipay_trade_pay(d_alipay.arg0, arg1, arg2)) {
            ret_out_trade_no = arg2.out_trade_no_not_null;
            ret_pay_type = OnlinePayTypeAliPay;
            toDDaccept();
            return;
        }
        errstring = alipay_help::errstring;
    }

    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("操作失败") +"   "+ errstring);
}

void w_scr_dish_phone_pay::toprint()
{
    timer_auto_start d(timer);
    d.stop();


    QString bottom_desc;
    OnlinePayType pay_type = OnlinePayTypeNone;
    if(ch_paymodeno == "05") {
        pay_type = OnlinePayTypeWeChat;
    } else if(ch_paymodeno == "06") {
        pay_type = OnlinePayTypeAliPay;
    } else if(ch_paymodeno == "BB") {
        pay_type = OnlinePayTypeAliPay;
        bottom_desc = ui->label_alipay_desc->text();
    } else {
        switch(lds_messagebox::question(this, MESSAGE_TITLE_VOID, tr("打印二维码"), tr("微信"), tr("支付宝"), tr("取消"))) {
        case 0:
            pay_type = OnlinePayTypeWeChat;
            bottom_desc = ui->label_wechat_desc->text();
            break;
        case 1:
            pay_type = OnlinePayTypeAliPay;
            bottom_desc = ui->label_alipay_desc->text();
            break;
        default:
            break;
        }
    }

    if(pay_type != OnlinePayTypeAliPay && pay_type != OnlinePayTypeWeChat) {
        return;
    }
    //打印小票
    blockIns printins;
    printins.append(backheader::printer.drawText("a00,s00", rb_ddata.rb_title+"\n"));//居中不放大
    if(bottom_desc.length() > 0)
        printins.append(backheader::printer.drawText("", bottom_desc + "\n"));//清除缓存 居中 pixmap路径
    printins.append(backheader::printer.drawQR_Code("", pay_type == OnlinePayTypeAliPay ? rb_ddata.rb_alipay : rb_ddata.rb_wechat));//清除缓存 居中 pixmap路径
    printins.append(backheader::printer.drawTextEnd("1B6405 1D5601", ""));//走纸5x， 切纸
    backheader::asyn_dataprint(printins);
}

void w_scr_dish_phone_pay::to_save_audio(bool f)
{
    lds::sysconf->setValue("w_scr_dish_phone_pay_" + ch_paymodeno, f);
}

void w_scr_dish_phone_pay::toDDexit()
{
    lds::DOUBLE_DATA ddata;
    ddata.udpKey = lds::udp_rb_code_request_end;
    ddata.rb_title = tr("支付取消");
    frontViewDialog_virtual::doublescreensenddataByconf(ddata);
    this->reject();
}

void w_scr_dish_phone_pay::toDDaccept()
{
    lds::DOUBLE_DATA ddata;
    ddata.udpKey = lds::udp_rb_code_request_end;
    ddata.rb_title = tr("支付成功");
    frontViewDialog_virtual::doublescreensenddataByconf(ddata);

    //
    if(ui->checkBox->isChecked()) {
        public_sql::save_login->ogg_play_polling->play("userdata/settings/ogg/", fee);
    }
    //
    this->accept();
}

