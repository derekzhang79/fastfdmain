#include "w_sys_manage_sqlcopy.h"
//数据库备份
#include "ui_w_sys_manage_sqlcopy_dialog.h"
#include "w_sys_manage.h"
#include "n_func.h"
#include <QPainter>
#include <QFileDialog>
#include <QTimer>
#include "lds_messagebox.h"
#include "lds.h"
#include "lds_dialog_input.h"
#include "public_logindialog.h"
#include "lds.h"
#include "getfilepath.h"
#include "getfilepath.h"
#include <QSqlError>
#include "back_view_dialog.h"
#include "w_sys_manage_sqlcopy_usb_select.h"


QStringList w_sys_manage_sqlcopy::static_tobackup_infos;

w_sys_manage_sqlcopy::w_sys_manage_sqlcopy(QWidget *parent) :
    QDialog(parent),
    ui(new Ui_w_sys_manage_sqlcopy_Dialog)
{
    ui->setupUi(this);

    ui->radioButton->setAutoExclusive(false);
    ui->radioButton_2->setAutoExclusive(false);
    ui->radioButton_3->setAutoExclusive(false);
    ui->radioButton->setChecked(lds::sysconf->value("w_sys_manage_sqlcopy/radioButton", true).toBool());
    ui->radioButton_2->setChecked(lds::sysconf->value("w_sys_manage_sqlcopy/radioButton_2", false).toBool());
    ui->radioButton_3->setChecked(lds::sysconf->value("w_sys_manage_sqlcopy/radioButton_3",false).toBool());
    //! toolbar
    ui->widget->setup(QStringList()
                      << tr("备份策略")
                      << tr("数据库备份")
                      << tr("数据库还原")
                      << tr("备份文件删除")
                      <<"|"
                      << tr("配置初始化")
                      << tr("数据库初始化")
                      << tr("数据库中文演示")
                      <<tr("数据库英文演示")
                      <<"|"
                      <<tr("卸载u盘")
                      <<tr("退出"), Qt::AlignTop);
    connect(ui->widget->index_widget(tr("备份策略")), SIGNAL(clicked()), this,SLOT(tobackup_strategy()));
    connect(ui->widget->index_widget(tr("数据库还原")), SIGNAL(clicked()), this,SLOT(torestore()));
    connect(ui->widget->index_widget(tr("数据库备份")), SIGNAL(clicked()), this,SLOT(tobackup()));
    connect(ui->widget->index_widget(tr("数据库初始化")), SIGNAL(clicked()), this,SLOT(tosqlinit()));
    connect(ui->widget->index_widget(tr("配置初始化")), SIGNAL(clicked()), this,SLOT(tosetinit()));
    connect(ui->widget->index_widget(tr("备份文件删除")), SIGNAL(clicked()), this,SLOT(filedelete()));
    connect(ui->widget->index_widget(tr("数据库中文演示")), SIGNAL(clicked()), this,SLOT(sqlchineseshow()));
    connect(ui->widget->index_widget(tr("数据库英文演示")), SIGNAL(clicked()), this,SLOT(sqlenglishshow()));
    connect(ui->widget->index_widget(tr("卸载u盘")), SIGNAL(clicked()), this,SLOT(toumount_upan()));

    connect(ui->widget->index_widget(tr("退出")), SIGNAL(clicked()), this,SLOT(toexit()));
    connect(ui->toolButton,SIGNAL(clicked()),this,SLOT(getfilepath()));
    connect(ui->toolButton_2,SIGNAL(clicked()),this,SLOT(getfilepath_2()));
    connect(ui->toolButton_3,SIGNAL(clicked()),this,SLOT(getfilepath_3()));

    connect(ui->radioButton,SIGNAL(toggled(bool)),this,SLOT(to_update_btn()));
    connect(ui->radioButton_2,SIGNAL(toggled(bool)),this,SLOT(to_update_btn()));
    connect(ui->radioButton_3,SIGNAL(toggled(bool)),this,SLOT(to_update_btn()));
    connect(ui->textEdit_amdin_set,SIGNAL(textChanged()),this,SLOT(to_admin_set()));
    connect(ui->checkBox_zipbackup,SIGNAL(toggled(bool)),this,SLOT(to_zipbackup(bool)));
    connect(ui->toolButton_upload_ftp, SIGNAL(clicked()), this, SLOT(to_sql_upload_ftp()));
    //! after
    //还原路径
    ui->label->setText(lds::sysconf->value("w_sys_manage_sqlcopy/sqlcurpath", public_sql::sql_backup_dir).toString());
    ui->label_2->setText(lds::sysconf->value("w_sys_manage_sqlcopy/sqlcurpath_2", "/mnt/sdcard").toString());
    ui->label_3->setText(lds::sysconf->value("w_sys_manage_sqlcopy/sqlcurpath_3", "/mnt/usb").toString());
    ui->label->setText(public_sql::sql_backup_dir);
    ui->toolButton->setEnabled(false);
    //获取路径下的所有文件
    ui->comboBox->clear();
    ui->comboBox->addItems(w_sys_manage::get_flilename_path(ui->label->text(), QStringList() << "*."+public_sql::sql_danwei));
    ui->comboBox_2->clear();
    ui->comboBox_2->addItems(w_sys_manage::get_flilename_path(ui->label_2->text(), QStringList() << "*."+public_sql::sql_danwei));
    ui->comboBox_3->clear();
    ui->comboBox_3->addItems(w_sys_manage::get_flilename_path(ui->label_3->text(), QStringList() << "*."+public_sql::sql_danwei));
    to_update_btn();

    //第一次使用是， 需要往conf写如新的数据
    lds::sysconf->setValue("w_sys_manage_sqlcopy/sqlcurpath", ui->label->text());
    lds::sysconf->setValue("w_sys_manage_sqlcopy/sqlcurpath_2", ui->label_2->text());
    lds::sysconf->setValue("w_sys_manage_sqlcopy/sqlcurpath_3", ui->label_3->text());

    ui->checkBox_zipbackup->setChecked(lds::sysconf->value("w_sys_manage_sqlcopy_backup_strategy/zipbackup", true).toBool());
    ui->checkBox_zipbackup->hide();

    QTimer::singleShot(100, this, SLOT(check_backuppath()));
    ui->toolButton_2->setEnabled(false);
    ui->label_2->setText("/mnt/sdcard");
}

