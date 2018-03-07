#include "w_bt_dish_paymode.h"
//收银方式
#include "ui_w_bt_dish_unitset_dialog.h"
#include "lds_tableview_delegate_check.h"
#include "backheader.h"
#include "lds_messagebox.h"
#include <QSqlRecord>
#include "lds_query.h"
#include <qsqlerror.h>
#include <QSqlRelationalDelegate>
#include <QDataWidgetMapper>
#include "ui_w_bt_dish_paymode_dialog.h"
#include <QFileDialog>
#include "alipay_help.h"
#include "w_scr_dish_phone_pay.h"
#include <QScrollArea>
#include <QHostInfo>
#include "ui_w_bt_dish_unitset_dialog.h"
#include "lds_menu.h"
#include "lds.h"
#include "n_func.h"
#include "w_sys_manage_cloudsync_with_time.h"
#include "mobile_pay_help.h"
#include "getfilepath.h"
#include <QClipboard>
#include "w_sys_manage_sqlcopy.h"
#include "ui_w_bt_dish_paymode_set.h"

/**
1.软件arg1 -arg3 用户记录支付方式， arg4用于记录色值
2.
*/
w_bt_dish_paymode::w_bt_dish_paymode(QWidget *parent) :
    QDialog(parent),
    ui(new Ui_w_bt_dish_paymode_dialog)
{
    ui->setupUi(this);
    //! tableview
    ui->label_vch_arg4->kcmap = w_bt_dish::get_kcmap();
    tablemodel=new paymodetablemodel(this);
    tablemodel->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    tablemodel->setTable("cey_bt_paymode");
    tablemodel->save_set_header("ch_paymodeno", tr("收银代码"));
    tablemodel->save_set_header("vch_paymodename", tr("收银名称"));

    ui->tableView->setEditTriggers(QTableView::AllEditTriggers);
    ui->tableView->setModel(tablemodel);
    ui->tableView->rePecifyHeader(tablemodel->get_save_set_header_saver_column_list());
    ui->tableView->horizontalHeader()->setStretchLastSection(true);

    mapper = new QDataWidgetMapper(this);
    mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
    mapper->setModel(tablemodel);
    mapper->setItemDelegate(new QSqlRelationalDelegate(mapper));
    mapper->addMapping(ui->lineEdit_ch_key,tablemodel->fieldIndex("ch_key"));
    mapper->addMapping(ui->lineEdit_ch_paymodeno,tablemodel->fieldIndex("ch_paymodeno"));
    mapper->addMapping(ui->lineEdit_vch_paymodename,tablemodel->fieldIndex("vch_paymodename"));
    mapper->addMapping(ui->checkBox_ch_changeflag,tablemodel->fieldIndex("ch_changeflag"));
    mapper->addMapping(ui->checkBox_ch_faceflag,tablemodel->fieldIndex("ch_faceflag"));
    mapper->addMapping(ui->checkBox_ch_idflag,tablemodel->fieldIndex("ch_IDflag"));
    mapper->addMapping(ui->checkBox_ch_incomeflag,tablemodel->fieldIndex("ch_incomeflag"));
    mapper->addMapping(ui->label_vch_arg4, tablemodel->fieldIndex("vch_arg4"), "color");
    //! toolbar
    ui->widget->setup(QStringList() << tr("团购新增") << tr("普通新增") << tr("删除") <<  tr("刷新") << tr("保存") << tr("退出"), Qt::AlignRight);
    connect(ui->widget->index_widget(tr("团购新增")), SIGNAL(clicked()),this,SLOT(tonew2()));
    connect(ui->widget->index_widget(tr("普通新增")), SIGNAL(clicked()),this,SLOT(tonew()));
    connect(ui->widget->index_widget(tr("删除")), SIGNAL(clicked()),this,SLOT(todel()));
    connect(ui->widget->index_widget(tr("刷新")), SIGNAL(clicked()),this,SLOT(torefresh()));
    connect(ui->widget->index_widget(tr("保存")), SIGNAL(clicked()),this,SLOT(tosave()));
    connect(ui->widget->index_widget(tr("退出")), SIGNAL(clicked()),this,SLOT(toexit()));
    connect(ui->tableView,SIGNAL(selectChanged(int)),this,SLOT(mapper_setcurindex(int)));
    connect(ui->pushButton_arg,SIGNAL(clicked()),this,SLOT(toarg()));

    //5
    QStandardItemModel *standmodel = new QStandardItemModel;
    for(QMap<QString, QString>::iterator itr = w_bt_dish::get_kcmapdesc().begin();
        itr != w_bt_dish::get_kcmapdesc().end(); itr++){
        QStandardItem *item = new QStandardItem;
        item->setData(QColor(itr.key()), Qt::DecorationRole);
        item->setData(itr.value(), Qt::DisplayRole);
        standmodel->appendRow(item);
    }
    standmodel->setHorizontalHeaderLabels(QStringList() << tr("说明"));
    ui->tableView_2->setModel(standmodel);
    ui->tableView_2->verticalHeader()->hide();
    ui->tableView_2->setEditTriggers(QTableView::NoEditTriggers);
    ui->tableView_2->horizontalHeader()->setStretchLastSection(true);
    ui->tableView_2->verticalHeader()->setDefaultSectionSize(30);


    ui->lineEdit_unitprefix->setText(n_func::f_get_sysparm("pay_unit_prefix"));
    connect(ui->lineEdit_unitprefix,SIGNAL(textEdited(QString)),this,SLOT(tosave_unitprefix(QString)));

    QTimer::singleShot(10, this, SLOT(torefresh()));
}

