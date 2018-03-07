#ifndef LDS_H
#define LDS_H

#include <QWidget>
#include <QVariant>
#include <QDesktopWidget>
#include <QStandardItemModel>
#include <QSettings>
#include <QHeaderView>
#ifdef QT_NO_DEBUG
#include <QNoDebug>
#endif
#include <QElapsedTimer>
#include <QMutex>

#define MAX_DOUBLE_VALUE  1000000000
#define MIN_DOUBLE_VALUE  -1000000000
#define MESSAGE_TITLE_VOID QString()

struct tableviewDATA{
    QString vch_dishname;
    QString ch_dishno;
    QString dish_price;
    QString dish_num;
    QString dish_total;
    QString vch_other;//做法特价等
    QString ch_suitflag;//套菜标志
};
Q_DECLARE_METATYPE(tableviewDATA)


class lds: public QObject
{
    Q_OBJECT
public:
    enum SIZETYPE{
        SIZEHNIT,
        SIZE
    };

    struct headerDATA{
        inline headerDATA(int c, float w, const QString &t)
            : column(c), width(w) ,text(t){}
        int column;
        float width;
        QString text;
    };

public:
    lds(QObject *parent = 0);

    //基础参数的设定，然而并不会对所有的参数进行赋值
    static bool LDS_INIT();

    static QPoint getPopPoint(const QPoint &gpos, const QPoint &gpos2, const QSize &popsize, Qt::LayoutDirection dir);
    static QPoint getPopPoint(QWidget *parent, QWidget *popwidget);
    static QPoint getPopPoint(QWidget *parent, const QRect &section, QWidget *popwidget);
    static QPoint getPopPoint_size(QWidget *parent, QWidget *popwidget);
    static QPoint getPopPoint0(QWidget *parent, QWidget *popwidget, SIZETYPE type);
    static QPoint getGlobalCenterPopup(QWidget *parent, QWidget *pop);
    static QPoint getParentCenterPopup(QWidget *parent, QWidget *pop);

    static Qt::LayoutDirection SYS_LAYOUT_DIRECTION;
    static QString get_sys_language();
    static QString get_soft_curent_language_first();
    static void set_soft_curent_language(const QString &key);
    static QString get_soft_curent_language_second();
    static void set_soft_curent_language_second(const QString &key);

    static const QString terminalCode;
    static void setTterminalCode(const QString &tcode);

    static QVariant dtifnull(const QString &dtstring = "");

    static void setHeaderResizeMode(QHeaderView *h, QHeaderView::ResizeMode mode);

    static const QString localdataPath;
private:
    static QString SYS_LANGUAGE;
public:
    static int QT_FIXED_HEIGHT_BOTTOM;
    static int QT_FIXED_BETWEEN;
    static int BACK_LOGO_HEIGHT;
    static int BORDER_DEFAULT_HEIGHT;
    static int PAY_LEFT_SPACE;

    static QSize MAIN_WINDOW_SIZE;//主窗口的大小，底部高40px
    static QSize FULL_WINDOW_SIZE;//前台,后台,中餐点菜,中餐前台
    static QString FONT_NUM_Family;//外部数字字体
    static QString FONT_DEFAULT_Family;

    //ip
    static const QString LOCAL_TERMINAL_INDEX_FF;

    //!双屏----------------------------------------------begin
    //tcp和server 是唯一个的
    static const QString udp_insert_request;
    static const QString udp_update_request;
    static const QString udp_delete_request ;
    static const QString udp_delete_all_request ;

    static const QString udp_title_request;
    static const QString udp_pay_request;
    static const QString udp_rb_code_request_begin;
    static const QString udp_rb_code_request_end;
    static const QString udp_close_request;
    static const QString udp_m_hlabels_request;
    struct DOUBLE_DATA{
        // insert update delete
        //title
        //pay
        //rb_code
        //close
        //
        QString udpKey;

        //insert update delete
        int row;
        int count;
        tableviewDATA items;//dishno dishname dishprice dishnum dishtotal

        //title
        QString sum_num_desc;//数量25.2
        QString sum_total_desc;//金额 100.0

        //pay
        QString p_desc;//表头
        QString p_receive;//应收
        QString p_pay;//已付
        QString p_change;//找零

        //rb_code
        QString rb_code;
        //        QString rb_title;
        QString rb_desc;

