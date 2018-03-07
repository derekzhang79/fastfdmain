#include "fastfd_update.h"
#include "ui_fastfd_update.h"
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>
#include <QTimer>
#include <QProcess>
#include <QCryptographicHash>
#include <QMessageBox>
#include <QtDebug>
#include <QCloseEvent>

const QString fastfd_update::gs_soft_version;
const QString fastfd_update::cy_path;
const QString fastfd_update::unzip_str;
const QString fastfd_update::unzip_password;


void fastfd_update::setSoftVersiontCyPath(const QString &gs_soft_version)
{
    *(QString *)&(fastfd_update::gs_soft_version) = gs_soft_version;
    QString sqlVersion = gs_soft_version.left(4);//V4.0

    *(QString *)&(fastfd_update::cy_path) = sqlVersion.compare("V4.0") >=0 ? "cyNew" : "cy";

    *(QString *)&(fastfd_update::unzip_str) = sqlVersion.compare("V4.0") >=0 ? "unzip" : "punzip" ;

    *(QString *)&(fastfd_update::unzip_password) = sqlVersion.compare("V4.0") >=0 ? "linpos.scpos.com" :  "www.scpos.com";

}

fastfd_update::fastfd_update(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::fastfd_update)
{
    ui->setupUi(this);
    checkstate = false;
    isdownload = false;
    ui->progressBar->setFormat("%p%(%v/%m)");
    ui->progressBar->setValue(0);
    ui->lineEdit_cur_version->setEnabled(false);
    ui->lineEdit_newest_version->setEnabled(false);
    ui->lineEdit_cur_version->setText(gs_soft_version);

    connect(ui->pushButton_update,SIGNAL(clicked()),this,SLOT(todownload()));
    connect(ui->pushButton_cancel,SIGNAL(clicked()),this,SLOT(tocancel()));
}

fastfd_update::~fastfd_update()
{
    delete ui;
}

bool fastfd_update::check_need_update(QString *errstring)
{
    if(!tocheckversion(errstring)) {
        return false;
    }
    if(ui->lineEdit_cur_version->text() >= ui->lineEdit_newest_version->text()) {
        if(errstring) *errstring = tr("已经是最新版本");
        return false;
    }
    return true;
}

void fastfd_update::tocheck_down()
{
    if(!gs_soft_version.isEmpty()) {
        tocheckversion();
        todownload();
    }
}

void fastfd_update::closeEvent(QCloseEvent *e)
{
#ifdef QT_DEBUG
    QDialog::closeEvent(e);
    return;
#endif
    if(isdownload) e->ignore();
    else
        QDialog::closeEvent(e);
}

void fastfd_update::showEvent(QShowEvent *e)
{
    QDialog::showEvent(e);
}

void fastfd_update::toprogress(qint64 bytesReceived, qint64 bytesTotal)
{
    bytesReceived = bytesReceived;
    bytesTotal = bytesTotal;
    if(ui->progressBar->maximum() != bytesTotal)
        ui->progressBar->setMaximum(bytesTotal);
    ui->progressBar->setValue(bytesReceived);

    //更新下载状态
    if(!isdownload)
        isdownload = true;
}

bool fastfd_update::tocheckversion(QString *errstring)
{
    ui->progressBar->setValue(0);
    QString version;
    if(getNewVersion(version, &isdownload ,errstring)){
        ui->lineEdit_newest_version->setText(version.trimmed());
        return true;
    }
    return false;
}

