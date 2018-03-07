#ifndef PRINTER_POS_PARALLEL_H
#define PRINTER_POS_PARALLEL_H
#include <stdio.h>
#include "printer_pos.h"

class Printer_POS_PARALLEL : public Printer_POS
{
public:
    static QString printPort();
public:
    Printer_POS_PARALLEL();
    virtual ~Printer_POS_PARALLEL();

    qint64 writedirect(const QByteArray & byte);
    qint64 write(const QString & byte);
    qint64 writeHex(const QString & byte);
    bool tryOpen(const QString &ipport);
    void close();
    QString lastError();
    QString port();
    bool waitForReadyReadv(int msecs);
    QByteArray readAll();

private:
    QString m_error;
    QString m_port;

    FILE *m_file;
};

#endif // PRINTER_POS_PARALLEL_H
