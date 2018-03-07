#include "rect_item.h"
#include <QtDebug>
#include <QWidget>
#include <QPainter>
#include <QTimer>
#include <QFileInfo>
#include <QDir>
#include <QGraphicsScene>
#include <QStyle>
#include <QStyleOptionGraphicsItem>
#include <QApplication>


static QList<rect_item_expand::BG_RUN_SET>
run_list;
static int         view_w = 800;
static int         view_h = 600;
static int         block_w = 100;
static int         block_h = 100;
static int         going_step = 0;
static bool        paint_enabled = false;
static int         columncount = view_w/block_w+(view_w%block_w > 0 ? 1 : 0);
static int         rowcount = view_h/block_h+(view_h%block_h > 0 ? 1 : 0);
static int         rowColumnCount = columncount*rowcount;
static QVector<QVector<rect_item*> >
itemrect;
static QStringList image_filepath_list;
static int         image_filepath_list_index = 0;
static QImage*     image = 0;//
static void        print(QBitArray b);

static void print(QBitArray b)
{
    QString s;
    for(int k = 0; k < b.count(); k++) {
        if(k % 8 == 0) s += " ";
        if(b[k]) {
            s += "1";
        } else {
            s += "0";
        }
    }
}

rect_item::rect_item(QGraphicsItem *parent, int row, int column)
    : QGraphicsObject(parent)
{
    this->row = row;
    this->column = column;
    setPos(block_w*column, block_h*row);
    setCacheMode(ItemCoordinateCache);
    ///
    if(0 == image)
        image = new QImage(image_filepath_list.value(0));
}

rect_item::~rect_item()
{
    if(image) {
        delete image;
        image = 0;
    }
}

QRectF rect_item::boundingRect() const
{
    return QRectF(0,0,block_w,block_h);
}

void rect_item::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget */*widget*/)
{
    if(!paint_enabled) return;
    //绘制的开启点，必须在image的size之内
    if(block_w*column < image->width() && block_h*row  < image->height()) {
        painter->drawImage(QPoint(0, 0),
                           image->copy(QRect(QPoint(block_w*column, block_h*row), QSize(block_w, block_h))));
    }
}

rect_text::rect_text(QGraphicsItem *parent, const rect_item_expand::textData &textd)
    : QGraphicsObject(parent)
{
    index = 0;
    textlen = 0;
    this->textd = textd;

    QFont font = qApp->font();
    font.setPointSize(textd.textpt);
    QFontMetrics fm(font);
    textlen = fm.width(textd.text);
    texth = fm.height();
}

rect_text::~rect_text()
{
}

QRectF rect_text::boundingRect() const
{
    return QRectF(0,0, view_w, view_h);
}

void rect_text::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    QFont font = painter->font();
    font.setPointSize(textd.textpt);
    painter->setFont(font);
    if(textlen == 0) {
        QFontMetrics fm(font);
        textlen = fm.width(textd.text);
        texth = fm.height();
    }
    painter->setPen(QColor(textd.textcolor));
    if(textd.text.trimmed().isEmpty()) {
        painter->fillRect(QRectF(0,0, view_w, texth), QColor("transparent"));
    } else {
        painter->fillRect(QRectF(0,0, view_w, texth), QColor(255,0,0,100));
    }

    painter->drawText(QRect(view_w + textlen - index, 0, view_w, texth), textd.text, Qt::AlignVCenter | Qt::AlignLeft);
}

void rect_text::togo()
{
    index += 50;
    if(index > view_w + textlen + textlen) {
        index = 0;
    }
    update();
}

rect_item_expand::rect_item_expand(QObject *parent)
    : QObject(parent)
{
    timer = new QTimer(this);
    timer->setInterval(100);
    connect(timer, SIGNAL(timeout()),this, SLOT(togo()));
}

