#include "public_ins_maker.h"
#include "printer_0.h"
#include "qextserialenumerator.h"
#include "lds.h"
#include <QApplication>
#include <QDebug>
#include "bmp_info.h"
#include "n_func.h"
#include "printer_normal.h"
#include "public_sql.h"
#include "lds.h"


public_ins_maker::public_ins_maker(QObject *parent):QObject(parent)
{
    clear();
}

void public_ins_maker::clear()
{
    is_column_print = false;
    column_count = 0;
    step = 0;
    paperwidth = 0;
    row = 0;
    print_ins.clear();
    print_path = "";
}

QByteArray public_ins_maker::textAlign(const QString &text, Qt::Alignment textflag, int textwidth, char c)
{
    //    pieceIns pret;
    QByteArray ret;
    //【0】    文本长度过长会破坏格式
    ret=text.toLocal8Bit();
    int actualtextwidth=lds::size_g_of_u(text);//actualtextwidth;
    switch(textflag){
    case Qt::AlignCenter://居中
        ret = QByteArray((textwidth-actualtextwidth)/2, c) + ret + QByteArray((textwidth-actualtextwidth)/2, c);
        break;
    case Qt::AlignRight://右对齐
        ret = QByteArray((textwidth-actualtextwidth), c) + ret;
        break;
    case Qt::AlignLeft://左对齐
        ret = ret + QByteArray((textwidth-actualtextwidth), c);
        break;
    }
    //最后统一按左对齐处理
    return ret;
}

void public_ins_maker::drawText(const QByteArray &hex, const QString &text, Qt::Alignment textflag)
{
    insertText(currentIndex(), hex, text, textflag);
}

void public_ins_maker::drawText(const QByteArray &hex, const QString &text, int textwidth, Qt::Alignment textflag)
{
    insertText(currentIndex(), hex, text, textwidth, textflag);
}

void public_ins_maker::insertText(int index, const QByteArray &hex, const QString &text, Qt::Alignment textflag)
{
    int  textwidth = -1;
    if(is_column_print){
        if(column_count == 0) {
            textwidth = paperwidth;
        } else {
            textwidth = paperwidth/column_count;
        }
    }

    int scale_h = 1;
    QList<QByteArray> hexs = hex.split(',');
    foreach(const QByteArray &h , hexs){
        if(h.startsWith('s')){
            int s0 = h.mid(1,1).toInt();
            if(s0 != 0){
                scale_h = s0 + 1;
                textwidth = textwidth / scale_h;// s00, s11, s22, s33 -> 不放大、x2、x3、x4
            }
            break;
        }
    }

    //    pieceIns pret;
    QString ret;
    //【0】    文本长度过长会破坏格式
    ret=text.toLocal8Bit();
    int actualtextwidth=lds::size_g_of_u(text);//actualtextwidth;
    char blank=' ';
    if(is_column_print && textwidth != 0){
        int columnadd = actualtextwidth / textwidth + ((actualtextwidth % textwidth) > 0 ? 1 : 0);
        column += columnadd;
        if(row > 0 && column > column_count){
            drawN();
            column = 0;
            column += columnadd;
        }
        textwidth = textwidth * (actualtextwidth / (textwidth + 1) + 1);
    }

    switch(textflag){
    case Qt::AlignCenter://居中
        ret = QByteArray((textwidth-actualtextwidth)/2, blank) + ret + QByteArray((textwidth-actualtextwidth)/2, blank);
        break;
    case Qt::AlignRight://右对齐
        ret = QByteArray((textwidth-actualtextwidth), blank) + ret;
        break;
    case Qt::AlignLeft://左对齐
        ret = ret + QByteArray((textwidth-actualtextwidth), blank);
        break;
    }
    //最后统一按左对齐处理

    print_ins.insert(index, pieceIns(hex, ret.toLocal8Bit()));

    step += lds::size_g_of_u(ret) *scale_h;

    if(is_column_print){
        if(column >= column_count){
            drawN();
        }
    }
}