void w_sys_manage_sqlcopy::static_tobackup(QWidget *parent)
{
    lds_messagebox_loading_show loading(parent, "");
    loading.show_delay();
    static_tobackup_infos.clear();
    
    QList<QStringList> pathslist;
    if(lds::sysconf->value("w_sys_manage_sqlcopy/radioButton", true).toBool()){
        pathslist.append(QStringList()
                         << public_sql::sql_backup_dir//lds::sysconf->value("w_sys_manage_sqlcopy/sqlcurpath",public_sql::sql_backup_dir).toString()
                         << "disk"
                         );
    }
    if(lds::sysconf->value("w_sys_manage_sqlcopy/radioButton_2").toBool()){
        pathslist.append(QStringList()
                         << lds::sysconf->value("w_sys_manage_sqlcopy/sqlcurpath_2").toString()
                         << "sdcard"
                         );
    }
    if(lds::sysconf->value("w_sys_manage_sqlcopy/radioButton_3").toBool()){
        pathslist.append(QStringList()
                         << lds::sysconf->value("w_sys_manage_sqlcopy/sqlcurpath_3").toString()
                         << "usb"
                         );
    }

    QString name = n_func::f_get_sysdatetime().toString("yyyyMMddhhmmss");
    foreach(const QStringList &paths, pathslist){
        //根据时间获取 文件名字
        QString path1=tr("sql-") + name+"."+public_sql::sql_danwei;
        //开始备份
        bool f = public_sql::MySQL_backup(paths.value(0) ,path1, &loading, tr("数据库备份") + ":" + paths.value(1));
        static_tobackup_infos.append(public_sql::MySQL_info);

        if(f && paths.value(1) == "usb") {
            static_tobackup_infos.append(tr("拔u盘前，务必卸载u盘，否则会造成数据丢失"));
        }
    }
    //清除备份文件
    if(lds::sysconf->value("w_sys_manage_sqlcopy_backup_strategy/sql_backup_upbound", true).toBool()){
        int upbound = lds::sysconf->value("w_sys_manage_sqlcopy_backup_strategy/sql_backup_upbound_value", 30).toInt();
        upbound = qMax(30, upbound);
        foreach(const QStringList &paths, pathslist){
            QFileInfoList infos = QDir(paths.value(0)).entryInfoList(QStringList() << "*.sql" << "*.sqlm" << "*.dat");
            QStringList fpaths;foreach(const QFileInfo &f, infos)fpaths.append(f.filePath());
            //  每个备份的文件夹去判断是否有超过备份上限
            if(fpaths.count() > upbound){
                qSort(fpaths.begin(), fpaths.end(), qGreater<QString>());
                while(fpaths.count() > upbound){
                    QFile(fpaths.takeLast()).remove();
                }
            }
        }
    }
}

void w_sys_manage_sqlcopy::check_backuppath()
{
    if(!QDir(ui->label_2->text()).exists()){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("SD卡路径未检测到，请插入SD卡"));
        ui->radioButton_2->setChecked(false);
        lds::sysconf->setValue("w_sys_manage_sqlcopy/radioButton_2", ui->radioButton_2->isChecked());
    }
    if(!QDir(ui->label_3->text()).exists()){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("USB路径未检测到，请插入U盘"));
        ui->radioButton_3->setChecked(false);
        lds::sysconf->setValue("w_sys_manage_sqlcopy/radioButton_3", ui->radioButton_3->isChecked());
    }
}

void w_sys_manage_sqlcopy::tobackup_strategy()
{
    w_sys_manage_sqlcopy_backup_strategy dialog(this);
    dialog.setWindowTitle(_TEXT_SLOT(this));lds_roundeddialog_rect_align(&dialog).exec();
}

void w_sys_manage_sqlcopy::torestore()
{
    if(!check_restore()) return;

    lds_dialog_input inputdialog(this);
    inputdialog.ui->label->setText(tr("密钥(***7):"));
    inputdialog.ui->lineEdit->setEchoMode(QLineEdit::Password);
#ifdef QT_DEBUG
    inputdialog.ui->lineEdit->setText("7777");
#endif
    inputdialog.setWindowTitle(tr("即将数据库初始化"));
    if(QDialog::Accepted== lds_roundeddialog_rect_align(&inputdialog).exec()){
        if(inputdialog.ui->lineEdit->text()!="7777"){
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("密钥(***7):")+tr("密码错误"));
            return;
        }
        if(0 ==lds_messagebox::question(this,MESSAGE_TITLE_VOID,tr("将会影响营业数据，是否继续?"),tr("是"),tr("否"))) {
            lds_messagebox_loading_show loading(this, "");
            loading.show_delay();
            bool flag=public_sql::MySQL_restore(getlabeltext(), getcomtext(), &loading, _TEXT_SLOT(this));
            ui->textBrowser->append(public_sql::MySQL_info);
            if(flag){
                if(lds::get_soft_curent_language_first() == "EN")flag = enkeep();
                lds_messagebox::information(this, MESSAGE_TITLE_VOID,tr("数据库还原成功,点击重启"));
                this->done(public_sql::Reboot);
                return;
            }
        }
    }
}


void w_sys_manage_sqlcopy::tobackup()
{
    if(0!=lds_messagebox::question(this,MESSAGE_TITLE_VOID,tr("该操作将会在多个盘符备份，是否继续?"),tr("继续"),tr("取消")))return;
    static_tobackup(this);
    updatecomrtext();
    foreach(QString info, static_tobackup_infos)
        ui->textBrowser->append(info);
}

void w_sys_manage_sqlcopy::tosqlinit()
{
    if(!check_restore()) return;

    QString errstring;
    lds_dialog_input inputdialog(this);
    inputdialog.ui->label->setText(tr("密码") + ":");
    inputdialog.ui->lineEdit->setEchoMode(QLineEdit::Password);
#ifdef QT_DEBUG
    inputdialog.ui->lineEdit->setText(n_func::f_get_godpassword());
#endif
    inputdialog.setWindowTitle(_TEXT_SLOT(this));
    if(QDialog::Accepted== lds_roundeddialog_rect_align(&inputdialog).exec()){
        errstring = tr("密码错误");
        if(inputdialog.ui->lineEdit->text()==n_func::f_get_godpassword()){
            lds_messagebox_loading_show loading(this, "");
            loading.show_delay();

            bool flag=public_sql::MySQL_restore(public_sql::show_empty_sql_file_dir, public_sql::show_empty_sql_file, &loading, _TEXT_SLOT(this));
            ui->textBrowser->append(tr("初始化")+public_sql::MySQL_info);
            if(lds::get_soft_curent_language_first() == "EN")flag = enkeep();
            errstring = tr("数据库错误");
            if(flag){
                lds_messagebox::information(this, MESSAGE_TITLE_VOID,tr("数据库还原成功,点击重启"));
                this->done(public_sql::Reboot);
                return;
            }
        }
    }
    if(!errstring.isEmpty())
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
}

void w_sys_manage_sqlcopy::tosetinit()
{
    QString errstring;
    lds_dialog_input inputdialog(this);
    inputdialog.ui->label->setText(tr("密码") + ":");
    inputdialog.ui->lineEdit->setEchoMode(QLineEdit::Password);
#ifdef QT_DEBUG
    inputdialog.ui->lineEdit->setText(n_func::f_get_godpassword());
#endif
    inputdialog.setWindowTitle(tr("即将配置初始化"));
    if(QDialog::Accepted==lds_roundeddialog_rect_align(&inputdialog).exec()){
        errstring = tr("密码错误");
        if(inputdialog.ui->lineEdit->text()==n_func::f_get_godpassword()){
            public_sql::fastfd_init_UNZIP();
            ui->textBrowser->append(tr("配置初始化")+public_sql::MySQL_info);
            return;
        }
    }

    if(!errstring.isEmpty()){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
    }
}

void w_sys_manage_sqlcopy::filedelete()
{
    if(0!=lds_messagebox::question(this,MESSAGE_TITLE_VOID,tr("该操作将会删除当前备份文件，是否继续?"),tr("继续"),tr("取消")))return;
    QString filename = getcomtext();

    if(filename.isEmpty()){

    } else {
        if(QFile(getlabeltext()+"/"+filename).remove()){
            if(filename.endsWith(".sql")){
                QFile(getlabeltext()+"/"+filename+"m").remove();
            }
            ui->textBrowser->append(tr("文件删除成功"));
            //获取路径下的所有文件
            updatecomrtext();
        } else {
            ui->textBrowser->append(tr("文件删除失败"));
        }
    }
}

