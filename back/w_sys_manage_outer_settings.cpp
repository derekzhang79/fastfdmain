#include "w_sys_manage_outer_settings.h"
#include "ui_w_sys_manage_outer_settings.h"
#include "ftableview_delegate.h"
#include "ftableview_standmodel_sql_none.h"
#include "w_sys_manage_outer_bill_preview.h"
#include "lds_messagebox.h"
#include "lds.h"
#include "commonheader.h"
#include "backheader.h"
#include "n_func.h"
#include "w_bt_dish_kitchenprinter.h"
#include "printer_0.h"
#include "clientdisplay.h"
#include "lds.h"
#include "electroniccountprice.h"
#include "w_sys_manage_wirelessordermonitoring.h"
#include "lds_tableview_delegate_com.h"
#include "w_sys_manage_outer_settings_label.h"
#include "w_bt_dish_base.h"
#include "n_func.h"
#include "w_scr_dish_barscale.h"
#include "system_setting_mobile_max_connect.h"
#include "lds_model_sqltablemodel_delegate_com.h"
#include "w_bt_dish_kitchen_dish.h"
#include "qextserialenumerator.h"
#include "public_printby_ch_billno.h"
#include <QScrollBar>
#include "getfilepath.h"
#include <QFontDatabase>
#include "w_sys_manage_outer_pay_set.h"
#include "colordialog.h"
#include "public_showimages.h"
#include <QTimer>
#include "public_logindialog.h"
#include "printer_pos_serial.h"

w_sys_manage_outer_settings::w_sys_manage_outer_settings(QWidget *parent, int fisrtIndex) :
    lds_Dialog(parent),
    ui(new Ui::w_sys_manage_outer_settings)
{
    ui->setupUi(this);
    countprice_scale = 0;
    wx_model = new QStandardItemModel(this);
    wx_model_2 = new QStandardItemModel(this);
    //1

    backlist_model = new ftableview_standmodel_sql_none(this);
    backlist_model->set_row_column(8, 1);

    ftableview_delegate *d = new ftableview_delegate(ui->tableView_list);
    d->keepConnect(ui->tableView_list);
    d->setMargin(2);
    d->indexisEmptystillDraw = true;
    ui->tableView_list->setProperty("outer_stylesheet", "tableView_type_pop");

    ui->tableView_list->setModel(backlist_model);
    ui->tableView_list->setItemDelegate(d);
    ui->tableView_list->setHeaderResizeMode(Qt::Horizontal, QHeaderView::Stretch);

    ui->tableView_list->verticalHeader()->setDefaultSectionSize(50);

    //
    tablemodel_dish_subtype = new lds_model_sqltablemodel(this);
    tablemodel_dish_subtype->setTable("cey_bt_dish_subtype");
    tablemodel_dish_subtype->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    ui->tableView_dish_subtype->setModel(tablemodel_dish_subtype);

    retranslateView();

    //connect
    connect(ui->tableView_list,SIGNAL(clicked(QModelIndex)),this,SLOT(toupdateSection(QModelIndex)));
    connect(ui->pushButton_ok, SIGNAL(clicked()), this, SLOT(took()));
    connect(ui->pushButton_cancel, SIGNAL(clicked()), this, SLOT(tocancel()));
    //3
    connect(ui->comboBox_vch_language, SIGNAL(currentIndexChanged(int)), this,SLOT(toSaaSCheck(int)));
    connect(ui->checkBox_pay_select_language, SIGNAL(clicked(bool)), this,SLOT(toSaaSCheck(bool)));
    connect(ui->pushButton_color_picker, SIGNAL(clicked()), this, SLOT(tocolorPicker()));

    ui->stackedWidget->removeWidget(ui->stackedWidget->widget(0));
    ui->tableView_list->setCurrentIndex(backlist_model->index(fisrtIndex, 0));

    toupdateSection(ui->tableView_list->currentIndex());
}

w_sys_manage_outer_settings::~w_sys_manage_outer_settings()
{
    Printer_POS_SERIAL::SERIAL_PORT_MUTEX_COM_MAP_CLOSE_ALL();
    if(countprice_scale) delete countprice_scale;
    delete ui;
}

void w_sys_manage_outer_settings::retranslateView()
{
    backlist_model->model_data_set(0, 0,tr("结账打印机"),Qt::UserRole);
    backlist_model->model_data_set(0, 0,model_normal_print,Qt::UserRole + 1);

    backlist_model->model_data_set(1, 0,tr("厨房打印机"),Qt::UserRole);
    backlist_model->model_data_set(1, 0,model_kitchen_print,Qt::UserRole + 1);

    backlist_model->model_data_set(2, 0,tr("标签打印机"),Qt::UserRole);
    backlist_model->model_data_set(2, 0,model_label_print,Qt::UserRole + 1);

    backlist_model->model_data_set(3, 0,tr("客显及副屏"),Qt::UserRole);
    backlist_model->model_data_set(3, 0,model_screen_print,Qt::UserRole + 1);

    backlist_model->model_data_set(4, 0,tr("前台计价秤"),Qt::UserRole);
    backlist_model->model_data_set(4, 0,model_price_scale,Qt::UserRole + 1);

    backlist_model->model_data_set(5, 0,tr("后台条码秤"),Qt::UserRole);
    backlist_model->model_data_set(5, 0,model_bar_scale,Qt::UserRole + 1);

    backlist_model->model_data_set(6, 0,tr("无线点菜机"),Qt::UserRole);
    backlist_model->model_data_set(6, 0,model_wireless_machine,Qt::UserRole + 1);

    backlist_model->model_data_set(7, 0,tr("平板/厨显/电话盒"),Qt::UserRole);
    backlist_model->model_data_set(7, 0,model_tablet,Qt::UserRole + 1);

    tablemodel_dish_subtype->save_set_header("ch_sub_typeno", tr("小类编码"));
    tablemodel_dish_subtype->save_set_header("vch_sub_typename", tr("小类名称"));
    tablemodel_dish_subtype->save_set_header("ch_labelprint_flag", tr("标签打印"), true);
    ui->tableView_dish_subtype->rePecifyHeader(tablemodel_dish_subtype->get_save_set_header_saver_column_list());
    ui->tableView_dish_subtype->setColumnWidth(tablemodel_dish_subtype->fieldIndex("ch_sub_typeno"), 100);
    ui->tableView_dish_subtype->setColumnWidth(tablemodel_dish_subtype->fieldIndex("vch_sub_typename"), 300);
    ui->tableView_dish_subtype->setColumnWidth(tablemodel_dish_subtype->fieldIndex("ch_labelprint_flag"), 100);

    qDebug() << __FILE__ << __LINE__;
    wx_model->setHorizontalHeaderLabels(QStringList()<<tr("基站号")<<tr("端口号"));
    wx_model_2->setHorizontalHeaderLabels(QStringList()<<tr("基站号")<<tr("点菜机号")<<tr("操作员"));


    qDebug() << __FILE__ << __LINE__;
    comIndexSaveClear(ui->comboBox_print_formate_def);
    foreach(const QFileInfo &info, QDir("userdata/settings/pint_format").entryInfoList()) {
        if(info.baseName() == ""
                || info.baseName() == "."
                || info.baseName() == ".."
                ) {
            continue;
        }

        if(saas_pay_detail::isSaaS()) {
            if(false == info.baseName().contains("saas", Qt::CaseInsensitive)) {
                continue;
            }
        }
        ui->comboBox_print_formate_def->addItem(info.fileName(), info.path());
    }


    comIndexSaveClear(ui->comboBox_fuping_mode);
    ui->comboBox_fuping_mode->addItem(tr("不启用"), -1);
    ui->comboBox_fuping_mode->addItem(tr("普通副屏"), 0);
    ui->comboBox_fuping_mode->addItem(tr("4.3吋客显"), 1);
    ui->comboBox_fuping_mode->addItem(tr("双屏异显"), 2);

    ui->comboBox_screen_type->addItem(tr("副屏尺寸"));
    ui->comboBox_screen_type->addItem(tr("主屏尺寸"));

    ui->comboBox_fuping_size->addItems(QStringList()<<tr("随主屏") << "1366x768"<< ("1024x768")<<("800x600")<<("480x800") );
    ui->comboBox_screen_primary_size->addItems(QStringList() << tr("自动获取") << "1366x768"<< ("1024x768")<<("800x600")<<("480x800") );


    comIndexSaveClear(ui->comboBox_fuping_direct);
    ui->comboBox_fuping_direct->addItems(QStringList() << tr("水平") << tr("垂直"));

    comIndexSaveClear(ui->comboBox_vch_language);
    w_sys_manage_outer_pay_set::addLanguageList(ui->comboBox_vch_language);

    comIndexSaveClear(ui->comboBox_vch_dishname_No);
    ui->comboBox_vch_dishname_No->addItem(tr("菜品名称")+"1", "vch_dishname");
    ui->comboBox_vch_dishname_No->addItem(tr("菜品名称")+"2", "vch_dishname_2");

    comIndexRestoreAll();

}

