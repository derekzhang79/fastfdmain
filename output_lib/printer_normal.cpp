#include "printer_normal.h"
#include "printer_normal_80.h"
#include "printer_normal_80_old.h"
#include "printer_pos.h"

//静态函数的定义，顺序依赖于pro或者pri里的cpp的定义顺序

QstandarditemmodelManager Printer_Normal::class_name_model;
Printer_Normal::Printer_Normal()
{

}

Printer_Normal::~Printer_Normal()
{

}


QByteArray Printer_Normal::init()
{
    QByteArray r = QByteArray::fromHex("1B40");
    return r.toHex();
}

QByteArray Printer_Normal::scale(char c)
{
    QByteArray r = QByteArray::fromHex("1D21")+c;
    return r.toHex();
}

QByteArray Printer_Normal::align(char c)
{
    QByteArray r;
    switch(c){
    case 0x00://中
        r = QByteArray::fromHex("1B6101");
        break;
    case 0x01://右
        r = QByteArray::fromHex("1B6102");
        break;
    case 0x10://左
        r = QByteArray::fromHex("1B6100");
        break;
    }
    return r.toHex();
}

QByteArray Printer_Normal::bold(char c)
{
    //01加粗 00取消加粗
    QByteArray r;
    switch(c){
    case 0x00://取消加粗
        r = QByteArray::fromHex("1B4500");
        break;
    case 0x01://加粗
        r = QByteArray::fromHex("1B4501");
        break;
    }
    return r.toHex();
}

QByteArray Printer_Normal::gonum(char c)
{
    QByteArray r =  QByteArray::fromHex("1D56")+c;
    return r.toHex();
}

Printer_Normal *Printer_Normal::byName(const QString &fname, QString &return_typename)
{
    Printer_Normal *printtype = 0;
    QByteArray typeName;
    int typeID;
    for(int row = 0, rowcount = class_name_model.mCreator()->rowCount(); row < rowcount ;row++){
        typeName = class_name_model.mCreator()->index(row, 0).data().toByteArray();
        typeID = QMetaType::type(typeName);
        if(typeID != 0){
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
            printtype = static_cast<Printer_Normal *>(QMetaType::create(typeID));
#else
            printtype = static_cast<Printer_Normal *>(QMetaType::construct(typeID));
#endif
            if(printtype && printtype->name() == fname){
                return_typename = typeName;
                break;
            }
            //
            if(printtype) {delete printtype; printtype = 0;}
        }
    }

    return printtype;
}


//Printer_Normal::Normal_INS_TYPE
void Printer_Normal::resolve(int print_instype, QString print_class_name, int paperwidth, int xinye_beep_count, blockIns &ins)
{
    //1
    Printer_Normal *insprinter = 0;
    switch(print_instype){
    case Enhance://增强型ESC/POS打印机
        insprinter = new Printer_Normal_80;
        break;
    case Standard://标准型ESC/POS打印机
        insprinter = new Printer_Normal_80_old;
        break;
    default://增强型ESC/POS打印机
        insprinter = new Printer_Normal_80;
        break;
    }
    //
    Printer_Normal_auto_destroy d(insprinter);
    //

    //2
    int  typeID = QMetaType::type(print_class_name.toLocal8Bit());
    if(typeID != 0) {
        Printer_Normal *printtype;
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
        printtype = static_cast<Printer_Normal *>(QMetaType::create(typeID));
#else
        printtype = static_cast<Printer_Normal *>(QMetaType::construct(typeID));
#endif
        //
        Printer_Normal_auto_destroy d2(printtype);
        //
        ins = printtype->tran(ins, xinye_beep_count);
    }
    //2
    QByteArray line;
    if(paperwidth == 0) paperwidth = 32;
    line = QByteArray(paperwidth, '-');

    //3 QPair("s00"," 1.000") ,  QPair("s00"," 18.00")->QPair("s00"," 1.000") ,  QPair(""," 18.00")
    for(int k = 0; k < ins.count(); k++){
        QList<QByteArray> plist = ins[k].first.split(',');
        foreach(const QByteArray &p, plist) {
            //filter
            if(p.startsWith("a")
                    ||p.startsWith("s")
                    ||p.startsWith("b") ) {

            } else {
                continue;
            }
            for(int k2 = k + 1; k2 < ins.count(); k2++) {
                //filter
                if(ins[k2].first.startsWith("a")
                        ||ins[k2].first.startsWith("s")
                        ||ins[k2].first.startsWith("b") ) {

                } else {
                    continue;
                }

                if(ins[k2].first.contains("i")) {//初始化
                    break;
                }
                if(ins[k2].first.isEmpty()){//空
                    continue;
                }
                if(false == ins[k2].first.contains(p)
                        &&ins[k2].first.contains(p.mid(0, 1))) {//检索s00时,没有预到s00,反而遇到s01
                    break;
                }
                ins[k2].first.replace(p, "");
                ins[k2].first.replace(" ", "");//避免 "   ", ""的情况
                while(ins[k2].first.startsWith(",")){//避免开头,,,的情况
                    ins[k2].first.remove(0, 1);
                }
                while(ins[k2].first.count(",,") > 0){//避免,,,的情况
                    ins[k2].first.replace(",,", ",");
                }
                continue;
            }
        }
    }
    //
    for(int index = 0, count = ins.count(); index < count; index++){
        QList<QByteArray> plist = ins[index].first.split(',');
        ins[index].first.clear();
        foreach(QByteArray p, plist){
            if(p.isEmpty()) continue;
            //31
            QByteArray pins = p.mid(1);
            char c = 0x00;
            if(pins == "00"){
                c = 0x00;
            } else if(pins == "01"){
                c = 0x01;
            } else if(pins == "10"){
                c = 0x10;
            } else if(pins == "11"){
                c = 0x11;
            }
            //32
            if(p.startsWith("i")){//初始化
                ins[index].first += insprinter->init();
            } else if(p.startsWith("a")){//对齐方式
                ins[index].first += insprinter->align(c);
            } else if(p.startsWith("s")){//倍高倍宽
                ins[index].first += insprinter->scale(c);
            } else if(p.startsWith("-")){//分隔线
                ins[index].first.clear();
                ins[index].second = line+"\n";
            } else if(p.startsWith("b")){//分隔线
                ins[index].first +=insprinter->bold(c);
            } else {
                ins[index].first += p;//默认
            }
        }
    }

    //4
    for(int index = 0, count = ins.count(); index < count; index++){
        // a, ""
        //xx, "dd" == axx, "dd"
        if(ins[index].second.isEmpty()) {
            if( (index+1) < count) {
                ins[index + 1].first = ins[index].first + ins[index + 1].first;
                ins[index].first = "";
            }
        }
    }
}