void w_bt_dish_paymode::tonew2()
{
    w_bt_dish_paymode_teampay dialog(this);
    dialog.setWindowTitle(qobject_cast<QPushButton *>(this->sender())->text());
    lds_roundeddialog_rect_align(&dialog).exec();
    torefresh();
}

void w_bt_dish_paymode::tonew()
{
    int rowcount=tablemodel->rowCount();
    tablemodel->insertRow(rowcount);
    tablemodel->model_data_set(rowcount, tablemodel->fieldIndex("ch_paymodeno"), tablemodel->max_column_add_1__max_sql_add_1_dec("ch_paymodeno","%02d"));
    tablemodel->model_data_set(rowcount, tablemodel->fieldIndex("vch_arg4"), QColor(w_bt_dish::get_kcmap().value("S")), Qt::DecorationRole);
    tablemodel->model_data_set(rowcount, tablemodel->fieldIndex("vch_arg4"), "S");


    tablemodel->model_data_set(rowcount, tablemodel->fieldIndex("ch_IDflag"), "N");
    tablemodel->model_data_set(rowcount, tablemodel->fieldIndex("ch_faceflag"), "N");
    tablemodel->model_data_set(rowcount, tablemodel->fieldIndex("ch_changeflag"), "N");
    tablemodel->model_data_set(rowcount, tablemodel->fieldIndex("ch_incomeflag"), "N");

    ui->tableView->selectRow(tablemodel->rowCount() - 1);
    ui->lineEdit_vch_paymodename->setFocus();

}

void w_bt_dish_paymode::todel()
{
    if(0==lds_messagebox::question(this, MESSAGE_TITLE_VOID,tr("确认删除"),tr("确认"),tr("取消"))){
        tablemodel->removeRow(ui->tableView->currentIndex().row());
        tablemodel->trySubmitAll();
        torefresh();
    }
}

void w_bt_dish_paymode::torefresh()
{
    //yun
    w_sys_manage_cloudsync_with_time_running::exec_no_up_then_down_byconf(this);
    ui->widget->index_widget(tr("退出"))->show_flag_top_right(w_sys_manage_cloudsync_with_time_running::q_bt_get_ready_total_byconf());
    //~yun

    QString filter = " 1= 1 ";
    if(lds::get_soft_curent_language_first() == "EN"){
        ui->widget->index_widget(tr("团购新增"))->hide();
        filter += " and ch_paymodeno not in ('07', '08', '09') ";
    }

    tablemodel->setFilter(filter +" order by vch_arg4 asc ");
    ui->tableView->restore(0);
}

bool w_bt_dish_paymode::tosave()
{
    for(int r=0,r_count=tablemodel->rowCount();r<r_count;r++){
        if(tablemodel->model_data(r, "vch_paymodename").toString().isEmpty()){
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("收银方式名称不能为空"));
            return false;
        }
        if(tablemodel->model_data(r, "ch_changeflag").toString()=="Y"
                &&tablemodel->model_data(r, "ch_faceflag").toString()=="Y"){
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("是否找零与是否面额只能选其一"));
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

void w_bt_dish_paymode::toexit()
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

    w_sys_manage_cloudsync_with_time_running::exec_keepdata_byconf(this);
    this->reject();
}

