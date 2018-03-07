#include "lds_picshow_item.h"
#include <QParallelAnimationGroup>
#include <QVariant>
#include <QPainter>
#include "math.h"
#include <QFile>
#include <QtDebug>
#include <QStateMachine>
#include <QSignalTransition>
#include <QPropertyAnimation>
#include <QTimer>
#include <QFileInfoList>
#include <QPaintEvent>
#include <QDir>
#include <QApplication>
#include <QDesktopWidget>
#include <QStyle>
#include <QLabel>
#include "lds.h"

static int paint_count = 0;
lds_picshow_item::lds_picshow_item(const QString &imagepath, QGraphicsItem *parent)
    : QGraphicsObject(parent)
{
    setCacheMode(ItemCoordinateCache);

    rotate0 = 90.0;
    valuestep = 0;
    imagestep = 0;
    imagestepbefore = 0;
    pathstep = 0;
    _isstop = false;

    run_widget = 0;
#ifdef QT_DEBUG
    savefilepath = "dscreen.txt";
#else
    savefilepath = "userdata/settings/dscreen.txt";
#endif

    QFileInfoList fileinfos;
    parentrect.addRect(rect());

    //1
#ifdef QT_NO_DEBUG
    if(!QFile((savefilepath)).exists())
#endif
    {
        regenrate_polygon_void();
        //        regenrate_polygon_vect(45.0, 10, 10);
        regenrate_polygon_vect(45.0, 1, 10);
        regenrate_round(10, 10, 10);
        //        regenrate_bar(45.0, 10, 25);
        //        regenrate_bar(90.0, 8, 20);
        regenrate_randbox(12,12);
    }
    resolve_polygon_vect();
    //1.2
    fileinfos=QDir(imagepath).entryInfoList();
    foreach(QFileInfo info, fileinfos){
        if(info.fileName() == "."
                || info.fileName() == "..")
            continue;
        imagefilepathlist.append(info.filePath());
    }
    //2
    valuestep_max = 10;
    machine = new QStateMachine(this);
    QState *s0 = new QState(machine);
    QState *s1 = new QState(machine);
    QState *s2 = new QState(machine);
    machine->setInitialState(s0);
    s0->assignProperty(this, "step", 0);
    s1->assignProperty(this, "step", valuestep_max);
    s2->assignProperty(this, "step", valuestep_max);

    QPropertyAnimation *anm = new QPropertyAnimation(this, "step");
    anm->setDuration(3*1000);
    anm->setEasingCurve(QEasingCurve::Linear);

    QSignalTransition *trans;
    trans = s0->addTransition(s0,SIGNAL(propertiesAssigned()),s1);
    trans->addAnimation(anm);
    trans = s1->addTransition(s1,SIGNAL(propertiesAssigned()),s2);
    trans->addAnimation(anm);
    trans = s2->addTransition(s2,SIGNAL(propertiesAssigned()),s0);

    connect(s2,SIGNAL(propertiesAssigned()),this,SLOT(nextpage()));
}

QRectF lds_picshow_item::boundingRect() const
{
    return rect();
}

void lds_picshow_item::paint(QPainter *painter, const QStyleOptionGraphicsItem */*option*/, QWidget */*widget*/)
{
    static int s = 0;
    if(_isstop){
        //底部图片
        if(!imagepre.isNull()){

            QPixmap p(rect().size());
            QPainter pp(&p);
            p.fill(QColor("white"));
            QSize size = imagepre.size();
            size.scale(this->size(), Qt::KeepAspectRatio);
            pp.drawPixmap(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size, rect()), imagepre);

            painter->drawPixmap(rect(), p);
        }
    } else {
        //底部图片
        if(!imagepre.isNull()){
            painter->fillPath(parentrect.subtracted(pathlistlist.value(pathstep).value(painterstep)),
                              imagepre);
        }
        //滚动图片
        if(!imagecur.isNull()){
            painter->fillPath(parentrect.intersected(pathlistlist.value(pathstep).value(painterstep)),
                              imagecur);
        }
    }
}

void lds_picshow_item::start()
{
    if(!machine->isRunning()){
        nextpage();
        machine->start();
        _isstop = false;
    }
    if(run_widget) {
        run_widget->show();
        run_widget->start();
    }
}

void lds_picshow_item::stop()
{
    if(machine->isRunning()){
        machine->stop();
        _isstop = true;
    }

    if(run_widget) {
        run_widget->hide();
        run_widget->stop();
    }
}

bool lds_picshow_item::isstop()
{
    return _isstop;
}

void lds_picshow_item::setrunningtext(const QString &text, const QString &textpt, const QString &textcolor, QFont f)
{
    if(!text.trimmed().isEmpty())
        run_widget = new runningtext_widget_item(text, textpt, textcolor, f, this);
}