void w_sys_manage_outer_settings::toupdateSection(const QModelIndex &index)
{
    qDebug() << __FILE__ << __LINE__;
    int sqr = index.data(Qt::UserRole + 1).toInt();
    qDebug() << __FILE__ << __LINE__ << sqr;
    qDebug() << __FILE__ << __LINE__ << ui->stackedWidget->count();
    ui->stackedWidget->setCurrentIndex(sqr);
    qDebug() << __FILE__ << __LINE__;
    saveData(sqr);
}

void w_sys_manage_outer_settings::took()
{
    //点菜机打印标签保留
    lds_model_sqltablemodel *tablemodel = static_cast<lds_model_sqltablemodel*>(ui->tableView_dish_subtype->model());
    if(tablemodel) {
        lds_query::tran_saction();
        tablemodel->trySubmitAll();
        lds_query::com_mit();
    }
    //
    foreach(int index, index_set){
        if(!saveData(index, true)) {
            return;
        }
    }

    //
    if(lds::sysconf->value("system_setting/kexianduankou").toString().length() > 0
            ||lds::sysconf->value("system_setting/dianzichengduankou").toString().length() > 0) {
        if(lds::sysconf->value("system_setting/kexianduankou").toString()  == lds::sysconf->value("system_setting/dianzichengduankou").toString()) {
            if(0 == lds_messagebox::warning(0, MESSAGE_TITLE_VOID, QObject::tr("设置冲突")+"\n" +
                                            QObject::tr("前台计价秤") + ":" + lds::sysconf->value("system_setting/kexianduankou").toString() + "\n"+
                                            QObject::tr("客显") + ":" + lds::sysconf->value("system_setting/dianzichengduankou").toString(),
                                            tr("返回修改"), tr("退出"))) {
                return;
            }
        }
    }

    this->accept();
}

void w_sys_manage_outer_settings::tocancel()
{
    this->reject();
}

void w_sys_manage_outer_settings::tobillset()
{
    if(!saas_pay_detail::SaaSFuncCheck(this, "0506"/*结账打印格式*/)) {
        return;
    }
    w_sys_manage_outer_bill_preview dialog(this);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    dialog.resize(lds::MAIN_WINDOW_SIZE);
    lds_roundeddialog_rect_align(&dialog, false).exec();
}

void w_sys_manage_outer_settings::todrawerdef()
{
    ui->lineEdit_drawer_desc->setText("1B 70 00 28 28");
}

void w_sys_manage_outer_settings::todrawertest()
{
    if(ui->checkBox_drawer->isChecked()){
        blockIns bins;
        bins.append(pieceIns(ui->lineEdit_drawer_desc->text().toLocal8Bit(),""));
        backheader::syn_dataprint(bins, ui->comboBox_printer_name->comusrdata().toString());
    } else {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("开钱箱未勾选"));
    }
}

void w_sys_manage_outer_settings::toprintermanager()
{
    QString port = ui->comboBox_printer_name->comusrdata().toString();
    w_bt_dish_Kitchenprinter_printermanager dialog(this);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    lds_roundeddialog_rect_align(&dialog).exec();

    ui->comboBox_printer_name->clear();
    //2
    lds_query query;
    query.execSelect("select vch_printip, vch_printname FROM cey_bt_kitchen_print;");
    while(query.next()) {
        ui->comboBox_printer_name->addItem(query.recordValue("vch_printname").toString(), query.recordValue("vch_printip"));
    }
    //3
    ui->comboBox_printer_name->setcomusrdata(port);
}

void w_sys_manage_outer_settings::tokexiantest()
{
    static int sqr=-0;
    QString showtext;
    QString path = ui->comboBox_kexian_port->currentText();
    if(path.length() > 0) {//88888888
        showtext = QString(8, path.right(1).at(0));
    } else {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("没有端口被选择"));
        return;
    }
    public_sql::display_led.setType(ui->comboBox_kexian_type->currentText());
    if(sqr%5==0){
        if(!public_sql::display_led.startShowinfo(ui->lineEdit_kexian_runinfo->text(), path)){
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID,public_sql::display_led.lastError());
        }
    } else {
        if(!public_sql::display_led.writeCom(sqr%5, showtext, path)) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID,public_sql::display_led.lastError());
        }
    }
    sqr ++;
}

void w_sys_manage_outer_settings::toshow_kexianruninfo(int index)
{
    ui->label_17->setVisible(index == 1);
    ui->lineEdit_kexian_runinfo->setVisible(index == 1);
}

void w_sys_manage_outer_settings::tofupingreset()
{
    QFile file("userdata/save.txt");
    if(file.exists()) {
        if(file.remove()){
            lds_messagebox::information(this, MESSAGE_TITLE_VOID, tr("重置成功"));
            return;
        }
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, file.errorString());
    }
}