void fastfd_update::todownload()
{
    QString errstring;
    QFile file;
    QByteArray      appArray;
    QProcess p;
    QString newupdatefilename;
    QString newupdatefilepath;
    QString targetpath;
#if defined(QT_DEBUG) || defined(Q_OS_WIN)
    targetpath = qApp->applicationDirPath();
#else
    targetpath = "/usr/desk";
#endif
    pushbutton_auto_enable d1(ui->pushButton_cancel);
    pushbutton_auto_enable d2(ui->pushButton_update);


    if(ui->lineEdit_cur_version->text().trimmed().isEmpty()) {
        errstring = tr("获取本地版本号失败");
        goto end;
    }

    if(ui->lineEdit_cur_version->text() >= ui->lineEdit_newest_version->text()) {
        errstring = tr("已经是最新版本");
        goto end;
    }

    //获取最大版本号
    if(ui->lineEdit_newest_version->text().isEmpty()){
        errstring = ui->label_newest_version->text()+tr("为空");
        goto end;
    }
    //开始下载


#if defined(Q_OS_WIN)
    newupdatefilename = ui->lineEdit_newest_version->text()+".exe";
    newupdatefilepath = qApp->applicationDirPath() + "/" + ui->lineEdit_newest_version->text()+".exe";
#endif


#if defined(Q_OS_UNIX)
    newupdatefilename = ui->lineEdit_newest_version->text()+".zip";
    newupdatefilepath = "/userdata/" + ui->lineEdit_newest_version->text()+".zip";
#endif

    {
        QUrl url;
        url.setUrl("ftp://115.28.155.165/" +cy_path + "/" + newupdatefilename);
        url.setUserName("cz");
        url.setPassword("111111");
        QNetworkRequest request;
        request.setUrl(url);

        QNetworkAccessManager networkAccessManager;
        /*计时*/fastfd_update_downstate overtimecheck(&isdownload);
        overtimecheck.startTimer(5000);
        connect(&networkAccessManager, SIGNAL(finished(QNetworkReply*)), &overtimecheck.loop, SLOT(quit()));
        QNetworkReply *reply = networkAccessManager.get(request);
        connect(reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(toprogress(qint64,qint64)));
        overtimecheck.loop.exec(QEventLoop::ExcludeUserInputEvents);
        /*超时*/if(overtimecheck.isOverTime()) { reply->abort();  errstring = tr("网络超时"); goto end; }

        if(reply->error() != QNetworkReply::NoError) {
            errstring = "net work error";//reply->errorString();
            goto end;
        }
        appArray = reply->readAll();
    }
    //解压完成
    file.setFileName(newupdatefilepath);
    if(!file.open(QFile::WriteOnly)){
        errstring = file.errorString();
        goto end;
    }
    file.write(appArray);
    file.close();

#if defined(Q_OS_WIN)
    if(!QFile::remove("SoftPos.exe")) {
        errstring = file.errorString();
        goto end;
    }
    if(!file.rename("SoftPos.exe")) {
        errstring = file.errorString();
        goto end;
    }
#endif

#ifdef Q_OS_UNIX
    //解压文件 直接重写，密码解压
    p.start(QString("%1 -o -P %2 %3 -d %4")
            .arg(unzip_str)
            .arg(unzip_password)
            .arg(newupdatefilepath)
            .arg(targetpath));
    if(!p.waitForFinished()){
        errstring = tr("操作超时");
        goto end;
    }
    //删除源文件
    if(!file.remove()) {
        errstring = file.errorString();
        goto end;
    }
    system("chmod +x /usr/desk/install.sh");
    system("/usr/desk/install.sh");

    system("chmod +x /usr/desk/pos/SoftPos");
    system("chmod +x /usr/desk/pos/fastfd_updated");
#endif
    //升级成功
    QMessageBox::information(this, tr("提示"), tr("升级成功"));
    QProcess::startDetached(qApp->applicationDirPath()+"/SoftPos", QStringList(),
                        #if defined(QT_DEBUG) || defined(Q_OS_WIN)
                            qApp->applicationDirPath()
                        #else
                            "/usr/desk"
                        #endif
                            );
    this->reject();
    return;
end:
    QMessageBox::warning(this, tr("提示"), errstring);
    return;
}

void fastfd_update::tocancel()
{
    this->reject();
}

bool fastfd_update::getNewVersion(QString &version, bool *isdownload, QString *errstring)
{
    QUrl url;
    url.setUrl("ftp://115.28.155.165/" +cy_path + "/version.txt");
    url.setUserName("cz_r");
    url.setPassword("111111");
    QNetworkRequest request;
    request.setUrl(url);
    QNetworkAccessManager networkAccessManager;
    /*计时*/fastfd_update_downstate overtimecheck(isdownload);
    overtimecheck.startTimer(5000);
    connect(&networkAccessManager, SIGNAL(finished(QNetworkReply*)), &overtimecheck.loop, SLOT(quit()));
    QNetworkReply *reply = networkAccessManager.get(request);
    overtimecheck.loop.exec(QEventLoop::ExcludeUserInputEvents);
    /*超时*/if(overtimecheck.isOverTime()) { reply->abort();  if(errstring) *errstring = tr("网络超时"); goto end; }

    if(reply->error() != QNetworkReply::NoError) {
        if(errstring) *errstring = reply->errorString();
        goto end;
    }
    version  = reply->readAll();
    version  = version.trimmed();
    return true;
end:
    return false;
}

fastfd_update_downstate::fastfd_update_downstate(bool *_isDW)
    : isDW(_isDW)
{
    *isDW = false;
    isovertime = false;
}

fastfd_update_downstate::~fastfd_update_downstate()
{
    *isDW = false;
}

void fastfd_update_downstate::startTimer(int msec)
{
    timer.setSingleShot(true);
    connect(&timer, SIGNAL(timeout()), this, SLOT(toCheck()));
    timer.start(msec);
}

bool fastfd_update_downstate::isOverTime()
{
    return isovertime;
}

void fastfd_update_downstate::toCheck()
{
    if(*isDW) {

    } else {
        loop.quit();
        isovertime = true;
    }
}
