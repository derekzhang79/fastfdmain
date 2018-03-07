#include "printer_normal_chuanwei.h"

////注册
static Printer_Normal_virtual_Register<Printer_Normal_CHUANWEI> r(QList<QStandardItem*>() << new QStandardItem("Printer_Normal_CHUANWEI"));
////

Printer_Normal_CHUANWEI::Printer_Normal_CHUANWEI()
{
}

blockIns Printer_Normal_CHUANWEI::tran(const blockIns &bins, int xinye_beep)
{
    return bins;
}

QString Printer_Normal_CHUANWEI::name()
{
    return QObject::tr("川唯");
}