void w_sys_manage_outer_settings::tofuping_adv()
{
    if(!saas_pay_detail::SaaSFuncCheck(this, "0504"/*广告只能读取云端*/)) {
        return;
    }

    ///home/cz/视频/build-fastfdmain_new-Qt_4_8_6_qt4-Debug/fastfd_localdata/image_double_screen_dir
    QString beforepath = lds::sysconf->value("w_sys_manage_outer_settings/tofuping_adv", lds::localdataPath + "/fastfd_localdata/image_double_screen_dir").toString();
    QString filepath = getFilePath::getExistingDirectory(this, tr("选择文件夹"),
                                                         beforepath.isEmpty()?"/mnt":beforepath,
                                                         QFileDialog::ShowDirsOnly);
    if(!filepath.isEmpty()) {
        lds::sysconf->setValue("w_sys_manage_outer_settings/tofuping_adv", filepath);
        public_showImages dialog(filepath, this);
        if(QDialog::Accepted == lds_roundeddialog_rect_align(&dialog).exec()){
            QString failfiles;
            int successnums = 0;
            int failnums = 0;
            int skipnums = 0;
            //mkpath
            if(false == QDir(lds::localdataPath +"/image_double_screen_dir").exists()) {
                QDir().mkpath(lds::localdataPath +"/image_double_screen_dir");
            }
            //copy
            if(dialog.selected_image_path_list().isEmpty()) {
                lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("没有图片被选中"));
                return;
            }
            foreach(const QString &image_path, dialog.selected_image_path_list()) {
                QFile file(image_path);
                QString destfile = lds::localdataPath +"/image_double_screen_dir" +"/"+ QFileInfo(image_path).fileName();
                if(QFile::exists(destfile)) {
                    if( 0 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, tr("文件已存在是否替换"), tr("是"), tr("否"))) {
                        QFile::remove(destfile);
                    } else {
                        skipnums++;
                        continue;
                    }
                }
                if(false == file.copy(destfile)) {
                    failfiles += image_path + "          " +file.errorString() + "\n";
                    failnums ++;
                } else {
                    successnums ++;
                }
            }
            //error
            if(failfiles.length() > 0) {
                lds_messagebox::warning(this,
                                        tr("成功") + ":" + QString::number(successnums)+","+tr("失败") + ":" + QString::number(failnums)+","+tr("跳过") + ":" + QString::number(skipnums)
                                        , failfiles);
            } else {
                lds_messagebox::information(this, MESSAGE_TITLE_VOID, tr("操作成功"));
            }
        }
    }
}

void w_sys_manage_outer_settings::tofuping_dish()
{
    QString str = ui->lineEdit_fuping_dish_dir->text();
    QString filepath = getFilePath::getExistingDirectory(this, tr("选择文件夹"),str.isEmpty()?"/mnt":str,QFileDialog::ShowDirsOnly);
    if(!filepath.isEmpty())
        ui->lineEdit_fuping_dish_dir->setText(filepath);
}

void w_sys_manage_outer_settings::tojijia_test()
{
    if(countprice_scale == 0) {
        countprice_scale=new   ElectronicCountPrice(0);
        connect(countprice_scale,SIGNAL(weighComming(QString)),ui->lineEdit_jijia_readback,SLOT(setText(QString)));
    }
    countprice_scale->stop();
    countprice_scale->setType(ui->comboBox_jijia_type->currentText());
    countprice_scale->setBaudrate(ui->comboBox_jijia_boud->currentText().toInt());
    if(!countprice_scale->tryStart_qs(ui->comboBox_jijia_port->currentText())){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,countprice_scale->lastError());
    }
}

void w_sys_manage_outer_settings::towxsavestart(bool f)
{
    lds::sysconf->setValue("w_sys_manage_wirelessordermonitoring/start", f);
    lds_messagebox::information(this, MESSAGE_TITLE_VOID, tr("操作成功"));
}

void w_sys_manage_outer_settings::towxdel()
{
    wx_model->removeRow(ui->tableView_wx_jizhan->currentIndex().row());

}

void w_sys_manage_outer_settings::towxdel2()
{
    wx_model_2->removeRow(ui->tableView_wx_diancaiji->currentIndex().row());

}

void w_sys_manage_outer_settings::towxnew()
{
    wx_model->insertRow(wx_model->rowCount(), QList<QStandardItem*>()
                        << new QStandardItem
                        << new QStandardItem);
}

void w_sys_manage_outer_settings::towxnew2()
{
    wx_model_2->insertRow(wx_model_2->rowCount(), QList<QStandardItem*>()
                          << new QStandardItem
                          << new QStandardItem
                          << new QStandardItem
                          );
}

void w_sys_manage_outer_settings::towxinit()
{
    w_sys_manage_wirelessordermonitoring::initdata(this, wx_dir);
}

void w_sys_manage_outer_settings::towxstartjz()
{
    w_sys_manage_wirelessordermonitoring::startjz(this);
}

void w_sys_manage_outer_settings::towxclose()
{
    w_sys_manage_wirelessordermonitoring::closejz();
}

void w_sys_manage_outer_settings::towxsave()
{
    if(wxsavejizhan()){
        if(wxsavediancaiji()){
            lds_messagebox::information(this, MESSAGE_TITLE_VOID, tr("保存成功"));
            return;
        }
    }
    lds_messagebox::information(this, MESSAGE_TITLE_VOID, tr("操作失败"));

}

void w_sys_manage_outer_settings::tolabel_print()
{
    if(!saas_pay_detail::SaaSFuncCheck(this, "0507"/*标签格式*/)) {
        return;
    }
    w_sys_manage_outer_settings_label dialog(this);
    bool islabel = false;
    //奶茶模式的表头是不可以改的
    switch(lds_messagebox::question(this,MESSAGE_TITLE_VOID, tr("标签打印机"), tr("导入基础资料"), tr("进入奶茶模式"), tr("退出"))){
    case 0://导入基础资料
        dialog.printmodel->setHorizontalHeaderLabels(QStringList()
                                                     << tr("打印次数")
                                                     << tr("品名")
                                                     << tr("品码")
                                                     << tr("价格")
                                                     << tr("生产日期")
                                                     << tr("保质期")
                                                     << tr("原料")
                                                     );
        //        query.execSelect("select * from cey_bt_dish where ch_stopflag='N' ");
        //        while(query.next()){
        //            dialog.printmodel->appendRow(QList<QStandardItem*>()
        //                                         <<new QStandardItem("1")
        //                                         <<new QStandardItem(query.recordValue("vch_dishname").toString())
        //                                         <<new QStandardItem(query.recordValue("ch_dishno").toString())
        //                                         <<new QStandardItem(QString().sprintf("%0.2f", query.recordValue("num_price").toFloat()))
        //                                         <<new QStandardItem(sqldt.toString("yyyy-MM-dd"))
        //                                         <<new QStandardItem("")
        //                                         <<new QStandardItem("")
        //                                         );
        //        }
        break;
    case 1://进入奶茶模式
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
                                     //                                 <<new QStandardItem(tr("抬头"))
                                     <<new QStandardItem(tr("单号"))
                                     <<new QStandardItem(tr("价格"))
                                     //                                 <<new QStandardItem(tr("电话"))
                                     //                                 <<new QStandardItem(tr("地址"))

                                     <<new QStandardItem(tr("品名"))
                                     <<new QStandardItem(tr("品码"))
                                     <<new QStandardItem(tr("时间"))
                                     <<new QStandardItem(tr("做法"))
                                     <<new QStandardItem(tr("店名"))
                                     <<new QStandardItem(tr("数量"))
                                     );
        dialog.set_label_enabled();
        islabel = true;
        break;
    default://退出或其他
        return;
    }
    dialog.inputfinished(islabel);
    dialog.setWindowTitle(tr("标签打印机"));
    dialog.resize(lds::MAIN_WINDOW_SIZE);
    lds_roundeddialog_rect_align(&dialog, false).exec();

}