void w_sys_manage_sqlcopy::toexit()
{
    lds::sysconf->setValue("w_sys_manage_sqlcopy/radioButton_2", QDir("/mnt/sdcard").exists());//强制根据sd卡是否插入，判断是否备份
    this->reject();
}

void w_sys_manage_sqlcopy::getfilepath()
{
    //获取路径
    QString filepath = getFilePath::getExistingDirectory(this, tr("导入文件夹"),ui->label->text().isEmpty()?"/usr/desk":ui->label->text(),QFileDialog::ShowDirsOnly);
    if(!filepath.isEmpty()){
        ui->label->setText( filepath
                            );
        //获取路径下的所有文件
        updatecomrtext();

        //保存路径

        lds::sysconf->setValue("w_sys_manage_sqlcopy/sqlcurpath", ui->label->text());
    }
}

void w_sys_manage_sqlcopy::getfilepath_2()
{
    //获取路径
    QString filepath = getFilePath::getExistingDirectory(this, tr("导入文件夹"),ui->label_2->text().isEmpty()?"/usr/desk":ui->label_2->text(),QFileDialog::ShowDirsOnly);
    if(!filepath.isEmpty()){
        ui->label_2->setText( filepath
                              );
        //获取路径下的所有文件
        updatecomrtext();

        //保存路径

        lds::sysconf->setValue("w_sys_manage_sqlcopy/sqlcurpath_2", ui->label_2->text());
    }
}

void w_sys_manage_sqlcopy::getfilepath_3()
{
    QString filepath;
#if defined(Q_OS_UNIX) && defined(QT_NEW_SYSTEM)// && defined(QT_NO_DEBUG)//unix 和 新系統 和发布版
    w_sys_manage_sqlcopy_usb_select dialog(this);
    dialog.setWindowTitle(tr("u盘"));
    if(QDialog::Accepted == lds_roundeddialog_rect_align(&dialog).exec()) {
        filepath = dialog.retData(Qt::UserRole + 3).toString();
        goto selectPath;
    }
    return ;
#endif
    //获取路径
    filepath = getFilePath::getExistingDirectory(this, tr("导入文件夹"),ui->label_3->text().isEmpty()?"/usr/desk":ui->label_3->text(),QFileDialog::ShowDirsOnly);
    if(!filepath.isEmpty()){
        goto selectPath;
    }
    return;
selectPath:

    ///////////
    ui->label_3->setText( filepath );
    //获取路径下的所有文件
    updatecomrtext();

    //保存路径

    lds::sysconf->setValue("w_sys_manage_sqlcopy/sqlcurpath_3", ui->label_3->text());
}


void w_sys_manage_sqlcopy::sqlchineseshow()
{
    if(!check_restore()) return;

    lds_dialog_input inputdialog(this);
    inputdialog.ui->label->setText(tr("密码") + ":");
    inputdialog.ui->lineEdit->setEchoMode(QLineEdit::Password);
#ifdef QT_DEBUG
    inputdialog.ui->lineEdit->setText(n_func::f_get_godpassword());
#endif
    inputdialog.setWindowTitle(_TEXT_SLOT(this));
    QString errstring;
    if(QDialog::Accepted==lds_roundeddialog_rect_align(&inputdialog).exec()){
        errstring = tr("密码错误");
        if(inputdialog.ui->lineEdit->text()==n_func::f_get_godpassword()){

            lds_messagebox_loading_show loading(this, "");
            loading.show_delay();
            bool flag=public_sql::MySQL_restore(public_sql::show_chinese_sql_file_dir, public_sql::show_chinese_sql_file, &loading, _TEXT_SLOT(this));
            ui->textBrowser->append(tr("演示")+public_sql::MySQL_info);
            if(lds::get_soft_curent_language_first() == "EN")flag = enkeep();
            errstring = tr("数据库错误");
            if(flag){
                lds_messagebox::information(this, MESSAGE_TITLE_VOID,tr("数据库还原成功,点击重启"));
                this->done(public_sql::Reboot);
                return;
            }
        }
    }
    if(!errstring.isEmpty()){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
    }
}

void w_sys_manage_sqlcopy::sqlenglishshow()
{
    if(!check_restore()) return;

    QString errstring;
    lds_dialog_input inputdialog(this);
    inputdialog.ui->label->setText(tr("密码") + ":");
    inputdialog.ui->lineEdit->setEchoMode(QLineEdit::Password);
#ifdef QT_DEBUG
    inputdialog.ui->lineEdit->setText(n_func::f_get_godpassword());
#endif
    inputdialog.setWindowTitle(_TEXT_SLOT(this));
    if(QDialog::Accepted==lds_roundeddialog_rect_align(&inputdialog).exec()){
        errstring = tr("密码错误");
        if(inputdialog.ui->lineEdit->text()==n_func::f_get_godpassword()){
            lds_messagebox_loading_show loading(this, "");
            loading.show_delay();
            bool flag=public_sql::MySQL_restore(public_sql::show_english_sql_file_dir, public_sql::show_english_sql_file, &loading, _TEXT_SLOT(this));
            ui->textBrowser->append(tr("演示")+public_sql::MySQL_info);
            /*if(lds::get_soft_curent_language_first() == "EN")*/flag = enkeep();
            errstring = tr("数据库错误");
            if(flag){
                lds_messagebox::information(this, MESSAGE_TITLE_VOID,tr("数据库还原成功,点击重启"));
                this->done(public_sql::Reboot);
                return;
            }
        }
    }
    if(!errstring.isEmpty()){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
    }
}

void w_sys_manage_sqlcopy::toumount_upan()
{
    unmount_u_pan(this);
}

void w_sys_manage_sqlcopy::to_update_btn()
{
    int checknum=ui->radioButton->isChecked()+ui->radioButton_2->isChecked()+ui->radioButton_3->isChecked();
    ui->widget->index_widget(tr("数据库备份"))->setEnabled(checknum>0);
    ui->widget->index_widget(tr("数据库还原"))->setEnabled(checknum==1);
    ui->widget->index_widget(tr("备份文件删除"))->setEnabled(checknum==1);

    //保存路径

    lds::sysconf->setValue("w_sys_manage_sqlcopy/radioButton", ui->radioButton->isChecked());
    lds::sysconf->setValue("w_sys_manage_sqlcopy/radioButton_2", ui->radioButton_2->isChecked());
    lds::sysconf->setValue("w_sys_manage_sqlcopy/radioButton_3", ui->radioButton_3->isChecked());
}

