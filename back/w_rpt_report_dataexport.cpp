#include "w_rpt_report_dataexport.h"
#include "ui_w_rpt_report_dataexport.h"
#include <QStandardItemModel>
#include <QFile>
#include <QtDebug>
#include "lds_messagebox.h"
#include <QFileDialog>
#include "public_sql.h"
#include "lds.h"
#include "getfilepath.h"
#include "lds.h"
#include "lds_query.h"
#include "w_sys_manage_sqlcopy.h"
#include "w_rpt_report_frame.h"
#include <QSqlRecord>
#include "lds_tableview_header.h"

static void SQL_REPORT_EXPORT(w_rpt_report_dataExport *receiver, const QString &path, lds_tableView *content_tableview,lds_tableView *tail_tableview , char check){

    bool headexport = check&0x01;
    bool checkdata = check&0x02;
    QFile file(path);
    if(!file.open(QFile::WriteOnly)){
        //打开错误
        receiver->export_info_progress(file.errorString(), 0, 1);
        return;
    }

    QString line;
    int ccount = content_tableview->horizontalHeader()->count();
    //导出表头
    if(headexport){
        for(int c = 0; c < ccount ; c++){
           QString d = content_tableview->hh_paint_transfer(c);

            //!核实数据----begin
            d=d.replace(",", "");//去掉逗号
            if(checkdata){
                QString tmpd;
                foreach(const QChar &c, d){//去掉null
                    if(!c.isNull()){
                        tmpd += c;
                    }
                }
                d = tmpd;
            }
            //!核实数据----end

            line += d+",";
        }
        line.chop(1);
        line += "\n";
        file.write(line.toLocal8Bit());
        line.clear();
        receiver->export_info_progress(QObject::tr("导出表头"), 1, 1);
    }

    //导出内容
    for(int r = 0, rcount = content_tableview->verticalHeader()->count(); r < rcount; r++){
        QString d;
        for(int c = 0; c < ccount ; c++){
            //data, headerdata, tableview, column
            d = content_tableview->dg_paint_transfer(r, c);

            d=d.replace(",", "");//去掉逗号
            if(checkdata){
                QString tmpd;
                foreach(const QChar &c, d){//去掉null
                    if(!c.isNull()){
                        tmpd += c;
                    }
                }
                d = tmpd;
            }
            //!核实数据----end

            line += d+",";
        }
        line.chop(1);
        line += "\n";
        file.write(line.toLocal8Bit());
        line.clear();
        receiver->export_info_progress(QObject::tr("导出表单"), r+1, rcount);
    }
    //合计
    if(tail_tableview) {
        ccount = tail_tableview->horizontalHeader()->count();
        int rcount = tail_tableview->verticalHeader()->count();
        for(int r = 0; r < rcount; r++) {
            for(int c = 0; c < ccount ; c++){
                QString d = tail_tableview->dg_paint_transfer(r, c);
                //!核实数据----begin
                d=d.replace(",", "");//去掉逗号
                if(checkdata){
                    QString tmpd;
                    foreach(const QChar &c, d){//去掉null
                        if(!c.isNull()){
                            tmpd += c;
                        }
                    }
                    d = tmpd;
                }
                //!核实数据----end

                line += d+",";
            }
            line.chop(1);
            line += "\n";
            file.write(line.toLocal8Bit());
            line.clear();
            receiver->export_info_progress(QObject::tr("导出表尾"), r, rcount);
        }
    }
    receiver->export_info_progress(QObject::tr("导出结束"), 1, 1);
}