void w_sys_manage_outer_settings::tolabel_print_naicai_test()
{
    QFile::remove(CommonHeader::com_root_dirpath+"/def.txt") ;
    QFile::copy(ui->comboBox_label_paperwidth->itemData(ui->comboBox_label_paperwidth->currentIndex()).toString(), CommonHeader::com_root_dirpath+"/def.txt");
    QFile::setPermissions(CommonHeader::com_root_dirpath+"/def.txt", QFile::Permission(0x777));

    int  w = ui->comboBox_label_paperwidth->currentText().split("x").value(0).toInt();
    int  h = ui->comboBox_label_paperwidth->currentText().split("x").value(1).toInt();
    lds::sysconf->setValue("label_print/baudrate" ,9600);
    lds::sysconf->setValue("label_print/column" ,1);
    lds::sysconf->setValue("label_print/danwei" ,"mm");
    lds::sysconf->setValue("label_print/fengbianlv" ,"200DPI");
    lds::sysconf->setValue("label_print/font" ,"TSS24.BF2");

    lds::sysconf->setValue("label_print/gap=" ,0);
    lds::sysconf->setValue("label_print/height" ,h);
    lds::sysconf->setValue("label_print/width" ,w);
    lds::sysconf->setValue("label_print/istag" ,false);
    lds::sysconf->setValue("label_print/narrow_width" ,2);

    lds::sysconf->setValue("label_print/wide_width" ,2);
    lds::sysconf->setValue("label_print/onebartype" ,93);
    lds::sysconf->setValue("label_print/printnormaltype" ,0);
    lds::sysconf->setValue("label_print/printpath" ,ui->frame_label_path_select->printerport());

    public_printby_ch_billno::print_label_test(this);
}

void w_sys_manage_outer_settings::tobar_scale()
{
    w_scr_dish_barscale dialog(this);
    //获取店名
    lds_query query;
    query.execSelect("select vch_company FROM hddpos.cey_sys_company;");
    query.next();
    dialog.set_shop_name(query.recordValue("vch_company").toString());
    // 同步商品名称/商品价格
    dialog.keep(query, "select vch_dishname, num_price, '' from cey_bt_dish where vch_barcode = '%1' ");
    //设置窗口参数
    dialog.setWindowTitle(tr("条码秤"));
    dialog.resize(lds::MAIN_WINDOW_SIZE);
    lds_roundeddialog_rect_align(&dialog, false).exec();
}

void w_sys_manage_outer_settings::toimport()
{
    QString printpath = ui->comboBox_printer_name->itemData(ui->comboBox_printer_name->currentIndex()).toString();

    QString path = ui->comboBox_print_formate_def->itemData(ui->comboBox_print_formate_def->currentIndex()).toString();
    QString filename = ui->comboBox_print_formate_def->itemText(ui->comboBox_print_formate_def->currentIndex());
    QString filepath = path +"/"+filename;

    QString errstring;
    if(filepath.isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, "filepath is empty");
        return;
    }
    errstring = tr("结账打印机未设置");
    if(!printpath.isEmpty()) {
        errstring = tr("操作取消");
        if(0 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, ui->comboBox_print_formate_def->currentText(), tr("确认"), tr("取消"))) {
            lds_query query;
            lds_query::tran_saction();
            errstring = tr("操作失败");
            if(w_sys_manage_outer_bill_preview::import(filepath)){
                if(filepath.contains("58"))
                    query.execUpdate("cey_bt_kitchen_print", qrtVariantPairList()
                                     << qrtVariantPair("int_paperwidth",32),
                                     QString(" vch_printip = '%1' ").arg(printpath));
                if(filepath.contains("80"))
                    query.execUpdate("cey_bt_kitchen_print", qrtVariantPairList()
                                     << qrtVariantPair("int_paperwidth",48),
                                     QString(" vch_printip = '%1' ").arg(printpath));
                
                lds_query::com_mit();
                lds::sysconf->setValue("system_setting/print_formate_def", ui->comboBox_print_formate_def->currentIndex());
                lds::sysconf->setValue("system_setting/printerpath",ui->comboBox_printer_name->comusrdata().toString());

                lds_messagebox::information(this, MESSAGE_TITLE_VOID, tr("操作成功"));
                return;
            }
            lds_query::roll_back();
        }
    }

    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
}

void w_sys_manage_outer_settings::tomobile_connect_max_value()
{
    if(!saas_pay_detail::SaaSFuncCheck(this, "0503"/*手机最大连接数*/)) {
        return;
    }
    system_setting_mobile_max_connect dialog(this);
    dialog.setWindowTitle(tr("移动端最大连接数"));
    lds_roundeddialog_rect_align(&dialog).exec();
}

void w_sys_manage_outer_settings::tofontapp()
{
    int fid = QFontDatabase::addApplicationFont("userdata/settings/font""/"+ui->comboBox_font_select->currentText());

    if(ui->comboBox_font_select->currentText().isEmpty() || fid != -1){
        lds::sysconf->setValue("system_setting/sys_font", ui->comboBox_font_select->currentText());
        //        w_sys_manage_outer_settings::SYS_FONT_APP_byconf();
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, tr("操作成功")+","+
                                    tr("重启生效"));
        return;
    }
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("操作失败"));
}

void w_sys_manage_outer_settings::toupdatefontexample(const QString &font)
{
    if(font.isEmpty()) {
        QFont font = qApp->font();
        font.setFamily(lds::FONT_DEFAULT_Family);
        ui->lineEdit_font_example->setFont(font);
        return;
    }

    int fid = QFontDatabase::addApplicationFont("userdata/settings/font""/"+font);
    QFont f = qApp->font();
    if(fid != -1){
        f.setFamily(QFontDatabase::applicationFontFamilies(fid).first());
    }
    ui->lineEdit_font_example->setFont(f);
}

void w_sys_manage_outer_settings::toSaaSCheck(int index)
{
    if(ui->comboBox_vch_language == this->sender()) {
        if(!ui->comboBox_vch_language->curusrdata().toString().isEmpty()) {
            if(!saas_pay_detail::SaaSFuncCheck(this, "0512"/*多语言*/)) {
                ui->comboBox_vch_language->setcomusrdata("");
            }
        }
    }
}

void w_sys_manage_outer_settings::toSaaSCheck(bool check)
{
    if(!check) return;

    if(ui->checkBox_pay_select_language == this->sender()) {
        if(!saas_pay_detail::SaaSFuncCheck(this, "0512"/*多语言*/)) {
            ui->checkBox_pay_select_language->setChecked(false);
        }
    }
}

void w_sys_manage_outer_settings::SYS_FONT_APP_byconf()
{
    int fid = QFontDatabase::addApplicationFont("userdata/settings/font""/"+lds::sysconf->value("system_setting/sys_font").toString());
    if(fid != -1) {
        QFont font = qApp->font();
        font.setFamily(lds::FONT_DEFAULT_Family);
        qApp->setFont(font);
        // << __LINE__ << "初始化字体成功";
    } else {
        // << __LINE__ << "初始化字体失败，默认字体";
    }
}