void w_bt_dish_paymode::toarg()
{
    QString ch_paymodeno = tablemodel->model_data(ui->tableView->currentIndex().row(), "ch_paymodeno").toString();
    QString vch_arg4 = tablemodel->model_data(ui->tableView->currentIndex().row(), "vch_arg4").toString();
    if(ch_paymodeno == "06"
            || ch_paymodeno =="05"
            ||ch_paymodeno == "AA"){
        w_bt_dish_paymode_set dialog(ch_paymodeno, this);
        if(ch_paymodeno == "06")dialog.setWindowTitle(tr("支付宝"));
        if(ch_paymodeno == "05")dialog.setWindowTitle(tr("微信"));
        if(ch_paymodeno == "AA")dialog.setWindowTitle(tr("移动支付"));
        lds_roundeddialog_rect_align(&dialog).exec();
    }

    if(vch_arg4 == "T") {
        w_bt_dish_paymode_teampay dialog(this);
        dialog.setWindowTitle(qobject_cast<QPushButton *>(this->sender())->text());
        dialog.set_ch_paymodeno(ch_paymodeno);
        lds_roundeddialog_rect_align(&dialog).exec();
        torefresh();
    }
}

void w_bt_dish_paymode::mapper_setcurindex(int index)
{
    QString ls_paymodeno=tablemodel->model_data(index, "ch_paymodeno").toString();
    bool isenable=
            !(ls_paymodeno=="00" ||
              ls_paymodeno=="02"||
              ls_paymodeno=="03"||
              ls_paymodeno=="04"||

              ls_paymodeno=="05"||
              ls_paymodeno=="06"||

              ls_paymodeno=="07"||
              ls_paymodeno=="08"||
              ls_paymodeno=="09"||

              ls_paymodeno=="10"||
              ls_paymodeno=="11"||

              ls_paymodeno=="AA"||
              ls_paymodeno=="XX"||
              ls_paymodeno=="YY"||
              ls_paymodeno=="ZZ");
    //    ui->lineEdit_ch_key->setEnabled(isenable);
    ui->lineEdit_ch_paymodeno->setEnabled(isenable);
    ui->lineEdit_vch_paymodename->setEnabled(isenable);
    ui->checkBox_ch_changeflag->setEnabled(isenable);
    ui->checkBox_ch_faceflag->setEnabled(isenable);
    ui->checkBox_ch_idflag->setEnabled(isenable);
    ui->checkBox_ch_incomeflag->setEnabled(isenable);
    ui->widget->index_widget(tr("删除"))->setEnabled(isenable);

    ui->pushButton_arg->setEnabled(ls_paymodeno=="05"||
                                   ls_paymodeno=="06"||
                                   ls_paymodeno=="07"||
                                   ls_paymodeno=="08"||
                                   ls_paymodeno=="09"||
                                   ls_paymodeno=="AA"
                                   );

    mapper->setCurrentIndex(index);
}

void w_bt_dish_paymode::todef()
{
    QPushButton *b = qobject_cast<QPushButton *>(this->sender());
    if(b){
        lds_query query;
        if(query.execUpdate("cey_bt_paymode",qrtVariantPairList()
                            <<qrtVariantPair("vch_arg1", "wxd9ccc742b1c39110")
                            <<qrtVariantPair("vch_arg2", "10036785")
                            <<qrtVariantPair("vch_arg3", "wwwscposcomhdd802600000000000000"),
                            qrtEqual("ch_paymodeno", "05"))) {
            if(query.execUpdate("cey_bt_paymode",qrtVariantPairList()
                                <<qrtVariantPair("vch_arg1", "2015121000954514")
                                <<qrtVariantPair("vch_arg2", "")
                                <<qrtVariantPair("vch_arg3", "121.41.82.112:9100"),
                                qrtEqual("ch_paymodeno", "06"))) {
                lds_messagebox::information(this, MESSAGE_TITLE_VOID,  b->text() +"\n"+ tr("设置成功"));
                return;
            }
        }
    }
    lds_messagebox::information(this, MESSAGE_TITLE_VOID, b->text() + "\n" +  tr("操作失败"));
}

void w_bt_dish_paymode::tosave_unitprefix(const QString &text)
{
    n_func::f_set_sysparm("pay_unit_prefix", text, "付款金额是的标志如￥、$");
}

//=============
#include "ui_w_bt_dish_unitset_dialog.h"
//05
//06
static QStandardItem * creatitem(const QString &e, const QString &u){
    QStandardItem *item = new QStandardItem;
    item->setData(e, Qt::EditRole);
    item->setData(u, Qt::UserRole);
    return item;

}

