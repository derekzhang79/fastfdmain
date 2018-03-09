#include "lds.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QLocale>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QStyle>
#include <QSettings>
#include <QFontDatabase>
#include <QDateTime>
#include <QPainter>
#include <QTimer>
#include <QHostInfo>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <QApplication>
#ifdef Q_OS_UNIX
#include <sys/ioctl.h>
#endif

#ifdef QT_NEW_SYSTEM
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusReply>
#endif
#include <QFileInfo>
#include <QUdpSocket>
#include "math.h"

const QString lds::dogID;

Qt::LayoutDirection lds::SYS_LAYOUT_DIRECTION = Qt::LeftToRight;
int lds::QT_FIXED_HEIGHT_BOTTOM = 45;
int lds::QT_FIXED_BETWEEN = 120;
int lds::BACK_LOGO_HEIGHT = 30;
int lds::BORDER_DEFAULT_HEIGHT = 2;
int lds::PAY_LEFT_SPACE = 2;
int lds::SYS_DOUBLE_SCREEN_PORT = 45454;
const QString lds::udp_insert_request = "insert";
const QString lds::udp_update_request = "update";
const QString lds::udp_delete_request = "delete";
const QString lds::udp_delete_all_request = "delete_all";

const QString lds::udp_title_request = "title";
const QString lds::udp_pay_request = "pay";
const QString lds::udp_rb_code_request_begin = "rb_code_begin";
const QString lds::udp_rb_code_request_end = "rb_code_end";
const QString lds::udp_close_request = "close";
const QString lds::udp_m_hlabels_request = "m_hlabels";

QSize lds::MAIN_WINDOW_SIZE;//主窗口的大小，底部高40px
QSize lds::FULL_WINDOW_SIZE;//前台,后台,中餐点菜,中餐前台
QString lds::FONT_NUM_Family;
QString lds::FONT_DEFAULT_Family;
QString lds::SYS_LANGUAGE;
QSettings * lds::sysconf = 0;
QNetworkAccessManager * lds::sysmanager = 0;

const QString lds::LOCAL_TERMINAL_INDEX_FF;
const QString lds::localdataPath =
        #if defined(Q_OS_WIN) || defined(QT_DEBUG)
        "fastfd_localdata";
#else
        "userdata/fastfd_localdata";
#endif
const QString lds::terminalCode;
QElapsedTimer lds::etimer;


lds::lds(QObject *parent):QObject(parent)
{

}

bool lds::LDS_INIT()
{
    QString screen_primary_size = lds::sysconf->value("system_setting/screen_primary_size", tr("自动获取")).toString();
    if(screen_primary_size.contains("x")) {
        FULL_WINDOW_SIZE = QSize(screen_primary_size.split("x").value(0).toInt(), screen_primary_size.split("x").value(1).toInt());
    } else {
        FULL_WINDOW_SIZE = qApp->desktop()->availableGeometry().size();
#ifdef Q_OS_WIN
        FULL_WINDOW_SIZE = qApp->desktop()->size();
#endif
    }

//    int fuping_mode = lds::sysconf->value("system_setting/fuping_mode", -1).toInt();
//    if(fuping_mode == 2) {//主副屏同屏
//        FULL_WINDOW_SIZE = QSize(
//                    qMax(800, FULL_WINDOW_SIZE.width() - lds::dsize().width()),
//                    FULL_WINDOW_SIZE.height());
//    }
    QSettings desktop_setting("/usr/desk/desktop.ini", QSettings::IniFormat);
    QString tran=desktop_setting.value("desktop/language", "1").toString();
    lds::SYS_LANGUAGE="CN";
    if(tran == "0")lds::SYS_LANGUAGE="EN";

    int sys_desk_bottom_height = 40;
#ifdef Q_OS_WIN
    sys_desk_bottom_height = qApp->desktop()->size().height() - qApp->desktop()->availableGeometry().height();
#endif
    QT_FIXED_HEIGHT_BOTTOM = 45;
    if(FULL_WINDOW_SIZE.height() == 1080) {
        QT_FIXED_HEIGHT_BOTTOM = 60;//3的倍数
    }

    BACK_LOGO_HEIGHT = 30;
    if(FULL_WINDOW_SIZE.width() == 800) {
        BACK_LOGO_HEIGHT = 25;
    }

    //如果修改本处，可能导致软件闪退，
    MAIN_WINDOW_SIZE = QSize(FULL_WINDOW_SIZE.width(), FULL_WINDOW_SIZE.height()-sys_desk_bottom_height);

    //
    PAY_LEFT_SPACE = (qMin(1024, lds::MAIN_WINDOW_SIZE.width()) / 3) + 0;
    if(lds::FULL_WINDOW_SIZE.width() == 1920) {
        PAY_LEFT_SPACE = lds::MAIN_WINDOW_SIZE.width()*1/4;
    }
    //~
    //
    int FONT_NUM_ID = QFontDatabase::addApplicationFont(":image/HYQINGTING-55J.TTF");
    if(-1 != FONT_NUM_ID){
        FONT_NUM_Family = QFontDatabase::applicationFontFamilies(FONT_NUM_ID).first();
    }
    lds::FONT_DEFAULT_Family = qApp->font().family();
    lds::SYS_LAYOUT_DIRECTION = Qt::LeftToRight;      //控件的布局方向

    return true;
}