bool w_sys_manage_outer_settings::saveData(int index, bool issave)
{
    qDebug() << __FILE__ << __LINE__ << index;
    lds_query query;
    qDebug() << __FILE__ << __LINE__;
    switch(index) {
    case model_normal_print://小票格式
        if(issave) {
            lds::sysconf->setValue("system_setting/printerpath",ui->comboBox_printer_name->comusrdata().toString());
            lds::sysconf->setValue("system_setting/maidanzhangdan",ui->checkBox_pay_print->isChecked()?"1":"0");
            lds::sysconf->setValue("system_setting/pay_select_language",ui->checkBox_pay_select_language->isChecked());
            lds::sysconf->setValue("system_setting/maidan_language",ui->comboBox_vch_language->comusrdata());

            n_func::f_set_sysparm_q(query, "bp_vch_dishname_No", ui->comboBox_vch_dishname_No->comusrdata());
            n_func::f_set_sysparm_q(query, "bp_count", ui->lineEdit_pay_print_count->text());
            n_func::f_set_sysparm_q(query, "bp_togo_count", ui->lineEdit_pay_waimai_count->text());
            //本机落单打印总单
            lds::sysconf->setValue("system_setting/luodanzongdan", ui->checkBox_9->isChecked()?"1":"0");

            //结帐打开钱箱
            lds::sysconf->setValue("system_setting/qianxiang",ui->checkBox_drawer->isChecked());
            lds::sysconf->setValue("system_setting/qianxiangzhiling",ui->lineEdit_drawer_desc->text());
            lds::sysconf->setValue("system_setting/drawer_timeout",ui->comboBox_drawer_timeout->curusrdata());

            return true;
        }
        if(index_set.find(index) == index_set.end()) {
            //结帐打印机
            if(ui->comboBox_printer_name->count() == 0) {
                query.execSelect("select vch_printip, vch_printname FROM cey_bt_kitchen_print;");
                while(query.next()) {
                    ui->comboBox_printer_name->addItem(query.recordValue("vch_printname").toString(), query.recordValue("vch_printip"));
                }
            }
            ui->comboBox_printer_name->setcomusrdata(lds::sysconf->value("system_setting/printerpath", "/dev/usb/lp0").toString());
            ui->checkBox_pay_print->setChecked(lds::sysconf->value("system_setting/maidanzhangdan").toString() == "1");
            ui->checkBox_pay_select_language->setChecked(lds::sysconf->value("system_setting/pay_select_language", false).toBool());
            ui->comboBox_vch_language->setcomusrdata(lds::sysconf->value("system_setting/maidan_language").toString());

            ui->comboBox_vch_dishname_No->setcomusrdata(n_func::f_get_sysparm_q(query, "bp_vch_dishname_No", "vch_dishname"));
            ui->lineEdit_pay_print_count->setText(n_func::f_get_sysparm_q(query,"bp_count"));
            ui->lineEdit_pay_waimai_count->setText(n_func::f_get_sysparm_q(query,"bp_togo_count"));
            //本机落单打印总单
            ui->checkBox_9->setChecked(lds::sysconf->value("system_setting/luodanzongdan").toString() == "1");

            //结帐打开钱箱
            ui->checkBox_drawer->setChecked(lds::sysconf->value("system_setting/qianxiang").toBool());
            ui->lineEdit_drawer_desc->setText(lds::sysconf->value("system_setting/qianxiangzhiling", "1B 70 00 28 28").toString());

            ui->comboBox_print_formate_def->setCurrentIndex(lds::sysconf->value("system_setting/print_formate_def").toInt());

            ui->comboBox_drawer_timeout->addItem("500ms", 500);
            ui->comboBox_drawer_timeout->addItem("1000ms", 1000);
            ui->comboBox_drawer_timeout->addItem("1500ms", 1500);
            ui->comboBox_drawer_timeout->setcomusrdata(lds::sysconf->value("system_setting/drawer_timeout", 1000).toInt());

            connect(ui->pushButton_import,SIGNAL(clicked()),this,SLOT(toimport()));

            connect(ui->pushButton_bill, SIGNAL(clicked()),this,SLOT(tobillset()));
            connect(ui->pushButton_drawer_def, SIGNAL(clicked()), this,SLOT(todrawerdef()));
            connect(ui->pushButton_drawer_test, SIGNAL(clicked()), this,SLOT(todrawertest()));
            connect(ui->pushButton_printer_manager,SIGNAL(clicked()),this,SLOT(toprintermanager()));
        }
        break;
        ///////////////////////////

    case model_kitchen_print://厨房打印机
    {
        if(issave) {
            return true;
        }
        //        if(backheader::operid_right_code(this, "0103"))
        {//菜品厨打设置【 0103 】
            QString oldlanguage = lds::get_soft_curent_language_first();
            w_bt_dish_kitchen_dish dialog(this);
            dialog.setObjectName("w_bt_dish_kitchen_dish");
            dialog.setWindowTitle(tr("厨房打印机"));
            dialog.resize(lds::MAIN_WINDOW_SIZE);
            dialog.move(0, 0);
            dialog.refresh();
            dialog.exec();
            if(oldlanguage != lds::get_soft_curent_language_first()) {
                retranslateView();
                ui->retranslateUi(this);
            }
            ui->tableView_list->setCurrentIndex(ui->tableView_list->model()->index(0, 0));
            ui->stackedWidget->setCurrentIndex(0);
        }
    }
        break;
        ////////////////////////////
    case model_label_print://标价签
        if(issave) {
            lds::sysconf->setValue("system_setting/label_print_path",ui->frame_label_path_select->printerport());
            lds::sysconf->setValue("system_setting/label_print_type",ui->comboBox_label_print_type->currentIndex());
            lds::sysconf->setValue("system_setting/label_print_paper",ui->comboBox_label_paperwidth->currentIndex());
            return true;
        }
        if(index_set.find(index) == index_set.end()) {
            ui->comboBox_label_paperwidth->addItem("40x30", ":/image/print_format/label_40x30_def.txt");
            ui->comboBox_label_paperwidth->addItem("60x40", ":/image/print_format/label_60x40_def.txt");
            ui->comboBox_label_print_type->addItems(QStringList() << "Gainscha");

            ui->frame_label_path_select->setPrinterport(lds::sysconf->value("system_setting/label_print_path").toString());
            ui->comboBox_label_print_type->setCurrentIndex(lds::sysconf->value("system_setting/label_print_type").toInt());
            ui->comboBox_label_paperwidth->setCurrentIndex(lds::sysconf->value("system_setting/label_print_paper").toInt());

            tablemodel_dish_subtype->select();

            ui->tableView_dish_subtype->setEditTriggers(QTableView::AllEditTriggers);
            ui->tableView_dish_subtype->setItemDelegate(new lds_tableview_delegate_check(QVector<int>()<<3,  ui->tableView_dish_subtype, true));

            connect(ui->pushButton_label_print, SIGNAL(clicked()), this, SLOT(tolabel_print()));
            connect(ui->pushButton_label_test, SIGNAL(clicked()), this, SLOT(tolabel_print_naicai_test()));
            connect(ui->pushButton_label_dish_subtype_up, SIGNAL(clicked()), ui->tableView_dish_subtype, SLOT(toPageUp()));
            connect(ui->pushButton_label_dish_subtype_down, SIGNAL(clicked()), ui->tableView_dish_subtype, SLOT(toPageDown()));
            connect(ui->pushButton_label_dish_subtype_checkun, SIGNAL(clicked()), this, SLOT(toLabelDishCheckUn()));
            connect(ui->pushButton_label_dish_subtype_checkall, SIGNAL(clicked()),  this, SLOT(toLabelDishCheckAll()));
        }
        break;
        ///////////////////////////

    case model_screen_print://客显与副屏
        if(issave) {
            lds::sysconf->setValue("system_setting/kexianleixing",ui->comboBox_kexian_type->currentText());
            lds::sysconf->setValue("system_setting/kexianduankou",ui->comboBox_kexian_port->currentText());

            lds::sysconf->setValue("system_setting/kexian_vfd_start",ui->lineEdit_kexian_runinfo->text());

            //副屏
            lds::sysconf->setValue("system_setting/bofanglujing",ui->lineEdit_fuping_dish_dir->text());
            lds::sysconf->setValue("system_setting/qt_show_dish_pic",ui->checkBox_qt_show_dish_pic->isChecked());
            lds::sysconf->setValue("system_setting/bofangg_runningtext",ui->lineEdit_fuping_runtext->text());
            lds::sysconf->setValue("system_setting/bofangg_runningtext_color",ui->lineEdit_fuping_runtext_color->text());
            lds::sysconf->setValue("system_setting/bofangg_runningtext_size",ui->lineEdit_fuping_runtext_font->text());

            lds::sysconf->setValue("system_setting/bofangg_rate",ui->comboBox_fuping_size->currentText());
            lds::sysconf->setValue("system_setting/bofangg_rote",ui->comboBox_fuping_rotate->currentText());

            lds::sysconf->setValue("system_setting/bofang_layout_dir",ui->comboBox_fuping_direct->currentIndex());
            lds::sysconf->setValue("system_setting/fuping_mode",ui->comboBox_fuping_mode->curusrdata());
            lds::sysconf->setValue("system_setting/dd_show_dish_list", ui->checkBox_dd_show_dish_list->isChecked());

            lds::sysconf->setValue("system_setting/screen_primary_size", ui->comboBox_screen_primary_size->currentText());

            return true;
        }
        if(index_set.find(index) == index_set.end()) {
            ui->comboBox_kexian_type->addItems(CommonHeader::creat_clientdisplay_ledtypes());
            ui->comboBox_kexian_type->setCurrentIndex(ui->comboBox_kexian_type->findText(lds::sysconf->value("system_setting/kexianleixing").toString()));
            ui->comboBox_kexian_port->addItems(printer_0::f_scan_ttyPort(true));
            ui->comboBox_kexian_port->setCurrentIndex(ui->comboBox_kexian_port->findText(lds::sysconf->value("system_setting/kexianduankou").toString()));

            ui->lineEdit_kexian_runinfo->setText(lds::sysconf->value("system_setting/kexian_vfd_start").toString());
            connect(ui->pushButton_kexian_test,SIGNAL(clicked()),this,SLOT(tokexiantest()));
            toshow_kexianruninfo(ui->comboBox_kexian_type->currentIndex());
            connect(ui->comboBox_kexian_type, SIGNAL(currentIndexChanged(int)),this, SLOT(toshow_kexianruninfo(int)));

            //副屏
            ui->lineEdit_fuping_dish_dir->setText(lds::sysconf->value("system_setting/bofanglujing").toString());
            ui->checkBox_qt_show_dish_pic->setChecked(lds::sysconf->value("system_setting/qt_show_dish_pic", true).toBool());
            ui->lineEdit_fuping_runtext->setText(lds::sysconf->value("system_setting/bofangg_runningtext").toString());
            ui->lineEdit_fuping_runtext_color->setText(lds::sysconf->value("system_setting/bofangg_runningtext_color", "white").toString());
            QPixmap p(30,30);
            p.fill(QColor(ui->lineEdit_fuping_runtext_color->text()));ui->pushButton_color_picker->setIcon(p);
            ui->lineEdit_fuping_runtext_font->setText(lds::sysconf->value("system_setting/bofangg_runningtext_size", "80").toString());

            setEditText(ui->comboBox_fuping_size, lds::sysconf->value("system_setting/bofangg_rate", tr("随主屏")).toString());
            ui->comboBox_fuping_mode->setcomusrdata(lds::sysconf->value("system_setting/fuping_mode", -1));
            ui->checkBox_dd_show_dish_list->setChecked(lds::sysconf->value("system_setting/dd_show_dish_list", true).toBool());

            ui->comboBox_fuping_rotate->addItems(QStringList()<<"0"<<"90"<<"180"<<"270");
            setEditText(ui->comboBox_fuping_rotate, QString::number(lds::sysconf->value("system_setting/bofangg_rote", 0).toInt()));

            ui->comboBox_fuping_direct->setCurrentIndex(lds::sysconf->value("system_setting/bofang_layout_dir", 0).toInt());
            setEditText(ui->comboBox_screen_primary_size, lds::sysconf->value("system_setting/screen_primary_size", tr("自动获取")).toString());
            //            connect(ui->pushButton_fuping_reset, SIGNAL(clicked()),this,SLOT(tofupingreset()));


            connect(ui->toolButton_fuping_adv, SIGNAL(clicked()),this,SLOT(tofuping_adv()));
            connect(ui->toolButton_fuping_dish, SIGNAL(clicked()),this,SLOT(tofuping_dish()));
            connect(ui->comboBox_fuping_mode, SIGNAL(currentIndexChanged(int)), this, SLOT(tofuping_4_3cun(int)));
            connect(ui->comboBox_screen_type, SIGNAL(currentIndexChanged(int)), this, SLOT(tofuping_screen_index(int)));


            lds::hideWidget(ui->label_62); lds::hideWidget(ui->lineEdit_fuping_runtext);
            lds::hideWidget(ui->label_63); lds::hideWidget(ui->lineEdit_fuping_runtext_color);lds::hideWidget(ui->pushButton_color_picker);
            lds::hideWidget(ui->label_66); lds::hideWidget(ui->lineEdit_fuping_runtext_font);lds::hideWidget(ui->label_65);
        }
        break;
        ///////////////////////////
    case model_price_scale://计价秤
        if(issave) {
            lds::sysconf->setValue("system_setting/dianzichengxinghao",ui->comboBox_jijia_type->currentText());
            lds::sysconf->setValue("system_setting/dianzichengduankou",ui->comboBox_jijia_port->currentText());
            lds::sysconf->setValue("system_setting/dianzicheng_baudrate",ui->comboBox_jijia_boud->currentText());
            return true;
        }
        if(index_set.find(index) == index_set.end()) {
            ui->comboBox_jijia_type->addItems(CommonHeader::creat_pricecount_scaletypes());
            ui->comboBox_jijia_type->setCurrentIndex(ui->comboBox_jijia_type->findText(lds::sysconf->value("system_setting/dianzichengxinghao").toString()));

            ui->comboBox_jijia_port->addItems(printer_0::f_scan_ttyPort());
            ui->comboBox_jijia_port->setCurrentIndex(ui->comboBox_jijia_port->findText(lds::sysconf->value("system_setting/dianzichengduankou").toString()));

            ui->comboBox_jijia_boud->addItems(CommonHeader::creat_baudrates().values());
            ui->comboBox_jijia_boud->setCurrentIndex(ui->comboBox_jijia_boud->findText(lds::sysconf->value("system_setting/dianzicheng_baudrate").toString()));

            connect(ui->pushButton_jijia_test, SIGNAL(clicked()),this,SLOT(tojijia_test()));
        }
        break;
        ///////////////////////////
    case model_bar_scale://条码秤
        if(issave) {
            return true;
        }
        tobar_scale();
        ui->tableView_list->setCurrentIndex(ui->tableView_list->model()->index(0, 0));
        ui->stackedWidget->setCurrentIndex(0);
        break;
        ///////////////////////////
    case model_wireless_machine://无线点菜机
        if(!saas_pay_detail::SaaSFuncCheck(this, "0502"/*点菜宝*/)) {
            ui->tableView_list->setCurrentIndex(ui->tableView_list->model()->index(0, 0));
            ui->stackedWidget->setCurrentIndex(0);
            return true;
        }
        if(index_set.find(index) == index_set.end()) {
            wx_dir = qApp->applicationDirPath();
            ui->tableView_wx_jizhan->setModel(wx_model);
            ui->tableView_wx_jizhan->setColumnWidth(0, 60);
            ui->tableView_wx_jizhan->horizontalHeader()->setStretchLastSection(true);
            ui->tableView_wx_jizhan->setEditTriggers(QTableView::AllEditTriggers);
            ui->tableView_wx_diancaiji->setModel(wx_model_2);
            ui->tableView_wx_diancaiji->setColumnWidth(0, 60);
            ui->tableView_wx_diancaiji->horizontalHeader()->setStretchLastSection(true);
            ui->tableView_wx_diancaiji->setEditTriggers(QTableView::AllEditTriggers);
            //
            QStandardItemModel *model = new QStandardItemModel(this);
            QList<QStandardItem *> items;
            QList<QextPortInfo> ports = QextSerialEnumerator::getPorts();
            foreach(const QextPortInfo &port, ports){
                items.clear();items << new QStandardItem(port.portName) << new QStandardItem(port.portName);model->appendRow(items);
            }

            lds_tableview_delegate_com *comdelegat = new lds_tableview_delegate_com(model, 1, ui->tableView_wx_jizhan);
            ui->tableView_wx_jizhan->setItemDelegate(comdelegat);

            lds_tableview_delegate_com *delegate_2 = new lds_tableview_delegate_com(wx_model,0, ui->tableView_wx_diancaiji);
            ui->tableView_wx_diancaiji->setItemDelegate(delegate_2);
            delegate_2->setonlyk(true);

            //
            QDir d(wx_dir+"/TXT");
            if(!d.exists()){
                d.mkdir(d.path());
            }
            wx_init_jizhan();
            wx_init_diancaiji();

            //
            connect(ui->pushButton_wx_save,SIGNAL(clicked()),this,SLOT(towxsave()));
            connect(ui->pushButton_wx_jizhan_del,SIGNAL(clicked()),this,SLOT(towxdel()));
            connect(ui->pushButton_wx_diancaiji_del,SIGNAL(clicked()),this,SLOT(towxdel2()));
            connect(ui->pushButton_wx_jizhan_new,SIGNAL(clicked()),this,SLOT(towxnew()));
            connect(ui->pushButton_wx_diancaiji_new,SIGNAL(clicked()),this,SLOT(towxnew2()));
            connect(ui->pushButton_wx_initdata,SIGNAL(clicked()),this,SLOT(towxinit()));
            connect(ui->pushButton_wx_startjz,SIGNAL(clicked()),this,SLOT(towxstartjz()));
            connect(ui->pushButton_wx_close,SIGNAL(clicked()),this,SLOT(towxclose()));
            ui->checkBox_wx->setChecked(lds::sysconf->value("w_sys_manage_wirelessordermonitoring/start", false).toBool());
            connect(ui->checkBox_wx,SIGNAL(toggled(bool)),this,SLOT(towxsavestart(bool)));

        }
        break;

        /////////
    case model_tablet:
    {
        //平板设置
        if(index_set.find(index) == index_set.end()) {
            connect(ui->pushButton_mobile_connect_max_value, SIGNAL(clicked()),this,SLOT(tomobile_connect_max_value()));
        }
        //电话盒子
        if(issave) {
            lds::sysconf->setValue("system_setting/read_tel_enable",ui->checkBox_read_tel_enable->isChecked());
            return true;
        }

        if(index_set.find(index) == index_set.end()) {
            ui->checkBox_read_tel_enable->setChecked(lds::sysconf->value("system_setting/read_tel_enable", false).toBool());
            connect(ui->pushButton_read_tel_test, SIGNAL(clicked()), this, SLOT(to_read_tel_test()));
            //线程后台没有运行,使用定时器辅助
            //read_tel_enable 控制后台的电话盒是否正在运行
            if(false == lds::sysconf->value("system_setting/read_tel_enable", false).toBool()){
                connect(&read_tel_timer, SIGNAL(timeout()), this, SLOT(to_read_tel_readdata()));
            }
            ui->pushButton_read_tel_test->setText(tr("开始测试") + "...");
        }
    }
        break;
    case model_font:
    {
        if(index_set.find(index) == index_set.end()) {
            QDir dir("userdata/settings/font");
            if(!dir.exists()) {
                dir.mkpath(dir.path());
            }
            ui->comboBox_font_select->addItem("");
            QFileInfoList infolist = dir.entryInfoList(QStringList() << "*.ttf" << "*.TTF");
            foreach(const QFileInfo &info, infolist) {
                if(info.baseName() == "."
                        || info.baseName() == "..") continue;

                ui->comboBox_font_select->addItem(info.fileName());
            }
            ui->comboBox_font_select->setCurrentIndex(ui->comboBox_font_select->findText( lds::sysconf->value("system_setting/sys_font").toString()));
            connect(ui->pushButton_font_app,SIGNAL(clicked()),this,SLOT(tofontapp()));
            connect(ui->comboBox_font_select, SIGNAL(currentIndexChanged(QString)),this,SLOT(toupdatefontexample(QString)));
        }
    }
        break;
    }

    qDebug() << __FILE__ << __LINE__;
    index_set.insert(index);
    qDebug() << __FILE__ << __LINE__;
    return true;
}

