#ifndef RECT_ITEM_H
#define RECT_ITEM_H

#include <QGraphicsObject>
#include <QBitArray>

/*
    QGraphicsScene *scene = new QGraphicsScene(this);
    rect_item::initScene(scene, "x");
    ui->graphicsView->setScene(scene);
    connect(&timer, SIGNAL(timeout()), this, SLOT(togo()));
    timer.setInterval(100);
    timer.start();
}

void MainWindow::togo()
{
    rect_item::togo();
}
*/
class rect_text;

class rect_item_expand : public QObject{
    Q_OBJECT
public:
    struct textData{
        inline textData(){}
        inline textData(const QString &text, int textpt, const QString &textcolor)
        {this->text = text; this->textpt = textpt; this->textcolor = textcolor;}
        QString text;
        int textpt;
        QString textcolor;
    };
    typedef  QList<QBitArray> BG_RUN_SET;
public:
    rect_item_expand(QObject *parent);

    void initScene(QGraphicsScene *scene, const QString & imagepath, const textData &textd);
    void showImage(const QString &mpath);//之前要手动停止timer
    void showImage(const QImage &m);//之前要手动停止timer
    void start(int msec);
    void start();
private:
    QTimer *timer;
    rect_text *itemtext;
public slots:
    void togo();
};

class rect_item : public QGraphicsObject{
    Q_OBJECT
public:
    rect_item(QGraphicsItem *parent, int row, int column);
    ~rect_item();
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget */*widget*/ = 0);
    int row, column;
};

class rect_text : public QGraphicsObject{
    Q_OBJECT

public:
    rect_text(QGraphicsItem *parent, const rect_item_expand::textData &textd);
    ~rect_text();
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem */*option*/, QWidget */*widget*/ = 0);
public slots:
    void togo();//
private:
    int index;//
    int textlen;
    int texth;
    rect_item_expand::textData textd;
};

#endif // RECT_ITEM_H