void public_ins_maker::insertText(int index, const QByteArray &hex, const QString &text, int textwidth, Qt::Alignment textflag)
{
    int scale_h = 1;
    QList<QByteArray> hexs = hex.split(',');
    foreach(const QByteArray &h , hexs){
        if(h.startsWith('s')){
            int s = h.mid(1,1).toInt();
            if(s != 0){
                scale_h = s + 1;// s00, s11, s22, s33 -> 不放大、x2、x3、x4
                textwidth = textwidth / scale_h;// s00, s11, s22, s33 -> 不放大、x2、x3、x4

            }
            break;
        }
    }
    QString ret;
    //【0】    文本长度过长会破坏格式
    ret=text.toLocal8Bit();
    int actualtextwidth=lds::size_g_of_u(text);
    char blank=' ';
    if(actualtextwidth < textwidth){//文本实际宽度，小于文本预宽度
        switch(textflag){
        case Qt::AlignCenter://居中
            ret=QByteArray((textwidth-actualtextwidth)/2, blank) + ret + QByteArray((textwidth-actualtextwidth)/2, blank);
            break;
        case Qt::AlignRight://右对齐
            ret=QByteArray((textwidth-actualtextwidth), blank) + ret;
            break;
        case Qt::AlignLeft://左对齐
            ret=ret + QByteArray((textwidth-actualtextwidth), blank);
            break;
        }
        //最后统一按左对齐处理
    }

    print_ins.insert(index, pieceIns(hex, ret.toLocal8Bit()));

    step += lds::size_g_of_u(ret) *scale_h;
}

int public_ins_maker::currentIndex()
{
    return print_ins.count();
}

void public_ins_maker::drawN()
{
    if(print_ins.count() > 0) {
        if(!print_ins.last().second.endsWith("\n")
                && false == print_ins.last().first.contains("-")
                && (false == is_column_print || (true == is_column_print && step < paperwidth))){
            print_ins += pieceIns("", "\n");
        }
    }
    step = 0;
    column = 0;
    row ++;
}

void public_ins_maker::drawLogo(const QByteArray &hex, const QString &logopath)
{
    QByteArray b0;
    Bmp_info bmpmaker;
    bmpmaker.Pic2printIns(logopath);
    bmpmaker.makeIns(b0);
    print_ins += pieceIns(hex+b0, "");
}

void public_ins_maker::drawQR_Code(const QByteArray &hex, const QString &code)
{
    print_ins  += draw_qr_code(hex, code);
}

void public_ins_maker::drawoneBarcode(const QByteArray &hex, const QString &code)
{
    QByteArray b0;
    QByteArray b1=QByteArray::number(stringToHex(code).length()/2,16);
    b0.append("1D6B45");
    b0.append(b1.count()==1?"0"+b1:b1);
    b0.append(stringToHex(code));
    b0.append("0A");

    print_ins += pieceIns(hex+b0, "");
}

void public_ins_maker::drawTextEnd(const QByteArray &hex)
{
    print_ins += pieceIns(hex, "");
    if(n_func::f_get_sysparm("yun_bill_bottom_enable") == "1") {
        drawText("", n_func::f_get_sysparm("yun_bill_bottom"));
    }
}

void public_ins_maker::setPrintCount(int printcount)
{
    blockIns b = print_ins;
    printcount--;
    while(printcount >=1 && printcount--){
        print_ins +=b;
    }
}

bool public_ins_maker::synchPrint()
{
    return asynchPrint(print_path, true);
}

bool public_ins_maker::asynchPrint(const QString &path, bool isblock)
{
    lds_query query;
    resolve(path, query, print_ins);
    return asynchPrintWithoutresolve(path, isblock);
}

bool public_ins_maker::asynchPrintWithoutresolve(const QString &path, bool isblock)
{
    //path 192.168.1.21:9100
    //path /dev/ttyS0,9600,N,8,1   / port,9600,0,8,0,0,100[端口，波特率，]
    /**
    port,9600       ,0        ,8            ,0             ,0            ,100
    port,baudrate,parity,databits,stopbits ,flowtype,timeout
    port              :/dev/ttyS0
    baudrate      :50 - 115200
    parity           :PAR_NONE = 0,PAR_ODD,PAR_EVEN,PAR_SPACE
    databits       :DATA_5=5, DATA_6 ,DATA_7 ,DATA_8
    stopbits       :STOP_1=0,STOP_2
    flowtype      :FLOW_OFF=0,FLOW_HARDWARE,FLOW_XONXOFF
    timeout       :100
    */
    QString error;
    //
    //    public_ins_maker::resolve(path, query, print_ins);
    //2
    if(Printer_Normal::toprint(path, print_ins, 1, &error)) {
        return true;
    }
    if(!error.isEmpty()) sendError(path + ":" +error, tr("打印信息"));
    return false;
}

bool public_ins_maker::synchPrintWithoutresolve(const QString &path)
{
    return asynchPrintWithoutresolve(path, true);
}

