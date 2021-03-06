#include "printer_normal_xinye.h"
////注册
static Printer_Normal_virtual_Register<Printer_Normal_XINYE> r(QList<QStandardItem*>() << new QStandardItem("Printer_Normal_XINYE"));
////
Printer_Normal_XINYE::Printer_Normal_XINYE()
{
}

blockIns Printer_Normal_XINYE::tran(const blockIns &bins, int xinye_beep_count)
{
    blockIns tmpins=bins;
    //1B 42 n t
    //n蜂鸣次数 t每次蜂鸣时长 (tx50)ms
    int xinye_beep_timeout =4;
    QString pre;
    pre =
            QString("1B42") +
            QString::number(xinye_beep_count, 16).rightJustified(2, '0', true) +
            QString::number(xinye_beep_timeout, 16).rightJustified(2, '0', true);
    tmpins.append(pieceIns(pre.toLocal8Bit(), ""));

    return tmpins;
}
QString Printer_Normal_XINYE::name()
{
    return "XINYE";
}
