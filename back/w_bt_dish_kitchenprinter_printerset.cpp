#include "w_bt_dish_kitchenprinter_printerset.h"
#include "ui_w_bt_dish_kitchenprinter_printerset.h"
#include "commonheader.h"
#include "printer_0.h"
#include "printer_normal_t2us.h"
#include "lds_messagebox.h"
#include "lds_query.h"
#include "backheader.h"
#include "lds_pushbuttonlock.h"
#include "lds_tableview.h"
#include "ui_w_bt_dish_unitset_dialog.h"
#include "printer_pos_vidpid.h"
#include "public_printby_ch_billno.h"
#include "n_func.h"
#include "frontviewdialog_virtual.h"

w_bt_dish_Kitchenprinter_printerSet0::w_bt_dish_Kitchenprinter_printerSet0(QWidget *parent)
    : QDialog(parent)
{
    ui = new Ui_w_bt_dish_Kitchenprinter_printerSet;
    ui->setupUi(this);

    //1
    {
        ui->comboBox_paperwidth->addItem("80mm", 48);
        ui->comboBox_paperwidth->addItem("58mm", 32);
    }
    //2
    backheader::fillData(ui->comboBox_printtype, &Printer_Normal::get_normal_print_map_ref());
    backheader::fillData(ui->comboBox_instype, &Printer_Normal::get_normal_ins_map_ref());

    connect(ui->comboBox_printtype,SIGNAL(currentIndexChanged(int)),this,SLOT(toprinttype(int)));
    connect(ui->pushButton_ok,SIGNAL(clicked()),this,SLOT(took()));
    connect(ui->pushButton_exit,SIGNAL(clicked()),this,SLOT(toexit()));
    connect(ui->pushButton_test,SIGNAL(clicked()),this,SLOT(totest()));
    connect(ui->pushButton_nextpage,SIGNAL(clicked()),this,SLOT(tonextpage()));
    connect(ui->pushButton_uppage,SIGNAL(clicked()),this,SLOT(touppage()));
    connect(ui->comboBox_ch_areano, SIGNAL(currentIndexChanged(int)),this,SLOT(toSaaSCheck(int)));

    ui->lineEdit_no->setEnabled(false);
    ui->pushButton_nextpage->hide();
    ui->pushButton_uppage->hide();
}

w_bt_dish_Kitchenprinter_printerSet0::~w_bt_dish_Kitchenprinter_printerSet0()
{
    delete ui;
}


void w_bt_dish_Kitchenprinter_printerSet0::initData(const QString &no, const QString &name, const QString &printtype, int instype, const QString &port, int paperwidth, const QString &ch_areano, int xinye_beep)
{
    setPrinterport(port);
    if(!no.isEmpty())ui->lineEdit_no->setText(no);
    if(!name.isEmpty())ui->lineEdit_name->setText(name);
    ui->comboBox_printtype->setCurrentIndex(ui->comboBox_printtype->findData(printtype));
    if(ui->comboBox_printtype->currentIndex() == -1) {
        ui->comboBox_printtype->setCurrentIndex(0);
    }
    ui->comboBox_instype->setCurrentIndex(ui->comboBox_instype->findData(instype));

    if(paperwidth != 0) {//exist
        ui->comboBox_paperwidth->setEditText(QString::number(paperwidth));//def
        for(int row = 0, rowcount = ui->comboBox_paperwidth->count(); row < rowcount ; row++){//find
            if(ui->comboBox_paperwidth->itemData(row).toInt() == paperwidth){
                ui->comboBox_paperwidth->setCurrentIndex(row);
                break;
            }
        }
    }

    ui->comboBox_ch_areano->setCurrentIndex(ui->comboBox_ch_areano->findData(ch_areano));
    toprinttype(ui->comboBox_printtype->currentIndex());
    ui->spinBox_XINYE_beep->setValue(xinye_beep);
}

QString w_bt_dish_Kitchenprinter_printerSet0::getno()
{
    return ui->lineEdit_no->text();
}

QString w_bt_dish_Kitchenprinter_printerSet0::getname()
{
    return ui->lineEdit_name->text();
}

QString w_bt_dish_Kitchenprinter_printerSet0::getprinttype()
{
    return ui->comboBox_printtype->itemData(ui->comboBox_printtype->currentIndex()).toString();
}

int w_bt_dish_Kitchenprinter_printerSet0::getinstype()
{
    return ui->comboBox_instype->itemData(ui->comboBox_instype->currentIndex()).toInt();
}

QString w_bt_dish_Kitchenprinter_printerSet0::getport()
{
    return port;
}