const QMap<QString, QVariant> &Printer_Normal::get_normal_print_map_ref()
{
    static QMap<QString, QVariant> map_normal_pint;
    if(map_normal_pint.isEmpty()) {
        for(int row = 0, rowcount = Printer_Normal::class_name_model.mCreator()->rowCount();
            row < rowcount ;row++){
            QByteArray typeName = Printer_Normal::class_name_model.mCreator()->index(row, 0).data().toByteArray();
            int typeId = QMetaType::type(typeName);
            if (typeId != 0) {
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
                Printer_Normal *p =  static_cast<Printer_Normal *>(QMetaType::create(typeId));
#else
                Printer_Normal *p =  static_cast<Printer_Normal *>(QMetaType::construct(typeId));
#endif
                if(p) {
                    map_normal_pint.insert(typeName, p->name());
                    delete p;
                }
            }
        }
    }
    return map_normal_pint;
}

const QMap<int, QString> &Printer_Normal::get_normal_ins_map_ref()
{
    static QMap<int, QString> map_normal_ins;
    if(map_normal_ins.isEmpty()) {
        map_normal_ins.insert(Enhance, QObject::tr("增强型ESC/POS打印机"));
        map_normal_ins.insert(Standard, QObject::tr("标准型ESC/POS打印机"));
    }
    return map_normal_ins;
}

bool Printer_Normal::toprint(int print_instype, QString print_class_name, int paperwidth, int xinye_beep_count, const QString &path, blockIns ins, int times, QString *errstring)
{
    resolve(print_instype, print_class_name, paperwidth, xinye_beep_count, ins);
    return toprint(path, ins, times, errstring);
}

bool Printer_Normal::toprint(const QString &path, blockIns ins, int times, QString *errstring)
{
    //
    Printer_POS *printer = Printer_POS::get_pos_printer_by_path(path);
    Printer_POS_auto_destroy d(printer);
    if(!printer) {
        if(errstring) *errstring = QObject::tr("打印机不存在")+":" +path;
        return false;
    }
    //
    if(!d.printer->tryOpen(path)) {
        if(errstring) *errstring = d.printer->lastError();
        return false;
    }
    //
    toprint(d.printer, ins, times);
    printer->close();
    return true;
}

bool Printer_Normal::toprint(Printer_POS *printer, const blockIns &ins, int times)
{
    while(times >= 1 && times--) {
        foreach(const pieceIns &pins,ins){
            printer->writeHex(pins.first);
            printer->write(pins.second);
        }
    }
    return true;
}
