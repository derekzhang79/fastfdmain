#include "printer_pos_parallel.h"

QString Printer_POS_PARALLEL::printPort()
{
    return "LPT1:";
}

Printer_POS_PARALLEL::Printer_POS_PARALLEL()
{
    m_file = 0;
}

Printer_POS_PARALLEL::~Printer_POS_PARALLEL()
{
    qDebug() << __FUNCTION__;

}

qint64 Printer_POS_PARALLEL::writedirect(const QByteArray &byte)
{
    return fwrite(byte.data(),byte.size(),1,m_file);
}

qint64 Printer_POS_PARALLEL::write(const QString &byte)
{
    return writedirect(printer_0::trans_u2g(byte));

}

qint64 Printer_POS_PARALLEL::writeHex(const QString &byte)
{
    return writedirect(QByteArray::fromHex(QByteArray().append(byte)));
}

bool Printer_POS_PARALLEL::tryOpen(const QString &ipport)
{
    m_error = "";
    m_port = ipport;
//    m_file = fopen("LPT1:","r+");
    m_file = fopen(ipport.toLocal8Bit().data(),"r+");
    if(m_file == NULL)
    {
        m_error =  "sorry! open LPT1 error!!";
        return false;
    }

    return true;
}

void Printer_POS_PARALLEL::close()
{
    fclose(m_file);
    m_file = NULL;
}

QString Printer_POS_PARALLEL::lastError()
{
    return m_error;
}

QString Printer_POS_PARALLEL::port()
{
    return m_port;
}

bool Printer_POS_PARALLEL::waitForReadyReadv(int msecs)
{
    Q_UNUSED(msecs)

    return false;
}

QByteArray Printer_POS_PARALLEL::readAll()
{
    QByteArray ret;
    return ret;
}