void w_sys_manage_outer_settings::setEditText(QComboBox *com, const QString &editText)
{
    int index = com->findText(editText);
    if(index >= 0)
        com->setCurrentIndex(index);
    else
        com->setEditText(editText);
}

bool w_sys_manage_outer_settings::wxsavejizhan()
{
    QString value;
    QString info;
    QMultiMap<QString,int > map;
    for(int row = 0, rowcount = wx_model->rowCount();row<rowcount;row++){
        value = wx_model->item(row, 0)->text();
        if(value.length() != 1){
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QString("[%1]").arg(row)+tr("基站号只能为1位"));
            return false;
        }
        if(value.toInt() == 0){
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QString("[%1]").arg(row)+tr("基站号必须为数字且不能为0"));
            return false;
        }
        map.insert(value, row);
    }
    ///重复数据
    info = "";
    foreach(const QString &k, map.keys().toSet()) {
        int count = map.count(k);
        if(count > 1) {
            QMapIterator<QString,int > mapitr(map);
            while(mapitr.hasNext()) {
                mapitr.next();
                if(mapitr.key() == k) {
                    info += QString("[%1%2]").arg(mapitr.value()  + 1).arg(tr("行"));
                    count --;
                }
                if(count <= 0) break;
            }
        }
    }
    if(!info.isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("基站号")+ tr("不能重复")+info);
        return false;
    }
    ///~重复数据


    QFile file;
    file.setFileName(wx_dir+_TXT_jz_txt);
    if(!file.open(QFile::WriteOnly)){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, file.errorString());
        return false;
    }

    for(int row = 0, rowcount = wx_model->rowCount(); row < rowcount; row++){
        QByteArray line;
        line += wx_model->data(wx_model->index(row, 0)).toString() + " ";
        line += wx_model->data(wx_model->index(row, 1)).toString() + "\r\n";
        file.write(line);

    }
    file.close();
    return true;
}