void lds_picshow_item::show_imagepre_pause()
{
    if(!imagepre.isNull()){
        stop();
        update();
    }
}

qreal lds_picshow_item::width() const
{
    return 800;
}

qreal lds_picshow_item::height() const
{
    return 600;
}

QRect lds_picshow_item::rect() const
{
    return QRect(0, 0, width(), height());
}

QSize lds_picshow_item::size() const
{
    return QSize(width(), height());
}

int lds_picshow_item::step() const
{
    return valuestep;

}

void lds_picshow_item::setstep(int value)
{
    static int beforeindex;
    valuestep = value;
    painterstep = valuestep*1.0/valuestep_max*(pathlistlist.value(pathstep).count()-1);
    if(beforeindex != painterstep){
        beforeindex = painterstep;
        this->update();
    }
}

void lds_picshow_item::nextpage()
{
    if(imagefilepathlist.count() > 0){
        imagepre = imagecur;
        imagestep = (++imagestep)%imagefilepathlist.count();
        imagecur = QPixmap(imagefilepathlist.value(imagestep));
    }

    if(pathlistlist.count() > 0)
        pathstep = (++pathstep)%pathlistlist.count();
}

void lds_picshow_item::regenrate_polygon_void()
{
    QFile file(savefilepath);
    if(!file.open(QFile::WriteOnly)){
        qDebug() << file.errorString();
        return;
    }
    file.write("");
    file.close();
}

QRect lds_picshow_item::centerByPixmap(const QRect &r, const QPixmap &p)
{
    return QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, p.size(), r);

}

void lds_picshow_item::regenrate_polygon_vect(qreal rotate, int count, int progresscount)
{
    //1
    QVector<QPolygonF> polygon_vect;
    rotate0=M_PI*rotate/180.0;
    qreal actual_length=width()*cos(rotate0)+height()*sin(rotate0);
    qreal actual_meta_step= fabs(actual_length/count);
    if(rotate == 0.0){//垂直
        for(int s=0;s<count;s++){
            polygon_vect.append(QPolygonF(
                                    QVector<QPointF>()
                                    << QPointF(actual_meta_step*s, 0)
                                    << QPointF(actual_meta_step*(s), height())
                                    << QPointF(actual_meta_step*(s+1), height())
                                    << QPointF(actual_meta_step*(s+1), 0)
                                    ));

        }
    } else if(rotate == 90.0){//水平
        for(int s=0;s<count;s++){
            polygon_vect.append(QPolygonF(
                                    QVector<QPointF>()
                                    << QPointF(0, actual_meta_step*s)
                                    << QPointF(width(), actual_meta_step*s)
                                    << QPointF(width(), actual_meta_step*(s+1))
                                    << QPointF(0, actual_meta_step*(s+1))
                                    ));

        }
    } else {//0-90;;''''''''''''''''''''''''''''''''''''''''''''''''''''''''[
        for(int s=0;s<count;s++){
            polygon_vect.append(QPolygonF(
                                    QVector<QPointF>()
                                    << QPointF(0, actual_meta_step*(s)/sin(rotate0))
                                    << QPointF(actual_meta_step*s/cos(rotate0), 0)
                                    << QPointF(actual_meta_step*(s+1)/cos(rotate0), 0)
                                    << QPointF(0, actual_meta_step*(s+1)/sin(rotate0))
                                    ));
        }
    }

    //2

    QFile file(savefilepath);
    if(!file.open(QFile::WriteOnly|QFile::Append)) return;
    float percent;
    QList<QPainterPath> pathlist;
    QPainterPath pathrect;
    pathrect.addRect(QRect(0,0,width(),height()));
    for(int index = 0;index <= progresscount;index++){
        percent = index/(progresscount*1.0);
        QPainterPath p0;
        foreach(QPolygonF polygon, polygon_vect){
            if(rotate==0.0){//垂直
                polygon[3].rx()=polygon[2].rx()=QString::number(polygon[1].x()+(polygon[2].x()-polygon[1].x())*percent, 'f', 1).toFloat();

            } else if(rotate==90.0){//水平
                polygon[3].ry()=polygon[2].ry()=QString::number(polygon[1].y()+(polygon[2].y()-polygon[1].y())*percent, 'f', 1).toFloat();

            } else {
                polygon[2].rx()=QString::number(polygon[1].x()+(polygon[2].x()-polygon[1].x())*percent, 'f', 1).toFloat();
                polygon[3].ry()=QString::number(polygon[0].y()+(polygon[3].y()-polygon[0].y())*percent, 'f', 1).toFloat();
            }
            p0.addPolygon(polygon);
        }
        pathlist.append(pathrect.intersected(p0));
    }

    QByteArray data;
    QDataStream in(&data, QIODevice::WriteOnly);
    in << pathlist;
    file.write(data);
    file.close();
}

