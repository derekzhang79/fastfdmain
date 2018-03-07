#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "commonheader.h"
#include "printer_0.h"
#include "printer_normal_t2us.h"
#include "printer_pos_serial.h"
#include "printer_pos_net.h"
#include "printer_pos.h"
#include "printer_pos_vidpid.h"
#include <QMessageBox>
#include "electroniccountprice.h"
#include "clientdisplay.h"
#include "reader_rf.h"
#include "labelprint_main_dialog.h"
#include "barscale_main_dialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    countprice_scale(0)
{
    ui->setupUi(this);
    //1
    ui->comboBox_3->addItems(CommonHeader::creat_baudrates().values());
    ui->comboBox_2->addItems(printer_0::f_scan_ttyPort());
    ui->comboBox_4->addItems(printer_0::f_scan_usbPort());

    //2
    ui->comboBox_5->addItems(CommonHeader::creat_pricecount_scaletypes());
    ui->comboBox_6->addItems(CommonHeader::creat_baudrates().values());

    //3
    ui->comboBox_7->addItems(CommonHeader::creat_clientdisplay_ledtypes());
    ui->comboBox_8->addItems(printer_0::f_scan_ttyPort());
    ui->comboBox_9->addItems(printer_0::f_scan_ttyPort());
    //4
    CommonHeader::com_ini_filepath=qApp->applicationDirPath()+"/"+appName()+".ini";
    CommonHeader::com_root_dirpath=qApp->applicationDirPath();
    //    CommonHeader::com_widget_dir=Qt::RightToLeft;

    this->setLayoutDirection(CommonHeader::com_widget_dir);
    ui->tabWidget->setCurrentIndex(0);
}

MainWindow::~MainWindow()
{
    delete ui;
    if(countprice_scale)delete countprice_scale;
}

QString MainWindow::appName()
{
    return QFileInfo(qApp->applicationFilePath()).fileName();
}

void MainWindow::on_comboBox_currentIndexChanged(int index)
{
    switch(index){
    case 0://58mm热敏
    case 1://80mm热敏
    case 2://KF58
        ui->groupBox->setEnabled(true);
        ui->groupBox_2->setEnabled(true);
        ui->groupBox_3->setEnabled(true);
        ui->groupBox_4->setEnabled(false);
        break;
    case 3://MINI58
        ui->groupBox->setEnabled(false);
        ui->groupBox_2->setEnabled(false);
        ui->groupBox_3->setEnabled(false);
        ui->groupBox_4->setEnabled(true);
        ui->lineEdit_3->setText(Printer_Normal_T2US().vidpid);
        break;
    case 4://XINYE
        break;
    }
}

void MainWindow::on_groupBox_toggled(bool arg1)
{
    if(arg1){
        ui->groupBox_4->setChecked(false);
        ui->groupBox_2->setChecked(false);
        ui->groupBox_3->setChecked(false);
    }
}
void MainWindow::on_groupBox_2_toggled(bool arg1)
{
    if(arg1){
        ui->groupBox_4->setChecked(false);
        ui->groupBox->setChecked(false);
        ui->groupBox_3->setChecked(false);
    }
}
void MainWindow::on_groupBox_3_toggled(bool arg1)
{
    if(arg1){
        ui->groupBox_4->setChecked(false);
        ui->groupBox_2->setChecked(false);
        ui->groupBox->setChecked(false);
    }
}
void MainWindow::on_groupBox_4_toggled(bool arg1)
{
    if(arg1){
        ui->groupBox->setChecked(false);
        ui->groupBox_2->setChecked(false);
        ui->groupBox_3->setChecked(false);
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    blockIns ins;
    ins.append(pieceIns("", "=====test successed!====\n"));
    QString path;
    if(ui->groupBox->isChecked()){
        path = ui->comboBox_2->currentText();
    }
    if(ui->groupBox_2->isChecked()){
        path = ui->comboBox_4->currentText();
    }
    if(ui->groupBox_3->isChecked()){
        path = ui->lineEdit->text()+":"+ui->lineEdit_2->text();
    }
    if(ui->groupBox_4->isChecked()){
        path = ui->lineEdit_3->text();
    }
    if(path.isEmpty()) {
        QMessageBox::warning(this, tr("小票打印错误"), tr("没有任何端口类型被选中"));
        return;
    }
}

void MainWindow::on_pushButton_3_clicked()
{
    if(countprice_scale == 0) {
        countprice_scale=new   ElectronicCountPrice(0, "", "");
        connect(countprice_scale,SIGNAL(weighComming(QString)),ui->lineEdit_4,SLOT(setText(QString)));
        connect(countprice_scale,SIGNAL(signalsuccessrate(int,int)),this,SLOT(tocountpricerate(int, int)));
    }
    countprice_scale->stop();
    countprice_scale->setType(ui->comboBox_5->currentText());
    countprice_scale->setBaudrate(ui->comboBox_6->currentText());
    if(!countprice_scale->tryStart_qs(ui->comboBox_9->currentText(), ui->lineEdit_pricescale_timeout->text().toInt())){
        QMessageBox::warning(this, MESSAGE_TITLE_VOID,countprice_scale->lastError());
    }
}

void MainWindow::on_comboBox_7_currentIndexChanged(int index)
{
    ui->lineEdit_5->setEnabled(index==1);
}

void MainWindow::on_pushButton_4_clicked()
{
    static int sqr=-0;
    ClientDisplay display(ui->comboBox_7->currentText());
    if(sqr%5==0){
        if(!display.startShowinfo(ui->lineEdit_5->text(), ui->comboBox_8->currentText())){
            QMessageBox::warning(this, tr("客显错误"),display.lastError());
        }
    } else {
        if(!display.writeCom(sqr%5, ui->lineEdit_6->text())){
            QMessageBox::warning(this, tr("客显错误"),display.lastError());
        }
    }
    sqr ++;
}

void MainWindow::on_pushButton_5_clicked()
{
#ifdef Q_OS_WIN
    ReadCard_mwrf32 reader;reader.setDefPwd(n_func::f_m_defcardpwd);//需要设置一次默认密码，因为changepwd后不会完全清卡
#else
    Reader_RF reader;
#endif
    reader.beep();
}

void MainWindow::on_pushButton_7_clicked()
{
    labelprint_main_dialog dialog(this);
    dialog.printmodel->setHorizontalHeaderLabels(QStringList() << tr("打印次数")
                                                 <<  tr("品名")
                                                 <<  tr("品码")
                                                 <<  tr("价格"));
    dialog.printmodel->appendRow(QList<QStandardItem*>()
                                 << new QStandardItem("1")
                                 << new QStandardItem("cookie")
                                 << new QStandardItem("1000")
                                 << new QStandardItem("-12.6")

                                 );
    dialog.inputfinished();
    dialog.setWindowTitle(tr("条码打印机"));
    dialog.exec();
}

void MainWindow::tocountpricerate(int cur, int total)
{
    ui->lineEdit_countpricerate->setText(QString("%1%(%2/%3)").arg(cur*1.0/total*100).arg(cur).arg(total));
}


void MainWindow::on_pushButton_hex_test_clicked()
{
    QString path = ui->lineEdit_hex_path->text();
    Printer_POS *printer = CommonHeader::getposprint_bypath(path);
    if(printer){
        if(printer->tryOpen(path)){
            printer->writeHex(ui->lineEdit_hex_content->text());
            printer->close();
        }
        delete printer;
    }
}