static void SQL_REPORT_EXPORT_BY_SQL(w_rpt_report_dataExport *receiver, lds_tableView *content_tableview,  const QString &path, const QString &sql , char check)
{
    bool headexport = check&0x01;
    bool checkdata = check&0x02;
    checkdata = true;
    QFile file(path);
    if(!file.open(QFile::WriteOnly)){
        //打开错误
        receiver->export_info_progress(file.errorString(), 0, 1);
        return;
    }
    QString d;
    QString line;
    lds_query query;
    query.execSelect("select count(0) from ("+sql+")t");
    query.next();
    int rowcount =query.recordValue(0).toLongLong();
    int going = 0;
    query.execSelect(sql);
    while(query.next()) {
        //
        if(headexport) {
            int qcount = query.recordCount();
            for(int qindex = 0; qindex < qcount; qindex ++) {
                d = content_tableview->hh_paint_transfer(qindex);

                //!核实数据----begin
                d=d.replace(",", "");//去掉逗号
                if(checkdata){
                    QString tmpd;
                    foreach(const QChar &c, d){//去掉null
                        if(!c.isNull()){
                            tmpd += c;
                        }
                    }
                    d = tmpd;
                }
                //!核实数据----end
                line += d+",";
            }
            line.chop(1);
            line += "\n";
            file.write(line.toLocal8Bit());
            line.clear();
            headexport = false;


            receiver->export_info_progress(QObject::tr("导出表头"), 1, 1);
        }

        //内容
        going ++;
        int qcount = query.recordCount();
        for(int qindex = 0; qindex < qcount; qindex ++) {
            //data, headerdata, tableview, column
            d = content_tableview->dg_paint_transfer(query.recordValue(qindex), QString::number(qindex));
            //!核实数据----begin
            d=d.replace(",", "");//去掉逗号
            if(checkdata){
                QString tmpd;
                foreach(const QChar &c, d){//去掉null
                    if(!c.isNull()){
                        tmpd += c;
                    }
                }
                d = tmpd;
            }
            //!核实数据----end
            line += d+",";
        }
        line.chop(1);
        line += "\n";
        file.write(line.toLocal8Bit());
        line.clear();
        receiver->export_info_progress(QObject::tr("导出表单"), going, rowcount);
    }
    file.close();
    receiver->export_info_progress(QObject::tr("导出结束"), 1, 1);
}

w_rpt_report_dataExport::w_rpt_report_dataExport(lds_tableView *content_tableview, lds_tableView *tail_tableview, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_rpt_report_dataExport)
{
    ui->setupUi(this);
    this->content_tableview = content_tableview;
    this->tail_tableview = tail_tableview;

    //数据初始化
    ui->progressBar->setValue(0);
    ui->label_path->setEditText(lds::sysconf->value("w_rpt_report_dataExport/path").toString());
    ui->checkBox_checkdata->setChecked(lds::sysconf->value("w_rpt_report_dataExport/checkdata", false).toBool());
    ui->checkBox_exportheader->setChecked(lds::sysconf->value("w_rpt_report_dataExport/exportheader",true).toBool());

    connect(ui->pushButton_exit,SIGNAL(clicked()),this,SLOT(toexit()));
    connect(ui->pushButton_export,SIGNAL(clicked()),this,SLOT(toexport()));
    connect(ui->toolButton_getfilepath,SIGNAL(clicked()),this,SLOT(togetfilepath()));
    connect(ui->pushButton_umount_upan, SIGNAL(clicked()),this,SLOT(toumount_upan()));
}


w_rpt_report_dataExport::~w_rpt_report_dataExport()
{
    delete ui;
}

void w_rpt_report_dataExport::defFilepath(const QString &tabtext)
{
    ui->label_path->addItems(QStringList()
                         #ifdef QT_DEBUG
                             << ("/home/cz/" + tabtext + ".csv")
                         #endif
                             << ("/mnt/usb/"+tabtext+".csv") << ("/mnt/sdcard/"+tabtext+".csv"));
}

void w_rpt_report_dataExport::export_info_progress(const QString &info, int v, int total)
{
    ui->progressBar->setMaximum(total);
    ui->progressBar->setValue(v);
    ui->textEdit->append(info+QString("%1/%2").arg(v).arg(total));
    this->repaint();
}

void w_rpt_report_dataExport::togetfilepath()
{
    QString filepath = getFilePath::getSaveFileName(this, tr("导出文件"),ui->label_path->currentText(),QString(),0,QFileDialog::ShowDirsOnly);
    if(!filepath.isEmpty()){
        ui->label_path->setEditText(filepath);
    }
}

