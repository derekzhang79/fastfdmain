#include "mainwindow.h"
#include <QApplication>
#include <QSettings>
#include <QTranslator>
#include <QTextCodec>
#include <QtDebug>
#include <QLabel>
#include <QFile>
#include "printer_pos_serial.h"
#include "printer_label_argox_ppla.h"
#include "printer_pos_net.h"
#include <QTableView>
#include "lds.h"
#include <QList>
#include "printer_normal.h"
#include "printer_normal_58.h"
#include "labelprint_main_dialog.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    //设置编码
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");//中文可用//包含头文件
    QTextCodec::setCodecForLocale(codec);

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#else
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForTr(codec);
#endif

    labelprint_main_dialog dialog;
    dialog.printmodel->setHorizontalHeaderLabels(QStringList() << ("打印次数")
                                                 <<  ("品名")
                                                 <<  ("品码")
                                                 <<  ("价格"));
    dialog.printmodel->appendRow(QList<QStandardItem*>()
                                 << new QStandardItem("1")
                                 << new QStandardItem("cookie")
                                 << new QStandardItem("1000")
                                 << new QStandardItem("-12.6")

                                 );
    dialog.inputfinished();
    dialog.setWindowTitle(("条码打印机"));
    dialog.setGeometry(0, 0, 800, 560);
    return dialog.exec();

    return app.exec();
}
