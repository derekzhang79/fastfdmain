#include "printer_pos_net.h"
#include <QTcpSocket>
#include <QStringList>
#include <QHostAddress>
#include <QDebug>
#include "printer_0.h"


Printer_POS_NET::Printer_POS_NET():tcpSocket(new QTcpSocket)
{
}

Printer_POS_NET::~Printer_POS_NET()
{
    qDebug() << __FUNCTION__;
    closeconnect();
    delete tcpSocket;
}

bool Printer_POS_NET::tryconnect(const QString &hostaddress, quint16 port, int msecs, bool *manual_break)
{
    ErrString.clear();
    if(printer_0::f_scan_dev_ups() == "1"){
        ErrString=(QObject::tr("检测到使用ups,供电暂时不打印小票"));
        return false;
    }
    //manual_break优先级最高
    if(manual_break && *manual_break) return false;

    //连接服务器
    int step = 1000;
    int loopcount = msecs / step;
    while(loopcount >= 1 && loopcount --){
        if(manual_break && *manual_break) return false;
        tcpSocket->connectToHost(QHostAddress(hostaddress),port);
        if(tcpSocket->state() == QAbstractSocket::ConnectedState
                || tcpSocket->waitForConnected(step)){
            return true;
        } else {
            qDebug() << tcpSocket->errorString();
        }
    }
    return false;
}

bool Printer_POS_NET::closeconnect(int msecs, bool *manual_break)
{
    //manual_break优先级最高
    if(manual_break && *manual_break) return false;
    //若未连接
    if(tcpSocket->state() == QAbstractSocket::UnconnectedState)
        return true;
    //断开服务器
    int step = 1000;
    int loopcount = msecs / step;
    while(loopcount >= 1 && loopcount --){
        if(manual_break && *manual_break) return false;
        tcpSocket->disconnectFromHost();
        if (tcpSocket->state() == QAbstractSocket::UnconnectedState
                || tcpSocket->waitForDisconnected(step)){
            return true;
        }
    }
    return false;
}

bool Printer_POS_NET::waitForReadyRead(int msecs, bool *manual_break)
{
    //manual_break优先级最高
    if(manual_break && *manual_break) return false;

    int step = 1000;
    int loopcount = msecs / step;
    while(loopcount >= 1 && loopcount --){
        if(manual_break && *manual_break) return false;
        if(tcpSocket->waitForReadyRead(step)){
            return true;
        }
    }
    qDebug() << tcpSocket->errorString();
    return false;
}

bool Printer_POS_NET::isOpen()
{
    return tcpSocket->state() == QAbstractSocket::ConnectedState;
}

QString Printer_POS_NET::errorString()
{
    return ErrString.isEmpty()?tcpSocket->errorString():ErrString;
}

qint64 Printer_POS_NET::writedirect(const QByteArray &byte)
{
    return tcpSocket->write(byte);
}

qint64 Printer_POS_NET::write(const QString &byte)
{
    return writedirect(printer_0::trans_u2g(byte));
}

qint64 Printer_POS_NET::writeHex(const QString &byte)
{
    return writedirect(QByteArray::fromHex(byte.toLocal8Bit()));
}

bool Printer_POS_NET::tryOpen(const QString &ipport)
{
    _ipport=ipport;
    QStringList pport=ipport.split(":");
    QString ip = pport.value(0);
    QString port = pport.value(1).trimmed();
    return tryconnect(ip, (port.isEmpty()?9100:port.toInt()));
}

void Printer_POS_NET::close()
{
    tcpSocket->waitForBytesWritten(5000);
    closeconnect();
}

QString Printer_POS_NET::lastError()
{
    return errorString();
}

QString Printer_POS_NET::port()
{
    return _ipport;
}

bool Printer_POS_NET::waitForReadyReadv(int msecs)
{
    return tcpSocket->waitForReadyRead(msecs);
}

QByteArray Printer_POS_NET::readAll()
{
    return tcpSocket->readAll();
}

void Printer_POS_NET::resetSocket(Printer_POS_NET::TYPE _type)
{
    delete tcpSocket;
    tcpSocket = 0;
    switch(_type){
    case TCP:
        tcpSocket = new QTcpSocket;
        break;
    case UDP:
        tcpSocket = new QUdpSocket;
        break;
    }
}

