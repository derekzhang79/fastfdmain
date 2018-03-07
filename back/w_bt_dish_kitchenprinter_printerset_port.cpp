#include "w_bt_dish_kitchenprinter_printerset_port.h"
#include "ui_w_bt_dish_kitchenprinter_printerset_port.h"
#include "commonheader.h"
#include "printer_0.h"
#include "printer_pos_vidpid.h"
#include "printer_pos_serial.h"
#include "ui_w_bt_dish_unitset_dialog.h"
#include "lds_messagebox.h"
#include "printer_pos_parallel.h"

w_bt_dish_kitchenprinter_printerset_port::w_bt_dish_kitchenprinter_printerset_port(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::w_bt_dish_kitchenprinter_printerset_port)
{
    ui->setupUi(this);

    ui->comboBox_porttype->addItems(CommonHeader::creat_normal_printpathtypes());
    ui->comboBox_port_tty->addItems(printer_0::f_scan_ttyPort());
    ui->comboBox_port_usb->addItems(printer_0::f_scan_usbPort());
    ui->comboBox_baud->addItems(CommonHeader::creat_baudrates().values());
    ui->comboBox_flowcontrol->addItem(tr("无流控"), FLOW_OFF);
    ui->comboBox_flowcontrol->addItem("XONXOFF", FLOW_XONXOFF);

    connect(ui->comboBox_porttype, SIGNAL(currentIndexChanged(int)),this,SLOT(toporttype(int)));
    connect(ui->pushButton_usb_vp, SIGNAL(clicked()),this,SLOT(toselectvidpidlist()));
    refreshPort();


}

w_bt_dish_kitchenprinter_printerset_port::~w_bt_dish_kitchenprinter_printerset_port()
{
    delete ui;
}

void w_bt_dish_kitchenprinter_printerset_port::setPrinterport(const QString &port)
{
    //1
    if(printer_0::comPortIsValid(port)){
        QStringList list = port.split(",");
        ui->comboBox_port_tty->setCurrentIndex(ui->comboBox_port_tty->findText(list.value(0)));
        ui->comboBox_baud->setCurrentIndex(ui->comboBox_baud->findText(list.value(1)));
        ui->comboBox_porttype->setCurrentIndex(1);
        ui->comboBox_flowcontrol->setCurrentIndex(ui->comboBox_flowcontrol->findData(list.value(5).toInt()));
    } else if(printer_0::usbPortIsValid(port)) {
        ui->comboBox_port_usb->setCurrentIndex(ui->comboBox_port_usb->findText(port));
        if(ui->comboBox_port_usb->currentIndex() == -1) {
            ui->comboBox_porttype->setCurrentIndex(0);
        }
        if(port.contains("&")){
            ui->comboBox_port_usb->setEditText(port);
        }
    } else if(port.count(".") == 3){
        QStringList list = port.split(":");
        ui->lineEdit_ip->setText(list.value(0));
        ui->lineEdit_port->setText(list.value(1));
        ui->comboBox_porttype->setCurrentIndex(2);
    } else if(port == Printer_POS_PARALLEL::printPort()) {
        ui->comboBox_porttype->setCurrentIndex(3);
    }

    //2
    toporttype(ui->comboBox_porttype->currentIndex());
}

QString w_bt_dish_kitchenprinter_printerset_port::printerport()
{
    QString p;
    if(ui->comboBox_port_usb->isVisible()&&!ui->comboBox_port_usb->currentText().isEmpty()){
        p += ui->comboBox_port_usb->currentText();
    }
    if(ui->comboBox_port_tty->isVisible()&&!ui->comboBox_port_tty->currentText().isEmpty()){
        p += ui->comboBox_port_tty->currentText();
    }

    if(ui->comboBox_baud->isVisible()&&!ui->comboBox_baud->currentText().isEmpty()){
        p += ","+ui->comboBox_baud->currentText();
    }
    if(ui->comboBox_flowcontrol->isVisible() && !ui->comboBox_flowcontrol->currentText().isEmpty()) {
        p += QString(5-p.count(","), ',')+QString::number(ui->comboBox_flowcontrol->itemData(ui->comboBox_flowcontrol->currentIndex()).toInt());
    }

    if(ui->lineEdit_ip->isVisible()&&!ui->lineEdit_ip->text().isEmpty()){
        p += ui->lineEdit_ip->text();
    }

    if(ui->lineEdit_port->isVisible()&&!ui->lineEdit_port->text().isEmpty()){
        p += ":"+ui->lineEdit_port->text();
    }

    return p;
}

void w_bt_dish_kitchenprinter_printerset_port::refreshPort()
{
    toporttype(ui->comboBox_porttype->currentIndex());
}

void w_bt_dish_kitchenprinter_printerset_port::set_com_usb_enable(bool e)
{
    ui->comboBox_port_usb->setEnabled(e);
}

