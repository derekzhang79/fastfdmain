#include "public_debugdialog.h"
#include "ui_public_debugdialog.h"
#include <QProcess>

QTableView *public_debugDialog::tableview=0;
QStandardItemModel *public_debugDialog::model=0;
public_debugDialog::public_debugDialog(QWidget *parent) :
    lds_dialog_drog(parent),
    ui(new Ui::public_debugDialog)
{
    ui->setupUi(this);
    setWindowFlags(this->windowFlags() | Qt::FramelessWindowHint|Qt::Tool);
    this->setAttribute(Qt::WA_DeleteOnClose);
    connect(&timer,SIGNAL(timeout()),this,SLOT(on_pushButton_clicked()));
    timer.setInterval(1000);
    timer.start();
    ui->pushButton->hide();
    ui->lineEdit->hide();
}

public_debugDialog::~public_debugDialog()
{
    delete ui;
}

void public_debugDialog::on_pushButton_clicked()
{
    QProcess p;
    QString out;
    QStringList outs;
    p.start("free");
    if(p.waitForReadyRead(5*1000)){
        out = p.readAll();
    }
    outs= out.split(" ", QString::SkipEmptyParts);
    if(!outs.isEmpty()){
        ui->tableWidget->item(0,0)->setText(outs[0]);
        ui->tableWidget->item(0,1)->setText(outs[1]);
        ui->tableWidget->item(0,2)->setText(outs[2]);
        ui->tableWidget->item(0,3)->setText(outs[3]);
        ui->tableWidget->item(0,4)->setText(outs[4]);

        ui->tableWidget->item(1,0)->setText(outs[6]);
        ui->tableWidget->item(1,1)->setText(outs[7]);
        ui->tableWidget->item(1,2)->setText(outs[8]);
        ui->tableWidget->item(1,3)->setText(outs[9]);
        ui->tableWidget->item(1,4)->setText(outs[10]);
    }
    p.close();
    this->raise();

    if(tableview == 0)tableview = new QTableView;
    if(model == 0){model = new QStandardItemModel;
        tableview->setModel(model);
    }
    tableview->show();
    tableview->raise();

}

void public_debugDialog::on_lineEdit_editingFinished()
{
    timer.setInterval(qMax(100, ui->lineEdit->text().toInt()));
}