w_bt_dish_paymode_set::w_bt_dish_paymode_set(const QString &ch_paymodeno, QWidget *parent)
    : QDialog(parent),
      ui(new Ui_w_bt_dish_paymode_set)
{
    ui->setupUi(this);

    ui->widget_com->hide();
    ui->label_desc->hide();

    _ch_paymodeno = ch_paymodeno;
    QStandardItem *item;
    //! tableview
    /*
     * 05 微信
*/
    standmodel = new QStandardItemModel;
    standmodel->setHorizontalHeaderLabels(QStringList() << "" << tr("参数说明") << tr("参数详情"));
    ui->tableView->setModel(standmodel);
    ui->tableView->setEditTriggers(QTableView::AllEditTriggers);


    //! data
    lds_query query;
    query.execSelect(QString("select * FROM cey_bt_paymode where ch_paymodeno = '%1' ").arg(ch_paymodeno));
    query.next();
    if(_ch_paymodeno == "05"){
        standmodel->appendRow(QList<QStandardItem *>()
                              << new QStandardItem << new QStandardItem(tr("APP_ID")+"(*)") << creatitem(query.recordValue("vch_arg1").toString(), "vch_arg1"));
        standmodel->appendRow(QList<QStandardItem *>()
                              << new QStandardItem  << new QStandardItem(tr("商户号")+"(*)") << creatitem(query.recordValue("vch_arg2").toString(), "vch_arg2"));
        standmodel->appendRow(QList<QStandardItem *>()
                              << new QStandardItem << new QStandardItem(tr("支付密钥")+"(*)") << creatitem(query.recordValue("vch_arg3").toString(), "vch_arg3"));
        standmodel->appendRow(QList<QStandardItem *>()
                              << new QStandardItem << new QStandardItem(tr("仅记账")) << (item = creatitem(query.recordValue("vch_arg6").toString(), "vch_arg6")));
        ui->tableView->setItemDelegate(new lds_tableview_delegate_check(item->row(), item->column(), ui->tableView, true));
    }
    if(_ch_paymodeno == "06") {
        ///支付宝
        //vch_arg1 ：appid
        //vch_arg2 ：商户门店编号
        //vch_arg3 ：私钥路径
        //vch_arg4 ：付款方式类别
        standmodel->appendRow(QList<QStandardItem *>()
                              << new QStandardItem << new QStandardItem("app_id(*)") << creatitem(query.recordValue("vch_arg1").toString(), "vch_arg1"));
        standmodel->appendRow(QList<QStandardItem *>()
                              << new QStandardItem << new QStandardItem(tr("商户门店编号")) << creatitem(query.recordValue("vch_arg2").toString(), "vch_arg2"));
        standmodel->appendRow(QList<QStandardItem *>()
                              << new QStandardItem << new QStandardItem(tr("私钥路径")+"(*)") << creatitem(query.recordValue("vch_arg3").toString(), "vch_arg3"));
        standmodel->appendRow(QList<QStandardItem *>()
                              << new QStandardItem << new QStandardItem(tr("仅记账")) << (item = creatitem(query.recordValue("vch_arg6").toString(), "vch_arg6")));
        ui->tableView->setItemDelegate(new lds_tableview_delegate_check(item->row(), item->column(), ui->tableView, true));

        standmodel->appendRow(QList<QStandardItem *>()
                              << new QStandardItem << new QStandardItem(tr("系统商编号")) << (item = new QStandardItem(public_sql::alipay_sys_service_provider_id)) );
        item->setFlags(Qt::ItemIsEnabled);
    }

    if(_ch_paymodeno == "AA") {
        ui->label->setText(tr("支付宝签名方式"));
        ui->widget_com->show();
        ui->comboBox->addItem("RSA2");
        ui->comboBox->addItem("RSA");
        ui->comboBox->setCurrentIndex(ui->comboBox->findText(n_func::f_get_sysparm("mobile_alipay_sign_type", "RSA2")));

        standmodel->appendRow(QList<QStandardItem *>()
                              << new QStandardItem(tr("微信")) << new QStandardItem(tr("微信子商户号")+"(*)") << creatitem(query.recordValue("vch_arg1").toString(), "vch_arg1"));

        standmodel->appendRow(QList<QStandardItem *>()
                              << new QStandardItem(tr("支付宝")) << new QStandardItem(tr("appid")+"(*)") << creatitem(query.recordValue("vch_arg2").toString(), "vch_arg2"));
        standmodel->appendRow(QList<QStandardItem *>()
                              << new QStandardItem(tr("支付宝")) << new QStandardItem(tr("口碑门店")) << creatitem(query.recordValue("vch_arg3").toString(), "vch_arg3"));
    }

    //! end
    ui->tableView->setColumnWidth(0, 250);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->widget->setup(QStringList()  << tr("演示用例")<< tr("获取路径") << tr("测试") << tr("获取支付宝公钥") << tr("支付宝测试") << tr("微信测试") << tr("保存") << tr("退出"), Qt::AlignRight);
    connect(ui->widget->index_widget(tr("获取路径")),SIGNAL(clicked()),this,SLOT(togetpath()));
    connect(ui->widget->index_widget(tr("测试")),SIGNAL(clicked()),this,SLOT(totest()));
    connect(ui->widget->index_widget(tr("保存")),SIGNAL(clicked()),this,SLOT(tosave()));
    connect(ui->widget->index_widget(tr("退出")),SIGNAL(clicked()),this,SLOT(toexit()));
    connect(ui->widget->index_widget(tr("演示用例")),SIGNAL(clicked()),this,SLOT(todef()));
    connect(ui->widget->index_widget(tr("支付宝测试")),SIGNAL(clicked()),this,SLOT(towxzfb_alipay_test()));
    connect(ui->widget->index_widget(tr("微信测试")),SIGNAL(clicked()),this,SLOT(towxzfb_wechat_test()));
    connect(ui->widget->index_widget(tr("获取支付宝公钥")),SIGNAL(clicked()),this,SLOT(towxzfb_paste_public_key()));

    if(_ch_paymodeno == "06"){
        ui->label_desc->show();
        ui->label_desc->setText(tr("商店编号,填写会享受相应的优惠")
                                );
        //        tr("系统商编号，该参数作为系统商返佣数据提取的依据")
    }

    if(_ch_paymodeno == "AA") {//移动支付
        ui->widget->index_widget(tr("测试"))->hide();
        ui->widget->index_widget(tr("获取路径"))->hide();
        ui->tableView->setSpan(0, 0, 1, 1);
        ui->tableView->setSpan(1, 0, 2, 1);
    } else {//微信、支付宝
        ui->tableView->hideColumn(0);
        ui->widget->index_widget(tr("支付宝测试"))->hide();
        ui->widget->index_widget(tr("获取支付宝公钥"))->hide();
        ui->widget->index_widget(tr("微信测试"))->hide();
    }
    QTimer::singleShot(10, ui->tableView, SLOT(resizeColumnsToContents2()));
}