void w_bt_dish_kitchenprinter_printerset_port::toporttype(int index)
{
    switch(index){
    case 0://USB
        ui->label_baud->hide();ui->label_baud->setText(tr("波特率"));
        ui->comboBox_baud->hide();
        ui->lineEdit_ip->hide();ui->label_port->setText(tr("端口"));
        ui->lineEdit_port->hide();
        ui->comboBox_port_tty->hide();
        ui->comboBox_port_usb->show();
        ui->comboBox_port_usb->setEnabled(true);
        ui->pushButton_usb_vp->show();
        ui->comboBox_flowcontrol->hide();
        ui->label_flowcontrol->hide();
        ui->lineEdit_port->setText("");
#ifdef Q_OS_WIN
        ui->pushButton_usb_vp->hide();
#endif
        break;
    case 1://串口
        ui->label_baud->show();ui->label_baud->setText(tr("波特率"));
        ui->comboBox_baud->show();
        ui->lineEdit_ip->hide();ui->label_port->setText(tr("端口"));
        ui->lineEdit_port->hide();
        ui->comboBox_port_tty->show();
        ui->comboBox_port_usb->hide();
        ui->comboBox_port_usb->setEnabled(true);
        ui->pushButton_usb_vp->hide();
        ui->comboBox_flowcontrol->show();
        ui->label_flowcontrol->show();
        ui->lineEdit_port->setText("");
        break;
    case 2://网口
        ui->label_baud->show();ui->label_baud->setText(tr("端口"));
        ui->comboBox_baud->hide();
        ui->lineEdit_ip->show();ui->label_port->setText("IP");
        ui->lineEdit_port->show();
        ui->comboBox_port_tty->hide();
        ui->comboBox_port_usb->hide();
        ui->comboBox_port_usb->setEnabled(true);
        ui->pushButton_usb_vp->hide();
        ui->comboBox_flowcontrol->hide();
        ui->label_flowcontrol->hide();
        ui->lineEdit_port->setText("9100");
        if(ui->lineEdit_ip->text().isEmpty()) {
            ui->lineEdit_ip->setText("192.168.");
            ui->lineEdit_ip->setFocus();
        }
        break;
    case 3://并口
        ui->label_baud->hide();ui->label_baud->setText(tr("波特率"));
        ui->comboBox_baud->hide();
        ui->lineEdit_ip->hide();ui->label_port->setText(tr("端口"));
        ui->lineEdit_port->hide();
        ui->comboBox_port_tty->hide();
        ui->comboBox_port_usb->show();
        ui->comboBox_port_usb->setEnabled(false);
        ui->pushButton_usb_vp->hide();
        ui->comboBox_flowcontrol->hide();
        ui->label_flowcontrol->hide();
        ui->comboBox_port_usb->setEditText(Printer_POS_PARALLEL::printPort());
        break;
    }

}

void w_bt_dish_kitchenprinter_printerset_port::toselectvidpidlist()
{
    QDialog dialog(this);
    Ui_w_bt_dish_unitset_Dialog mui;
    mui.setupUi(&dialog);
    QStandardItemModel model;
    model.setHorizontalHeaderLabels(QStringList() << "VID" << "PID" << tr("端口号"));
    mui.tableView->setModel(&model);
    mui.tableView->horizontalHeader()->setStretchLastSection(true);
    mui.tableView->setEditTriggers(QTableView::NoEditTriggers);
    QList<Printer_POS_VIDPID::vidpid_Data>  dlist = Printer_POS_VIDPID::get_vidpid_print_type_list();
    foreach(const Printer_POS_VIDPID::vidpid_Data &d, dlist){
        model.appendRow(QList<QStandardItem*>()
                        << new QStandardItem<< new QStandardItem<< new QStandardItem
                        );
        model.setData(model.index(model.rowCount() - 1, 0), QString().sprintf("%d", d.idVendor));
        model.setData(model.index(model.rowCount() - 1, 1), QString().sprintf("%d", d.idProduct));
        model.setData(model.index(model.rowCount() - 1, 2), QString().sprintf("%d", d.device_address));
    }

    mui.widget->setup(QStringList() << tr("确定") << tr("取消"), Qt::AlignRight);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    connect(mui.widget->index_widget(tr("确定")), SIGNAL(clicked()),&dialog, SLOT(accept()));
    connect(mui.widget->index_widget(tr("取消")), SIGNAL(clicked()),&dialog, SLOT(reject()));
    if(QDialog::Accepted == lds_roundeddialog_rect_align(&dialog).exec()) {
        int row = mui.tableView->currentIndex().row();
        if(row >= 0) {
            ui->comboBox_port_usb->setEditText(QString("%1&%2").arg(model.item(row, 0)->text()).arg(model.item(row, 1)->text()));
        }
    }
}

void w_bt_dish_kitchenprinter_printerset_port::resizeEvent(QResizeEvent *e)
{
    QFrame::resizeEvent(e);
    int off = this->height() - 80;
    ui->pushButton_usb_vp->setGeometry(QRect(370, off + 40, 150, 28));
    ui->lineEdit_port->setGeometry(QRect(370, off + 40, 150, 28));
    ui->comboBox_port_tty->setGeometry(QRect(120, off + 40, 150, 28));
    ui->comboBox_port_usb->setGeometry(QRect(120, off + 40, 150, 28));
    ui->label_port->setGeometry(QRect(10, off + 40, 80, 28));
    ui->label_baud->setGeometry(QRect(290, off + 40, 80, 28));
    ui->label_flowcontrol->setGeometry(QRect(290, off + 10, 80, 28));
    ui->lineEdit_ip->setGeometry(QRect(120, off + 40, 150, 28));
    ui->comboBox_baud->setGeometry(QRect(370, off + 40, 150, 28));
    ui->comboBox_flowcontrol->setGeometry(QRect(370, off + 10, 150, 28));
    ui->comboBox_porttype->setGeometry(QRect(120, off + 10, 150, 28));
    ui->label_6->setGeometry(QRect(10, off + 10, 80, 28));
}
