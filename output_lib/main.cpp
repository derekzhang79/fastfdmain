#include <QApplication>
#include "printer_0.h"
#include <QtDebug>
#include "printer_normal.h"
#include "printer_pos_vidpid.h"
#include "printer_pos_serial.h"
#include <QTextCodec>
#include "dialog.h"

static bool contain_zn(const QString &text){
    QByteArray p1;
    p1.append(text);
    return p1.length() != text.length();
}
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

//    qDebug() << contain_zn(("kk"));
//    qDebug() << Printer_Normal::get_normal_ins_map_ref().values();
//    qDebug() << Printer_Normal::get_normal_print_map_ref().keys();
//    bool f;
//    Printer_POS_SERIAL printer;
//    if((f=printer.tryOpen("/dev/ttyS0"))){
//        printer.setBaudRate(9600/*CommonHeader::creat_baudrates().key(_baudrate)*/); //波特率必须时9600 才可以通信
////        qDebug() <<  printer.writeHex("0205433903");
////        qDebug() <<  printer.writeHex("026749");



//        qDebug() <<  printer.writedirect("27,81,65+'xx' ");
////        printer.writedirect(QByteArray().append("name"));
////        printer.writeHex("1F240102");//12.  x  y光标移到指定的位置 [1FH][24H]x  y;1H<=x<=14H，01H<=y<=02H
////        printer.writedirect(QByteArray("PRICE:").append(QString().sprintf("%0.2f",12.9)));
//    }

//    if(!f) qDebug() << "打开失败";

    Dialog dialog;
    dialog.show();

    return app.exec();
}