void w_sys_manage_sqlcopy::to_admin_set()
{
    if(public_sql::gs_operid!="0000")return;

    QString key=ui->textEdit_amdin_set->toPlainText().trimmed();
    if(key=="80.90"){//更改logo 80.90
        //1 若是没有图片则默认新建；所有图片都会以28的高度等比例转换
        //2 若是没有默认title则默认新建
        lds_messagebox::information(this,MESSAGE_TITLE_VOID,
                                    tr("1.后台logo路径'pos/fastfdbacklogo';自定义logo修改图片即可")+"\n"+
                                    tr("2.后台标题路径'pos/fastfdbacktext.txt';自定义标题修改文本内容即可")+"\n"+
                                    tr("3.'pos/'为项目路径")
                                    );
    }

    if(key=="70.80"){//软件使用限制 70.80
        if(saas_pay_detail::isSaaS()) {
            lds_messagebox::information(this, MESSAGE_TITLE_VOID, tr("SaaS不支持该功能"));
            return;
        } else {
            w_sys_manage_sqlcopy_soft_limit dialog(this);
            dialog.setWindowTitle(tr("设置软件到期无法使用"));
            lds_roundeddialog_rect_align(&dialog).exec();
        }
    }
}

void w_sys_manage_sqlcopy::to_zipbackup(bool f)
{
    lds::sysconf->setValue("w_sys_manage_sqlcopy_backup_strategy/zipbackup", f);
}

void w_sys_manage_sqlcopy::to_sql_upload_ftp()
{
    QString filepath = ui->label->text() + "/" + ui->comboBox->currentText();
    QString filename = ui->comboBox->currentText();
    QFile file(filepath);
    if(false == file.open(QFile::ReadOnly)) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, file.errorString());
        return;
    }

    //
    lds_dialog_input inputdialog(this);
    inputdialog.ui->label->setText(tr("密码") + ":");
    inputdialog.ui->lineEdit->setEchoMode(QLineEdit::Password);
#ifdef QT_DEBUG
    inputdialog.ui->lineEdit->setText(n_func::f_get_godpassword());
#endif
    inputdialog.setWindowTitle(_TEXT_SLOT(this));
    if(QDialog::Accepted != lds_roundeddialog_rect_align(&inputdialog).exec()){
        return;
    }
    if(inputdialog.ui->lineEdit->text()!=n_func::f_get_godpassword()){
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("密码错误"));
        return;
    }

    lds_messagebox_loading_show loading(this, "");
    loading.setText(tr("正在上传"));
    loading.show_delay();
    //
    QUrl url;
    url.setUrl("ftp://115.28.155.165/cySql/" + filename);
    url.setUserName("cz");
    url.setPassword("111111");
    QNetworkRequest request;
    request.setUrl(url);

    QEventLoop eloop;
    QNetworkAccessManager networkAccessManager;
    connect(&networkAccessManager, SIGNAL(finished(QNetworkReply*)), &eloop, SLOT(quit()));
    QNetworkReply *reply = networkAccessManager.put(request, file.readAll());
    connect(reply, SIGNAL(uploadProgress(qint64,qint64)), &loading, SLOT(updateProgress(qint64, qint64)));
    eloop.exec();

    if(reply->error() == QNetworkReply::NoError) {
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, tr("上传成功"));
        return;
    }
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("上传失败") );
}

QString w_sys_manage_sqlcopy::getcomtext()
{
    QString filename;
    if(ui->radioButton->isChecked()){
        filename=ui->comboBox->currentText();
    } else if(ui->radioButton_2->isChecked()){
        filename=ui->comboBox_2->currentText();

    } else if(ui->radioButton_3->isChecked()){
        filename=ui->comboBox_3->currentText();
    }

    return filename;
}

QString w_sys_manage_sqlcopy::getlabeltext()
{
    QString filename;
    if(ui->radioButton->isChecked()){
        filename=ui->label->text();
    } else if(ui->radioButton_2->isChecked()){
        filename=ui->label_2->text();

    } else if(ui->radioButton_3->isChecked()){
        filename=ui->label_3->text();
    }

    return filename;
}

void w_sys_manage_sqlcopy::updatecomrtext()
{
    if(ui->radioButton->isChecked()){//磁盘
        ui->comboBox->clear();
        ui->comboBox->addItems(w_sys_manage::get_flilename_path(ui->label->text(), QStringList() << "*."+public_sql::sql_danwei));

    }

    if(ui->radioButton_2->isChecked()){//sd
        ui->comboBox_2->clear();
        ui->comboBox_2->addItems(w_sys_manage::get_flilename_path(ui->label_2->text(), QStringList() << "*."+public_sql::sql_danwei));

    }

    if(ui->radioButton_3->isChecked()){//usb
        ui->comboBox_3->clear();
        ui->comboBox_3->addItems(w_sys_manage::get_flilename_path(ui->label_3->text(), QStringList() << "*."+public_sql::sql_danwei));
    }
}


bool w_sys_manage_sqlcopy::check_restore()
{
    if(public_sql::getdbHostName() == "127.0.0.1"
            || public_sql::localContainIp(public_sql::getdbHostName())
            ) {//本地数据库

    } else {//服务器
        if(1 == lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("正在操作服务器数据库"), tr("继续"), tr("取消"))) {
            return false;
        }
    }
    return true;
}

