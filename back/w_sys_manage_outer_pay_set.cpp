#include "w_sys_manage_outer_pay_set.h"
#include "ui_w_sys_manage_outer_pay_set.h"
#include "ftableview_delegate.h"
#include "ftableview_standmodel_sql_none.h"
#include "n_func.h"
#include "lds.h"
#include "public_printby_ch_billno.h"
#include "lds_menu.h"
#include "lds.h"
#include "lds_messagebox.h"
#include "lds_dialog_input.h"
#include "w_sys_manage_sqlcopy.h"
#include "public_sql.h"
#include "n_func.h"
#include <QSqlError>
#include "w_sys_manage_businessdataclear.h"
#include "w_sys_manage_outer_pay_set_sql.h"
#include "printer_pos.h"
#include "w_scr_dish.h"
#include <QFile>
#include <QTextEdit>
#include "back_view_dialog.h"
#include "lds_tableview_delegate_radio.h"
#include "lds_tableview_delegate_com2.h"
#include "lds_dialog_input_double.h"
#include "ui_lds_dialog_input_dialog_double.h"
#include <QDir>
#include "business_data_upload_isoft.h"
#include "public_logindialog.h"

QList<QPair<QString, QString> > w_sys_manage_outer_pay_set::languagelist;

w_sys_manage_outer_pay_set::w_sys_manage_outer_pay_set(QWidget *parent) :
    lds_Dialog(parent),
    ui(new Ui::w_sys_manage_outer_pay_set)
{
    ui->setupUi(this);
    frame_upload_dialog = 0;
    //1
    backlist_model = new ftableview_standmodel_sql_none;
    backlist_model->set_row_column(3, 1);

    backlist_model->model_data_set(0, 0,"anyvalue", Qt::UserRole+1);
    backlist_model->model_data_set(1, 0,"anyvalue", Qt::UserRole+1);
    backlist_model->model_data_set(2, 0,"anyvalue", Qt::UserRole+1);
    ftableview_delegate *d = new ftableview_delegate(ui->tableView_list);
    d->keepConnect(ui->tableView_list);
    d->setMargin(2);
    d->indexisEmptystillDraw = true;
    ui->tableView_list->setProperty("outer_stylesheet", "tableView_type_pop");

    ui->tableView_list->setModel(backlist_model);
    ui->tableView_list->setItemDelegate(d);
    ui->tableView_list->setHeaderResizeMode(Qt::Horizontal, QHeaderView::Stretch);

    ui->tableView_list->verticalHeader()->setDefaultSectionSize(50);
    ui->radioButton_upload_disable->setChecked(true);
    //3
    ui->tableView_list->setCurrentIndex(backlist_model->index(0, 0));
    ui->comboBox_font_size->hide();
    ui->label_font_size->hide();
    ui->frame_upload->hide();
    ui->frame_liaoning_inventory->hide();

    ui->stackedWidget_2->setCurrentIndex(0);
    retranslateView();


    toupdateSection(ui->tableView_list->currentIndex());
    connect(ui->pushButton_ok, SIGNAL(clicked()),this,SLOT(took()));
    connect(ui->pushButton_cancel, SIGNAL(clicked()),this,SLOT(tocancel()));
    connect(ui->tableView_list,SIGNAL(clicked(QModelIndex)),this,SLOT(toupdateSection(QModelIndex)));
    connect(ui->comboBox_blotout_round,SIGNAL(activated(int)),this,SLOT(toshowround(int)));

    connect(ui->pushButton_skin_app,SIGNAL(clicked()),this,SLOT(toskinapp()));
    connect(ui->pushButton_function, SIGNAL(clicked()),this,SLOT(tofunction()));
    connect(ui->pushButton_function_2, SIGNAL(clicked()),this,SLOT(tofunction_2()));
    connect(ui->pushButton_function_3, SIGNAL(clicked()),this,SLOT(tofunction_3()));
    connect(ui->pushButton_function_4, SIGNAL(clicked()),this,SLOT(tofunction_4()));
    connect(ui->pushButton_function_5, SIGNAL(clicked()),this,SLOT(tofunction_5()));
    connect(ui->pushButton_function_6, SIGNAL(clicked()),this,SLOT(tofunction_6()));
    connect(ui->pushButton_function_7, SIGNAL(clicked()),this,SLOT(tofunction_7()));
    connect(ui->pushButton_function_8, SIGNAL(clicked()),this,SLOT(tofunction_8()));
    connect(ui->pushButton_function_9, SIGNAL(clicked()),this,SLOT(tofunction_9()));
    connect(ui->pushButton_function_10, SIGNAL(clicked()),this,SLOT(tofunction_10()));
    connect(ui->pushButton_function_11, SIGNAL(clicked()),this,SLOT(tofunction_11()));

    connect(ui->radioButton_upload1, SIGNAL(clicked(bool)),this, SLOT(toupload1(bool)));
    connect(ui->radioButton_upload2, SIGNAL(clicked(bool)),this, SLOT(toupload2(bool)));
    connect(ui->radioButton_upload3, SIGNAL(clicked(bool)),this, SLOT(toupload3(bool)));
    connect(ui->radioButton_upload4, SIGNAL(clicked(bool)),this, SLOT(toupload4(bool)));
    connect(ui->radioButton_liaoning_inventory_keep, SIGNAL(clicked(bool)),this, SLOT(toliaoning_inventory_keep(bool)));

    connect(ui->lineEdit_upload_awake, SIGNAL(textChanged(QString)),this,SLOT(touploadaware(QString)));

    connect(ui->checkBox_4, SIGNAL(clicked(bool)), this,SLOT(toSaaSCheck(bool)));

    old_language = lds::get_soft_curent_language_first();

    //暂时不启用
    ui->radioButton_liaoning_inventory_keep->hide();
}

w_sys_manage_outer_pay_set::~w_sys_manage_outer_pay_set()
{
    delete ui;
}

