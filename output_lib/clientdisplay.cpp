#include "clientdisplay.h"
#include "clientdisplay_no.h"
#include "printer_pos_serial.h"
#include <QtDebug>

static bool contain_zn(const QString &text){
    QByteArray p1;
    if(text.trimmed().isEmpty()) return false;

    p1.append(text);
    return p1.length() != text.length();
}

ClientDisplay::ClientDisplay(const QString &type)
{
    _type = type;
    display =  0;
}

ClientDisplay::ClientDisplay()
{
    display = 0;
}

void ClientDisplay::setType(const QString &type)
{
    _type = type;
}

ClientDisplay::~ClientDisplay()
{
    if(display)   delete display;
}

bool ClientDisplay::writeCom(int sqr, const QString &in, const QString &devpath, const QString &name)
{    
    if(devpath.isEmpty()) return false;

    bool f=false;
    switch(creat_clientdisplay_ledtypes().indexOf(_type)){
    case 0://LED-8
        if(display == 0)
            display = new ClientDisplay_NO;

        f=display->writeCom(sqr,in,devpath);
        break;
    case 1://VFD220
    {
        err0.clear();
        Printer_POS_SERIAL printer;
        if((f=printer.tryOpen(devpath))){
            printer.setBaudRate(9600); //波特率必须时9600 才可以通信
            printer.writeHex("0C");//22. 清除屏幕的所有字符

            switch(sqr){
            case 1://单价 PRICE
                if(!contain_zn(name) )
                    printer.writedirect(QByteArray().append(name));
                printer.writeHex("1F240102");//12.  x  y光标移到指定的位置 [1FH][24H]x  y;1H<=x<=14H，01H<=y<=02H
//                printer.writedirect(QByteArray("PRICE:").append(QString().sprintf("%0.2f",in.toDouble())));
                printer.writedirect(QByteArray().append(QString().sprintf("%0.2f",in.toDouble())));
                break;
            case 2://总计 TATOL
                printer.writedirect(QByteArray().append(name));
                printer.writeHex("1F240102");//12.  x  y光标移到指定的位置 [1FH][24H]x  y;1H<=x<=14H，01H<=y<=02H
                printer.writedirect(QByteArray("TATOL:").append(QString().sprintf("%0.2f",in.toDouble())));
                break;
            case 3://收款 CASH
                printer.writedirect(QByteArray().append(name));
                printer.writeHex("1F240102");//12.  x  y光标移到指定的位置 [1FH][24H]x  y;1H<=x<=14H，01H<=y<=02H
                printer.writedirect(QByteArray("CASH:").append(QString().sprintf("%0.2f",in.toDouble())));
                break;
            case 4://找零 CHANGE
                printer.writedirect(QByteArray().append(name));
                printer.writeHex("1F240102");//12.  x  y光标移到指定的位置- [1FH][24H]x  y;1H<=x<=14H，01H<=y<=02H
                printer.writedirect(QByteArray("CHANGE:").append(QString().sprintf("%0.2f",in.toDouble())));
                break;
            }
        } else {
            err0 = printer.lastError();
        }
    }
        break;
    }

    return f;
}

bool ClientDisplay::cleanCom(const QString &devpath, const QString &/*baudrate*/)
{
    bool f=false;
    switch(creat_clientdisplay_ledtypes().indexOf(_type)){
    case 0://LED-8
        if(display == 0)
            display = new ClientDisplay_NO;

        f=display->cleanCom(devpath);

        break;
    case 1://VFD220
    {
        err0.clear();
        Printer_POS_SERIAL printer;
        if((f=printer.tryOpen(devpath))){
            printer.setBaudRate(9600);//波特率必须时9600 才可以通信
            printer.writeHex("0C");//22. 清除屏幕的所有字符
        } else {
            err0 = printer.lastError();
        }
    }

        break;
    }

    return f;
}

bool ClientDisplay::startShowinfo(const QString &text, const QString &devpath)
{
    bool f=false;
    switch(creat_clientdisplay_ledtypes().indexOf(_type)){
    case 0://LED-8
        f=true;
        break;
    case 1://VFD220
    {
        err0.clear();
        Printer_POS_SERIAL printer;
        if((f=printer.tryOpen(devpath))){
            printer.setBaudRate(9600); //波特率必须时9600 才可以通信
            printer.writeHex("0C");//22. 清除屏幕的所有字符
            printer.writedirect(QByteArray().append(text));
        } else {
            err0 = printer.lastError();
        }
    }
        break;
    }

    return f;
}

QString ClientDisplay::lastError()
{
    if(display == 0)
        display = new ClientDisplay_NO;

    return display->lastError()+err0;
}

QStringList ClientDisplay::creat_clientdisplay_ledtypes()
{
    static QStringList clientdisplay_ledtypes;
    if(clientdisplay_ledtypes.isEmpty()){
        clientdisplay_ledtypes << "LED-8"<<"VFD220";
    }
    return clientdisplay_ledtypes;
}