w_rpt_report_dataExport::w_rpt_report_dataExport(lds_tableView *content_tableview, const QString &sql, lds_tableView *tail_tableview, QWidget *parent)
    : QDialog(parent),
      ui(new Ui::w_rpt_report_dataExport)
{
    ui->setupUi(this);
    this->sql = sql;
    this->tail_tableview = tail_tableview;
    this->content_tableview = content_tableview;

    //数据初始化
    ui->progressBar->setValue(0);
    ui->label_path->setEditText(lds::sysconf->value("w_rpt_report_dataExport/path").toString());
    ui->checkBox_checkdata->setChecked(lds::sysconf->value("w_rpt_report_dataExport/checkdata", false).toBool());
    ui->checkBox_exportheader->setChecked(lds::sysconf->value("w_rpt_report_dataExport/exportheader",true).toBool());

    connect(ui->pushButton_exit,SIGNAL(clicked()),this,SLOT(toexit()));
    connect(ui->pushButton_export,SIGNAL(clicked()),this,SLOT(toexport_sql()));
    connect(ui->toolButton_getfilepath,SIGNAL(clicked()),this,SLOT(togetfilepath()));
    connect(ui->pushButton_umount_upan, SIGNAL(clicked()),this,SLOT(toumount_upan()));

    //强制核实数据
    ui->checkBox_checkdata->hide();
}

void w_rpt_report_dataExport::toexit()
{
    //    lds::sysconf->setValue("w_rpt_report_dataExport/path", ui->label_path->text());
    lds::sysconf->setValue("w_rpt_report_dataExport/checkdata", ui->checkBox_checkdata->isChecked());
    lds::sysconf->setValue("w_rpt_report_dataExport/exportheader", ui->checkBox_exportheader->isChecked());
    this->reject();
}

void w_rpt_report_dataExport::toexport()
{
    char check = 0;
    if(ui->checkBox_exportheader->isChecked()){
        check |= 0x01;
    }
    if(ui->checkBox_checkdata->isChecked()){
        check |= 0x02;
    }
    //开始写入
    if(0==lds_messagebox::question(this, MESSAGE_TITLE_VOID,tr("该操作会覆盖目录下的同名文件，是否继续"),tr("确认"),tr("取消"))){
        ui->textEdit->clear();
        SQL_REPORT_EXPORT(this, ui->label_path->currentText(), content_tableview, tail_tableview, check);
    }

#ifdef QT_DEBUG
    QDialog dialog(this);
    dialog.setWindowTitle("DEBUG_DIALOG");
    QTextEdit text(&dialog);
    text.setGeometry(10, 10, 800 ,500);
    dialog.resize(820, 520);

    QFile file(ui->label_path->currentText());
    file.open(QFile::ReadOnly);
    text.setText(file.readAll());

    dialog.exec();
#endif
}

void w_rpt_report_dataExport::toexport_sql()
{
    char check = 0;
    if(ui->checkBox_exportheader->isChecked()){
        check |= 0x01;
    }
    if(ui->checkBox_checkdata->isChecked()){
        check |= 0x02;
    }
    //开始写入
    if(0==lds_messagebox::question(this, MESSAGE_TITLE_VOID,tr("该操作会覆盖目录下的同名文件，是否继续"),tr("确认"),tr("取消"))){
        ui->textEdit->clear();
        SQL_REPORT_EXPORT_BY_SQL(this, content_tableview, ui->label_path->currentText(), sql, check);
    }

#ifdef QT_DEBUG
    QDialog dialog(this);
    dialog.setWindowTitle("DEBUG_DIALOG");
    QTextEdit text(&dialog);
    text.setGeometry(10, 10, 500 ,500);
    dialog.resize(520, 520);

    QFile file(ui->label_path->currentText());
    file.open(QFile::ReadOnly);
    text.setText(file.readAll());

    dialog.exec();
#endif
}

void w_rpt_report_dataExport::toumount_upan()
{
    w_sys_manage_sqlcopy::unmount_u_pan(this);
}
