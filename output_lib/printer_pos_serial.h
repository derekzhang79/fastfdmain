#ifndef PRINTER_POS_SERIAL_H
#define PRINTER_POS_SERIAL_H


//=====================================================================
//【0】这里再次说明，Polling方式是不能使用readyRead()信号的，所以我们需要自己设置定时器，来不断地读取缓冲区的数据。


class QextSerialPort;
#include <QThread>
#include <QObject>
#include "qextserialport.h"
#include "printer_pos.h"
/**
exmaple:
            QString path="/dev/usb/lp0";
            Printer_POS_SERIAL printer;
            if(path.contains("usb"))printer.setType(Printer_POS_SERIAL::USB);
            if(printer.tryOpen(path)){//打开USB口
                foreach(pieceIns ins, printIns){
                    printer.writedirect(QByteArray::fromHex(ins.first));
                    printer.writedirect(printer_0::trans_u2g(ins.second));
                }
                printer.close();
            } else {
                // (error=path+printer.lastError());
            }
*/
//目前软件外部可以设置波特率，路径也可以直接设置，路径设置权限最高
class  Printer_POS_SERIAL:public Printer_POS
{
public:
    enum PORT_TYPE{
        SERIAL,
        USB
    };
    PortSettings psettings;

    static void SERIAL_PORT_MUTEX_COM_MAP_CLOSE_ALL();
    static void SERIAL_PORT_MUTEX_COM_MAP_DELETE_ALL();
private:
    static QMap<QString, QextSerialPort *> SERIAL_PORT_MUTEX_COM_MAP;
    static QextSerialPort *myCom(const QString &port);
    QString _port;
public:
    PORT_TYPE _type;
    QString ErrString;
    QByteArray receive_data;
    bool flag;
public:
    Printer_POS_SERIAL();
   virtual ~Printer_POS_SERIAL();

    /*gbk,和十六进制 的读写*/
    virtual qint64 writedirect(const QByteArray & byte);
    virtual qint64 write(const QString &in);//if n have write then n is return; if n == -1 then error
    virtual qint64 writeHex(const QString &in);//if n have write then n is return; if n == -1 then error
    virtual bool tryOpen(const QString &port);
    virtual void close();
    virtual QString lastError();
    virtual QString port();
    virtual bool waitForReadyReadv(int msecs);
    virtual QByteArray readAll();

    /*设置打印类型, 在tryopen前设置有效*/
    void setType( PORT_TYPE type);

    /*返回可以访问打数据字节数*/
    qint64 bytesAvailable();
    void flush();
    qint64 read(char * data, qint64 maxSize);
    bool isOpen();
    void setBaudRate(int baudRate);

    QStringList fixedPorts();
    QString pathBaudRate(const QString &path, int baudRate);
    static QHash<int, QString> creat_baudrates();
private:
    bool FLOW_XONXOFF_readydata();
    bool FLOW_XONXOFF_wait_for();
    bool FLOW_XONXOFF_enable;
    bool devOpen();
};


Q_DECLARE_METATYPE(Printer_POS_SERIAL)


#endif // PRINTER_POS_SERIAL_H