w_bt_dish_paymode_set::~w_bt_dish_paymode_set()
{
    delete ui;
}

void w_bt_dish_paymode_set::togetpath()
{
    int row = ui->tableView->currentIndex().row();
    if(row < 0){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("没有选中行"));
        return;
    }
    //获取路径
    QString filepath = getFilePath::getOpenFileName(this, tr("导入文件"),standmodel->item(row, 2)->text());
    if(!filepath.isEmpty()){
        standmodel->item(row, 2)->setText(filepath);
    }
}

void w_bt_dish_paymode_set::totest()
{
    QString ls_error;
    //微信测试
    if(_ch_paymodeno == "05"){
        w_scr_dish_phone_pay::weixinqueryDATA wxdata;
        QByteArray qrcodecontent;
        QDateTime sqldt = n_func::f_get_sysdatetime();
        if(standmodel->rowCount() >= 2){
            wxdata.appid = standmodel->item(0, 2)->text();//APPID
            wxdata.mch_id = standmodel->item(1, 2)->text();//商户号
            wxdata.paykey = standmodel->item(2, 2)->text();//API密钥
            wxdata.out_trade_no = lds::LOCAL_TERMINAL_INDEX_FF+sqldt.toString("yyyyMMddhhmmsszzz");
            ls_error = tr("测试不正常");//2
            if(testwx_help::AmountQRcodeRequest(wxdata.appid
                                                ,this->windowTitle()+sqldt.toString("yyMMdd hh:mm:ss")
                                                ,int(0.01*100)
                                                ,wxdata.paykey
                                                ,wxdata.mch_id
                                                ,wxdata.out_trade_no
                                                ,qrcodecontent)){
                ls_error = tr("测试正常:二维码不正常");//1
                if(!qrcodecontent.isEmpty()){
                    lds_messagebox::information(this, MESSAGE_TITLE_VOID, tr("测试正常:二维码正常"));
                    return;
                }//1
            }//2
            lds_messagebox::information(this, MESSAGE_TITLE_VOID, ls_error);
        }
    }

    if(_ch_paymodeno == "06"){//支付宝
        alipay_help::alipay_public_request_argument arg0;
        alipay_help::alipay_trade_precreate_argument arg1;
        alipay_help::alipay_trade_precreate_return_argument arg2;
        QString rsa_private_key_path = standmodel->item(2, 2)->text();

        arg0.app_id_not_null = standmodel->item(0, 2)->text();//APPID
        arg0.charset_not_null="utf-8";
        arg0.sign_type= lds::sysconf->value("alipay_help/" + arg0.app_id_not_null).toString();//alipay_help::sign_type_SHA256;
        arg0.timestamp_not_null="2015-12-17 03:07:50";
        arg0.version_not_null="1.0";
        arg0.rsa_private_key_data_not_null = alipay_help::alipay_private_key_add_start_line(rsa_private_key_path);
        arg1.out_trade_no_not_null = "201503022001";
        arg1.subject_not_null = "apple";
        arg1.total_amount_not_null="21.3";
        arg1.store_id = "njgdn_001";//商店编号，可以不填，填写后享受支付宝的折扣
        arg1.extend_params.sys_service_provider_id = "njgdn_001";//系统商编号 该参数作为系统商返佣数据提取的依据，请填写系统商签约协议的PID

        if(alipay_help::alipay_trade_precreate(arg0, arg1, arg2)){
            lds::sysconf->setValue("alipay_help/" + arg0.app_id_not_null, arg2.sign_type_not_null);
            lds_messagebox::information(this, MESSAGE_TITLE_VOID,  arg2.sign_type_not_null + tr("测试成功:")
                                        +"\nout_trade_no"+(arg2.out_trade_no_not_null.isEmpty()?tr("不正常"):tr("正常"))
                                        +"\nqr_code"+(arg2.qr_code_not_null.isEmpty()?tr("不正常"):tr("正常")));
        } else {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID,  tr("操作失败") + alipay_help::errstring);
        }
    }
}