void w_sys_manage_outer_pay_set::dataCheck(QWidget *parent, bool ispop)
{
    lds_query::tran_saction();
    qlonglong errcount = 0;
    lds_query query;
    QStringList ch_payno_list;
    QStringList ch_billno_list;
    //1
    query.execSelect("select count(0) from cey_u_checkout_master where ch_billno = '' or ch_tableno = '' ");
    if(query.next()){
        errcount += query.recordValue(0).toLongLong();
        if(!query.execDelete("cey_u_checkout_master",  "ch_billno = '' or ch_tableno = '' ")){
            goto failure;
        }
    }
    //2
    query.execSelect("select count(0) from cey_u_orderdish where ch_billno = '' or ch_tableno = '' ");
    if(query.next()){
        errcount += query.recordValue(0).toLongLong();
        if(!query.execDelete("cey_u_orderdish",  " ch_billno = '' or ch_tableno = '' ")){
            goto failure;
        }
    }
    //3
    query.execSelect("select count(0) from cey_u_master where ch_billno = '' ");
    if(query.next()){
        errcount += query.recordValue(0).toLongLong();
        if(!query.execDelete("cey_u_master",  "ch_billno = '' ")){
            goto failure;
        }
    }
    //4
    query.execSelect("select count(0) from cey_u_checkout_detail where ch_paymodeno = '' ");
    if(query.next()){
        errcount += query.recordValue(0).toLongLong();
        if(!query.execDelete("cey_u_checkout_detail", "ch_paymodeno = '' ")){
            goto failure;
        }
    }
    //6
    query.execSelect("select count(0) from t_m_hang_balance where isnull(num_remain)");
    if(query.next()){
        errcount += query.recordValue(0).toLongLong();
        if(!query.execUpdate("t_m_hang_balance", "num_remain", qrtFunc("num_pay"), "isnull(num_remain)")) {
            goto failure;
        }
    }
    //    7.
    query.execSelect("select count(0) from cey_u_master where  ch_billno like '%,%' ");
    if(query.next()) {
        errcount += query.recordValue(0).toLongLong();
        if(!query.execDelete("cey_u_master", "ch_billno like '%,%' ")){
            goto failure;
        }
    }

    ch_payno_list.clear();
    query.execSelect(
                " select (t2.p), (t3.p), t3.ch_payno from "
                " (Select a.num_payamount as p, a.ch_payno from cey_u_checkout_detail a, cey_u_checkout_master b, cey_bt_paymode c WHERE a.ch_payno = b.ch_payno and b.ch_state = 'Y' and a.ch_paymodeno = c.ch_paymodeno and b.dt_operdate > '2016-10-19 00:00:00' and b.dt_operdate <='2016-10-19 23:59:59') t3"

                " left join ( select sum(amount - discount - present) as p, ch_payno from (SELECT b.ch_payno, a.ch_dishno, c.vch_dishname,   c.ch_unitno,    (a.num_num - a.num_back) as 'num',  ((a.num_num - a.num_back) * a.num_price + a.num_price_add) as 'amount',  ((a.num_num - a.num_back) * a.num_price + a.num_price_add) * (1 - a.int_discount * 0.01) as 'discount',  case a.ch_presentflag when 'Y' then (((a.num_num - a.num_back) * a.num_price + a.num_price_add) * a.int_discount * 0.01) else 0 end as 'present' , b.vch_operID as 'vch_operID', c.ch_dish_typeno as 'typeno', b.dt_operdate as 'dt_operdate', a.vch_operID as 'vch_order', a.dt_operdate as 'dt_order' FROM cey_u_orderdish a,cey_u_checkout_master b,cey_bt_dish c WHERE(a.ch_payno = b.ch_payno)and(b.ch_state = 'Y')and(a.ch_dishno = c.ch_dishno)and(a.num_num - a.num_back <> 0)and(b.dt_operdate > '2016-10-19 00:00::00' and b.dt_operdate <='2016-10-19 23:59::59' and ''=''))t group by ch_payno) t2"

                " on t2.ch_payno = t3.ch_payno  where isnull(t2.p)");
    while(query.next()) {
        ch_payno_list.append(query.recordValue("ch_payno").toString());
    }
    foreach(const QString &p, ch_payno_list) {
        query.execDelete("cey_u_checkout_detail", QString("ch_payno = '%1'  ").arg(p));
        query.execDelete("cey_u_checkout_detail", QString("ch_payno = '%1'  ").arg(p));
    }
    errcount += ch_payno_list.count();


    //清除业务数据后，正常打印厨显退菜单的情况
    ch_billno_list.clear();
    query.execSelect("select ch_billno from (select count(0) as count, ch_billno, group_concat(ch_payno) as pays from cey_u_checkout_master where ch_state = 'Y' group by ch_billno ) t where t.count > 1;");
    while(query.next()) {
        errcount ++;
        QString ch_billno = query.recordValue("ch_billno").toString();
        ch_billno_list.append(ch_billno);
    }
    ch_payno_list.clear();
    foreach(const QString &ch_billno, ch_billno_list) {
        query.execSelect(QString("select ch_payno from cey_u_checkout_master where ch_billno = '%1' and ch_state = 'Y'  order by ch_payno asc").arg(ch_billno));
        while(query.next())
            ch_payno_list.append(query.recordValue("ch_payno").toString());
        if(!ch_payno_list.isEmpty())
            ch_payno_list.removeLast();
    }
    foreach(const QString &ch_payno, ch_payno_list) {
        query.execDelete("cey_u_checkout_master", QString("ch_payno = '%1' ").arg(ch_payno));
        query.execDelete("cey_u_checkout_detail", QString("ch_payno = '%1' ").arg(ch_payno));
    }

    query.execSelect("select count(0) from cey_u_checkout_master where ifnull(ch_payno,'') ='' ");
    query.next();
    if(query.recordValue(0).toInt()) {
        errcount += query.recordValue(0).toInt();
        query.execDelete("cey_u_checkout_master", "ifnull(ch_payno, '') = '' ");
    }
    query.execSelect("select count(0) from cey_u_checkout_detail where ifnull(ch_payno,'') ='' ");
    query.next();
    if(query.recordValue(0).toInt()) {
        errcount += query.recordValue(0).toInt();
        query.execDelete("cey_u_checkout_detail", "ifnull(ch_payno, '') = '' ");
    }

    query.execSelect("select count(0) from t_m_curamount where vch_memberno = '' ");
    query.next();
    errcount += query.recordValue(0).toInt();
    query.execDelete("t_m_curamount", "vch_memberno = '' ");


    if(errcount > 0){
        lds_query::com_mit();
        if(ispop)lds_messagebox::information(parent, MESSAGE_TITLE_VOID, tr("成功清除了一些脏数据总共:") + QString::number(errcount));
        return;
    }
failure:
    lds_query::roll_back();
    if(errcount > 0){
        if(ispop)lds_messagebox::information(parent, MESSAGE_TITLE_VOID, tr("清除失败"));
    } else {
        if(ispop)lds_messagebox::information(parent, MESSAGE_TITLE_VOID, tr("没有需要校对的数据"));
    }
}

void w_sys_manage_outer_pay_set::addLanguageList(QComboBox *com)
{
    QList<QPair<QString, QString> >& list = w_sys_manage_outer_pay_set::getLanguagelist();
    for(int k = 0; k < list.count();k ++) {
        com->addItem(list[k].first, list[k].second);
    }
}

QList<QPair<QString, QString> > &w_sys_manage_outer_pay_set::getLanguagelist()
{
    if(languagelist.isEmpty()) {
        languagelist.append(QPair<QString, QString>(tr("默认"), ""));
        languagelist.append(QPair<QString, QString>(tr("中文"), "CN"));
        languagelist.append(QPair<QString, QString>(tr("英文"), "EN"));
        languagelist.append(QPair<QString, QString>(tr("波斯文"), "FA"));
        languagelist.append(QPair<QString, QString>(tr("德语"), "DE"));

        languagelist.append(QPair<QString, QString>(tr("法语"), "FR"));
        languagelist.append(QPair<QString, QString>(tr("俄罗斯语"), "RU"));
        languagelist.append(QPair<QString, QString>(tr("西班牙语"), "ES"));
        languagelist.append(QPair<QString, QString>(tr("土耳其语"), "TR"));
        languagelist.append(QPair<QString, QString>(tr("阿拉伯语"), "AR"));

        languagelist.append(QPair<QString, QString>(tr("韩语"), "KO"));
        languagelist.append(QPair<QString, QString>(tr("日语"), "JA"));
        languagelist.append(QPair<QString, QString>(tr("以色列语"), "IW"));
        languagelist.append(QPair<QString, QString>(tr("意大利语"), "IT"));
        languagelist.append(QPair<QString, QString>(tr("葡萄牙语"), "PT"));

        languagelist.append(QPair<QString, QString>(tr("印地语"), "HI"));
        languagelist.append(QPair<QString, QString>(tr("孟加拉语"), "BN"));
        languagelist.append(QPair<QString, QString>(tr("旁遮普语"), "PA"));
    }
    return languagelist;
}

//随系统 is empty
QString w_sys_manage_outer_pay_set::getLanguageValue(const QString &key, bool skip_with_sys)
{
    if(skip_with_sys && key.isEmpty()) return "";

    QList<QPair<QString, QString> >& list = w_sys_manage_outer_pay_set::getLanguagelist();
    for(int k = 0; k < list.count();k ++) {
        if(list[k].second == key) {
            return list[k].first;
        }
    }
    return "";
}

void w_sys_manage_outer_pay_set::clearCache()
{
    languagelist.clear();
}

void w_sys_manage_outer_pay_set::retranslateView()
{
    ui->retranslateUi(this);
    //
    backlist_model->model_data_set(0, 0,tr("设置1"), Qt::UserRole);
    backlist_model->model_data_set(1, 0,tr("设置2"), Qt::UserRole);
    backlist_model->model_data_set(2, 0,tr("设置3"), Qt::UserRole);

    comIndexSaveClear(ui->comboBox_blotout_round);
    ui->comboBox_blotout_round->addItem(tr("选用抹零"),1);
    ui->comboBox_blotout_round->addItem(tr("选用四舍五入"),2);

    comIndexSaveClear(ui->comboBox_blotout_mode);
    ui->comboBox_blotout_mode->addItem(tr("不抹零"), 1);
    ui->comboBox_blotout_mode->addItem(tr("抹零到元"), 2);
    ui->comboBox_blotout_mode->addItem(tr("抹零到十"), 3);
    ui->comboBox_blotout_mode->addItem(tr("抹零到百"), 4);

    comIndexSaveClear(ui->comboBox_round_mode);
    ui->comboBox_round_mode->addItem(tr("不四舍五入"), 1);
    ui->comboBox_round_mode->addItem(tr("到角"), 2);
    ui->comboBox_round_mode->addItem(tr("到元"), 3);
    ui->comboBox_round_mode->addItem(tr("到十元"), 4);
    ui->comboBox_round_mode->addItem(tr("到五厘"), 5);

    comIndexSaveClear(ui->comboBox_yingye_model);
    ui->comboBox_yingye_model->addItem(tr("快餐模式"));
    ui->comboBox_yingye_model->addItem(tr("酒楼模式"));
    ui->comboBox_yingye_model->addItem(tr("酒吧模式"));
    comIndexSaveClear(ui->comboBox_skin);
    ui->comboBox_skin->addItem(tr("曜石黑"), "obsidian");
    ui->comboBox_skin->addItem(tr("天空蓝"), "skyblue");


    comIndexRestoreAll();
}


void w_sys_manage_outer_pay_set::took()
{
    foreach(int index, index_set){
        if(!saveData(index, true))
            return;
    }

    this->accept();
}

void w_sys_manage_outer_pay_set::tocancel()
{
    this->reject();
}

void w_sys_manage_outer_pay_set::toupdateSection(const QModelIndex &index)
{
    saveData(index.row());
}