bool w_sys_manage_outer_settings::wxsavediancaiji()
{
    QMultiMap<QString,int > map;
    QString info;
    QString value;
    for(int row = 0, rowcount = wx_model_2->rowCount();row<rowcount;row++){
        value = wx_model_2->item(row, 1)->text();
        if(value.length() != 3){
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QString("[%1]").arg(row+1)+tr("点菜机号只能为3位"));
            return false;
        }
        if(value.toInt() == 0){
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QString("[%1]").arg(row+1)+tr("点菜机号必须为数字且不能为0"));
            return false;
        }

        map.insert(value, row);
    }


    ///重复数据
    info = "";
    foreach(const QString &k, map.keys().toSet()) {
        int count = map.count(k);
        if(count > 1) {
            QMapIterator<QString,int > mapitr(map);
            while(mapitr.hasNext()) {
                mapitr.next();
                if(mapitr.key() == k) {
                    info += QString("[%1%2]").arg(mapitr.value()  + 1).arg(tr("行"));
                    count --;
                }
                if(count <= 0) break;
            }
        }
    }
    if(!info.isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("点菜机号") + tr("不能重复")+info);
        return false;
    }
    ///~重复数据


    QFile file;
    file.setFileName(wx_dir+_TXT_dcj_txt);
    if(!file.open(QFile::WriteOnly)){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, file.errorString());
        return false;
    }

    for(int row = 0, rowcount = wx_model_2->rowCount(); row < rowcount; row++){
        QByteArray line;
        line += wx_model_2->data(wx_model_2->index(row, 1)).toString() + " ";
        line += wx_model_2->data(wx_model_2->index(row, 0)).toString() + "\r\n";
        file.write(line);
    }
    file.close();
    return true;
}