void w_bt_dish_paymode_set::tosave()
{
    ///支付宝
    //vch_arg1 ：appid
    //vch_arg2 ：商户门店编号
    //vch_arg3 ：私钥路径
    //vch_arg4 ：付款方式类别

    qrtVariantPairList vplist;
    //usr = key
    //edit = value
    for(int r = 0; r < standmodel->rowCount(); r++) {
        QString u = standmodel->item(r, 2)->data(Qt::UserRole).toString();
        if(!u.isEmpty()) {
            vplist << qrtVariantPair(u, standmodel->item(r, 2)->data(Qt::EditRole).toString());
        }
    }
    lds_query query;
    if(query.execUpdate("cey_bt_paymode",
                        vplist,
                        qrtEqual("ch_paymodeno", _ch_paymodeno))) {
        if(_ch_paymodeno == "AA") {//移动支付
            n_func::f_set_sysparm("mobile_alipay_sign_type", ui->comboBox->currentText());
        }
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, tr("保存成功"));
        return;
    }
    lds_messagebox::information(this, MESSAGE_TITLE_VOID, tr("操作失败")+QString("%1").arg(__LINE__));
}

void w_bt_dish_paymode_set::toexit()
{
    this->reject();
}

void w_bt_dish_paymode_set::todef()
{
    if(_ch_paymodeno == "05") {
        standmodel->item(0, 2)->setText("wxd9ccc742b1c39110");
        standmodel->item(1, 2)->setText("10036785");
        standmodel->item(2, 2)->setText("wwwscposcomhdd802600000000000000");
    }
    if(_ch_paymodeno == "06"){
        standmodel->item(0, 2)->setText("2015121000954514");
        standmodel->item(1, 2)->setText("");
        //        standmodel->item(2, 2)->setText("wwwscposcomhdd802600000000000000");
    }
    if(_ch_paymodeno == "AA"){
        standmodel->item(0, 2)->setText("1431014702");
        standmodel->item(1, 2)->setText("2015121000954514");
        standmodel->item(2, 2)->setText("0000000100010005");
    }
}

void w_bt_dish_paymode_set::towxzfb_alipay_test()
{
    wxzfb_test(OnlinePayTypeAliPay);
}

void w_bt_dish_paymode_set::towxzfb_wechat_test()
{
    wxzfb_test(OnlinePayTypeWeChat);
}

void w_bt_dish_paymode_set::towxzfb_paste_public_key()
{
    QByteArray ret_rsa_public_key;
    bool f = false;
    {
        //!等待
        lds_messagebox_loading_show loading(this, "WAITING...", "WAITING...");
        loading.show();
        //!~等待
        f = mobile_pay_help::getzfbpublic_key(ui->comboBox->currentText(), ret_rsa_public_key);
    }
    if(f) {
        QApplication::clipboard()->setText(ret_rsa_public_key);
        if(0 == lds_messagebox::information(this, MESSAGE_TITLE_VOID, tr("已复制到剪切板\n请打开记事本，新建，粘贴，保存到u盘，卸载u盘"), tr("卸载u盘"), tr("退出"))) {
            w_sys_manage_sqlcopy::unmount_u_pan(this);
        }
        return;
    }
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("操作失败")+QString::number(__LINE__) + mobile_pay_help::errstring);
}