int w_bt_dish_Kitchenprinter_printerSet0::getpaperwidth()
{
    if(ui->comboBox_paperwidth->findText(ui->comboBox_paperwidth->currentText()) < 0){//not find
        return ui->comboBox_paperwidth->currentText().toInt();
    }
    //find
    return ui->comboBox_paperwidth->itemData(ui->comboBox_paperwidth->currentIndex()).toInt();
}

QString w_bt_dish_Kitchenprinter_printerSet0::get_ch_areano()
{
    return ui->comboBox_ch_areano->itemData(ui->comboBox_ch_areano->currentIndex()).toString();
}

int w_bt_dish_Kitchenprinter_printerSet0::get_xinye_beep()
{
    return ui->spinBox_XINYE_beep->value();
}

QString w_bt_dish_Kitchenprinter_printerSet0::printerport()
{
    return ui->frame_port_select->printerport();
}

void w_bt_dish_Kitchenprinter_printerSet0::setPrinterport(const QString &port)
{
    ui->frame_port_select->setPrinterport(port);
}

void w_bt_dish_Kitchenprinter_printerSet0::toprinttype(int index)
{
    QString print_class_name = ui->comboBox_printtype->itemData(index).toString();
    ui->frame_port_select->refreshPort();
    ui->frame_port_select->set_com_usb_enable(true);

    ui->label_XINYE_beep->hide();
    ui->spinBox_XINYE_beep->hide();
    ui->label_XINYE_desc->hide();
    if("Printer_Normal_T2US" == print_class_name) {
        ui->frame_port_select->setPrinterport(Printer_Normal_T2US().vidpid);
        ui->frame_port_select->set_com_usb_enable(false);
    }
    if("Printer_Normal_XINYE" == print_class_name) {
        ui->label_XINYE_beep->show();
        ui->spinBox_XINYE_beep->show();
        ui->label_XINYE_desc->show();
    }
}

void w_bt_dish_Kitchenprinter_printerSet0::took()
{
    port =printerport();
    if(trysave(getno(), getname(), getprinttype(),getinstype(),getport())){
        this->accept();
    }
}

bool w_bt_dish_Kitchenprinter_printerSet0::trysave(const QString &int_id, const QString &vch_printname, const QString & vch_printtype, int vch_instype, const QString &vch_printip)
{
    if(int_id.isEmpty()){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("打印机编号不能为空"));
        return false;
    }
    if(vch_printname.isEmpty()){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("打印机名字不能为空"));
        return false;
    }
    if(vch_printtype.isEmpty()){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("打印机类型为空"));
        return false;
    }
    if(vch_instype < 0){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("打印类型为空"));
        return false;
    }
    if(vch_printip.isEmpty()){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("打印端口为空"));
        return false;
    }
    return true;
}

void w_bt_dish_Kitchenprinter_printerSet0::toexit()
{
    this->reject();
}

void w_bt_dish_Kitchenprinter_printerSet0::totest()
{
    lds_pushbuttonlock b(qobject_cast<QWidget *>(this->sender()));
    b.toLock();
    backheader::syn_dataprin_test_byresolveself(printerport(), getinstype(), getprinttype(),getpaperwidth(), get_xinye_beep());
}

void w_bt_dish_Kitchenprinter_printerSet0::tonextpage()
{

}

void w_bt_dish_Kitchenprinter_printerSet0::touppage()
{

}

void w_bt_dish_Kitchenprinter_printerSet0::toSaaSCheck(int index)
{
}




w_bt_dish_Kitchenprinter_printerSet::w_bt_dish_Kitchenprinter_printerSet(QWidget *parent) :
    w_bt_dish_Kitchenprinter_printerSet0(parent)
{
    QSqlQueryModel querymodel;
    querymodel.setQuery(" SELECT ch_areaNo, vch_areaname FROM cey_bt_table_area ");
    backheader::fillData(ui->comboBox_ch_areano, &querymodel, true);
}

void w_bt_dish_Kitchenprinter_printerSet::toSaaSCheck(int index)
{
    if(!ui->comboBox_ch_areano->curusrdata().toString().isEmpty()) {
        if(!saas_pay_detail::SaaSFuncCheck(this, "0509"/*区域厨打*/)) {
            ui->comboBox_ch_areano->setcomusrdata("");
            return;
        }
    }
}

