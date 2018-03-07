#ifndef LDS_PICSHOW_ITEM_H
#define LDS_PICSHOW_ITEM_H


#include <QGraphicsObject>
#include <QLinearGradient>
#include <QTimer>
#include <QStateMachine>
#include <QFont>

class runningtext_widget_item;

class lds_picshow_item : public QGraphicsObject
{
    Q_OBJECT
    Q_PROPERTY(qreal step READ step WRITE setstep USER true)

public:
    lds_picshow_item(const QString &imagepath, QGraphicsItem *parent = 0);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem */*option*/, QWidget */*widget*/);


    void start();
    void stop();
    bool isstop();
    void setrunningtext(const QString &text, const QString &textpt, const QString &textcolor, QFont f);

    void show_imagepre_pause();

    qreal width() const;
    qreal height() const;
    QRect rect() const;
    QSize size() const;
public:
    QPixmap imagepre;
private:
    int step() const;
    void setstep(int value);

    QPixmap imagecur;
    int imagestep;                  //图片播放的step
    int imagestepbefore;            //记录上一次图片的step
private slots:
    void nextpage();
private:
    void regenrate_polygon_void();
    QRect centerByPixmap(const QRect &r, const QPixmap &p);
    ///
    /// \brief regenrate_polygon_vect
    /// \param rotate
    /// \param count
    /// \param progresscount
    ///
    void regenrate_polygon_vect(qreal rotate, int count/*分割数*/, int progresscount/*进度数*/);//百叶窗
    ///
    /// \brief regenrate_round
    /// \param ccount
    /// \param rcount
    /// \param progresscount
    ///
    void regenrate_round(int ccount, int rcount, int progresscount/*进度数*/);
    ///
    /// \brief regenrate_bar
    /// \param rotate
    /// \param count
    /// \param progresscount
    ///
    void regenrate_bar(qreal rotate, int count/*分割数*/, int progresscount/*进度数*/);//百叶窗
    ///
    /// \brief regenrate_randbox
    /// \param ccount
    /// \param rcount
    /// \ccount > 10
    void regenrate_randbox(int ccount, int rcount);//随机盒子

    void resolve_polygon_vect();
    qreal rotate0;
private:
    int valuestep;
    int valuestep_max;
    int painterstep;                //绘制的step
    QList< QList<QPainterPath> > pathlistlist;
    int pathstep;
    QStateMachine *machine;
    QStringList imagefilepathlist;
    QPainterPath parentrect;
    bool _isstop;

    QString savefilepath;
    runningtext_widget_item *run_widget;
};

class runningtext_widget_item : public QGraphicsObject{
    Q_OBJECT
public:
    runningtext_widget_item(const QString &text, const QString &textpt, const QString &textcolor, QFont f, QGraphicsItem *parent = 0);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

    void start();
    void stop();
private:
    QString runningtext;
    int runningwidth;
    int runningstep;
    QTimer runningtimer;
    QColor runningtextcolor;
    int runningtextheight;
    QFont runningf;
private slots:
    void torunningupdate();
};

#endif // LDS_PICSHOW_ITEM_H