void w_sys_manage_outer_pay_set::toskinapp()
{
    QString key = ui->comboBox_skin->itemData(ui->comboBox_skin->currentIndex()).toString();
    if(!key.isEmpty()){
        QFile file(QString("userdata/settings/skin/fastfd_%1.qss")
                   .arg(key));
        if(file.open(QFile::ReadOnly)){
            qApp->setStyleSheet(file.readAll());
            file.close();
            lds::sysconf->setValue("system_setting/skin_name", key);
            return;
        }
        qDebug() << file.errorString();
    }
    qDebug("filename is empty");
}

void w_sys_manage_outer_pay_set::toshowround(int index)
{
    switch(index) {
    case 0:
        ui->comboBox_blotout_mode->show();
        ui->comboBox_round_mode->hide();
        break;
    case 1:
        ui->comboBox_blotout_mode->hide();
        ui->comboBox_round_mode->show();
        break;
    }
}

void w_sys_manage_outer_pay_set::tofunction()
{
    lds_query query;
    if(1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, tr("该操作将会将会员特价同步为零售价,是否继续"), tr("继续"), tr("取消"))){
        return;
    }
    if(query.execUpdate("cey_bt_dish", "num_m_price", qrtFunc("num_price") , "")) {
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, tr("同步成功"));
        return;
    }
    lds_messagebox::information(this, MESSAGE_TITLE_VOID, tr("同步失败"));
    return;
}

void w_sys_manage_outer_pay_set::tofunction_2()
{
    lds_dialog_input_double inputdialog(this);
    lds::showWidget(inputdialog.ui->checkBox);
    inputdialog.setWindowTitle(ui->pushButton_function_2->text());
    inputdialog.ui->label->setText(tr("服务费"));
    inputdialog.ui->checkBox->setText(tr("固定服务费"));
    inputdialog.ui->checkBox->setChecked(n_func::f_get_sysparm("pay_fixed_service", "0") == "1");
    inputdialog.ui->lineEdit->setValue(n_func::f_get_sysparm("pay_fixed_service_value", 0).toDouble());
    inputdialog.ui->lineEdit->setDecimals(0);
    inputdialog.ui->lineEdit->setSuffix("%");
    //    inputdialog.showSecondLineEdt();
    //    inputdialog.ui->label_2->setText(tr("最高服务费"));
    //    inputdialog.ui->lineEdit_2->setText("0");

    int ret = lds_roundeddialog_rect_align(&inputdialog).exec();
    n_func::f_set_sysparm("pay_fixed_service", inputdialog.ui->checkBox->isChecked()?"1":"0");
    n_func::f_set_sysparm("pay_fixed_service_value", inputdialog.ui->lineEdit->value());
    if(QDialog::Accepted == ret){
        //1商品设置服务费、设置服务费率模式、设置服务费率值
        lds_query query;
        if(query.execUpdate("cey_bt_dish", "ch_serviceflag", "Y", "")){
            if(query.execUpdate("cey_bt_table_type", qrtVariantPairList()
                                << qrtVariantPair("ch_serviceflag", "N")
                                << qrtVariantPair("ch_service_mode", 4)
                                << qrtVariantPair("int_rate", inputdialog.ui->lineEdit->value())
                                //                                        << qrtVariantPair("num_max_service", inputdialog.ui->lineEdit_2->text())
                                ,  ""
                                )) {
                lds_messagebox::information(this, MESSAGE_TITLE_VOID, tr("操作成功"));
                return;
            }
        }

        lds_messagebox::information(this, MESSAGE_TITLE_VOID, tr("操作失败"));
    }
}

void w_sys_manage_outer_pay_set::tofunction_3()
{
    lds_messagebox_loading *loading = new lds_messagebox_loading(this, "WAITING...");
    loading->setText(tr("正在数据库升级..."));
    lds_roundeddialog_rect_align loading_help(loading, false);
    loading_help.show();

    lds_dialog_input inputdialog(this);
    QString sql_error;
    inputdialog.ui->label->setText(tr("密码") + ":");
    inputdialog.ui->lineEdit->setEchoMode(QLineEdit::Password);
#ifdef QT_DEBUG
    inputdialog.ui->lineEdit->setText(n_func::f_get_godpassword());
#endif
    inputdialog.setWindowTitle(tr("请输入密钥"));
    if(QDialog::Accepted==lds_roundeddialog_rect_align(&inputdialog).exec()){
        sql_error = tr("密码错误");
        if(inputdialog.ui->lineEdit->text()==n_func::f_get_godpassword()){
            lds_query query;
            query.execDelete("cey_sys_sqlversion  ");
            this->done(public_sql::Reboot);
            return;
        }
    }
    if(!sql_error.isEmpty()) {
        public_sql::Other_sql_update();//不会影响前他升级
        lds_messagebox::information(this, MESSAGE_TITLE_VOID,tr("数据库升级:")+sql_error);
    }
}

void w_sys_manage_outer_pay_set::tofunction_4()
{
    lds_dialog_input_double inputdialog(this);
    lds::showWidget(inputdialog.ui->checkBox);
    inputdialog.setWindowTitle(ui->pushButton_function_4->text());
    inputdialog.ui->label->setText(tr("税率"));
    inputdialog.ui->checkBox->setText(tr("固定税率"));
    inputdialog.ui->checkBox->setChecked(n_func::f_get_sysparm("pay_fixed_tax", "0") == "1");
    inputdialog.ui->lineEdit->setValue(n_func::f_get_sysparm("pay_fixed_tax_value", 0).toDouble());
    inputdialog.ui->lineEdit->setDecimals(2);
    inputdialog.ui->lineEdit->setSuffix("%");

    int ret = lds_roundeddialog_rect_align(&inputdialog).exec();
    n_func::f_set_sysparm("pay_fixed_tax", inputdialog.ui->checkBox->isChecked()?"1":"0");
    n_func::f_set_sysparm("pay_fixed_tax_value", inputdialog.ui->lineEdit->value());
    if(QDialog::Accepted == ret){
        //1菜品设置服务费、设置服务费率模式、设置服务费率值
        lds_query query;
        if(query.execUpdate("cey_bt_dish", "int_rate", inputdialog.ui->lineEdit->value(), "")) {
            lds_messagebox::information(this, MESSAGE_TITLE_VOID, tr("操作成功"));
            return;
        }
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, tr("操作失败"));
    }
}

void w_sys_manage_outer_pay_set::tofunction_5()
{
    switch(lds_messagebox::question(this, MESSAGE_TITLE_VOID, tr("切换"), tr("中文"), tr("英文"), tr("取消"))) {
    case 0:
        if(w_sys_manage_sqlcopy::cnkeep()){
            lds_messagebox::information(this, MESSAGE_TITLE_VOID, tr("操作成功"));
            return;
        }
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, tr("操作失败"));
        break;
    case 1:
        if(w_sys_manage_sqlcopy::enkeep()){
            lds_messagebox::information(this, MESSAGE_TITLE_VOID, tr("操作成功"));
            return;
        }
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, tr("操作失败"));
        break;
    case 2:
        return;
        break;
    }
}

void w_sys_manage_outer_pay_set::tofunction_6()
{

    lds_dialog_input inputdialog(this);
    QString sql_error;
    inputdialog.ui->label->setText(tr("密码") + ":");
    inputdialog.ui->lineEdit->setEchoMode(QLineEdit::Password);
#ifdef QT_DEBUG
    inputdialog.ui->lineEdit->setText(n_func::f_get_godpassword());
#endif
    inputdialog.setWindowTitle(tr("请输入密钥"));
    if(QDialog::Accepted==lds_roundeddialog_rect_align(&inputdialog).exec()){
        sql_error = tr("密码错误");
        if(inputdialog.ui->lineEdit->text()==n_func::f_get_godpassword()){
            inputdialog.hideSecondLineEdt();
            inputdialog.ui->label->setText(tr("年"));
            inputdialog.setWindowTitle(tr("请输入年"));
            inputdialog.ui->lineEdit->setEchoMode(QLineEdit::Normal);
            inputdialog.ui->lineEdit->clear();
            inputdialog.ui->lineEdit->setFocus();
            sql_error = "";
            if(QDialog::Accepted==lds_roundeddialog_rect_align(&inputdialog).exec()){
                int year = inputdialog.ui->lineEdit->text().toInt();
                bool resultflag = true;
                lds_query query;
                query.execSelect("show tables;");
                while(query.next()) {
                    QString tname = query.recordValue(0).toString();
                    lds_query query2;
                    query2.execSelect(QString("SHOW COLUMNS FROM %1 where type like 'datetime' ;").arg(tname));
                    while(query2.next()) {
                        QString cname = query2.recordValue(0).toString();
                        lds_query query3;
                        if(!query3.execUpdate(tname,
                                              cname, qrtFunc("concat(year(curdate()), '-', month(%2),'-',  day(%2),' ',  time(%2))"),
                                              qrtEqual("year(%2)", year))) {
                            resultflag = false;
                            break;
                        }
                        continue;
                    }
                    if(!resultflag) break;
                }
                if(resultflag) {
                    lds_messagebox::information(this, MESSAGE_TITLE_VOID, tr("操作成功")+QString::number(year));
                } else {
                    lds_messagebox::information(this, MESSAGE_TITLE_VOID, tr("操作失败"));
                }
                return;
            }
        }
    }
    if(!sql_error.isEmpty()) {
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, sql_error);
    }
}