/**
本来方式试用与widget的flag是frameless的窗口
*/
QPoint lds::getPopPoint(const QPoint &gpos,const QPoint &gpos2, const QSize &popsize, Qt::LayoutDirection dir)
{
    QPoint pos = gpos;
    QPoint pos2 =  gpos2;
    QRect screen = QRect(QApplication::desktop()->availableGeometry().topLeft(), lds::MAIN_WINDOW_SIZE);// QApplication::desktop()->availableGeometry(pos);
    if (dir == Qt::RightToLeft) {
        pos.setX(pos.x()-popsize.width());
        pos2.setX(pos2.x()-popsize.width());
        if (pos.x() < screen.left())
            pos.setX(qMax(pos.x(), screen.left()));
        else if (pos.x()+popsize.width() > screen.right())
            pos.setX(qMax(pos.x()-popsize.width(), screen.right()-popsize.width()));
    } else {
        if (pos.x()+popsize.width() > screen.right())
            pos.setX(screen.right()-popsize.width());
        pos.setX(qMax(pos.x(), screen.left()));
    }
    if (pos.y() + popsize.height() > screen.bottom())
        pos.setY(pos2.y() - popsize.height());
    else if (pos.y() < screen.top())
        pos.setY(screen.top());
    if (pos.y() < screen.top())
        pos.setY(screen.top());
    if (pos.y()+popsize.height() > screen.bottom())
        pos.setY(screen.bottom()-popsize.height());
    return pos;
}

QPoint lds::getPopPoint(QWidget *parent, QWidget *popwidget)
{
    return getPopPoint(parent, parent->rect(), popwidget);
}

QPoint lds::getPopPoint(QWidget *parent, const QRect &section, QWidget *popwidget)
{
    QPoint bottomLeft = section.bottomLeft();
    QPoint topLeft = section.topLeft();

    bottomLeft = parent->mapToGlobal(bottomLeft);
    topLeft = parent->mapToGlobal(topLeft);

    return lds::getPopPoint(bottomLeft, topLeft, popwidget->size(), Qt::LeftToRight);
}

QPoint lds::getPopPoint_size(QWidget *parent, QWidget *popwidget)
{
    return lds::getPopPoint0(parent, popwidget, SIZE);
}

QPoint lds::getPopPoint0(QWidget *parent, QWidget *popwidget, SIZETYPE type)
{
    QPoint pos = (parent->layoutDirection() == Qt::RightToLeft) ? parent->rect().bottomRight() : parent->rect().bottomLeft();
    QPoint pos2 = (parent->layoutDirection() == Qt::RightToLeft) ? parent->rect().topRight() : parent->rect().topLeft();

    pos = parent->mapToGlobal(pos);
    pos2 = parent->mapToGlobal(pos2);

    QSize size;
    switch(type){
    case SIZEHNIT:
        size = popwidget->sizeHint();
        break;
    case SIZE:
        size = popwidget->size();
        break;
    }

    return getPopPoint(pos, pos2, size, parent->layoutDirection());
}

/**
pop居中与父类的的左上角的坐标*/
QPoint lds::getGlobalCenterPopup(QWidget *parent, QWidget *pop){
    QPoint p = parent->mapToGlobal(QPoint(0,0));
    return QStyle::alignedRect(parent->layoutDirection(), Qt::AlignCenter, pop->size(), QRect(p, QSize(parent->size()))).topLeft();
}

QPoint lds::getParentCenterPopup(QWidget *parent, QWidget *pop)
{
    QPoint p =QPoint(0,0);
    return QStyle::alignedRect(parent->layoutDirection(), Qt::AlignCenter, pop->size(), QRect(p, QSize(parent->size()))).topLeft();

}