bool w_sys_manage_sqlcopy::enkeep()
{
    bool ret = true;
    lds_query query;
    query.execAlter("alter table cey_bt_paymode add column vch_arg1  VARCHAR(128);");
    query.execAlter("alter table cey_bt_paymode add column vch_arg2  VARCHAR(128);");
    query.execAlter("alter table cey_bt_paymode add column vch_arg3  VARCHAR(128);");
    query.execAlter("alter table cey_bt_paymode add column vch_arg4  VARCHAR(128);");
    ret = ret && query.execUpdate("cey_bt_paymode", "vch_arg4","N", "vch_paymodename", "Cash", "ch_paymodeno = '00'");
    ret = ret && query.execUpdate("cey_bt_paymode", "vch_arg4","N", "vch_paymodename", "Prepaid card", "ch_paymodeno = '02'");
    ret = ret && query.execUpdate("cey_bt_paymode", "vch_arg4","N", "vch_paymodename", "Bank card", "ch_paymodeno = '03'");
    ret = ret && query.execUpdate("cey_bt_paymode", "vch_arg4","N", "vch_paymodename", "Coupon", "ch_paymodeno = '04'");
    ret = ret && query.execUpdate("cey_bt_paymode", "vch_arg4","N", "vch_paymodename", "Wechat", "ch_paymodeno = '05'");

    ret = ret && query.execUpdate("cey_bt_paymode", "vch_arg4","N", "vch_paymodename", "Alipay", "ch_paymodeno = '06'");
    ret = ret && query.execUpdate("cey_bt_paymode", "vch_arg4","T", "vch_paymodename", "Baidu nuomi", "ch_paymodeno = '07'");
    ret = ret && query.execUpdate("cey_bt_paymode", "vch_arg4","T", "vch_paymodename", "Dianping", "ch_paymodeno = '08'");
    ret = ret && query.execUpdate("cey_bt_paymode", "vch_arg4","T", "vch_paymodename", "Meituan", "ch_paymodeno = '09'");
    ret = ret && query.execUpdate("cey_bt_paymode", "vch_arg4","N", "vch_paymodename", "Prepaid card(cloud)", "ch_paymodeno = '10'");

    ret = ret && query.execUpdate("cey_bt_paymode", "vch_arg4","N", "vch_paymodename", "Vip credit", "ch_paymodeno = '11'");
    ret = ret && query.execUpdate("cey_bt_paymode", "vch_arg4","N", "vch_paymodename", "No cost", "ch_paymodeno = 'XX'");
    ret = ret && query.execUpdate("cey_bt_paymode", "vch_arg4","N", "vch_paymodename", "Auto-round", "ch_paymodeno = 'YY'");
    ret = ret && query.execUpdate("cey_bt_paymode", "vch_arg4","N", "vch_paymodename", "Manual-round", "ch_paymodeno = 'ZZ'");
    ret = ret && query.execUpdate("cey_bt_paymode", "vch_arg4","N", "vch_paymodename", "Mobile payment", "ch_paymodeno = 'AA'");
    ret = ret && query.execUpdate("cey_bt_paymode", "vch_arg4","N", "vch_paymodename", "QR Code Pay", "ch_paymodeno = 'BB' ");

    ret = ret && query.execDelete("cey_sys_purview_list");
    ret = ret && query.execInsert("cey_sys_purview_list", "0101,Dishes Info Settings,N,SHEE,w_bt_dish,Basic,01,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0102,Set Dishes Info Settings,N,SHEE,w_bt_dish_suit,Basic,01,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0103,Kitchen Printer Settings,N,SHEE,w_bt_dish_cp,Basic,01,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0104,Customize Dishes,N,SHEE,w_bt_print_memo,Basic,01,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0105,Table Info Settings,N,SHEE,w_bt_table,Basic,01,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0106,Cash Settings,N,EDIT,w_bt_paymode,Basic,01,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0107,Shifts,N,EDIT,w_bt_class,Basic,01,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0110,Special Operation Reason,N,SHEE,w_bt_other,Basic,01,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0201,Receipt Inquiry,N,EDIT,w_u_bill_search,Front Settings,02,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0202,Current Table Statistics,N,EDIT,w_u_curtable,Front Settings,02,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0203,Dishes Trading Inquiry,N,SHEE,w_rpt_cost_flow,Front Settings,02,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0204,Cash Inquiry,N,SHEE,w_rpt_checkout_flow,Front Settings,02,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0205,Special Price Inquiry,N,SHEE,w_rpt_dish_special,Front Settings,02,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0206,Sold-out Dishes Inquiry,N,SHEE,w_rpt_dish_warn,Front Settings,02,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0207,Pay,N,EDIT,w_u_togo_balance,Front Settings,02,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0208,Table Lock Status,N,EDIT,w_u_table_lock,Front Settings,02,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0209,Open Cashdrawer,O,EDIT,tableState_NONE,Front Settings,02,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0210,Open Table,O,EDIT,tableState_NONE,Front Settings,02,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0211,Transfer Table,O,EDIT,tableState_NONE,Front Settings,02,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0212,Cancel Table,O,EDIT,tableState_NONE,Front Settings,02,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0213,Order Dishes,O,EDIT,tableState_NONE,Front Settings,02,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0214,Pay,O,EDIT,tableState_NONE,Front Settings,02,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0215,Reminder,O,EDIT,tableState_NONE,Front Settings,02,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0216,Transfer Tables,O,EDIT,tableState_NONE,Front Settings,02,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0217,Shifts Change,O,EDIT,tableState_NONE,Front Settings,02,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0218,Return Dishes,O,EDIT,tableState_NONE,Front Settings,02,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0219,No Service Fee,O,EDIT,tableState_NONE,Front Settings,02,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0220,No Lowest Consumption Standard,O,EDIT,tableState_NONE,Front Settings,02,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0221,Pre-print Bill,O,EDIT,tableState_NONE,Front Settings,02,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0222,Pending,O,EDIT,tableState_NONE,Front Settings,02,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0223,Delivery,O,EDIT,tableState_NONE,Front Settings,02,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0224,Cashier Statistics,O,EDIT,tableState_NONE,Front Settings,02,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0225,Delivery Return,O,EDIT,tableState_NONE,Front Settings,02,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0226,Add Temporary Dishes,O,EDIT,tableState_NONE,Front Settings,02,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0227,Anti-settlement,O,EDIT,tableState_NONE,Front Settings,02,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0228,Chargeback,O,EDIT,tableState_NONE,Front Settings,02,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0240,Daily Settlement,N,EDIT,w_ub_day_sumup,Front Settings,02,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0241,Front Business,N,SHEE,w_mdi_qt,Front Settings,02,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0242,Today Specails                  ,N,SHEE,w_bt_dish_special,Front Settings,02,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0243,Today sold out                  ,N,SHEE,w_bt_dish_warn,Front Settings,02,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0401,VIP Settings,N,EDIT,w_m_parameter,Member,04,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0402,VIP Info Settings,N,SHEE,w_m_member_list,Member,04,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0403,VIP Status,N,SHEE,w_m_manage,Member,04,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0404,VIP Recharge,N,EDIT,w_m_member_deposit,Member,04,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0406,VIP Points,N,SHEE,w_m_point_manage,Member,04,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0407,Change VIP Type,N,EDIT,w_m_type_change,Member,04,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0411,VIP Reports,N,SHEE,w_rpt_member_total,Member,04,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0413,Clear VIP Card Chip,N,EDIT,w_m_icblankout,Member,04,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0602,Product Warehousing,N,SHEE,w_st_voucher_list,Iventory,06,N,RK ", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0603,Returns Bill,N,SHEE,w_st_voucher_list,Iventory,06,N,TH ", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0604,Report the loss,N,SHEE,w_st_voucher_list,Iventory,06,N,CK ", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0605,Inventory Checking,N,SHEE,w_st_store_check,Iventory,06,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0606,Inventory Inquiry,N,SHEE,w_st_rpt_store_num,Iventory,06,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0609,Inventory Checking,N,SHEE,w_st_rpt_material_check_query,Iventory,06,N,RK ", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0610,Invoicing Reports,N,SHEE,w_st_material_jxc,Iventory,06,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0611,Receipt Inquiry,N,SHEE,w_st_rpt_total,Iventory,06,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0612,Inventory Parameter Settings,N,EDIT,w_st_parameter,Iventory,06,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0701,Consumption Reports,N,SHEE,w_rpt_cost,Reports,07,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0702,Sales Promotion Reports,N,SHEE,w_rpt_cost_favourable,Reports,07,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0703,Table Business Data Statistics,N,SHEE,w_rpt_cost_tableno,Reports,07,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0704,Returned Dishes Reports,N,SHEE,w_rpt_cost_back,Reports,07,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0705,Cash Reports,N,SHEE,w_rpt_checkout,Reports,07,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0706,Service Fee and Lowest Consumption Inquiry,N,SHEE,w_rpt_service_low,Reports,07,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0707,No Cost Inquiry,N,SHEE,w_rpt_checkout_free,Reports,07,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0708,Daily or Weekly or Monthly Reports,N,SHEE,w_rpt_dwm,Reports,07,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0710,Daily Business Reports,N,SHEE,w_rpt_day,Reports,07,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0711,Sales Statistics for Business Day,N,SHEE,w_rpt_day_series,Reports,07,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0712,Category Sales Statistics,N,SHEE,w_rpt_cost_series,Reports,07,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0713,Business Statistics for Different Period of Time,N,SHEE,w_rpt_period,Reports,07,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0714,Sales Rank Statistics,N,SHEE,w_rpt_cost_order,Reports,07,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0715,Waiter commission inquiry                ,N,SHEE,w_rpt_cost_waiter,Reports,07,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0719,Anti-settlement Inquiry,N,SHEE,w_rpt_uncheckout_flow,Reports,07,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0720,Direct Selling Product Gross Profit Statistics,N,SHEE,w_rpt_dish_gross_profit,Reports,07,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0901,Company Info Settings,N,EDIT,w_sys_company,Settings,09,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0902,System Parameter Settings,N,EDIT,w_sys_parameter,Settings,02,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0903,Operator Info Settings,N,EDIT,w_sys_operator,Settings,09,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0904,Operation Permission Settings,N,EDIT,w_sys_purview,Settings,09,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0905,Database Backup,N,EDIT,w_sys_backup,Settings,09,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0906,Basic Data Export,N,EDIT,w_sys_data_export,Settings,09,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0907,Basic Data Import,N,EDIT,w_sys_data_import,Settings,09,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0908,Business Data Remove,N,EDIT,w_sys_data_clear,Settings,09,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0909,Permission Settings,N,SHEE,w_sys_purview,Settings,09,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0910,MySQL Server,N,EDIT,,Settings,09,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0911,File Manager,N,EDIT,,Settings,09,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0912,Wirelss Ordering Monitoring,N,EDIT,,Settings,09,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0913,Cloud Sync,N,EDIT,,Settings,09,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "1501,Message Group Management,N,EDIT,w_ms_group_form,SMS Management,15,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "1502,Message Member Management,N,EDIT,w_ms_cust_input,SMS Management,15,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "1503,Message Settings,N,EDIT,w_ms_message_setup,SMS Management,15,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "1504,Message Sending to Individuals,N,EDIT,w_ms_send_single,SMS Management,15,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "1505,Message Group Sending,N,EDIT,w_ms_send_multi,SMS Management,15,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "1506,Message Inquiry,N,EDIT,w_ms_query_send,SMS Management,15,N,", ",");

    return ret;
}