void public_ins_maker::begin_column_print(int columnCount)
{
    is_column_print = true;
    column_count = columnCount;
    column = 0;
    step = 0;
}

void public_ins_maker::end_column_print()
{
    is_column_print = false;
}

int public_ins_maker::column_width()
{
    if(column_count != 0)
        return paperwidth / column_count;
    return 0;
}

int public_ins_maker::left()
{
    if(paperwidth != 0)
        return step % paperwidth;
    return step;
}

void public_ins_maker::resolve()
{
    lds_query query;
    resolve(print_path, query, print_ins);
}


void public_ins_maker::resolve(const QString &path, lds_query &query, blockIns &ins)
{
    int vch_instype = 0;
    QString vch_printtype = 0;
    int int_paperwidth = 32;
    int xinye_beep_count = 4;

    //1
    //打印机类型， 打印类型
    query.execSelect(QString("select  vch_printtype, vch_instype,int_paperwidth,int_xinye_beep_count  from cey_bt_kitchen_print where vch_printip = '%1'")
                     .arg(path));
    if(query.next()){
        //2
        vch_instype = query.recordValue("vch_instype").toInt();
        vch_printtype = query.recordValue("vch_printtype").toString();
        int_paperwidth = query.recordValue("int_paperwidth").toInt();
        xinye_beep_count = query.recordValue("int_xinye_beep_count").toInt();
    } else {
    }
    //3
    Printer_Normal::resolve(vch_instype,
                            vch_printtype,
                            int_paperwidth,
                            xinye_beep_count,
                            ins);
}

void public_ins_maker::sendError(const QString &err, const QString &title)
{
    QMetaObject::invokeMethod(public_sql::save_login_object(),
                              "showprinterr",
                              Qt::QueuedConnection,
                              Q_ARG(QString, err),
                              Q_ARG(QString, title)
                              );
}

QTranslator *public_ins_maker::getPrintTranslate(const QString &languageKey)
{
    if(this->languageKey != languageKey) {
        this->languageKey = languageKey;
        printTranslate.delPointer();
    }

    if( 0 == printTranslate.getPointer() ) {
        printTranslate.setPointer(new QTranslator);
    }

    if(printTranslate.getPointer()->isEmpty()) {
        if(!printTranslate.getPointer()->load("userdata/settings/language/"+languageKey+"_fastfd.qm")) {
            qDebug() << "languageKey:" << languageKey << "load error";
        }
    }

    return printTranslate.getPointer();
}

void public_ins_maker::setPrintTranslate(const QString &languageKey, QObject *languageObj)
{
    this->languageObj = languageObj;
    if(languageKey.isEmpty()) {
        this->languageKey = languageKey;//languageKey为空，随软件环境语言
        return ;
    }
    if(languageKey == "CN") {
        this->languageKey = languageKey;//中文
        return ;
    }

    printTranslate.delPointer();
    getPrintTranslate(languageKey);
}

QString public_ins_maker::printTr(const char *key)
{
    if(languageKey.isEmpty()) {
        return QApplication::translate(languageObj->metaObject()->className(), key);
    }
    if(languageKey == "CN") {
        return QString(key);
    }
    QString ret = getPrintTranslate(languageKey)->translate(languageObj->metaObject()->className(), key);
    return ret;
}

bool public_ins_maker::languageIsCN()
{
    if(languageKey.isEmpty()) {
        return tr("中文") == "中文";
    }
    if(languageKey == "CN") {
        return true;
    }
    return false;
}

QString public_ins_maker::stringToHex(const QString &str)
{
    return printer_0::trans_u2g(str).toHex();
}

//https://stackoverflow.com/questions/23577702/printing-qr-codes-through-an-esc-pos-thermal-printer
pieceIns public_ins_maker::draw_qr_code(const QByteArray &hex, const QString &code)
{
    //编码转成gb2312
    QByteArray content = printer_0::trans_u2g(code);
    //b
    qint16 lenght = content.length() + 3;//315030 三位
    char size0 = lenght & 0xff;//取低8位
    char size1 = lenght >> 8;//取高8位
    QByteArray pLH;
    pLH += size0;
    pLH += size1;
    //b0
    QByteArray b0;
    b0+="1B6101";
    b0+="1D286B0300314308";
    b0+="1d286b";
    b0 += pLH.toHex();
    b0 += "315030";
    b0 += content.toHex();
    b0+="1D286B0300315130";

    //ret
    return pieceIns(hex+b0, "");
}