QString lds::get_sys_language()
{
    return lds::SYS_LANGUAGE;
}

QString lds::get_soft_curent_language_first()
{
    QString language = lds::sysconf->value("system_setting/SYS_LANGUAGE").toString();
    if(language.isEmpty()) {//随系统
        language = lds::SYS_LANGUAGE;
    }
    return language;
}

void lds::set_soft_curent_language(const QString &key)
{
    lds::sysconf->setValue("system_setting/SYS_LANGUAGE", key);
}

QString lds::get_soft_curent_language_second()
{
    QString language = lds::get_soft_curent_language_first();
    QString language2 = lds::sysconf->value("system_setting/SYS_LANGUAGE_SECOND").toString();
    if(language2.isEmpty()) {//随系统
        if(language == "CN") return "EN";
        if(language == "EN") return "CN";
        return "EN";
    }
    return language2;
}

void lds::set_soft_curent_language_second(const QString &key)
{
    lds::sysconf->setValue("system_setting/SYS_LANGUAGE_SECOND", key);
}

void lds::setTterminalCode(const QString &tcode)
{
    QString c;
    if(tcode.startsWith("FA")) {
        c = tcode;
    } else if(tcode.startsWith("AA")) {
        c= tcode;
    } else {
        c =  "AA"   +tcode;
    }
    *(QString *)&terminalCode = c;
    qDebug() << "terminalCode:" << terminalCode;
}

//!约定的主线程的mysql数据库别名为：空
//!约定的子线程的mysql数据库别名为：线程号
//!约定的主线程的其他数据库别名为：非空、非defconnectionname
//!约定的子线程的其他数据库别名为：线程号+"-"+connectionname
//! 本函数值获取mysql数据库的别名

void lds::DOUBLE_DATA_input(QString &d, const lds::DOUBLE_DATA &data)
{
    d = "";
    d += data.udpKey+"\t";

    d += QString::number(data.row)+"\t";
    d += QString::number(data.count)+"\t";

    d += data.items.vch_dishname+"\t";
    d += data.items.ch_dishno+"\t";
    d += data.items.dish_price+"\t";
    d += data.items.dish_num+"\t";
    d += data.items.dish_total+"\t";
    d += data.items.vch_other+"\t";
    d += data.items.ch_suitflag+"\t";

    d += data.sum_num_desc+"\t";
    d += data.sum_total_desc+"\t";

    d += data.p_desc+"\t";
    d += data.p_receive+"\t";
    d += data.p_pay+"\t";
    d += data.p_change+"\t";

    d += data.rb_title+"\t";
    d += data.rb_title_wechat+"\t";
    d += data.rb_title_alipay+"\t";
    d += data.rb_wechat+"\t";
    d += data.rb_alipay+"\t";
    d += data.rb_ch_paymodeno+"\t";

    d += data.m_hlabels+"\t";
    d += QString::number(data.parentPID)+"\t";
}

void lds::DOUBLE_DATA_output(const QString &d, lds::DOUBLE_DATA &data)
{
    int index = 0;
    QStringList list = d.split("\t");
    data.udpKey = list[index++];

    data.row = list[index++].toInt();
    data.count = list[index++].toInt();

    data.items.vch_dishname = list[index++];
    data.items.ch_dishno = list[index++];
    data.items.dish_price = list[index++];
    data.items.dish_num = list[index++];
    data.items.dish_total = list[index++];
    data.items.vch_other = list[index++];
    data.items.ch_suitflag = list[index++];

    data.sum_num_desc = list[index++];
    data.sum_total_desc = list[index++];

    data.p_desc = list[index++];
    data.p_receive = list[index++];
    data.p_pay = list[index++];
    data.p_change = list[index++];

    data.rb_title = list[index++];
    data.rb_title_wechat = list[index++];
    data.rb_title_alipay = list[index++];
    data.rb_wechat = list[index++];
    data.rb_alipay = list[index++];
    data.rb_ch_paymodeno = list[index++];

    data.m_hlabels = list[index++];
    data.parentPID = list[index++].toInt();
}

void lds::doublescreensenddata(const lds::DOUBLE_DATA &ddata)//往服务端发送信息
{
    if(ddata.udpKey.isEmpty()) return;

    QUdpSocket udpSocket;
    QString datagram;
    lds::DOUBLE_DATA_input(datagram, ddata);
    QByteArray datagram_byte = datagram.toLocal8Bit();

    udpSocket.writeDatagram(datagram_byte.data(),
                            datagram_byte.size(),
                            QHostAddress::LocalHost,
                            SYS_DOUBLE_SCREEN_PORT);
}