bool w_sys_manage_sqlcopy::cnkeep()
{
    bool ret = true;
    lds_query query;
    query.execAlter("alter table cey_bt_paymode add column vch_arg1  VARCHAR(128);");
    query.execAlter("alter table cey_bt_paymode add column vch_arg2  VARCHAR(128);");
    query.execAlter("alter table cey_bt_paymode add column vch_arg3  VARCHAR(128);");
    query.execAlter("alter table cey_bt_paymode add column vch_arg4  VARCHAR(128);");

    ret = ret && query.execUpdate("cey_bt_paymode", "vch_arg4","N", "vch_paymodename", "现金", "ch_paymodeno = '00'");
    ret = ret && query.execUpdate("cey_bt_paymode", "vch_arg4","N", "vch_paymodename", "充值卡", "ch_paymodeno = '02'");
    ret = ret && query.execUpdate("cey_bt_paymode", "vch_arg4","N", "vch_paymodename", "银行卡", "ch_paymodeno = '03'");
    ret = ret && query.execUpdate("cey_bt_paymode", "vch_arg4","N", "vch_paymodename", "代金券", "ch_paymodeno = '04'");
    ret = ret && query.execUpdate("cey_bt_paymode", "vch_arg4","N", "vch_paymodename", "微信支付", "ch_paymodeno = '05'");

    ret = ret && query.execUpdate("cey_bt_paymode", "vch_arg4","N", "vch_paymodename", "支付宝支付", "ch_paymodeno = '06'");
    ret = ret && query.execUpdate("cey_bt_paymode", "vch_arg4","T", "vch_paymodename", "百度糯米", "ch_paymodeno = '07'");
    ret = ret && query.execUpdate("cey_bt_paymode", "vch_arg4","T", "vch_paymodename", "大众点评", "ch_paymodeno = '08'");
    ret = ret && query.execUpdate("cey_bt_paymode", "vch_arg4","T", "vch_paymodename", "美团", "ch_paymodeno = '09'");
    ret = ret && query.execUpdate("cey_bt_paymode", "vch_arg4","N", "vch_paymodename", "充值卡(云)", "ch_paymodeno = '10'");

    ret = ret && query.execUpdate("cey_bt_paymode", "vch_arg4","N", "vch_paymodename", "会员挂账", "ch_paymodeno = '11'");
    ret = ret && query.execUpdate("cey_bt_paymode", "vch_arg4","N", "vch_paymodename", "手工抹零", "ch_paymodeno = 'XX'");
    ret = ret && query.execUpdate("cey_bt_paymode", "vch_arg4","N", "vch_paymodename", "自动抹零", "ch_paymodeno = 'YY'");
    ret = ret && query.execUpdate("cey_bt_paymode", "vch_arg4","N", "vch_paymodename", "优惠", "ch_paymodeno = 'ZZ'");
    ret = ret && query.execUpdate("cey_bt_paymode", "vch_arg4","N", "vch_paymodename", "移动支付", "ch_paymodeno = 'AA'");
    ret = ret && query.execUpdate("cey_bt_paymode", "vch_arg4","N", "vch_paymodename", "快捷支付", "ch_paymodeno = 'BB' ");

    ret = ret && query.execDelete("cey_sys_purview_list");
    ret = ret && query.execInsert("cey_sys_purview_list", "0101,菜品资料维护                  ,N,SHEE,w_bt_dish,数据维护,01,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0102,套菜资料维护                  ,N,SHEE,w_bt_dish_suit,数据维护,01,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0103,菜品厨打设置                  ,N,SHEE,w_bt_dish_cp,数据维护,01,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0104,菜品做法设置                  ,N,SHEE,w_bt_print_memo,数据维护,01,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0105,餐桌资料维护                  ,N,SHEE,w_bt_table,数据维护,01,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0106,收银方式维护                  ,N,EDIT,w_bt_paymode,数据维护,01,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0107,营业班次                      ,N,EDIT,w_bt_class,数据维护,01,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0110,特殊操作原因                  ,N,SHEE,w_bt_other,数据维护,01,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0201,营业单据查询                  ,N,EDIT,w_u_bill_search,营业管理,02,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0202,当前台况统计                  ,N,EDIT,w_u_curtable,营业管理,02,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0203,菜品销售流水查询              ,N,SHEE,w_rpt_cost_flow,营业管理,02,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0204,收银流水查询                  ,N,SHEE,w_rpt_checkout_flow,营业管理,02,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0205,菜品历史特价查询              ,N,SHEE,w_rpt_dish_special,营业管理,02,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0206,菜品历史估清查询              ,N,SHEE,w_rpt_dish_warn,营业管理,02,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0207,外卖收银结算                  ,N,EDIT,w_u_togo_balance,营业管理,02,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0208,餐台锁态管理                  ,N,EDIT,w_u_table_lock,营业管理,02,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0209,开钱箱                        ,O,EDIT,NONE,营业管理,02,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0210,开台                          ,O,EDIT,NONE,营业管理,02,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0211,转台                          ,O,EDIT,NONE,营业管理,02,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0212,消台                          ,O,EDIT,NONE,营业管理,02,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0213,点菜                          ,O,EDIT,NONE,营业管理,02,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0214,结算                          ,O,EDIT,NONE,营业管理,02,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0215,厨房催菜                      ,O,EDIT,NONE,营业管理,02,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0216,菜品转台                      ,O,EDIT,NONE,营业管理,02,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0217,接班操作                      ,O,EDIT,NONE,营业管理,02,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0218,退菜                          ,O,EDIT,NONE,营业管理,02,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0219,免服务费                      ,O,EDIT,NONE,营业管理,02,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0220,免低消差                      ,O,EDIT,NONE,营业管理,02,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0221,预打账单                      ,O,EDIT,NONE,营业管理,02,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0222,快餐挂单                      ,O,EDIT,NONE,营业管理,02,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0223,快餐外卖                      ,O,EDIT,NONE,营业管理,02,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0224,前台营业情况统计              ,O,EDIT,NONE,营业管理,02,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0225,外卖退货                      ,O,EDIT,NONE,营业管理,02,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0226,增加临时菜                    ,O,EDIT,NONE,营业管理,02,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0227,反结账                        ,O,EDIT,NONE,营业管理,02,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0228,退单                          ,O,EDIT,NONE,营业管理,02,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0240,营业日结                      ,N,EDIT,w_ub_day_sumup,营业管理,02,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0241,前台营业                      ,N,SHEE,w_mdi_qt,营业管理,02,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0242,今日特价维护                  ,N,SHEE,w_bt_dish_special,营业管理,02,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0243,今日菜品估清                  ,N,SHEE,w_bt_dish_warn,营业管理,02,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0401,会员参数设置                  ,N,EDIT,w_m_parameter,会员管理,04,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0402,会员资料维护                  ,N,SHEE,w_m_member_list,会员管理,04,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0403,会员状态管理                  ,N,SHEE,w_m_manage,会员管理,04,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0404,充值会员充值                  ,N,EDIT,w_m_member_deposit,会员管理,04,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0406,会员积分管理                  ,N,SHEE,w_m_point_manage,会员管理,04,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0407,会员类型变更                  ,N,EDIT,w_m_type_change,会员管理,04,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0411,会员报表                      ,N,SHEE,w_rpt_member_total,会员管理,04,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0413,会员卡芯片清除                ,N,EDIT,w_m_icblankout,会员管理,04,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0602,商品入库                      ,N,SHEE,w_st_voucher_list,库存管理,06,N,RK", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0603,商品退货单                    ,N,SHEE,w_st_voucher_list,库存管理,06,N,TH", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0604,报损单                        ,N,SHEE,w_st_voucher_list,库存管理,06,N,CK", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0605,库存盘点                      ,N,SHEE,w_st_store_check,库存管理,06,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0606,库存查询                      ,N,SHEE,w_st_rpt_store_num,库存管理,06,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0609,盘点查询                      ,N,SHEE,w_st_rpt_material_check_query,库存管理,06,N,RK", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0610,进销存汇总报表                ,N,SHEE,w_st_material_jxc,库存管理,06,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0611,单据汇总查询                  ,N,SHEE,w_st_rpt_total,库存管理,06,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0612,库存参数设置                  ,N,EDIT,w_st_parameter,库存管理,06,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0701,菜品消费报表                  ,N,SHEE,w_rpt_cost,报表中心,07,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0702,菜品促销报表                  ,N,SHEE,w_rpt_cost_favourable,报表中心,07,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0703,餐桌营业数据统计              ,N,SHEE,w_rpt_cost_tableno,报表中心,07,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0704,退菜情况报表                  ,N,SHEE,w_rpt_cost_back,报表中心,07,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0705,营业收银报表                  ,N,SHEE,w_rpt_checkout,报表中心,07,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0706,服务费与低消情况查询          ,N,SHEE,w_rpt_service_low,报表中心,07,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0707,收银免单情况查询              ,N,SHEE,w_rpt_checkout_free,报表中心,07,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0708,日月周报表分析                ,N,SHEE,w_rpt_dwm,报表中心,07,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0710,日营业情况统计                ,N,SHEE,w_rpt_day,报表中心,07,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0711,营业日销售情况统计            ,N,SHEE,w_rpt_day_series,报表中心,07,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0712,品类销售营业分析              ,N,SHEE,w_rpt_cost_series,报表中心,07,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0713,时段营业分析                  ,N,SHEE,w_rpt_period,报表中心,07,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0714,菜品销售排行统计              ,N,SHEE,w_rpt_cost_order,报表中心,07,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0715,服务员提成查询                ,N,SHEE,w_rpt_cost_waiter,报表中心,07,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0719,反结帐查询                    ,N,SHEE,w_rpt_uncheckout_flow,报表中心,07,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0720,原料毛利报表            ,N,SHEE,w_rpt_dish_gross_profit,报表中心,07,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0901,公司信息设置                  ,N,EDIT,w_sys_company,系统管理,09,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0902,系统参数设置                  ,N,EDIT,w_sys_parameter,营业管理,02,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0903,操作员资料维护                ,N,EDIT,w_sys_operator,系统管理,09,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0904,操作权限维护                  ,N,EDIT,w_sys_purview,系统管理,09,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0905,数据库备份                    ,N,EDIT,w_sys_backup,系统管理,09,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0906,基础数据导出                  ,N,EDIT,w_sys_data_export,系统管理,09,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0907,基础数据导入                  ,N,EDIT,w_sys_data_import,系统管理,09,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0908,业务数据清除                  ,N,EDIT,w_sys_data_clear,系统管理,09,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0909,权限设置                      ,N,SHEE,w_sys_purview,系统管理,09,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0910,MySQL服务器,N,EDIT,,系统管理,09,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0911,文件管理器,N,EDIT,,系统管理,09,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0912,无线点菜监控,N,EDIT,,系统管理,09,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "0913,云同步,N,EDIT,,系统管理,09,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "1501,短信组管理                    ,N,EDIT,w_ms_group_form,短信管理,15,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "1502,短信成员管理                  ,N,EDIT,w_ms_cust_input,短信管理,15,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "1503,短信设置                      ,N,EDIT,w_ms_message_setup,短信管理,15,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "1504,短信单发                      ,N,EDIT,w_ms_send_single,短信管理,15,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "1505,短信群发                      ,N,EDIT,w_ms_send_multi,短信管理,15,N,", ",");
    ret = ret && query.execInsert("cey_sys_purview_list", "1506,短信查询                      ,N,EDIT,w_ms_query_send,短信管理,15,N,", ",");

    return ret;
}

