#include "printer_0.h"
#include "qextserialenumerator.h"
#include "printer_normal.h"
#include "printer_pos.h"
#include <QTextCodec>

const QString printer_0::POS80 = "POS80";
printer_0::printer_0()
{
}

QString printer_0::f_scan_dev_ups()
{
    //获取
    QString ret;
    if(Printer_POS::com_upsiseffect){
        QProcess p;
        p.start("cat /dev/ups");
        if(p.waitForReadyRead(5*1000)){
            ret= p.readAll().trimmed();
        }
        p.close();
    }
    return ret;
}

QString printer_0::trans_g2u(const QByteArray &in)
{
    QTextCodec *c = QTextCodec::codecForName("gb2312");
    return c->toUnicode(in);
}

QByteArray printer_0::trans_u2g(const QString &in)
{
    QByteArray b;
    QTextCodec *c = QTextCodec::codecForName("gb2312");
    b.append(c->fromUnicode(in));
    return b;
}

QStringList printer_0::f_scan_usbPort(bool firstIsEmpty)
{
    QStringList portlist;

#ifdef Q_OS_UNIX

    if(!QFile::exists("/dev/usb")) return QStringList();

    //获取
    char buf_1[255] = {0};
    FILE *fd = popen("ls /dev/usb/*","r");
    if(fd == NULL)return QStringList();
    fread(buf_1, sizeof(buf_1) - 1, 1,  fd);
    buf_1[sizeof(buf_1)] = '\0';
    pclose(fd);

    //过滤
    QStringList proclist = QString(buf_1).split("\n", QString::SkipEmptyParts);
    foreach(const QString &str,proclist) {
        if(str.contains("/dev/usb/lp"))
            portlist<<str;
    }
#endif

#ifdef Q_OS_WIN
    portlist << POS80;
#endif


    if(firstIsEmpty) {
        portlist.push_front("");
    }
    return portlist;
}

bool printer_0::usbPortIsValid(const QString &port)
{
    return port.contains("usb")
            || port.contains("&")
        #ifdef Q_OS_WIN
            || port == POS80
        #endif
            ;
}

QStringList printer_0::f_scan_ttyPort(bool firstIsEmpty)
{
    QList<QextPortInfo> ports=QextSerialEnumerator::getPorts();
    QStringList portlist;
#ifdef Q_OS_UNIX
    foreach(QextPortInfo port, ports)portlist.append(port.physName);
#endif

#ifdef Q_OS_WIN
    foreach(QextPortInfo port, ports)portlist.append(port.portName);
#endif

    //release 版本
#ifdef QT_NO_DEBUG
    for(int k = 0; k < portlist.count(); k++) {
        if(portlist[k].contains("ttyS0")) {
            portlist.takeAt(k);
            break;
        }
    }
#endif

    if(firstIsEmpty) {
        portlist.push_front("");
    }
    return portlist;
}

bool printer_0::comPortIsValid(const QString &port)
{
    return port.contains("tty")
            || port.contains("com")
            || port.contains("COM")
            ;
}

QByteArray printer_0::hanzi2quweima(const QString &hanzi)
{
    QByteArray b0;
    foreach(const QChar &c, hanzi){
        if(QByteArray().append(c).size() >= 3){
            QByteArray b = printer_0::trans_u2g(QString(c));
            //开始转换
            uint bb = b.toHex().toUInt(0, 16);//b.toHex();
            uint bb1 = 0x2020;//国标码
            uint bb2 = 0x8080;//机内码
            bb = bb - bb1 - bb2;

            unsigned p1 = (bb & 0xff00) >> 8;
            unsigned p2 = bb & 0x00ff;

            b0 +=  QString().sprintf("%02d%02d", p1, p2);//QByteArray::number(p1) + QByteArray::number(p2);
        } else {
            QByteArray b = QByteArray().append(c);
            if(QByteArray("") <= b && b <= QByteArray("}")){
                b0 += QByteArray("0") + QByteArray::number(b.toHex().toInt(0,16) - 0x21 + 301);
            }
        }
    }
    return b0;
}

QByteArray  printer_0::_alignRight_fixedLen_int2bcd(const QString &data/*123*/, int len, char c){
    //将data直接反转换为16进制，右对齐，取len长度，不够补c（一个字节0x00之类的）
    return QByteArray::fromHex(QByteArray().append(data)).rightJustified(len, c,true);
}
QByteArray  printer_0::_alignRight_fixedLen_int2hex(const QString &data/*123*/, int len, char c){
    //将data转成16进制，右对齐，取len长度，不够补c（一个字节0x00之类的）
    return QByteArray::fromHex(QByteArray::number(data.toInt(),16)).rightJustified(len, c, true);
}
QByteArray  printer_0::_alignLeft_fixedLen_byte2byte(const QByteArray &data, int len, char c){
    //将data左对齐，取len长度，不够补c（一个字节0x00之类的）
    return data.leftJustified(len,c,true);
}
QByteArray  printer_0::_alignRight_fixedLen_byte2byte(const QByteArray &data, int len, char c){
    //将data左对齐，取len长度，不够补c（一个字节0x00之类的）
    return data.rightJustified(len,c,true);
}

char printer_0::_2xor(const QByteArray &data)
{
    char c0 = 0;
    foreach(char c, data){
        c0 ^= c;
    }
    return c0;
}

QByteArray printer_0::bytecutBybeginEnd(const QByteArray &val, const QByteArray &begin, const QByteArray &end)
{
    int be = val.indexOf(begin);
    int en = val.indexOf(end);
    return val.mid(be, en-be+end.count());
}

/////////////////////
QstandarditemmodelManager::QstandarditemmodelManager()
{
    mCreator();
}

QstandarditemmodelManager::~QstandarditemmodelManager()
{
    if(p) delete p;
}

QStandardItemModel* QstandarditemmodelManager::mCreator()
{
    if(p == 0)
        p = new QStandardItemModel;
    return p;
}



void printer_0_thread::msecSleep(unsigned long msec)
{
    msleep(msec);
}

void printer_0_thread::secSleep(unsigned long sec)
{
    sleep(sec);
}