QVariant lds::dtifnull(const QString &dtstring)
{
    if(dtstring.isEmpty()) return QVariant(QVariant::DateTime);
    return dtstring;
}

void lds::setHeaderResizeMode(QHeaderView *h, QHeaderView::ResizeMode mode)
{
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    h->setSectionResizeMode(mode);
#else
    h->setResizeMode(mode);
#endif
}

void lds::setwflagFrameless(QWidget *widget)
{
    widget->setWindowFlags(Qt::FramelessWindowHint|widget->windowFlags());
}

QColor lds::static_color_light(const QString &c, double light_0_3, int a)
{
    QColor color(c);
    int h = color.hslHue();
    int s = color.hslSaturation();
    int l = color.lightness();
    return QColor::fromHsl(h, s, l + light_0_3 * 22, a);
}

QString lds::appName()
{
    return QFileInfo(qApp->applicationFilePath()).fileName();
}
void lds::showWidget(QWidget *w) {
    if(w->isHidden())
        w->show();
}

void lds::hideWidget(QWidget *w){
    //又是widget->isVisiabled() 会判断无效
    if(!w->isHidden())
        w->hide();
}

void lds::hideWidget(QWidget *w, bool hide)
{
    if(hide) {
        hideWidget(w);
    } else {
        showWidget(w);
    }
}

bool lds::widgetIsShow(QWidget *w)
{
    return !w->isHidden();
}

bool lds::widgetIsHide(QWidget *w)
{
    return w->isHidden();
}

QSize lds::dsize()
{
    QStringList bofangg_wh = lds::sysconf->value("system_setting/bofangg_rate").toString().split("x");
    QSize dsize = FULL_WINDOW_SIZE;
    if(bofangg_wh.count() == 2) {
        dsize = QSize(bofangg_wh[0].toInt(), bofangg_wh[1].toInt());
    }
    return dsize;
}

QString lds::groupSeparatorV2str(double value1, int decimal)
{
    //#ifdef QT_NO_DEBUG
    //    if(lds::get_soft_curent_language_first() == "CN") {
    //        return QString::number(value1, 'f', decimal);;
    //    }
    //#endif
    //

    // 由于QString::number(-0.0003, 'f', 2) = "-0.00";的情况 所以这边
    value1 = QString::number(value1, 'f',decimal).toDouble();
    //
    QString f = value1 < 0 ? "-" : "";
    QString d = QString::number(qAbs(value1), 'f', decimal);
    QString p2 = d.split(".").value(1);
    QString p1 = d.split(".").value(0);
    for(int k = p1.count() - 3; k > 0; k = k - 3) {
        p1.insert(k, ",");
    }
    if(p2.length() > 0)
        return f + p1 + "." +p2;
    else
        return f + p1;
}

double lds::groupSeparatorStr2v(const QString &text)
{
    //#ifdef QT_NO_DEBUG
    //    if(lds::get_soft_curent_language_first() == "CN") {
    //        return text.toDouble();
    //    }
    //#endif
    //
    QString r = text;
    return r.replace(",", "").toDouble();
}

QString lds::septNumber(int value, int decimal)
{
    return lds::groupSeparatorV2str(value * 1.0, decimal);
}

QString lds::septNumber(double value, int decimal)
{
    return lds::groupSeparatorV2str(value, decimal);
}

bool lds::isSeptNumber(const QString &value)
{
    QString v1 = value;
    bool ok = false;
    v1.replace(",", "").toDouble(&ok);
    return ok;
}