void w_sys_manage_outer_pay_set::tofunction_7()
{
    w_sys_manage_businessdataclear dialog(this);
    dialog.setWindowTitle(tr("业务数据库清除"));
    dialog.setFixedHeight(300);
    lds_roundeddialog_rect_align(&dialog).exec();
}

void w_sys_manage_outer_pay_set::tofunction_8()
{
    dataCheck(this);
}

void w_sys_manage_outer_pay_set::tofunction_9()
{
    lds_dialog_input inputdialog(this);
    inputdialog.ui->label->setText(tr("数量"));
    inputdialog.ui->lineEdit->setText("0");
    inputdialog.setWindowTitle(_TEXT_SLOT(this));
    if(QDialog::Accepted == lds_roundeddialog_rect_align(&inputdialog).exec()){
        //1菜品设置服务费、设置服务费率模式、设置服务费率值
        lds_query query;
        if(query.execUpdate("cey_bt_dish", "num_item_warn", inputdialog.ui->lineEdit->text() , "")) {
            lds_messagebox::information(this, MESSAGE_TITLE_VOID, tr("操作成功"));
            return;
        }
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, tr("操作失败"));
    }
}

void w_sys_manage_outer_pay_set::tofunction_10()
{
    if(1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, _TEXT_SLOT(this), tr("确定"), tr("取消"))){
        return;
    }
    lds_query::tran_saction();
    lds_query query;
    if(query.execUpdate("cey_bt_dish", "ch_groupno", qrtFunc("ch_dishno"),"")) {
        lds_query::com_mit();
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, tr("操作成功"));
        return;
    }
    lds_query::roll_back();
    lds_messagebox::information(this, MESSAGE_TITLE_VOID, tr("操作失败"));
    return;
}

void w_sys_manage_outer_pay_set::tofunction_11()
{
    if(1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, tr("只会保留：厨打方案、打印机设置、会员设置、软件设置等设置?"), tr("确认"), tr("取消"))){
        return;
    }
    lds_dialog_input inputdialog(this);
    inputdialog.ui->label->setText(tr("密码") + ":");
    inputdialog.ui->lineEdit->setEchoMode(QLineEdit::Password);
#ifdef QT_DEBUG
    inputdialog.ui->lineEdit->setText(n_func::f_get_godpassword());
#endif
    inputdialog.setWindowTitle(tr("请输入密钥"));
    if(QDialog::Accepted==lds_roundeddialog_rect_align(&inputdialog).exec()){
        if(inputdialog.ui->lineEdit->text() != n_func::f_get_godpassword()){
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("密码错误"));
            return;
        }
        lds_messagebox_loading_show loading(this, "");
        loading.show_delay();
        if(public_sql::MySQL_restore("userdata/settings/sql", "init_without_settings", &loading, tr("开业清库"))) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("操作成功"));
            this->done(public_sql::Reboot);
        } else {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("操作失败"));
        }
    }
}

void w_sys_manage_outer_pay_set::toupload1(bool f)
{
    if(false == f) return;

    w_sys_manage_outer_pay_set_ftp dialog(this);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    if(QDialog::Accepted == lds_roundeddialog_rect_align(&dialog).exec()){

    }
}

void w_sys_manage_outer_pay_set::toupload2(bool f)
{
    if(false == f) return;

    w_sys_manage_outer_pay_set_kechuan dialog(this);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    if(QDialog::Accepted == lds_roundeddialog_rect_align(&dialog).exec()){

    }
}

void w_sys_manage_outer_pay_set::toupload3(bool f)
{
    if(false == f) return;

    w_sys_manage_outer_pay_set_xiexin dialog(this);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    if(QDialog::Accepted == lds_roundeddialog_rect_align(&dialog).exec()){

    }
}

void w_sys_manage_outer_pay_set::toupload4(bool f)
{
    if(false == f) return;

    w_sys_manage_outer_pay_set_isoft dialog(this);
    dialog.setWindowTitle(_TEXT_SLOT(this));
     lds_roundeddialog_rect_align(&dialog).exec();
}

void w_sys_manage_outer_pay_set::toliaoning_inventory_keep(bool f)
{
    if(false == f) return;

    ui->comboBox_liaoning_inventory_cache->clear();
    QString path = lds::localdataPath+"/liaoning_inventory_keep_cache";
    QDir().mkpath(path);
    QFileInfoList fileinfos=QDir(path).entryInfoList(QStringList() << "*.txt");
    foreach(const QFileInfo &info, fileinfos){
        ui->comboBox_liaoning_inventory_cache->addItem(info.fileName(), info.filePath());
    }

    ui->frame_liaoning_inventory->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    ui->frame_liaoning_inventory->move(lds::getPopPoint(ui->lineEdit_upload_awake, ui->frame_upload));
    ui->frame_liaoning_inventory->raise();
    ui->frame_liaoning_inventory->show();
}

void w_sys_manage_outer_pay_set::touploadaware(const QString &arg)
{
    if(arg.trimmed().toLower() == "sc"){//上传
        if(frame_upload_dialog == 0){
            frame_upload_dialog = new QDialog(this);
            frame_upload_dialog->setWindowFlags(frame_upload_dialog->windowFlags() | Qt::FramelessWindowHint);
            frame_upload_dialog->resize(ui->frame_upload->size() + QSize(20, 20));
            connect(ui->pushButton_upload_exit, SIGNAL(clicked()), frame_upload_dialog, SLOT(reject()));

            ui->frame_upload->setParent(frame_upload_dialog);
            ui->frame_upload->move(10, 10);
        }
        frame_upload_dialog->exec();

    } else if(arg.trimmed().toLower() == "sql"){
        w_sys_manage_outer_pay_set_sql dialog(this);
        dialog.setWindowTitle("SQL_RUN");
        dialog.exec();
    }
}

