#include "w_sys_manage_basicdataimport.h"
//基础数据导入
#include "ui_w_sys_manage_basicdataimport_dialog.h"
#include "lds_query.h"

#include <QSqlError>
#include "w_sys_manage.h"
#include "lds_messagebox.h"
#include "w_sys_manage_basicdataimport_m_dialog.h"
#include <QFileDialog>
#include <QCloseEvent>
#include "lds.h"
#include "getfilepath.h"
#include "lds.h"


static void SQL_TABLE_IMPORT(w_sys_manage_basicdataimport *receiver, const QString &path,  const QStringList &tablenames)
{
    QFile file;
    QString filename;
    QString tablename;
    QString errtext;
    //! <--transaction begin-->
    lds_query query;
    int index_going;
    QString sql_keys;
    QString sql_values;
    lds_query::tran_saction();
    if(!QDir(path).exists()) {
        receiver->export_info_progress(QObject::tr("文件不存在"));
        goto rollback;
    }
    if(tablenames.isEmpty()) {
        receiver->export_info_progress(QObject::tr("没有需要导入的文件"));
        goto rollback;
    }

    for(int table_c=0,table_count=tablenames.count();table_c<table_count;table_c++){//0

        errtext.clear();

        tablename=tablenames[table_c];
        filename = path+"/"+tablename+".csv";
        file.setFileName(filename);
        switch(0){//switch
        case 0:
            receiver->export_info_progress(
                        QString("%1[%2/%3]    start export...")
                        .arg(tablename)
                        .arg(table_c+1)
                        .arg(table_count),
                        table_c+1,
                        table_count);
            if(!file.open(QFile::ReadOnly)){
                errtext= file.errorString();
                break;
            }

        case 1:
            if(!query.execDelete(tablename, &errtext)){
                break;
            }
        case 2://列名
            sql_keys=file.readLine().trimmed();//先读一行/去除表头后继续

            //开始插入数据
            index_going = 0;
            while(!file.atEnd()){
                sql_values = file.readLine().trimmed();
                if(!query.execInsert(tablename, sql_keys, sql_values, ",",&errtext)) {
                    break;
                }
                index_going ++;
                receiver->export_info_progress(
                            QString("%1[%2/%3]   %4")
                            .arg(tablename)
                            .arg(table_c+1)
                            .arg(table_count)
                            .arg(index_going),
                            table_c+1,
                            table_count
                            );
            }
        }//switch
        file.close();
        receiver->export_info_progress(
                    QString("%1[%2/%3]   %4")
                    .arg(tablename)
                    .arg(table_c+1)
                    .arg(table_count)
                    .arg(errtext.isEmpty()?QString("success"):errtext),
                    table_c+1,
                    table_count
                    );
        if(!errtext.isEmpty()){
            break;
        }
    }

    if(errtext.isEmpty()){
        
        lds_query::com_mit();
        lds_messagebox::information(receiver, MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
        return;
    }

rollback:
    lds_query::roll_back();
    lds_messagebox::information(receiver, MESSAGE_TITLE_VOID, QObject::tr("操作失败"));
    //! <--transaction end-->
    return;
}


w_sys_manage_basicdataimport::w_sys_manage_basicdataimport(QWidget *parent) :
    QDialog(parent),
    ui(new Ui_w_sys_manage_basicdataimport_Dialog)
{
    ui->setupUi(this);
    //! toolbar
    ui->widget->setup(QStringList() << tr("第三方软件会员导入") << tr("确定") << tr("退出"), Qt::AlignRight);
    connect(ui->toolButton,SIGNAL(clicked()),this,SLOT(togetfilepath()));
    connect(ui->widget->index_widget(tr("第三方软件会员导入")),SIGNAL(clicked()),this,SLOT(tomemberimport()));
    connect(ui->widget->index_widget(tr("确定")),SIGNAL(clicked()),this,SLOT(tosave()));
    connect(ui->widget->index_widget(tr("退出")),SIGNAL(clicked()),this,SLOT(toexit()));

    //! after
    
    ui->lineEdit->setEditText(lds::sysconf->value("w_sys_manage_basicdataimport/path", public_sql::sql_table_export_dir).toString());


    ui->label->setText(tr("如现有数据已存在数据，导入时将清除;导入前做好备份,以防数据丢失"));
    ui->label_2->setText(tr("指定导入文件夹位置"));
    reset_tablenames_by_path(ui->lineEdit->currentText());

    if(lds::get_soft_curent_language_first() == "EN"){
        ui->label->hide();
    }
    return;/*
    _tablenames= QStringList()
            << "cey_bt_unit"
            << "cey_bt_dish"
            << "cey_bt_dish_subtype"
            << "cey_bt_dish_type"
            << "cey_bt_dish_suit"

            << "cey_bt_suit_change"
            << "cey_bt_table"
            << "cey_bt_table_type"
            << "cey_bt_table_area"
            << "cey_bt_cp_dish"

            << "cey_bt_cp_memo"
            << "cey_bt_cp_type"
            << "cey_sys_company"
            << "cey_bt_dish_warn"
            << "cey_bt_paymode"

            << "t_m_gift"
            << "t_m_curamount"
            << "t_m_member"
            << "t_m_member_type"

            << "t_m_parameter"
               ;*/

}

void w_sys_manage_basicdataimport::togetfilepath()
{
    //获取路径
    QString filepath = getFilePath::getExistingDirectory(this, tr("导入文件夹"),ui->lineEdit->currentText(),QFileDialog::ShowDirsOnly);
    if(!filepath.isEmpty()){
        ui->lineEdit->setEditText( filepath);
        lds::sysconf->setValue("w_sys_manage_basicdataimport/path", ui->lineEdit->currentText());
        reset_tablenames_by_path(ui->lineEdit->currentText());
    }
}

void w_sys_manage_basicdataimport::reset_tablenames_by_path(const QString &path)
{
    lds_query query;
    QFileInfoList infolist = QDir(path).entryInfoList(QStringList() << "*.csv");
    _tablenames.clear();
    foreach(const QFileInfo &info, infolist){
        query.execSelect(QString(" SHOW TABLES like '%1' ").arg(info.baseName()));
        if(query.next())
            _tablenames.append(info.baseName());
    }
}

void w_sys_manage_basicdataimport::tosave()
{
    if(0==lds_messagebox::critical(this, MESSAGE_TITLE_VOID,tr("该操作会重置数据库一些表的内容，是否继续"), tr("确认"),tr("取消"))){
        //清除数据库对应table
        SQL_TABLE_IMPORT(this,
                         ui->lineEdit->currentText(),
                         _tablenames
                         );
    }
}

void w_sys_manage_basicdataimport::toexit()
{
    this->reject();
}

void w_sys_manage_basicdataimport::tomemberimport()
{
    QPushButton *b = qobject_cast<QPushButton *>(this->sender());
    w_sys_manage_basicdataimport_m_Dialog dialog(this);
    dialog.setWindowTitle(b->text())    ;
    lds_roundeddialog_rect_align(&dialog).exec();
}

void w_sys_manage_basicdataimport::export_info_progress(const QString &info, int step, int max)
{
    if(max!=-1){
        if(ui->progressBar->maximum() != max){
            ui->progressBar->setMaximum(max);
        }
    }
    if((step)!=-1){
        ui->progressBar->setValue(step);
    }
    ui->lineEdit_sub_going->setText(info);
    ui->lineEdit_sub_going->repaint();
}

void w_sys_manage_basicdataimport::closeEvent(QCloseEvent *e)
{
    e->ignore();
}


w_sys_manage_basicdataimport_sub::w_sys_manage_basicdataimport_sub(const QStringList &tablenames, const QString &confname, QWidget *parent)
    :w_sys_manage_basicdataimport(parent)
{
    reset_export_tablenames(tablenames);
    reset_export_confname(confname);
}

void w_sys_manage_basicdataimport_sub::reset_export_tablenames(const QStringList &tablenames)
{
    ui->widget->index_widget(tr("第三方软件会员导入"))->hide();
    _tablenames=tablenames;
}

void w_sys_manage_basicdataimport_sub::reset_export_confname(const QString &confname)
{
    _confname=confname;
    ui->lineEdit->setEditText(lds::sysconf->value(_confname, public_sql::sql_table_export_dir).toString());
}

void w_sys_manage_basicdataimport_sub::reset_tablenames_by_path(const QString &/*path*/)
{
    //保持 _tablenames
}