void w_sys_manage_sqlcopy::unmount_u_pan(QWidget *parent)
{
    bool execFlag = true;
#if defined (Q_OS_UNIX)
    execFlag = (0 == system("umount /mnt/usb"));
#endif

    if(execFlag){
        lds_messagebox::information(parent, MESSAGE_TITLE_VOID, tr("卸载成功,请拔出u盘"));
        return;
    }
    lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, tr("已成功卸载或者u盘不存在"));
}

//===========================
#include "ui_w_sys_manage_sqlcopy_backup_strategy_dialog.h"
QMap<QDateTime, bool> w_sys_manage_sqlcopy_backup_strategy::timeBackupRecords;
w_sys_manage_sqlcopy_backup_strategy::w_sys_manage_sqlcopy_backup_strategy(QWidget *parent):
    QDialog(parent),
    ui(new Ui_w_sys_manage_sqlcopy_backup_strategy_Dialog)
{
    ui->setupUi(this);

    ui->checkBox_start->setChecked(lds::sysconf->value("w_sys_manage_sqlcopy_backup_strategy/sql_start_backup", false).toBool());
    ui->checkBox_time->setChecked(lds::sysconf->value("w_sys_manage_sqlcopy_backup_strategy/sql_time_backup", true).toBool());
    ui->checkBox_upbound->setChecked(lds::sysconf->value("w_sys_manage_sqlcopy_backup_strategy/sql_backup_upbound", true).toBool());
    ui->timeEdit->setTime(lds::sysconf->value("w_sys_manage_sqlcopy_backup_strategy/sql_time_backup_value", QTime(10,0,0)).toTime());
    ui->spinBox->setValue(lds::sysconf->value("w_sys_manage_sqlcopy_backup_strategy/sql_backup_upbound_value", 30).toInt());
    ui->widget->setup(QStringList()  << tr("确定")<< tr("取消"), Qt::AlignRight);
    connect(ui->widget->index_widget(tr("确定")), SIGNAL(clicked()),this,SLOT(took()));
    connect(ui->widget->index_widget(tr("取消")), SIGNAL(clicked()),this,SLOT(toexit()));
}