void w_sys_manage_outer_settings::wx_init_jizhan()
{
    QFile file;
    file.setFileName(wx_dir+_TXT_jz_txt);
    if(!file.open(QFile::ReadOnly)){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("未找到基站文件,请先[新增基站]"));
        return;
    }

    while(!file.atEnd()){
        QString readline = file.readLine();
        QStringList line = readline.trimmed().split(" ");
        wx_model->appendRow(QList<QStandardItem*>()
                            << new QStandardItem(line.value(0)) << new QStandardItem(line.value(1)));
    }
    file.close();
}

void w_sys_manage_outer_settings::wx_init_diancaiji()
{
    QFile file;
    file.setFileName(wx_dir+_TXT_dcj_txt);
    if(!file.open(QFile::ReadOnly)){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("未找到点菜机文件,请先[新增点菜机]"));
        return;
    }

    while(!file.atEnd()){
        QString readline = file.readLine();
        QStringList line = readline.trimmed().split(" ");
        wx_model_2->appendRow(QList<QStandardItem*>()
                              << new QStandardItem(line.value(1)) << new QStandardItem(line.value(0)));
    }
    file.close();
}

void w_sys_manage_outer_settings::tocolorPicker()
{
    colordialog dialog(this);
    dialog.setWindowTitle(tr("选择颜色"));
    if(QDialog::Accepted == lds_roundeddialog_rect_align(&dialog).exec()) {
        ui->lineEdit_fuping_runtext_color->setText(dialog.result_color_name());
        QPixmap p(30,30);p.fill(QColor(ui->lineEdit_fuping_runtext_color->text()));ui->pushButton_color_picker->setIcon(p);
    }
}

void w_sys_manage_outer_settings::to_read_tel_test()
{
    if(read_tel_timer.isActive()) {
        read_tel_timer.stop();
        ui->pushButton_read_tel_test->setText(tr("停止测试"));
        //这边有后台有运行电话盒子和未运行的两种，但是两种都只要disconnect即可
        disconnect(public_sql::save_login_object(), SIGNAL(signal_telNoComingState(QString)), ui->plainTextEdit_read_tel, SLOT(appendPlainText(QString)));

    } else{
        read_tel_timer.start(public_sql::save_login->runTelBoxTimeout * 1000);
        ui->pushButton_read_tel_test->setText(tr("开始测试") + "...");
        connect(public_sql::save_login_object(), SIGNAL(signal_telNoComingState(QString)), ui->plainTextEdit_read_tel, SLOT(appendPlainText(QString)));
    }
    ui->plainTextEdit_read_tel->appendPlainText(ui->pushButton_read_tel_test->text());
}

void w_sys_manage_outer_settings::to_read_tel_readdata()
{
    TelBox_thread_event::readTel();
}

void w_sys_manage_outer_settings::toLabelDishCheckUn()
{
    lds_model_sqltablemodel *tablemodel = static_cast<lds_model_sqltablemodel*>(ui->tableView_dish_subtype->model());
    if(tablemodel)
        for(int row = 0; row < tablemodel->rowCount(); row ++)
            tablemodel->model_data_set(row, "ch_labelprint_flag", "N");
}

void w_sys_manage_outer_settings::toLabelDishCheckAll()
{
    lds_model_sqltablemodel *tablemodel = static_cast<lds_model_sqltablemodel*>(ui->tableView_dish_subtype->model());
    if(tablemodel)
        for(int row = 0; row < tablemodel->rowCount(); row ++)
            tablemodel->model_data_set(row, "ch_labelprint_flag", "Y");
}

void w_sys_manage_outer_settings::tofuping_4_3cun(int index)
{
    if(ui->comboBox_fuping_mode->comusrdata(index).toInt() == 1) {//4.3吋客显
        ui->comboBox_fuping_size->setCurrentIndex(0);
        ui->comboBox_fuping_rotate->setCurrentIndex(2);
        ui->comboBox_fuping_direct->setCurrentIndex(0);
        ui->comboBox_screen_primary_size->setCurrentIndex(0);
    }
}

void w_sys_manage_outer_settings::tofuping_screen_index(int index)
{
    switch(index) {
    case 0://副屏
        ui->comboBox_screen_primary_size->hide();
        ui->comboBox_fuping_size->show();
        break;
    case 1://主屏
        ui->comboBox_screen_primary_size->show();
        ui->comboBox_fuping_size->hide();
        break;
    }
}