bool  w_sys_manage_outer_pay_set::saveData(int index, bool issave)
{
    ui->stackedWidget_2->setCurrentIndex(index);
    lds_query query;
    switch(index) {
    case 0:
        if(issave) {
            n_func::f_set_sysparm_q(query,"blotout_mode", ui->comboBox_blotout_mode->itemData(ui->comboBox_blotout_mode->currentIndex()));
            n_func::f_set_sysparm_q(query,"round_mode", ui->comboBox_round_mode->itemData(ui->comboBox_round_mode->currentIndex()));
            n_func::f_set_sysparm_q(query,"blotout_round", ui->comboBox_blotout_round->itemData(ui->comboBox_blotout_round->currentIndex()));

            lds::sysconf->setValue("system_setting/yingyemoshi", ui->comboBox_yingye_model->currentIndex());

            n_func::f_set_sysparm_q(query,"pay_auto_discount", ui->checkBox_auto_dis->isChecked()?"1":"0");
            n_func::f_set_sysparm_q(query,"pay_auto_discount_value", ui->spinBox_auto_dis_value->value());

            n_func::f_set_sysparm_q(query,"serial_no_begin", ui->lineEdit_serial_no_begin->text());
            n_func::f_set_sysparm_q(query,"serial_no_end", ui->lineEdit_serial_no_end->text());

            lds::sysconf->setValue("system_setting/qt_type_column_count", ui->lineEdit_type_columns->text());

            lds::sysconf->setValue("system_setting/qt_iconsize", ui->lineEdit_dish_columns->text()+"x"+ui->lineEdit_dish_rows->text());

            lds::sysconf->setValue("system_setting/getweigthtimeout", ui->lineEdit_auto_weight_timeout->text());
            //字体选择
            lds::sysconf->setValue("system_setting/sys_font", ui->comboBox_font_select->curusrdata().toString());

            n_func::f_set_sysparm("bp_fmt_num_dec", ui->bp_fmt_num_dec->value());
            n_func::f_set_sysparm("bp_fmt_price_dec", ui->bp_fmt_price_dec->value());
            n_func::f_set_sysparm("bp_fmt_amount_dec", ui->bp_fmt_amount_dec->value());

            n_func::f_set_sysparm("ftptp_use", ui->radioButton_upload1->isChecked()? "1" : "0");
            n_func::f_set_sysparm("njgj_use", ui->radioButton_upload2->isChecked()? "1" : "0");
            n_func::f_set_sysparm("xiexin_use", ui->radioButton_upload3->isChecked()? "1" : "0");
            n_func::f_set_sysparm("isoft_use", ui->radioButton_upload4->isChecked()? "1" : "0");
            n_func::f_set_sysparm("liaoning_inventory_keep", ui->radioButton_liaoning_inventory_keep->isChecked()? "1" : "0");

            return true;
        }
        if(index_set.find(index) == index_set.end()) {
            ui->comboBox_blotout_round->setCurrentIndex(ui->comboBox_blotout_round->findData(n_func::f_get_sysparm_q(query,"blotout_round").toInt()));
            ui->comboBox_blotout_mode->setCurrentIndex(ui->comboBox_blotout_mode->findData(n_func::f_get_sysparm_q(query,"blotout_mode").toInt()));
            ui->comboBox_round_mode->setCurrentIndex(ui->comboBox_round_mode->findData(n_func::f_get_sysparm_q(query,"round_mode").toInt()));
            toshowround(ui->comboBox_blotout_round->currentIndex());
            //

            //字体选择
            QDir dir("userdata/settings/font");
            if(!dir.exists()) dir.mkpath(dir.path());
            ui->comboBox_font_select->addItem("", "");
            QFileInfoList infolist = dir.entryInfoList(QStringList() << "*.ttf" << "*.TTF");
            foreach(const QFileInfo &info, infolist) {
                if(info.baseName() == "." || info.baseName() == "..") continue;
                ui->comboBox_font_select->addItem(info.baseName(), info.fileName());
            }
            ui->comboBox_font_select->setCurrentIndex(ui->comboBox_font_select->findData(lds::sysconf->value("system_setting/sys_font").toString()));

            //
            ui->comboBox_yingye_model->setCurrentIndex(lds::sysconf->value("system_setting/yingyemoshi").toInt());

            ui->checkBox_auto_dis->setChecked(n_func::f_get_sysparm_q(query,"pay_auto_discount", "0") == "1");
            ui->spinBox_auto_dis_value->setValue(n_func::f_get_sysparm_q(query,"pay_auto_discount_value", 0).toInt());

            ui->lineEdit_serial_no_begin->setText(n_func::f_get_sysparm_q(query,"serial_no_begin"));
            ui->lineEdit_serial_no_end->setText(n_func::f_get_sysparm_q(query,"serial_no_end"));

            ui->lineEdit_type_columns->setText(lds::sysconf->value("system_setting/qt_type_column_count", "5").toString());

            QStringList xy = lds::sysconf->value("system_setting/qt_iconsize", "5x4").toString().split("x");
            ui->lineEdit_dish_columns->setText(xy.value(0));
            ui->lineEdit_dish_rows->setText(xy.value(1));

            ui->lineEdit_auto_weight_timeout->setText(lds::sysconf->value("system_setting/getweigthtimeout", "3").toString());

            ui->comboBox_skin->setCurrentIndex(ui->comboBox_skin->findData(lds::sysconf->value("system_setting/skin_name", "obsidian").toString()));

            ui->bp_fmt_num_dec->setValue(n_func::f_get_sysparm("bp_fmt_num_dec", 3).toInt());
            ui->bp_fmt_price_dec->setValue(n_func::f_get_sysparm("bp_fmt_price_dec", 2).toInt());
            ui->bp_fmt_amount_dec->setValue(n_func::f_get_sysparm("bp_fmt_amount_dec", 2).toInt());
            //科传接口
            ui->radioButton_upload1->setChecked(n_func::f_get_sysparm("ftptp_use") == "1");
            ui->radioButton_upload2->setChecked(n_func::f_get_sysparm("njgj_use") == "1");
            ui->radioButton_upload3->setChecked(n_func::f_get_sysparm("xiexin_use") == "1");
            ui->radioButton_upload4->setChecked(n_func::f_get_sysparm("isoft_use") == "1");
            ui->radioButton_liaoning_inventory_keep->setChecked(n_func::f_get_sysparm("liaoning_inventory_keep") == "1");

//            connect(ui->pushButton_upload_exit, SIGNAL(clicked()), ui->frame_upload, SLOT(hide()));
            connect(ui->pushButton_liaoning_inventory_exit, SIGNAL(clicked()), ui->frame_liaoning_inventory, SLOT(hide()));
            connect(ui->pushButton_liaoning_inventory_upload, SIGNAL(clicked()), this, SLOT(to_liaoning_inventory_upload()));
            connect(ui->pushButton_liaoning_inventory_look, SIGNAL(clicked()),this, SLOT(to_liaoning_inventory_look()));
        }
        break;
        //////////////////////////////
    case 1:
        if(issave) {
            //外卖送餐员默认0000
            lds::sysconf->setValue("system_setting/waimaidef0000", ui->checkBox->isChecked());
            //中餐下退出输入当前操作员信息
            lds::sysconf->setValue("system_setting/restaurant_inputoperinfo", ui->checkBox_13->isChecked());
            //启动ups检测
            lds::sysconf->setValue("system_setting/upsiseffect", ui->checkBox_14->isChecked());
            Printer_POS::com_upsiseffect = lds::sysconf->value("system_setting/upsiseffect", false).toBool();
            n_func::f_set_sysparm("tax_service_form_value", ui->tableView_tax_service_form->currentIndex().row() * 2 + ui->tableView_tax_service_form->currentIndex().column());
            return true;
        }
        if(index_set.find(index) == index_set.end()) {
            //外卖送餐员默认0000
            ui->checkBox->setChecked(lds::sysconf->value("system_setting/waimaidef0000", false).toBool());
            //中餐下退出输入当前操作员信息
            ui->checkBox_13->setChecked(lds::sysconf->value("system_setting/restaurant_inputoperinfo",false).toBool());
            //启动ups检测
            ui->checkBox_14->setChecked(lds::sysconf->value("system_setting/upsiseffect", false).toBool());

#ifdef QT_DEBUG
            ui->lineEdit_upload_awake->setPlaceholderText("输入sc激活；""输入sql执行；输入cc执行顶有鸣（辽宁）食品科技有限公司");
            ui->radioButton_upload3->show();
#endif


            QStandardItemModel *form_model = new QStandardItemModel(this);
            form_model->setHorizontalHeaderLabels(QStringList() << tr("不含税") << tr("含税"));
            form_model->setVerticalHeaderLabels(QStringList() << tr("不含服务费") << tr("含服务费"));
            ui->tableView_tax_service_form->setModel(form_model);
            ui->tableView_tax_service_form->setHeaderResizeMode(Qt::Horizontal, QHeaderView::Stretch);
            ui->tableView_tax_service_form->setHeaderResizeMode(Qt::Vertical, QHeaderView::Stretch);
            ui->tableView_tax_service_form->setAlternatingRowColors(false);
            ui->tableView_tax_service_form->setSelectionBehavior(QTableView::SelectItems);
            ui->tableView_tax_service_form->setSelectionMode(QTableView::SingleSelection);
            ui->tableView_tax_service_form->setEditTriggers(QTableView::NoEditTriggers);
            ui->tableView_tax_service_form->setItemDelegate(new lds_tableview_delegate_radio(this));
            ui->tableView_tax_service_form->setEditTriggers(QTableView::AllEditTriggers);
            ui->tableView_tax_service_form->setStyleSheet(
                        "border:0;background:transparent;selection-background-color: transparent;"
                        );
            ui->label_desc->setText(
                        tr("含服务费") +":" + tr("服务费按菜品税率计入税收") +";" +
                        tr("含税")+":"+tr("价格中包含税收"));

            int tax_service_form_value = n_func::f_get_sysparm("tax_service_form_value",0).toInt();
            ui->tableView_tax_service_form->setCurrentIndex(form_model->index(tax_service_form_value / 2, tax_service_form_value % 2));
        }
        break;

    case 2:
        if(issave) {
            //交班必须接班人
            n_func::f_set_sysparm_q(query,"handover_oper", ui->checkBox_1->isChecked()?"1":"0");
            //退菜必须输入退菜原因
            n_func::f_set_sysparm_q(query,"orderdish_back", ui->checkBox_2->isChecked()?"1":"0");
            //是否输入手牌号
            lds::sysconf->setValue("system_setting/fukuanshishurushoupai", ui->checkBox_4->isChecked()?"1":"0");
            //点菜时弹出菜品详细
            n_func::f_set_sysparm_q(query,"ordertonum", ui->checkBox_6->isChecked()?"1":"0");
            //锁屏解锁必须为系统登录员工
            lds::sysconf->setValue("system_setting/jiesuoyuangong", ui->checkBox_7->isChecked()?"1":"0");
            //是否启用会员价
            n_func::f_set_sysparm_q(query,"bp_discountproject", ui->checkBox_8->isChecked()?"1":"0");
            //手工处理服务费
            n_func::f_set_sysparm_q(query,"service_handwork", ui->checkBox_10->isChecked()?"1":"0");
            //前台付款默认现金
            n_func::f_set_sysparm_q(query,"qt_def_money", ui->checkBox_qt_def_money->isChecked()?"1":"0");
            //点菜时相同菜品不叠加
            lds::sysconf->setValue("system_setting/samedishNotadd_order", ui->checkBox_15->isChecked());
            //前台打印时相同菜品不叠加
            lds::sysconf->setValue("system_setting/samedishNotadd_qtprint", ui->checkBox_21->isChecked());
            //交班前显示交班信息
            n_func::f_set_sysparm_q(query,"show_info_before_shift", ui->checkBox_show_info_before_shift->isChecked()?"1":"0");
            //前台显示库存信息
            lds::sysconf->setValue("system_setting/inventory_qt_show_info", ui->checkBox_16->isChecked());
            //负库存可以消费
            n_func::f_set_sysparm_q(query,"inventory_negative_cannot_pay", ui->checkBox_17->isChecked()?"1":"0");
            //前台点菜时子套菜折叠
            n_func::f_set_sysparm_q(query,"qt_Y_dish_fold", ui->checkBox_19->isChecked()?"1":"0");
            //前台允许会员发卡
            lds::sysconf->setValue("system_setting/qt_can_member_sendcard", ui->checkBox_18->isChecked());
            //字体大小
            lds::sysconf->setValue("system_setting/sys_font_size", ui->comboBox_font_size->currentText());
            //前台显示原料
            n_func::f_set_sysparm_q(query,"qt_show_ingredient", ui->checkBox_ingredient->isChecked()?"1":"0");
            //菜品启用大类
            lds::sysconf->setValue("system_setting/dish_type_enabled", ui->checkBox_20->isChecked()?"1":"0");
            //快餐开启会员付款
            lds::sysconf->setValue("system_setting/kuaican_mpay_enabled", ui->checkBox_kuaican_mpay->isChecked());
            //前台禁用长按功能
            n_func::f_set_sysparm_q(query,"qt_forbide_change_new", ui->checkBox_qt_forbide_change_new->isChecked()?"1":"0");
            //折让模式
            n_func::f_set_sysparm_q(query,"discount_off_mode", ui->checkBox_discount_off_mode->isChecked()?"1":"0");
            //开启二维码点餐
            n_func::f_set_sysparm_q(query,"qr_code_order_mode", ui->checkBox_qr_code_order_mode->isChecked()?"1":"0");

        }
        if(index_set.find(index) == index_set.end()) {

            //交班必须接班人
            ui->checkBox_1->setChecked(n_func::f_get_sysparm_q(query,"handover_oper") == "1");
            //退菜必须输入退菜原因
            ui->checkBox_2->setChecked(n_func::f_get_sysparm_q(query,"orderdish_back") == "1");
            //是否输入手牌号
            ui->checkBox_4->setChecked(lds::sysconf->value("system_setting/fukuanshishurushoupai").toBool());
            //点菜时弹出菜品详细
            ui->checkBox_6->setChecked(n_func::f_get_sysparm_q(query,"ordertonum") == "1");
            //锁屏解锁必须为系统登录员工
            ui->checkBox_7->setChecked(lds::sysconf->value("system_setting/jiesuoyuangong").toBool());
            //是否启用会员价
            ui->checkBox_8->setChecked(n_func::f_get_sysparm_q(query,"bp_discountproject") == "1");
            //手工处理服务费
            ui->checkBox_10->setChecked(n_func::f_get_sysparm_q(query,"service_handwork") == "1");
            //前台付款默认现金
            ui->checkBox_qt_def_money->setChecked(n_func::f_get_sysparm_q(query,"qt_def_money","1") == "1");
            //点菜时相同菜品不叠加
            ui->checkBox_15->setChecked(lds::sysconf->value("system_setting/samedishNotadd_order", false).toBool());
            //前台打印时相同菜品不叠加
            ui->checkBox_21->setChecked(lds::sysconf->value("system_setting/samedishNotadd_qtprint", false).toBool());
            //交班前显示交班信息
            ui->checkBox_show_info_before_shift->setChecked(n_func::f_get_sysparm_q(query,"show_info_before_shift", "0") == "1");
            //前台显示库存信息
            ui->checkBox_16->setChecked(lds::sysconf->value("system_setting/inventory_qt_show_info", false).toBool());
            //负库存可以消费
            ui->checkBox_17->setChecked(n_func::f_get_sysparm_q(query,"inventory_negative_cannot_pay","0") == "1");
            //前台点菜时子套菜折叠
            ui->checkBox_19->setChecked(n_func::f_get_sysparm_q(query,"qt_Y_dish_fold","0") == "1");
            //前台允许会员发卡
            ui->checkBox_18->setChecked(lds::sysconf->value("system_setting/qt_can_member_sendcard", false).toBool());
            //字体大小
            ui->comboBox_font_size->setEditText(lds::sysconf->value("system_setting/sys_font_size", "10").toString());
            //前台显示原料
            ui->checkBox_ingredient->setChecked(n_func::f_get_sysparm_q(query,"qt_show_ingredient","0") == "1");
            //菜品启用大类
            ui->checkBox_20->setChecked(lds::sysconf->value("system_setting/dish_type_enabled", false).toBool());
            //快餐开启会员付款
            ui->checkBox_kuaican_mpay->setChecked(lds::sysconf->value("system_setting/kuaican_mpay_enabled", false).toBool());
            //前台禁用长按功能
            ui->checkBox_qt_forbide_change_new->setChecked(n_func::f_get_sysparm_q(query,"qt_forbide_change_new","0") == "1");
            //折让模式
            ui->checkBox_discount_off_mode->setChecked(public_sql::discount_off_mode() == "1");
            //开启二维码点餐
            ui->checkBox_qr_code_order_mode->setChecked(n_func::f_get_sysparm_q(query,"qr_code_order_mode","0") == "1");

        }
        break;
    }
    index_set.insert(index);

    return true;
}