void w_sys_manage_sqlcopy_backup_strategy::took()
{
    lds::sysconf->setValue("w_sys_manage_sqlcopy_backup_strategy/sql_start_backup", ui->checkBox_start->isChecked());
    lds::sysconf->setValue("w_sys_manage_sqlcopy_backup_strategy/sql_time_backup", ui->checkBox_time->isChecked());
    lds::sysconf->setValue("w_sys_manage_sqlcopy_backup_strategy/sql_backup_upbound", ui->checkBox_upbound->isChecked());
    lds::sysconf->setValue("w_sys_manage_sqlcopy_backup_strategy/sql_time_backup_value", ui->timeEdit->time());
    lds::sysconf->setValue("w_sys_manage_sqlcopy_backup_strategy/sql_backup_upbound_value", ui->spinBox->value());

    this->accept();
}

void w_sys_manage_sqlcopy_backup_strategy::toexit()
{
    this->reject();
}

//=========================
#include "ui_w_sys_manage_sqlcopy_soft_limit_dialog.h"
#include "lds_dialog_input.h"

w_sys_manage_sqlcopy_soft_limit::   w_sys_manage_sqlcopy_soft_limit(QWidget *parent):
    QDialog(parent),
    ui(new Ui_w_sys_manage_sqlcopy_soft_limit_Dialog)
{
    ui->setupUi(this);

    connect(ui->pushButton_beginset,SIGNAL(clicked()),this,SLOT(to_beginset()));
    connect(ui->pushButton_cancel,SIGNAL(clicked()),this,SLOT(to_quit()));
    connect(ui->pushButton_cancelset,SIGNAL(clicked()),this,SLOT(to_cancelset()));

    readData();
}

bool w_sys_manage_sqlcopy_soft_limit::static_soft_limit_check(QWidget *parent)
{
    //软件试用期
    //1第一次用的话，跳过，直接进入界面
    //2进入界面，【后台】【系统设置】【数据库备份】【右边的输入框 70.80】
    //3选择号结束日期，和密码(可以为空)；点击开始设置，下次启动就是实行该流程
    //4设置界面的左边，绿色表示启用，灰色表示停用
    //5进入设置界面必须是0000账户才可以
    w_sys_manage_sqlcopy_soft_limit limit_dialog;
    if(!limit_dialog.dt_end.isValid()) return true;//!若是第一次使用则，通过
    if(!limit_dialog.isrun) return true;//!没有启动

    //保证dt_begin只增不减
    if(limit_dialog.dt_begin < QDateTime::currentDateTime()){//1
        limit_dialog.dt_begin = QDateTime::currentDateTime();
        limit_dialog.saveData();
        limit_dialog.readData();
    }//!1

    lds_dialog_input inputdialog(parent);
    inputdialog.ui->label->setText(QObject::tr("密码") + ":");
    inputdialog.hideKey();
    inputdialog.ui->lineEdit->setEchoMode(QLineEdit::Password);

    int limit_day=limit_dialog.dt_begin.daysTo(limit_dialog.dt_end);

    if(limit_day >10) return true;//!启动，过期时间超过10

    QString info;
    if(limit_day <= 10 && limit_day > 0){
        info=tr("软件试用期还剩%1天，请输入验证密钥").arg(limit_day);
    }
    if(limit_day  <= 0){
        info=tr("软件超过试用期，请输入验证密钥");
    }
    inputdialog.setWindowTitle(info);
    if(QDialog::Accepted==lds_roundeddialog_rect_align(&inputdialog).exec()){//5
        if(inputdialog.ui->lineEdit->text()==limit_dialog.password){
            limit_dialog.isrun=false;//未启动
            limit_dialog.saveData();
            lds_messagebox::information(parent, MESSAGE_TITLE_VOID,QObject::tr("验证成功"));
            return true;//!验证通过
        } else {
            lds_messagebox::warning(parent, MESSAGE_TITLE_VOID,QObject::tr("验证错误"));
            return false;//!验证错误
        }
    }
    if(limit_day <= 10 && limit_day > 0){
        lds_messagebox::warning(parent, MESSAGE_TITLE_VOID,QObject::tr("取消验证，但是在使用期限内"));
        return true;//!取消验证，但是在使用期限内
    }

    return false;//!取消验证，但是已过期
}

void w_sys_manage_sqlcopy_soft_limit::to_cancelset()//取消设置
{
    isrun=false;//停止
    saveData();
    readData();
    lds_messagebox::information(this, MESSAGE_TITLE_VOID, _TEXT_SLOT(this));
}

void w_sys_manage_sqlcopy_soft_limit::to_beginset()
{
    dt_end=ui->dateTimeEdit->dateTime();
    password=ui->lineEdit_pwd->text();
    isrun=true;//开始启动
    saveData();
    readData();
    lds_messagebox::information(this, MESSAGE_TITLE_VOID, tr("操作成功"));
}

void w_sys_manage_sqlcopy_soft_limit::to_quit()
{
    this->reject();
}

bool w_sys_manage_sqlcopy_soft_limit::saveData()
{
    QByteArray byte;
    QDataStream in(&byte, QIODevice::WriteOnly);
    in << dt_begin;//开始时间指示会在开机是更新
    in << dt_end;
    in << isrun;
    in << password;

    QFile file(public_sql::settings_txt_soft_limit_dir_file);
    if(file.open(QFile::WriteOnly)){
        file.write(byte);
        file.close();
        return true;
    }
    return false;
}

void w_sys_manage_sqlcopy_soft_limit::readData()
{
    QFile file(public_sql::settings_txt_soft_limit_dir_file);
    if(file.open(QFile::ReadOnly)){
        QByteArray byte;
        byte=file.readAll();
        QDataStream in(&byte, QIODevice::ReadOnly);
        if(!in.atEnd())
            in >> dt_begin;//开始时间指示会在开机是更新
        if(!in.atEnd())
            in >> dt_end;
        if(!in.atEnd())
            in >> isrun;
        if(!in.atEnd())
            in >> password;
        if(dt_end.isNull())
            isrun=false;
    }
    ui->dateTimeEdit->setDateTime(dt_end.isValid()?dt_end:QDateTime::currentDateTime());
    ui->lineEdit_pwd->setText(password);
    ui->label_led->setStyleSheet(isrun?
                                     "background:green":
                                     "background:red");
}