void w_bt_dish_paymode_set::wxzfb_test(OnlinePayType type)
{
    if(_ch_paymodeno == "AA"){//支付宝
        //金额为整数，单位分，如12.34元，实际传入1234
        //auth_code 自动替换清除空格
        QString appid = standmodel->item(1, 2)->text();//arg2
        QString sub_mch_id = standmodel->item(0, 2)->text();//arg1
        QString kbstore_id = standmodel->item(2, 2)->text();//arg3
        QString auth_code;
        QString total_amount = "1";

        QString out_trade_no;
        QString body = tr("移动支付");
        QString sub_appid = "";
        QString device_info = "";
        out_trade_no = lds::LOCAL_TERMINAL_INDEX_FF+QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzz");
        auth_code = "1300 6767 7289 989988";

        QByteArray code_url;
        QString errstring;
        bool flag = false; {
            //!等待
            lds_messagebox_loading_show loading(this, "WAITING...", "WAITING...");
            loading.show();
            //!~等待
            if(type == OnlinePayTypeWeChat) {
                flag = mobile_pay_help::getwxpayurl(code_url, sub_mch_id, total_amount, out_trade_no, body);
            }
            if(type == OnlinePayTypeAliPay) {
                flag = mobile_pay_help::getzfbpayurl(ui->comboBox->currentText(), code_url, appid, total_amount, out_trade_no, body, kbstore_id);
            }
        }
        if(flag){
            lds_messagebox::information(this, MESSAGE_TITLE_VOID,  tr("测试成功"));
            return;
        }
        lds_messagebox::warning(this ,MESSAGE_TITLE_VOID, tr("测试失败")+":"+mobile_pay_help::errstring);
    }
}


w_bt_dish_paymode::paymodetablemodel::paymodetablemodel(QObject *parent)
    :lds_model_sqltablemodel(parent)
{
}

w_bt_dish_paymode::paymodetablemodel::~paymodetablemodel()
{

}

QVariant w_bt_dish_paymode::paymodetablemodel::data(const QModelIndex &item, int role) const
{
    if(role == Qt::TextColorRole){
        QString d = record(item.row()).value("vch_arg4").toString();
        QColor c = QColor(w_bt_dish::get_kcmap().value(d));
        if(c.isValid()) return c;
    }
    return lds_model_sqltablemodel::data(item, role);
}

w_bt_dish_paymode_teampay::w_bt_dish_paymode_teampay(QWidget *parent) : QDialog(parent), ui(new Ui_w_bt_dish_unitset_Dialog)
{
    ui->setupUi(this);
    //1
    //2
    tablemodel = new w_bt_dish_paymode::paymodetablemodel ;
    tablemodel->setTable("cey_bt_paymode");
    tablemodel->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);

    tablemodel->save_set_header("ch_paymodeno", tr("收银方式"));
    tablemodel->save_set_header("vch_paymodename", tr("方式名称"));
    tablemodel->save_set_header("vch_arg1", tr("网址简称"));
    tablemodel->save_set_header("vch_arg2", tr("网址"), true);
    ui->tableView->setModel(tablemodel);
    ui->tableView->rePecifyHeader(tablemodel->get_save_set_header_saver_column_list());
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->setEditTriggers(QTableView::AllEditTriggers);

    //3
    ui->widget->setup(QStringList() << tr("新增") << tr("删除") << tr("测试") << tr("保存") << tr("退出"), Qt::AlignRight);
    connect(ui->widget->index_widget(tr("删除")), SIGNAL(clicked()),this, SLOT(todel()));
    connect(ui->widget->index_widget(tr("测试")), SIGNAL(clicked()),this, SLOT(tonettest()));
    connect(ui->widget->index_widget(tr("保存")), SIGNAL(clicked()),this, SLOT(tosave()));
    connect(ui->widget->index_widget(tr("退出")), SIGNAL(clicked()),this, SLOT(toexit()));

    //4
    {
        //
        QStandardItemModel *standmodel = new QStandardItemModel;
        standmodel->setHorizontalHeaderLabels(QStringList()<<tr("新增团购"));
        QStandardItem *item;
        for(int index = 0, indexcount = w_bt_dish::get_kvaddresslist().count(); index < indexcount; index++){
            standmodel->appendRow(item = new QStandardItem(w_bt_dish::get_kvaddresslist()[index].first));item->setData(QColor("moccasin"), Qt::BackgroundRole);
        }
        standmodel->appendRow(item = new QStandardItem(tr("其他团购")));item->setData(QColor("mintcream"), Qt::BackgroundRole);

        lds_tableView *t = creatpopView(standmodel->rowCount());
        t->setModel(standmodel);
        t->resizeColumnToContents(0);
        t->setFixedWidth(t->columnWidth(0));
        //2
        menu=new lds_menu(this);
        QWidgetAction *widgetaction=new QWidgetAction(ui->widget->index_widget(tr("新增")));
        widgetaction->setDefaultWidget(t);
        menu->addAction(widgetaction);
        ui->widget->index_widget(tr("新增"))->setMenu(menu);
        connect(t,SIGNAL(clicked(QModelIndex)),this,SLOT(newpaymode_team(QModelIndex)));
        connect(menu,SIGNAL(aboutToShow()),t,SLOT(clearSelection()));
    }

    //5
    ui->label_desc->setMaximumHeight(100);
    ui->label_desc->setText("");

    tablemodel->setFilter("vch_arg4='T' ");
}