void rect_item_expand::togo()
{
    ///第一次进入不会绘制
    if(false == paint_enabled) {
        paint_enabled = true;
    }
    ///
    static int run_going = 0;
    BG_RUN_SET *run0 = &run_list[run_going];
    int loop = going_step / run0->count();//0 1
    if(loop == 1) {
        run_going = rand()%run_list.count();
        run0 = &run_list[run_going];
        going_step = 0;
        image_filepath_list_index++;
        if(image_filepath_list.count() > 0)
            image_filepath_list_index = image_filepath_list_index % image_filepath_list.count();
        else
            image_filepath_list_index = 0;
        delete image;
        image = 0;
        image = new QImage(image_filepath_list.value(image_filepath_list_index));
    }
    ///
    const QBitArray &bit = run0->at(going_step);
    for(int k = 0; k < bit.count(); k++) {
        if(bit[k] == true) {
            rect_item *item = itemrect[k/columncount][k%columncount];
            item->update();
        }
    }

    going_step++;
    ///
    itemtext->togo();
}

void rect_item_expand::initScene(QGraphicsScene *scene, const QString &imagepath, const textData &textd)
{
    ///
    QFileInfoList fileinfos=QDir(imagepath).entryInfoList();
    foreach(const QFileInfo &info, fileinfos){
        if(info.fileName() == "."
                || info.fileName() == "..")
            continue;
        image_filepath_list.append(info.filePath());
    }
    ///
    for(int k = 0; k < rowcount; k++)
        itemrect.append(QVector<rect_item*>(columncount));

    ///
    for(int row = 0; row < rowcount; row++) {
        for(int column = 0; column < columncount; column++) {
            rect_item *item = new rect_item(0, row, column);
            itemrect[row][column] = item;
            scene->addItem(item);
        }
    }
    ///
    itemtext = new rect_text(0, textd);
    scene->addItem(itemtext);
    ///
    BG_RUN_SET run0;
    //左上-》右上  左下-》右下
    run0.clear();
    for(int column = 0; column < rowColumnCount; column++) {
        run0.append(QBitArray(rowColumnCount, false));
        run0[column][column] = true;
    }
    run_list.append(run0);

    //上-》下
    run0.clear();
    for(int column = 0; column < rowcount; column++) {
        run0.append(QBitArray(rowColumnCount, false));
        for(int row = 0; row < columncount; row++) {
            run0[column][column*columncount + row] = true;
        }
    }
    run_list.append(run0);
    //左-》右
    run0.clear();
    for(int column = 0; column < columncount; column++) {
        run0.append(QBitArray(rowColumnCount, false));
        for(int row = 0; row < rowcount; row++) {
            run0[column][row*columncount + column] = true;
        }
    }
    run_list.append(run0);
    //左上-》右下
    run0.clear();
    for(int column = 0; column < columncount + rowcount; column++) {
        run0.append(QBitArray(rowColumnCount, false));
        for(int row = 0; row < column + 1; row++) {
            int k = row*columncount + (column-row);
            if(k >= 0 && k < rowColumnCount){
                run0[column][k] = true;
            }
        }
    }
    run_list.append(run0);
}

void rect_item_expand::showImage(const QString &mpath)
{
    showImage(QImage(mpath));
}

void rect_item_expand::showImage(const QImage &m)
{
    if(m.isNull()) return;

    if(false == paint_enabled) {
        paint_enabled = true;
    }
    //
    timer->stop();
    itemtext->hide();//不显示滚动条
    ///
    if(image) {
        delete image;
        image = 0;
    }
    image = new QImage(view_w, view_h,m.format());
    QSize size = m.size();
    size.scale(view_w, view_h, Qt::KeepAspectRatio);

    QPainter p(image);
    p.drawImage(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size, QRect(0, 0, view_w, view_h)), m);

    ///
    for(int row = 0; row < rowcount; row++) {
        for(int column = 0; column < columncount; column++) {
            itemrect[row][column]->update();
        }
    }
}

void rect_item_expand::start(int msec)
{
    itemtext->show();//显示滚动条
    timer->start(msec);//滚动运行
}

void rect_item_expand::start()
{
    itemtext->show();//显示滚动条
    timer->start();//滚动运行
}