void lds_picshow_item::regenrate_round(int ccount, int rcount, int progresscount)
{
    QRect rect = QRect(0,0,width(),height());
    int w = rect.width();
    int h = rect.height();
    int x = rect.x();
    int y = rect.y();
    int c = ccount;
    int r = rcount;
    //2
    QFile file(savefilepath);
    if(!file.open(QFile::WriteOnly|QFile::Append))return;

    //3
    float percent;
    QList<QPainterPath> pathlist;
    QPainterPath pathrect;
    QRect rect0;
    pathrect.addRect(QRect(0,0,width(),height()));

    for(int index = 0;index <= progresscount;index++){
        percent = index/(progresscount*1.0);
        QPainterPath p0;
        for(int rr = 0;rr < r;rr++){
            for(int cc = 0;cc < c;cc++){
                rect0 = QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, QSize(w/c,h/r)*percent, QRect(x+w/c*cc,y+h/r*rr,w/c,h/r));
                int halfminwh = qMin(rect0.width(), rect0.height())/2.0*(1-percent);
                int arcvalue = 2*halfminwh;
                p0.addRoundRect(rect0, arcvalue);
            }
        }
        pathlist.append(pathrect.intersected(p0));
    }

    //4
    QByteArray data;
    QDataStream in(&data, QIODevice::WriteOnly);
    in << pathlist;
    file.write(data);
    file.close();
}

void lds_picshow_item::regenrate_bar(qreal rotate, int count, int progresscount)
{
    QRect rect = QRect(0,0,width(),height());
    int w = rect.width();
    int h = rect.height();
    //1
    QVector<QPolygonF> polygon_vect;
    rotate0=M_PI*rotate/180.0;
    qreal actual_length=width()*cos(rotate0)+height()*sin(rotate0);
    int onevalue = 20;
    int oneadd = onevalue/count;
    for(int hasadd = onevalue, step = 0;hasadd >= 0;hasadd -=oneadd,step++){
        if(rotate == 0.0){//垂直
            polygon_vect.append(QPolygonF(
                                    QVector<QPointF>()
                                    << QPointF(step*onevalue,   0)
                                    << QPointF(step*onevalue+hasadd, 0)
                                    << QPointF(step*onevalue+hasadd, h)
                                    << QPointF(step*onevalue,   h)
                                    ));
        } else if(rotate == 90.0){
            polygon_vect.append(QPolygonF(
                                    QVector<QPointF>()
                                    << QPointF(0,   step*onevalue)
                                    << QPointF(w,   step*onevalue)
                                    << QPointF(w,   step*onevalue+hasadd)
                                    << QPointF(0,   step*onevalue+hasadd)
                                    ));
        } else {
            polygon_vect.append(QPolygonF(
                                    QVector<QPointF>()
                                    << QPointF((step*onevalue)/cos(rotate0), 0)
                                    << QPointF((step*onevalue+hasadd)/cos(rotate0), 0)
                                    << QPointF(0, (step*onevalue+hasadd)/sin(rotate0))
                                    << QPointF(0, (step*onevalue)/sin(rotate0))
                                    ));
        }
    }
    //2
    QFile file(savefilepath);
    if(!file.open(QFile::WriteOnly|QFile::Append)) return;
    float percent;
    QList<QPainterPath> pathlist;

    QPainterPath pathrect;
    pathrect.addRect(QRect(0,0,width(),height()));
    for(int index = -1*onevalue*count/(progresscount*1.0);
        index <= progresscount;index++){
        percent =index/(progresscount*1.0);
        QPainterPath p0;
        if(rotate == 0.0){//垂直
            foreach(QPolygonF polygon, polygon_vect){
                polygon[0].rx()+=percent*actual_length;
                polygon[1].rx()+=percent*actual_length;
                polygon[2].rx()+=percent*actual_length;
                polygon[3].rx()+=percent*actual_length;
                p0.addPolygon(polygon);
            }
            p0.addPolygon(QVector<QPointF>()
                          << QPointF(0,   0)
                          << QPointF(percent*w, 0)
                          << QPointF(percent*w, h)
                          << QPointF(0,   h)
                          );
        } else if(rotate == 90.0){//水平
            foreach(QPolygonF polygon, polygon_vect){
                polygon[0].ry()+=percent*actual_length;
                polygon[1].ry()+=percent*actual_length;
                polygon[2].ry()+=percent*actual_length;
                polygon[3].ry()+=percent*actual_length;
                p0.addPolygon(polygon);
            }
            p0.addPolygon(QVector<QPointF>()
                          << QPointF(0,   0)
                          << QPointF(w,   0)
                          << QPointF(w,   percent*h)
                          << QPointF(0,   percent*h)
                          );
        } else {
            foreach(QPolygonF polygon, polygon_vect){
                polygon[0].rx()+=percent*actual_length/cos(rotate0);
                polygon[1].rx()+=percent*actual_length/cos(rotate0);
                polygon[2].ry()+=percent*actual_length/sin(rotate0);
                polygon[3].ry()+=percent*actual_length/sin(rotate0);
                p0.addPolygon(polygon);
            }
            p0.addPolygon(QVector<QPointF>()
                          << QPointF(0,   0)
                          << QPointF(percent*actual_length/cos(rotate0), 0)
                          << QPointF(0, percent*actual_length/sin(rotate0))
                          << QPointF(0,   0)
                          );
        }
        pathlist.append(pathrect.intersected(p0));
    }

    //3
    QByteArray data;
    QDataStream in(&data, QIODevice::WriteOnly);
    in << pathlist;
    file.write(data);
    file.close();
}