QString w_sys_manage_outer_pay_set_ftp::FtpAddress = "FTP://ftp.lncrland.com";

w_sys_manage_outer_pay_set_ftp::w_sys_manage_outer_pay_set_ftp(QWidget *parent) :
    lds_model_mapwidget_vertical(parent), ui(new Ui_w_sys_manage_outer_pay_set_ftp)
{
    ui->setupUi(this);
    //
    tablemodel = new lds_model_sqltablemodel(this);
    tablemodel->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    tablemodel->setTable("cey_sys_parameter");

    mapper = new QDataWidgetMapper(this);
    mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
    mapper->setModel(tablemodel);
    mapper->setOrientation(Qt::Vertical);

    scan_all_objectname_add_map_setFilter("vch_parameter","ftptp");
    //
    mapper->setCurrentIndex(1);
    //
    connect(ui->pushButton_exit, SIGNAL(clicked()),this,SLOT(reject()));
    connect(ui->pushButton_ok, SIGNAL(clicked()),this,SLOT(took()));
    connect(ui->pushButton_test, SIGNAL(clicked()),this,SLOT(totest()));
    /*
    connect(ui->pushButton_detail, SIGNAL(clicked()),this,SLOT(todetail()));
    connect(ui->pushButton_example, SIGNAL(clicked()),this,SLOT(toexample()));*/
    //    ui->pushButton_detail->hide();

    //
    QString path = public_sql::ftp_businessdataupload_dir;
    QDir  dir(path);
    if(!dir.exists()) dir.mkpath(path);
    QFileInfoList fileinfos=dir.entryInfoList();
    ui->comboBox_records->clear();
    foreach(QFileInfo info, fileinfos){
        if(info.fileName()=="."
                ||info.fileName()=="..")continue;
        ui->comboBox_records->addItem(info.fileName());
    }

    ui->lineEdit_ftptp_address->setText(w_sys_manage_outer_pay_set_ftp::FtpAddress);
}

w_sys_manage_outer_pay_set_ftp::~w_sys_manage_outer_pay_set_ftp()
{
    delete ui;
}

void w_sys_manage_outer_pay_set_ftp::took()
{
    lds_query::tran_saction();
    mapper_save_data();
    lds_query::com_mit();
    this->accept();
}

void w_sys_manage_outer_pay_set_ftp::todetail()
{

}

void w_sys_manage_outer_pay_set_ftp::toexample()
{
    /*
商铺号 B41B0231N01

货号 B41B0231N01CY0001

FTP地址  ftp.lncrland.com

账号 B41B41B0231N01

密码 J9w8400Q*/
    ui->ftptp_storecode->setText("B41B0231N01");
    ui->ftptp_plu->setText("B41B0231N01CY0001");
    //    ui->ftptp_address->setText("FTP://ftp.lncrland.com");
    ui->ftptp_usr->setText("B41B0231N01");
    //    ui->ftptp_pwd->setText("J9w8400Q");
    //
    mapper->submit();
}

