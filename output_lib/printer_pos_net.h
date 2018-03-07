#ifndef PRINTER_POS_NET_H
#define PRINTER_POS_NET_H

#include <QTcpSocket>
#include <QUdpSocket>
#include "printer_pos.h"


/**
example:
        Printer_POS_NET printer;
        if(printer.tryconnect("192.168.1.201", 9100)){
            foreach(pieceIns ins, printIns){
                printer.writedirect(QByteArray::fromHex(ins.first));
                printer.writedirect(printer_0::trans_u2g(ins.second));
            }
        } else {
            //printer.errorString();
        }
        printer.closeconnect();
*/
class Printer_POS_NET : public Printer_POS
{
public:
    enum TYPE{
        TCP,
        UDP
    };
    Printer_POS_NET();
    virtual ~Printer_POS_NET();

    bool isOpen();
    QString errorString();

    qint64 writedirect(const QByteArray & byte);
    qint64 write(const QString & byte);
    qint64 writeHex(const QString & byte);
    bool tryOpen(const QString &ipport);
    void close();
    QString lastError();
    QString port();
    bool waitForReadyReadv(int msecs);
    QByteArray readAll();

    QAbstractSocket *tcpSocket;
    void resetSocket(TYPE _type);

    bool tryconnect(const QString &hostaddress, quint16 port, int msecs = 5000, bool *manual_break = 0);

    bool closeconnect(int msecs = 5000, bool *manual_break = 0);

    bool waitForReadyRead(int msecs = 5000, bool *manual_break = 0);
private:
    QString ErrString;
    QString _ipport;
};

Q_DECLARE_METATYPE(Printer_POS_NET)

#endif // PRINTER_POS_NET_H
