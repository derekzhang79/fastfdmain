#include "printer_pos_win_pos80.h"
#ifdef Q_OS_WIN
#include<QTextDocument>
#include <winspool.h>
#endif

Printer_POS_win_POS80::Printer_POS_win_POS80()
{
    isOpen = false;
}

Printer_POS_win_POS80::~Printer_POS_win_POS80()
{
    close();
}

qint64 Printer_POS_win_POS80::writedirect(const QByteArray &byte)
{
    //发送数据到打印机
    ulong dwCount =byte.length();
#ifdef Q_OS_WIN
    DWORD dwBytesWritten;
    if(!WritePrinter(hPrinter,(unsigned char *)byte.data(),dwCount,&dwBytesWritten))
    {
        EndPagePrinter(hPrinter);
        EndDocPrinter(hPrinter);
        ClosePrinter(hPrinter);
        return 0;
    }
    //检查实际写入数据是否与原始数据大小相符
    if(dwBytesWritten != dwCount)
    {
        EndPagePrinter(hPrinter);
        EndDocPrinter(hPrinter);
        ClosePrinter(hPrinter);
        return 0;
    }
#endif
    return dwCount;
}

qint64 Printer_POS_win_POS80::write(const QString &in)
{
    return writedirect(printer_0::trans_u2g(in));
}

qint64 Printer_POS_win_POS80::writeHex(const QString &in)
{
    return writedirect(QByteArray::fromHex(QByteArray().append(in)));
}

bool Printer_POS_win_POS80::tryOpen(const QString &port)
{
    isOpen = false;
#ifdef Q_OS_WIN
    this->_port = "";
    DOC_INFO_1A DocInfo;
    DWORD dwJob;
    LPSTR szPrinterName;

    szPrinterName = (char*)port.toLocal8Bit().data();
    //获取打印机的handle
    if(!OpenPrinterA(szPrinterName,&hPrinter,NULL))
    {
        errstring = QObject::tr("操作失败") + "OpenPrinterA";
        return false;
    }
    //填充打印文档的DOC_INFO_1A
    DocInfo.pDocName = "BarCode";
    DocInfo.pOutputFile = NULL;
    DocInfo.pDatatype = "RAW";

    //通知打印服务，准备开始打印文档
    if((dwJob = StartDocPrinterA(hPrinter,1,(PBYTE)&DocInfo)) == 0)
    {
        ClosePrinter(hPrinter);
        errstring = QObject::tr("操作失败") + "StartDocPrinterA";
        return false;
    }
    //开始一页的打印
    if(!StartPagePrinter(hPrinter))
    {
        EndDocPrinter(hPrinter);
        ClosePrinter(hPrinter);
        errstring = QObject::tr("操作失败") + "StartPagePrinter";
        return false;
    }

    isOpen = true;
    this->_port = port;
#endif
    return true;
}

void Printer_POS_win_POS80::close()
{
    isOpen = false;
#ifdef Q_OS_WIN
    //结束一页的打印
    if(!EndPagePrinter(hPrinter))
    {
        EndDocPrinter(hPrinter);
        ClosePrinter(hPrinter);
        return ;
    }


    //通知打印服务，文档打印结束
    if(!EndDocPrinter(hPrinter))
    {
        ClosePrinter(hPrinter);
        return ;
    }


    //收回handle
    ClosePrinter(hPrinter);
#endif
    return ;
}

QString Printer_POS_win_POS80::lastError()
{
    return errstring;
}

QString Printer_POS_win_POS80::port()
{
    return _port;
}

bool Printer_POS_win_POS80::waitForReadyReadv(int msecs)
{
    return true;
}

QByteArray Printer_POS_win_POS80::readAll()
{
    return "";
}