        QString rb_title;
        QString rb_title_wechat;
        QString rb_title_alipay;
        QString rb_wechat;
        QString rb_alipay;
        QString rb_ch_paymodeno;

        //m_hlabels, splic ,
        QString m_hlabels;

        //parentPID
        qint64 parentPID;
    };
    static void DOUBLE_DATA_input(QString &d, const DOUBLE_DATA &data);
    static void DOUBLE_DATA_output(const QString &d, DOUBLE_DATA &data);
    //客户端
    static void doublescreensenddata(const DOUBLE_DATA &ddata);//客户端
    //服务端
    static int SYS_DOUBLE_SCREEN_PORT;
    //!双屏----------------------------------------------end

    static void setwflagFrameless(QWidget *widget);

    static QSettings *sysconf;

    static QColor static_color_light(const QString &c, double light_0_3, int a = 255);//0 1 2 3 原 亮 很亮 最亮
    static QString appName();
    static void showWidget(QWidget *w);
    static void showWidget(QWidget *w, bool show);
    static void hideWidget(QWidget *w);
    static void hideWidget(QWidget *w, bool hide);
    static bool widgetIsShow(QWidget *w);
    static bool widgetIsHide(QWidget *w);

    static QSize dsize();

    static QString groupSeparatorV2str(double value1, int decimal);
    static double groupSeparatorStr2v(const QString & text);
    static QString septNumber(int value, int decimal = 0);
    static QString septNumber(double value, int decimal = 2);
    static bool isSeptNumber(const QString &value);

    static QString getHardWareID_unique();                                                          //获取机器号
    static int size_g_of_u(const QString &in);                                                          //获取utf8-》gbk的一个中文的大小
    static QString size_g_of_u_left_justified(const QString &in, int width, QChar fill = QLatin1Char(' '), bool trunc = false);

    static QString _fromlast_cutbetween(const QString &content, const QString &cmpvalue);
    static QString strcutBygbnum(const QString &data, int width, bool isfill = false, char fill = ' ', Qt::AlignmentFlag f = Qt::AlignLeft);
    static int min_max_value_add1_loop(int min, int max, int value);

    static QStringList getUsbPathList();

    static QString of_str_decrypt(QString as_source);
    static QString of_str_encrypt(QString as_source);
    static const QString dogID;

    static QElapsedTimer etimer;

    static double f_get_decimal(double d, int prec);

    static QString cur_machine_dt_str(const QString &displayFormat = "yyyy-MM-dd hh:mm:ss");
    static QString dt_str(const QDateTime &dt, const QString &displayFormat = "yyyy-MM-dd hh:mm:ss");

};


class lds_widget_auto_enable{
public:
    lds_widget_auto_enable(QWidget *b) : _b(b){b->setEnabled(false);b->repaint();if(b->parentWidget())b->parentWidget()->repaint();}
    ~lds_widget_auto_enable(){_b->setEnabled(true);}
private:
    QWidget *_b;
};

class lds_mutex_auto_unlock{
public:
    lds_mutex_auto_unlock(QMutex *m);
    ~lds_mutex_auto_unlock();
    bool tryLock(int timeout = 0) ;
    void lock() ;
private:
    QMutex *_m;
    bool cur_locked;
};

class lds_record_do_double_clicked{
public:
    lds_record_do_double_clicked()
        : press_event_do_press_mesc_off(0){}
    bool press_event_do_double_clicked(const QString &index);
    bool press_event_do_double_clicked(const QModelIndex &index);
private:
    QString press_event_do_press_index;
    int press_event_do_press_mesc_off;
};

typedef QPair<QString, QVariant> ldsVariantMapKeyValue;
class ldsVariantMap{
public:
    ldsVariantMap(){}
    ldsVariantMap(const QString &k1, const QVariant &v1){d.insert(k1, v1);}
    ldsVariantMap(const QString &k1, const QVariant &v1, const QString &k2, const QVariant &v2){d.insert(k1, v1);d.insert(k2, v2);}
    ldsVariantMap(const QString &k1, const QVariant &v1, const QString &k2, const QVariant &v2, const QString &k3, const QVariant &v3){d.insert(k1, v1);d.insert(k2, v2);d.insert(k3, v3);}
    inline ldsVariantMap &operator<<(const ldsVariantMapKeyValue &p) {d.insert(p.first, p.second); return *this;}
    QVariantMap d;
};

#endif // LDS_H
