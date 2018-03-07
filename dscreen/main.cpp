#include <QApplication>
#include "doublewidget_normal.h"
#include <QFile>
#include <QTextCodec>
#include <QProcess>
#include <QDesktopWidget>
#include <QSettings>
#include "lds.h"
#include <QLabel>
#include <QTranslator>
#include "doublewidget_1024x600.h"
#include <qdir.h>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
#if defined(Q_OS_UNIX )
    QDir::setCurrent("/usr/desk");
#endif
#if defined(QT_DEBUG)
    QDir::setCurrent(app.applicationDirPath());
#endif

    //设置编码
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");//中文可用//包含头文件
    QTextCodec::setCodecForLocale(codec);
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#else
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForTr(codec);
#endif
    //! 1-----------------配置文件
    QSettings conf(lds::localdataPath+ "/fastfd.ini", QSettings::IniFormat);
    lds::sysconf = &conf;

    lds::               LDS_INIT();                                 //公共参数里的初始化

    if(lds::get_soft_curent_language_first()=="EN"){
        //翻译
        QTranslator *translator = new QTranslator;
        if(translator->load("userdata/settings/language/"+lds::get_soft_curent_language_first()+"_fastfd.qm")) {
            qApp->installTranslator(translator);
        }
    }
    //样式表
    QFile file(QString("userdata/settings/skin/fastfd_%1.qss")
               .arg(lds::sysconf->value("system_setting/skin_name", "obsidian").toString()));
    if(file.open(QFile::ReadOnly)){
        //logo
        app.setStyleSheet(file.readAll());
        file.close();
    } else {
        qDebug() << file.errorString();
    }

    QSize dsize = lds::dsize();
    if(argc > 1) {
        dsize = QSize(QString(argv[1]).toInt(), QString(argv[2]).toInt());
    }
    for(int k = 0; k < argc; k++) {
        qDebug() << __LINE__ << argv[k];
    }
    int fuping_mode = lds::sysconf->value("system_setting/fuping_mode", -1).toInt();
    QDialog *dialog = 0;
//#ifdef QT_DEBUG
//    fuping_mode = 1;
//    dsize = QSize(800, 600);
//#endif
    switch(fuping_mode) {
    case 0://普通副屏
        dialog = new doublewidget_normal(dsize);
        dialog->move(0, 0);
        break;
    case 1://1024*600客显
        dialog = new doublewidget_1024x600(dsize);
        dialog->move(0, 0);
        break;
    case 2://双屏异显
        dialog = new doublewidget_normal(dsize);
        dialog->move(app.desktop()->availableGeometry().topLeft() + QPoint(lds::MAIN_WINDOW_SIZE.width(), 0));
        break;
    case -1://不启用
    default:
        return 1;
    }
    //显示
    bool ret = app.exec();

    if(dialog) delete dialog;
    return ret;
}
