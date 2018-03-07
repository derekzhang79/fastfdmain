#include "printer_normal_kf58.h"
////注册
static Printer_Normal_virtual_Register<Printer_Normal_KF58> r(QList<QStandardItem*>() << new QStandardItem("Printer_Normal_KF58"));
////
Printer_Normal_KF58::Printer_Normal_KF58()
{
}

Printer_Normal_KF58::~Printer_Normal_KF58()
{

}

blockIns Printer_Normal_KF58::tran(const blockIns &bins, int xinye_beep)
{
    Q_UNUSED(xinye_beep)
    blockIns tmpins=bins;
    for(int c=0,c_count=tmpins.count(); c<c_count;c++){//清除
        tmpins[c].first.clear();
    }
    if(tmpins.count() > 0){
        tmpins[0].first="1B6300";
        tmpins << pieceIns("20200a20200a20200a","");//出纸
    }

    return tmpins;
}

QString Printer_Normal_KF58::name()
{
    return "KF58";
}