QString lds::getHardWareID_unique()
{

    //    QFile file("machineID.txt");
    //    if(!file.exists()) {
    //        file.open(QFile::WriteOnly);
    //        file.write("FA1101");
    //        file.close();
    //    }

    //    file.open(QFile::ReadOnly);
    //    QString l = file.readAll().trimmed();
    //    return l;

    //#ifdef QT_DEBUG
    //    //    return "AA1102";
    //    return "FA22001";
    //#endif
    ////////////
#ifdef QT_DEBUG
    return "FA1101";
    //    return "";
    //    return "81020232410800C";
    return "AA80310092516736C";
#endif
    //unix 和 新系統 和 发布版
#if defined(Q_OS_UNIX) && defined(QT_NEW_SYSTEM) && defined(QT_NO_DEBUG)
    QString hardwareID;
    QDBusInterface dbusinterface("com.scpos.system", "/system/desktop", "com.scpos.desktop", QDBusConnection::systemBus(), 0);
    if(dbusinterface.isValid()) {
        QDBusReply<QString> reply = dbusinterface.call("getsysid");
        if(reply.isValid()) {
            hardwareID = reply.value();
        }
    }
    return hardwareID;
#endif

    //unix  和 旧系统 和 发布版
#if defined(Q_OS_UNIX) && !defined(QT_NEW_SYSTEM) && defined(QT_NO_DEBUG)
    QString hardwareID;
    static const char *device = "/dev/gpioctl";
    char bcdData[17]={0};
    ::system("busybox mknod -m 666 /dev/gpioctl c 232 0");
    int fd = ::open(device, O_RDWR);
    ioctl(fd, 0x82, &bcdData);
    hardwareID = QString(QLatin1String(bcdData));
    return hardwareID;
#endif

    //window
#if defined(Q_OS_WIN)
    return lds::dogID        ;//狗号
#endif

    //
    return "";
}

void lds::showWidget(QWidget *w, bool show)
{
    if(show) {
        showWidget(w);
    } else {
        hideWidget(w);
    }
}


int lds::size_g_of_u(const QString &in)
{
    int sum = 0;
    foreach(QChar c,in){
        if(QByteArray().append(c).size() == 1)     sum +=1;
        else    sum += 2;
    }
    return sum;
}

QString lds::size_g_of_u_left_justified(const QString &in, int width, QChar fill, bool trunc)
{
    int w = size_g_of_u(in);
    QString trunc_str;
    int gbk_len;
    //fill
    if(width >= w) {
        qDebug() << __FUNCTION__ << "fill";
        return in + QString(width - w, fill);
    }
    //~trunc
    if(!trunc) {
        qDebug() << __FUNCTION__ << "~trunc";
        return in;
    }

    //trunc
    qDebug() << __FUNCTION__ << "trunc";
    for(int k = 0; k < in.count() && (gbk_len = size_g_of_u(trunc_str)) < width; k++) {
        trunc_str += in.at(k);
    }
    if(gbk_len > width) { //a你 ->a
        trunc_str.chop(1);
    }
    return trunc_str;
}

QString lds::_fromlast_cutbetween(const QString &content, const QString &cmpvalue){
    //s=1000100001001abc10000
    //_fromlast_cutbetween(s,"1")  //return abc
    //_fromlast_cutbetween(s,"c")  //return 1000100001001ab
    int endindex= content.lastIndexOf(cmpvalue);
    int beginindex= content.lastIndexOf(cmpvalue, endindex-1);

    return content.mid(beginindex+cmpvalue.count(), endindex-beginindex-cmpvalue.count());
}

QString lds::strcutBygbnum(const QString &data, int width, bool isfill, char fill, Qt::AlignmentFlag f)
{
    // 你a好 = 5 width = 3 result:你a
    // 你a好 = 5 width = 4 result:你a
    // 你a好 = 5 width = 5 result:你a好
    //1获取字段信息
    QList<int> unitsizelist;
    foreach(const QChar &c,data){
        if(QByteArray().append(c).size() == 1)    unitsizelist.append(1);
        else   unitsizelist.append(2);
    }

    //开始拼接
    QString ret;
    int retsize = 0;
    for(int index = 0, curstep = 0, indexcount  = unitsizelist.count(); index < indexcount; index++){
        curstep += unitsizelist[index];
        if(curstep <= width){
            ret += data[index];
            retsize = curstep;
            continue;
        }
        break;
    }
    //填充判断空格
    if(isfill == true){
        if(f == Qt::AlignLeft){
            ret += QString(qMax(0, width-retsize), fill);
        }
        if(f == Qt::AlignRight){
            ret =  QString(qMax(0, width-retsize), fill) + ret;
        }
    }

    return ret;
}

int lds::min_max_value_add1_loop(int min, int max, int value)
{
    //这里value是每次会随数据递增的
    int ret;

    int div = max-min+1;
    if(div  >  0){
        ret = value%div+min;
    } else {
        ret = (value);
    }
    //v%(e-b+1)+b
    // b2 e5 v=0 r = 2
    // b2 e5 v=1 r = 3
    // b2 e5 v=2 r = 4
    // b2 e5 v=3 r = 5
    // b2 e5 v=4 r = 2
    return ret;
}

