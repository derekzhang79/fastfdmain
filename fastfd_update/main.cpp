#include "mainwindow.h"
#include <QApplication>
#include <QTextCodec>
#include "fastfd_update.h"
#include <QtDebug>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


    //设置编码
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");//中文可用//包含头文件
    QTextCodec::setCodecForLocale(codec);

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#else
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForTr(codec);
#endif

    switch(argc) {
    case 2:
        fastfd_update::setSoftVersiontCyPath(argv[1]);
        break;
    case 3:
        break;
    }

    fastfd_update w;
    w.setWindowTitle("software update"+QString::number(argc));
    w.setWindowIcon(QPixmap(":/fastfd.png"));
    w.show();
    QTimer::singleShot(100, &w, SLOT(tocheck_down()));

    return a.exec();
}