bool w_bt_dish_Kitchenprinter_printerSet::trysave(const QString &int_id, const QString &vch_printname, const QString &vch_printtype, int vch_instype, const QString &vch_printip)
{
    if(false == w_bt_dish_Kitchenprinter_printerSet0::trysave(int_id, vch_printname, vch_printtype, vch_instype, vch_printip)) {
        return false;
    }
    lds_query query;
    query.execSelect(QString("select count(0) from cey_bt_kitchen_print where vch_printname = '%1' and int_id <> '%2' ")
               .arg(vch_printname)
               .arg(int_id));
    query.next();
    if(query.recordValue(0).toLongLong() > 0){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("打印机名称有重复"));
        return false;
    }
    query.execSelect(QString("select count(0) from cey_bt_kitchen_print where vch_printip = '%1'  and int_id <> '%2' ")
               .arg(vch_printip)
               .arg(int_id));
    query.next();
    if(query.recordValue(0).toLongLong() > 0){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("打印端口有重复"));
        return false;
    }
    return true;
}
/////////////////////////////////////
QMutex w_bt_dish_Kitchenprinter_printer_thread_event::mutex;
w_bt_dish_Kitchenprinter_printer_thread::w_bt_dish_Kitchenprinter_printer_thread(QObject *parent)
    : lds_thread("结账打印", parent)
{
}

void w_bt_dish_Kitchenprinter_printer_thread::emitsignalTrigger(const QString &bp_ch_billno, const QString &bp_languageKey, int label_print_int_flowid_max, int pay_print_mode)
{
    //    //主线程打印
    //    public_printby_ch_billno print_ch_billno(bp_ch_billno, bp_languageKey);
    //    print_ch_billno.toPayPrint(print_ch_billno.defpath(), n_func::f_get_sysparm("bp_count", "1").toInt(), label_print_int_flowid_max, (public_printby_ch_billno::payPrintMode)pay_print_mode);
    //    return;
    //后台打印
    w_bt_dish_Kitchenprinter_printer_thread_event::mutex.lock();
    emit signalTrigger(bp_ch_billno, bp_languageKey, label_print_int_flowid_max, pay_print_mode);
}

void w_bt_dish_Kitchenprinter_printer_thread::run()
{
    w_bt_dish_Kitchenprinter_printer_thread_event event;
    connect(this, SIGNAL(signalTrigger(QString,QString,int, int)), &event, SLOT(toprintBill(QString,QString,int, int)));
    this->exec();
}


w_bt_dish_Kitchenprinter_printer_thread_event::w_bt_dish_Kitchenprinter_printer_thread_event(QObject *parent)
    : lds_thread_event(parent)
{

}

void w_bt_dish_Kitchenprinter_printer_thread_event::toprintBill(const QString &bp_ch_billno, const QString &bp_languageKey, int label_print_int_flowid_max, int pay_print_mode)
{
    public_printby_ch_billno print_ch_billno(bp_ch_billno, bp_languageKey);
    print_ch_billno.toPayPrint(print_ch_billno.defpath(), n_func::f_get_sysparm("bp_count", "1").toInt(), label_print_int_flowid_max, (public_printby_ch_billno::payPrintMode)pay_print_mode);
    w_bt_dish_Kitchenprinter_printer_thread_event::mutex.unlock();
}

void w_bt_dish_Kitchenprinter_printer_thread_event::toExec()
{
    //do nothing
}

////////////////////////////////////////
QMutex w_bt_dish_Kitchenprinter_display_thread_event::mutex;

w_bt_dish_Kitchenprinter_display_thread::w_bt_dish_Kitchenprinter_display_thread(QObject *parent)
    : lds_thread("客显线程", parent)
{

}

void w_bt_dish_Kitchenprinter_display_thread::emitsignalTrigger(int sqr, const QString &value, const QString &vch_dishname)
{
    if(w_bt_dish_Kitchenprinter_display_thread_event::mutex.tryLock()) {
        emit signalTrigger(sqr, value, vch_dishname);
    }
}

void w_bt_dish_Kitchenprinter_display_thread::run()
{
    w_bt_dish_Kitchenprinter_display_thread_event event;
    connect(this, SIGNAL(signalTrigger(int,QString,QString)), &event, SLOT(toDisplay(int,QString,QString)));
    this->exec();
}

w_bt_dish_Kitchenprinter_display_thread_event::w_bt_dish_Kitchenprinter_display_thread_event(QObject *parent)
    : lds_thread_event(parent)
{

}

void w_bt_dish_Kitchenprinter_display_thread_event::toDisplay(int sqr, const QString &value, const QString &vch_dishname)
{
    frontViewDialog_virtual::static_display(sqr, value, vch_dishname);
    w_bt_dish_Kitchenprinter_display_thread_event::mutex.unlock();
}

void w_bt_dish_Kitchenprinter_display_thread_event::toExec()
{
    //do nothing
}