QStringList lds::getUsbPathList()
{
    //获取usb路径list unix 和 新系統 和 发布版
#if defined(Q_OS_UNIX) && defined(QT_NEW_SYSTEM) && defined(QT_NO_DEBUG)
    QStringList ret_list;
    QDBusInterface dbusinterface("com.scpos.system", "/system/desktop", "com.scpos.desktop", QDBusConnection::systemBus(), 0);
    if(dbusinterface.isValid()) {
        QDBusReply<QStringList> reply = dbusinterface.call("getusbmounts");
        if(reply.isValid()) {
            ret_list = reply.value();
        }
    }
    return ret_list;
#endif

    //unix  和 旧系统 和 发布版
#if defined(Q_OS_UNIX) && !defined(QT_NEW_SYSTEM) && defined(QT_NO_DEBUG)
    return QStringList() << "/mnt/usb";
#endif

    //window
#if defined(Q_OS_WIN)
    return QStringList();
#endif

    //debug
    return QStringList()
            << "/home/cz/视频/calendar" << "/home/cz/视频/chip" << "/home/cz/视频/dcbapp"
            << "/home/cz/视频/dcbapp1" << "/home/cz/视频/fastfdmain0507" << "/home/cz/视频/fastfdmain2017-0816";

}

//解密
QString lds::of_str_decrypt(QString as_source)
{
    //Public function of_str_decrypt (string as_source) returns string
    //string as_source
    //    long i;
    long ll_len;
    QString ls_text;
    QString ls_result;
    QString ls_return;

    //Has been Shielded.
    ls_text = as_source.trimmed();
    ll_len = ls_text.length();
    ls_result = "";
    for(int i = 0; i < ll_len; i++) { // 123  -> 321
        //前插入
        ls_result.push_front(QChar((char)(ls_text.at(i).unicode() -  (2 * (ll_len - i - 3)))));//char(asc(mid(ls_text,i,1)) - (2 * (ll_len - i - 2)));
    }
    return ls_result;
}

//加密
QString lds::of_str_encrypt(QString as_source)//空格会被替换吊
{
    //Public function of_str_encrypt (string as_source) returns string
    //string as_source
    //    long i;
    long ll_len;
    QString ls_text;
    QString ls_result;
    //Has been Shielded.
    ls_text = as_source.replace(" ", "").trimmed();
    ll_len = ls_text.length();
    ls_result = "";
    for(int i = 0; i < ll_len ; i++){//321  -> 123
        ls_result.push_front(QChar((char)(ls_text.at(i).unicode() +  (2 * (i - 2)))));
    }
    return ls_result;

}

/*
1232.56 1 1232.6
1232.52 1 1232.5
1232.569 2 1232.57
1232.561 2 1232.56
1232.561 1 1232.6
1232.561 0 1233
1232.561 -1 1230
1232.561 -2 1200
*/
double lds::f_get_decimal(double d, int prec)
{
    double p = pow(10, prec);
    double r = round(d * p) / p;
    return r;
}

QString lds::cur_machine_dt_str(const QString &displayFormat)
{
    return dt_str(QDateTime::currentDateTime(), displayFormat);
}

QString lds::dt_str(const QDateTime &dt, const QString &displayFormat)
{
    return dt.toString(displayFormat);
}

lds_mutex_auto_unlock::lds_mutex_auto_unlock(QMutex *m)
    : _m(m)
{
    cur_locked = false;
}

lds_mutex_auto_unlock::~lds_mutex_auto_unlock()
{
    if(cur_locked) {
        _m->unlock();
        qDebug() << "unlock";
    }
}

bool lds_mutex_auto_unlock::tryLock(int timeout)
{
    cur_locked = _m->tryLock(timeout);
    return cur_locked;
}

void lds_mutex_auto_unlock::lock()
{
    _m->lock();
    cur_locked = true;
}


bool lds_record_do_double_clicked::press_event_do_double_clicked(const QString &index)
{
    int mescOff = QTime::currentTime().msecsTo(QTime(0, 0, 0));
    bool is_double_clicked = false;

    if(press_event_do_press_index == index) {
        if(press_event_do_press_mesc_off - mescOff  < 200) {
            is_double_clicked = true;
        }
    }
    press_event_do_press_index = index;
    press_event_do_press_mesc_off = mescOff;
    return is_double_clicked;
}

bool lds_record_do_double_clicked::press_event_do_double_clicked(const QModelIndex &index)
{
    QString pos;
    if(index.isValid()) {
        pos  = QString("%1,%2").arg(index.row()).arg(index.column());
    }

    return press_event_do_double_clicked(pos);
}