void w_sys_manage_outer_pay_set_ftp::totest()
{
    tablemodel->trySubmitAll();

    QDate last_update_date = n_func::f_get_sysdatetime().date();
    QString ls_err;
    QList<w_scr_dish::HUARUN_BUSINESS_DATA> datalist;
    w_scr_dish::GET_HUARUN_BUSINESS_DATA(last_update_date, datalist, ls_err);
    int ret = w_scr_dish::HUARUN_BUSINESS_DATA_UPLOAD(w_sys_manage_outer_pay_set_ftp::FtpAddress
                                                      ,n_func::f_get_sysparm("ftptp_usr")
                                                      ,n_func::f_get_sysparm("ftptp_pwd")
                                                      ,datalist
                                                      );
    if(w_scr_dish::huarun_ftp_server_success == ret) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("操作成功"));
        return;
    }
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, w_scr_dish::huarunInfo_error(ret));
}

void w_sys_manage_outer_pay_set_ftp::toaddress_def()
{
    //    ui->ftptp_address->setText("FTP://ftp.lncrland.com");
}

void w_sys_manage_outer_pay_set_ftp::saveData()
{

}

bool w_sys_manage_outer_pay_set_ftp::insert_sql_objectname(const QString &objectname)
{
    lds_query query;
    return query.execInsert("cey_sys_parameter", qrtVariantPairList()
                            << qrtVariantPair("vch_parameter", objectname)
                            << qrtVariantPair("vch_value", ""));
}


#include "w_scr_dish_data_collection.h"

w_sys_manage_outer_pay_set_kechuan::w_sys_manage_outer_pay_set_kechuan(QWidget *parent)
    : lds_model_mapwidget_vertical(parent), ui(new Ui_w_sys_manage_outer_pay_set_kechuan)
{
    ui->setupUi(this);
    //
    tablemodel = new lds_model_sqltablemodel(this);
    tablemodel->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    tablemodel->setTable("cey_sys_parameter");

    mapper = new QDataWidgetMapper(this);
    mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
    mapper->setModel(tablemodel);
    mapper->setOrientation(Qt::Vertical);

    scan_all_objectname_add_map_setFilter("vch_parameter", "njgj");
    //
    mapper->setCurrentIndex(1);
    //
    connect(ui->pushButton_exit, SIGNAL(clicked()),this,SLOT(reject()));
    connect(ui->pushButton_ok, SIGNAL(clicked()),this,SLOT(took()));
    connect(ui->pushButton_detail, SIGNAL(clicked()),this,SLOT(todetail()));
    connect(ui->pushButton_example, SIGNAL(clicked()),this,SLOT(toexample()));
}

w_sys_manage_outer_pay_set_kechuan::~w_sys_manage_outer_pay_set_kechuan()
{
    delete ui;
}

void w_sys_manage_outer_pay_set_kechuan::took()
{
    tablemodel->trySubmitAll();
    this->accept();
}

void w_sys_manage_outer_pay_set_kechuan::todetail()
{
    w_scr_dish_data_collection dialog(this);
    dialog.setWindowTitle(tr("数据采集"));
    lds_roundeddialog_rect_align(&dialog).exec();
}

void w_sys_manage_outer_pay_set_kechuan::toexample()
{
    ui->njgj_ip->setText("211.94.93.147");
    ui->njgj_itemcode->setText("33201L15000131");
    ui->njgj_license_agreement->setText("");
    ui->njgj_mallid->setText("33201");
    ui->njgj_port->setText("");
    //    ui->njgj_port->setText("7088");
    ui->njgj_storecode->setText("33201L1500013");
    ui->njgj_username->setText("");
    //    ui->njgj_username->setText("jinmao");
    ui->njgj_web->setText("/salestrans.asmx");
    //
    mapper->submit();
}

bool w_sys_manage_outer_pay_set_kechuan::insert_sql_objectname(const QString &objectname)
{
    lds_query query;
    return query.execInsert("cey_sys_parameter", qrtVariantPairList()
                            << qrtVariantPair("vch_parameter", objectname)
                            << qrtVariantPair("vch_value", ""));
}

//////////////////
#include "ui_w_sys_manage_outer_pay_set_xiexin.h"
w_sys_manage_outer_pay_set_xiexin::w_sys_manage_outer_pay_set_xiexin(QWidget *parent)
    : lds_model_mapwidget_vertical(parent)
{
    ui = new Ui_w_sys_manage_outer_pay_set_xiexin;
    ui->setupUi(this);

    //
    tablemodel = new lds_model_sqltablemodel(this);
    tablemodel->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    tablemodel->setTable("cey_sys_parameter");

    mapper = new QDataWidgetMapper(this);
    mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
    mapper->setModel(tablemodel);
    mapper->setOrientation(Qt::Vertical);

    scan_all_objectname_add_map_setFilter("vch_parameter", "xiexin");
    //
    mapper->setCurrentIndex(1);
    //
    connect(ui->pushButton_cancel, SIGNAL(clicked()),this,SLOT(tocancel()));
    connect(ui->pushButton_ok, SIGNAL(clicked()),this,SLOT(took()));
    connect(ui->pushButton_test_upload_today, SIGNAL(clicked()),this,SLOT(touploadyesterday()));


    //
    QString path = lds::localdataPath+"/XIEXIN_BUSINESS_DATA_UPLOAD";
    QDir  dir(path);
    if(!dir.exists()) dir.mkpath(path);
    QFileInfoList fileinfos=dir.entryInfoList();
    ui->comboBox_records->clear();
    foreach(QFileInfo info, fileinfos){
        if(info.fileName()=="."
                ||info.fileName()=="..")continue;
        ui->comboBox_records->addItem(info.fileName());
    }
    ui->pushButton_test_upload_today->show();
}
w_sys_manage_outer_pay_set_xiexin::~w_sys_manage_outer_pay_set_xiexin()
{
    delete ui;
}

void w_sys_manage_outer_pay_set_xiexin::took()
{
    saveData();
    this->accept();
}

bool w_sys_manage_outer_pay_set_xiexin::insert_sql_objectname(const QString &objectname)
{
    lds_query query;
    return query.execInsert("cey_sys_parameter", qrtVariantPairList()
                            << qrtVariantPair("vch_parameter", objectname)
                            << qrtVariantPair("vch_value", ""));
}

void w_sys_manage_outer_pay_set_xiexin::tocancel()
{
    this->reject();
}

void w_sys_manage_outer_pay_set_xiexin::touploadyesterday()
{
    saveData();

    QString errstring;
    if(1 == w_scr_dish::XIEXIN_BUSINESS_DATA_UPLOAD(n_func::f_get_sysdatetime().date(), &errstring)) {
        //
        QString path = lds::localdataPath+"/XIEXIN_BUSINESS_DATA_UPLOAD";
        QDir  dir(path);
        if(!dir.exists()) dir.mkpath(path);
        QFileInfoList fileinfos=dir.entryInfoList();
        ui->comboBox_records->clear();
        foreach(QFileInfo info, fileinfos){
            if(info.fileName()=="."
                    ||info.fileName()=="..")continue;
            ui->comboBox_records->addItem(info.fileName());
        }
        //


        lds_messagebox::information(this ,MESSAGE_TITLE_VOID, tr("上传成功"));
        this->accept();
        return;
    } else {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("操作失败")+errstring);
    }
}

bool w_sys_manage_outer_pay_set_xiexin::saveData()
{
    if(n_func::f_get_sysparm("xiexin_lastdate").isEmpty())
        n_func::f_set_sysparm("xiexin_lastdate", n_func::f_get_sysdatetime().toString("yyyy-MM-dd"), "XIEXIN LAST UPLOAD DATE");
    tablemodel->trySubmitAll();

    return true;
}

void w_sys_manage_outer_pay_set::toSaaSCheck(bool check)
{
    if(!check) return;

    if(ui->checkBox_4 == this->sender()) {
        if(!saas_pay_detail::SaaSFuncCheck(this, "0101"/*手牌号*/)) {
            ui->checkBox_4->setChecked(false);
        }
    }
}

void w_sys_manage_outer_pay_set::to_liaoning_inventory_upload()
{
    w_sys_manage_outer_pay_set_liaoning_inventory_delayUpLoad dialog(this);
    lds_roundeddialog_rect_align(&dialog).exec();
}

void w_sys_manage_outer_pay_set::to_liaoning_inventory_look()
{
    QDialog *dialog = new QDialog;
    dialog->setStyleSheet("border:2px solid #ff650b");
    dialog->setWindowFlags(Qt::Popup);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    QTextEdit *edit = new QTextEdit(dialog);
    dialog->resize(500, 500);
    edit->setGeometry(0, 0, 500, 500);
    dialog->move(lds::getGlobalCenterPopup(this, dialog));

    int index = ui->comboBox_liaoning_inventory_cache->currentIndex();
    QFile file;
    file.setFileName(ui->comboBox_liaoning_inventory_cache->itemData(index).toString());
    file.open(QFile::ReadOnly);
    edit->setText(file.readAll());
    file.close();

    dialog->exec();
}

bool w_sys_manage_outer_pay_set::get_is_language_changed()
{
    return old_language != lds::get_soft_curent_language_first();
}