void lds_picshow_item::regenrate_randbox(int ccount, int rcount)
{
    /*00 01 02
      10 11 12
      20 21 22*/
    //1
    QRect rect = QRect(0,0,width(),height());
    int w = rect.width();
    int h = rect.height();
    int x = rect.x();
    int y = rect.y();

    int rate = 10;
    int c0 = ccount;
    while((c0=c0/10) != 0) {rate *= 10;}

    QList<int> content;
    for(int r = 0; r < rcount; r++){
        for(int c = 0; c < ccount; c++){
            content.append(r*rate+c);
        }
    }
    //2
    QFile file(savefilepath);
    if(!file.open(QFile::WriteOnly|QFile::Append))return;

    //3
    QList<QPainterPath> pathlist;
    QPainterPath pathrect;
    pathrect.addRect(QRect(0,0,width(),height()));
    int boxw = w/ccount;
    int boxh = h/rcount;

    QPainterPath p0;
    while(!content.isEmpty()){
        int total0 = rcount;
        while(total0--){
            int one = content.takeAt(rand()%content.count());
            p0.addRect(QRect(x+(one%rate)*boxw, y+(one/rate)*boxh, boxw, boxh));
        }
        pathlist.append(pathrect.intersected(p0));
    }

    //4
    QByteArray data;
    QDataStream in(&data, QIODevice::WriteOnly);
    in << pathlist;
    file.write(data);
    file.close();
}

void lds_picshow_item::resolve_polygon_vect()
{
    QFile file(savefilepath);
    if(!file.open(QFile::ReadOnly))return;
    QByteArray byte =  file.readAll();
    QDataStream in(&byte, QIODevice::ReadOnly);
    while(!in.atEnd()){
        QList<QPainterPath> pathlist;
        in >> pathlist;
        pathlistlist.append(pathlist);
    }
}


///////////

runningtext_widget_item::runningtext_widget_item(const QString &text, const QString &textpt, const QString &textcolor, QFont f, QGraphicsItem *parent)
    : QGraphicsObject(parent)
{
    setCacheMode(ItemCoordinateCache);

    f.setPointSize(textpt.toInt());
    runningf = f;
    QFontMetrics fontm(runningf);
    runningtextheight = fontm.height();
    runningtext = text;
    runningwidth = fontm.width(runningtext);
    runningstep = this->boundingRect().width();
    runningtextcolor = QColor(textcolor);

    //2
    connect(&runningtimer,SIGNAL(timeout()),this,SLOT(torunningupdate()));
    runningtimer.setInterval(boundingRect().width());
}

QRectF runningtext_widget_item::boundingRect() const
{
    return QRectF(0,0, 800, runningtextheight);
}

void runningtext_widget_item::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->save();

    painter->setPen(runningtextcolor);
    painter->setOpacity(0.5);
    painter->setFont(runningf);

    painter->fillRect(boundingRect(), QColor("rgba(112, 128, 144)"));
    painter->setOpacity(1.0);
    painter->drawText(QRect(runningstep, 0, runningwidth, runningtextheight), int(Qt::AlignVCenter|Qt::AlignRight), runningtext);

    painter->restore();

}

void runningtext_widget_item::start()
{
    runningtimer.start();

}

void runningtext_widget_item::stop()
{
    runningtimer.stop();

}

void runningtext_widget_item::torunningupdate()
{
    runningstep -= 100;
    if(runningstep <= -runningwidth){
        runningstep = this->boundingRect().width();
    }
    this->update();
}
