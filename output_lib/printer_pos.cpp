#include "printer_pos.h"
#include <QtDebug>
#include "printer_pos_serial.h"
#include "printer_pos_net.h"
#include "printer_pos_vidpid.h"
#include "printer_pos_parallel.h"
#include "printer_pos_win_pos80.h"

//静态函数的定义，顺序依赖于pro或者pri里的cpp的定义顺序
QstandarditemmodelManager Printer_POS::class_name_model;
bool Printer_POS::com_upsiseffect = false;
Printer_POS::Printer_POS()
{
}

Printer_POS::~Printer_POS()
{
    qDebug() << __FUNCTION__;
}

Printer_POS *Printer_POS::get_pos_printer_by_path(const QString &path)
{
    if(printer_0::comPortIsValid(path)){
        return new Printer_POS_SERIAL;
    }
    if(path.contains("usb")){//u口
        Printer_POS_SERIAL *p = new Printer_POS_SERIAL;
        p->setType(Printer_POS_SERIAL::USB);
        return p;
    }
    if(path.contains("&")){ //vidpid
        return new Printer_POS_VIDPID;
    }
    if(path.count(".")==3){//网口
        return new Printer_POS_NET;
    }
    if(path == Printer_POS_PARALLEL::printPort()) {
        return new Printer_POS_PARALLEL;
    }
    if(path == printer_0::POS80) {
        return new Printer_POS_win_POS80;
    }
    return 0;
}

Printer_POS_auto_destroy::Printer_POS_auto_destroy(Printer_POS *printer)
{
    this->printer = printer;
}

Printer_POS_auto_destroy::~Printer_POS_auto_destroy()
{
    if(printer)
        delete printer;
}