/////////////////////////////
#include "ui_w_sys_manage_outer_pay_set_isoft.h"
w_sys_manage_outer_pay_set_isoft::w_sys_manage_outer_pay_set_isoft(QWidget *parent)
    : lds_model_mapwidget_vertical(parent)
{
    ui = new Ui_w_sys_manage_outer_pay_set_isoft;
    ui->setupUi(this);
    //
    tablemodel = new lds_model_sqltablemodel(this);
    tablemodel->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    tablemodel->setTable("cey_sys_parameter");

    mapper = new QDataWidgetMapper(this);
    mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
    mapper->setModel(tablemodel);
    mapper->setOrientation(Qt::Vertical);

    scan_all_objectname_add_map_setFilter("vch_parameter", "isoft");
    //
    mapper->setCurrentIndex(1);

    connect(ui->pushButton_exit, SIGNAL(clicked()),this,SLOT(reject()));
    connect(ui->pushButton_ok, SIGNAL(clicked()),this,SLOT(took()));
    connect(ui->pushButton_show,SIGNAL(clicked()),this,SLOT(toshowcontent()));
    connect(ui->pushButton_reset,SIGNAL(clicked()),this,SLOT(to_reset()));

    //
    makeCache();
}

w_sys_manage_outer_pay_set_isoft::~w_sys_manage_outer_pay_set_isoft()
{
    delete ui;
}

void w_sys_manage_outer_pay_set_isoft::makeCache()
{
    ui->comboBox_upload_caches->clear();

    QString path = lds::localdataPath+"/business_data_upload_isoft";
    QDir().mkpath(path);
    QFileInfoList fileinfos=QDir(path).entryInfoList(QStringList() << "*.txt");
    foreach(const QFileInfo &info, fileinfos){
        ui->comboBox_upload_caches->addItem(info.fileName(), info.filePath());
    }
}

void w_sys_manage_outer_pay_set_isoft::took()
{
    tablemodel->trySubmitAll();
    this->accept();
}

void w_sys_manage_outer_pay_set_isoft::tocancel()
{
    this->reject();
}

void w_sys_manage_outer_pay_set_isoft::toshowcontent()
{
    int index = ui->comboBox_upload_caches->currentIndex();
    if(index >= 0) {

        QDialog *dialog = new QDialog;
        dialog->setStyleSheet("border:2px solid #ff650b");
        dialog->setWindowFlags(Qt::Popup);
        dialog->setAttribute(Qt::WA_DeleteOnClose);
        QTextEdit *edit = new QTextEdit(dialog);
        dialog->resize(500, 500);
        edit->setGeometry(0, 0, 500, 500);
        dialog->move(lds::getGlobalCenterPopup(this, dialog));

        QFile file;
        file.setFileName(ui->comboBox_upload_caches->itemData(index).toString());
        file.open(QFile::ReadOnly);
        edit->setText(file.readAll());
        file.close();

        dialog->exec();
    }
}

void w_sys_manage_outer_pay_set_isoft::to_reset()
{
    //remove rmFilePathList
    QStringList rmFilePathList;
    QString path = lds::localdataPath+"/business_data_upload_isoft";
    foreach(const QFileInfo &info, QDir(path).entryInfoList()) {
        if(info.baseName() == ""
                || info.baseName() == "."
                || info.baseName() == "..") {
            continue;
        }

        if(info.baseName().endsWith("_error")) {
            rmFilePathList << info.filePath();
        }
    }
    foreach(const QString &filepath, rmFilePathList){//删除错误新建_error;
        QFile::remove(filepath);
    }
    //重新生成上传文件
    foreach(const QFileInfo &info, QDir(path).entryInfoList()) {
        if(info.baseName() == ""
                || info.baseName() == "."
                || info.baseName() == "..") {
            continue;
        }
        business_data_upload_isoft::genrateBill(info.baseName());
    }
    //
    if(1 == lds_messagebox::information(this, MESSAGE_TITLE_VOID, tr("操作成功"), tr("上传"), tr("取消"))) {
        return;
    }

    //
    lds_messagebox_loading_show loading(this, "");
    loading.show_delay();
    QEventLoop loop;
    connect(public_sql::save_login, SIGNAL(signal_isoft_upload_polling_upload(QString)),
            &loading, SLOT(setText(QString)));
    connect(public_sql::save_login, SIGNAL(signal_isoft_upload_polling_finished(QString)),
            &loading, SLOT(setText(QString)));
    connect(public_sql::save_login, SIGNAL(signal_isoft_upload_polling_finished(QString)),
            &loop, SLOT(quit()));
    public_sql::save_login->isoft_upload_polling->emitsignalTrigger();
    loop.exec();
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, loading.text());
    makeCache();
}

bool w_sys_manage_outer_pay_set_isoft::insert_sql_objectname(const QString &objectname)
{
    lds_query query;
    return query.execInsert("cey_sys_parameter", qrtVariantPairList()
                            << qrtVariantPair("vch_parameter", objectname)
                            << qrtVariantPair("vch_value", ""));
}


QByteArray w_sys_manage_outer_pay_set_liaoning_inventory_delayUpLoad::car_name()
{
    return "顶有鸣餐车1";
}

w_sys_manage_outer_pay_set_liaoning_inventory_delayUpLoad::w_sys_manage_outer_pay_set_liaoning_inventory_delayUpLoad(QWidget *parent)
    : QDialog(parent)
{
    lds_tableView *tableview = new lds_tableView(this);
    standmodel = new lds_model_sqlstandardmodel(this);
    standmodel->setHorizontalHeaderLabels(QStringList() << tr("进度") << tr("商品") << tr("库存"));
    tableview->setModel(standmodel);
    tableview->setEditTriggers(QTableView::NoEditTriggers);
    tableview->setColumnWidth(0, 200);
    tableview->setColumnWidth(1, 250);
    tableview->setColumnWidth(3, 100);
    btn_exit =new QPushButton(this);
    btn_exit->setText(tr("退出"));
    label_desc = new QLabel(this);
    resize(800, 400);
    tableview->setGeometry(10, 10, width() - 20, height() - 20 - 50);
    btn_exit->setGeometry(width() -60, height() - 40, 50, 30);
    label_desc->setGeometry(10, height() - 40, width() - 80, 30);
    connect(btn_exit, SIGNAL(clicked()),this, SLOT(close()));
    btn_exit->hide();

    this->setWindowTitle(car_name());
    QTimer::singleShot(1000, this, SLOT(toUpLoad()));
}

void w_sys_manage_outer_pay_set_liaoning_inventory_delayUpLoad::toUpLoad()
{
    QNetworkAccessManager manager;
    lds_query query;
    QString url;
    QNetworkReply *reply = 0;
    int httpStatus;
    QByteArray readall;
    QString err_code;
    double num_num;
    QString item_name;
    int item_count;
    int item_frame;
    int item_frame_success;

    query.execSelect(" select count(0) from cey_st_material_num a, cey_bt_dish b where a.ch_materialno = b.ch_dishno; ");
    query.next();
    item_count = query.recordValue(0).toInt();

    item_frame = 0;
    item_frame_success = 0;
    query.execSelect(" select b.vch_dishname, a.num_num from cey_st_material_num a, cey_bt_dish b where a.ch_materialno = b.ch_dishno; ");
    while(query.next()) {
        item_frame ++;
        item_name = query.recordValue("vch_dishname").toString();
        num_num = query.recordValue("num_num").toDouble();
        url = QString("http://www.ddyouming.com/interface/sysdata/Item_AddMenu?") +
                "item_name=" + item_name.toLocal8Bit().toPercentEncoding() +
                "&car_name=" + car_name().toPercentEncoding() +
                "&item_num=" + QString("%1").arg(num_num).toLocal8Bit().toPercentEncoding();
        reply = w_sys_manage_cloudsync_transfer::directGet(&manager, url);
        httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        readall = reply->readAll();
        if(httpStatus == 200) {
            err_code = cJSON_help::getcJSONvalue(readall, "err_code").toString();
            if(err_code == "0" || err_code == "1") {
                item_frame_success++;
                standmodel->appendRow(QList<QStandardItem*>()
                                      << standmodel->newItem(QPixmap(":/image/widgets_pic/radio_on.png"), QString("同步成功(%1/%2)").arg(item_frame).arg(item_count))
                                      << standmodel->newItem(item_name)
                                      << standmodel->newItem(num_num));
                continue;
            }
        }
        standmodel->appendRow(QList<QStandardItem*>()
                              << standmodel->newItem(QPixmap(":/image/widgets_pic/radio_off_warn.png"), QString("同步失败(%1/%2)").arg(item_frame).arg(item_count))
                              << standmodel->newItem(item_name)
                              << standmodel->newItem(num_num));
    }
    label_desc->setText(QString("成功:%1/%2").arg(item_frame_success).arg(item_count));
    btn_exit->show();
}