lds_tableView *w_bt_dish_paymode_teampay::creatpopView(int rowcount)
{
    lds_tableView *t = new lds_tableView;
    //    t->horizontalHeader()->setStretchLastSection(true);
    t->verticalHeader()->hide();
    t->horizontalHeader()->hide();
    t->setEditTriggers(QTableView::NoEditTriggers);
    t->setSelectionMode(QTableView::SingleSelection);
    t->verticalHeader()->setDefaultSectionSize(30);
    t->horizontalHeader()->setFixedHeight(30);
    t->setFixedHeight(qMin(500, 30 + rowcount * t->verticalHeader()->defaultSectionSize()));
    return t;

}

void w_bt_dish_paymode_teampay::set_ch_paymodeno(const QString &ch_paymodeno)
{
    tablemodel->setFilter(QString("ch_paymodeno = '%1' ").arg(ch_paymodeno));
}

void w_bt_dish_paymode_teampay::tonew()
{
}

void w_bt_dish_paymode_teampay::todel()
{
    tablemodel->removeRow(ui->tableView->currentIndex().row());
}

void w_bt_dish_paymode_teampay::torefresh()
{
    tablemodel->select();
}

void w_bt_dish_paymode_teampay::tonettest()
{
    int row = ui->tableView->currentIndex().row();
    if(row < 0 ) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("没有选中行"));
        return;
    }
    QString vch_paymodeaddress = tablemodel->model_data(row, "vch_arg2").toString();
    QProcess p;
    p.start(("/usr/desk/ie/QtWeb"" " + vch_paymodeaddress));
#ifndef QT_DEBUG
    if(p.waitForFinished(-1)) {
    } else {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("浏览器打开超时"));
    }
#endif
}

bool w_bt_dish_paymode_teampay::tosave()
{
    if(tablemodel->model_is_dirty()){
        if(tablemodel->trySubmitAll()){
            lds_messagebox::information(this, MESSAGE_TITLE_VOID, tr("保存成功"));
            return true;
        }
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("操作失败"));
        return false;
    }
    return true;
}

void w_bt_dish_paymode_teampay::toexit()
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

void w_bt_dish_paymode_teampay::newpaymode_team(const QModelIndex &index)
{
    QPair<QString, QString> def(tr("外卖团购"), tr("外卖团购"));
    int rowcount = tablemodel->rowCount();
    tablemodel->insertRow(rowcount);
    tablemodel->model_data_set(rowcount,  tablemodel->fieldIndex("ch_paymodeno"), tablemodel->max_column_add_1__max_sql_add_1_dec("ch_paymodeno", "%02d"));
    tablemodel->model_data_set(rowcount,  tablemodel->fieldIndex("vch_paymodename"), w_bt_dish::get_kvaddresslist().value(index.row(),def).first);
    tablemodel->model_data_set(rowcount,  tablemodel->fieldIndex("vch_arg1"), w_bt_dish::get_kvaddresslist().value(index.row(), def).first);
    tablemodel->model_data_set(rowcount,  tablemodel->fieldIndex("vch_arg2"), w_bt_dish::get_kvaddresslist().value(index.row()).second);
    tablemodel->model_data_set(rowcount,  tablemodel->fieldIndex("vch_arg4"), "T");

    menu->close();
}
