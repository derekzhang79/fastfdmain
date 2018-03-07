#include "tableview_item.h"
#include <QFileInfoList>
#include <QDir>
#include <QTimer>
#include <QHeaderView>
#include <QPainter>
#include "tableview_item.h"
#include <QHideEvent>
#include <QtDebug>
#include <QApplication>
#include <qdesktopwidget.h>
#include "lds.h"
#include "doublewidget.h"

static QString pix_path;
static QList<tableview_item::BG_RUN_SET>
run_list;
static int time_step = 0;
static QStringList image_filepath_list;
static int image_step = 0;
static bool time_running = false;
static QPixmap *pix = 0;

tableview_item::tableview_item(QWidget *parent)
    : QTableView(parent)
{
    model = new tableview_item_model(parent);
    setModel(model);
    lds::setHeaderResizeMode(horizontalHeader(), QHeaderView::Stretch);
    lds::setHeaderResizeMode(verticalHeader(), QHeaderView::Stretch);
    horizontalHeader()->hide();
    verticalHeader()->hide();
    setItemDelegate(new tableview_item_delegate(this, this));
    setShowGrid(false);
    setFocusPolicy(Qt::NoFocus);
    setFrameShape(QFrame::NoFrame);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    text_rect = 0;
    //
    int rowColumnCount = model->rowCount() * model->columnCount();
    int rowcount = model->rowCount();
    int columncount = model->columnCount();
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

    //
    timer_text.setInterval(200);
    timer.setInterval(100);
    connect(&timer, SIGNAL(timeout()),this,SLOT(togo()));
    QTimer::singleShot(1000, this, SLOT(text_rect_rasie()));
    //
}

tableview_item::~tableview_item()
{
    if(pix) delete pix; pix = 0;
    if(text_rect) delete text_rect;
    if(pix) delete pix;
}

void tableview_item::initScene(const QString &imagepath, const tableview_item::textData &textd)
{
    ///
    QFileInfoList fileinfos=QDir(imagepath).entryInfoList();
    foreach(const QFileInfo &info, fileinfos){
        if(info.fileName() == "."
                || info.fileName() == ".."
                || info.fileName().endsWith(".ini"))
            continue;
        image_filepath_list.append(info.filePath());
    }

    //
    if(!textd.text.trimmed().isEmpty()) {
        //        text_rect = new tableview_item_text(0, textd);
        //        text_rect->hide();
        //        connect(&timer_text, SIGNAL(timeout()),text_rect,SLOT(togo()));
    }
}

void tableview_item::showPixmap(const QString &mpath)
{
    showPixmap(QPixmap(mpath));
}

void tableview_item::showPixmap(const QPixmap &m)
{
    if(pix) delete pix;
    pix  = new QPixmap(m);

    stop();
    for(int row = 0; row < model->rowCount(); row ++) {
        for(int column = 0; column < model->columnCount(); column ++) {
            this->update(model->index(row, column));
        }
    }
}

void tableview_item::start(int msec)
{
    //
    if(text_rect){
        timer_text.start(msec);
        text_rect->show();
    }
    timer.start(msec);
}

void tableview_item::start()
{
    //
    if(text_rect){
        timer_text.start();
        text_rect->show();
    }
    timer.start();
}

void tableview_item::stop()
{
    //
    if(text_rect){
        timer_text.stop();
        text_rect->hide();
    }
    timer.stop();
}

void tableview_item::text_rect_rasie()
{
    if(text_rect){
        text_rect->raise();
        text_rect->setGeometry(QRect(this->mapToGlobal(QPoint(0, 0)), QSize(this->width(), text_rect->getTexth() )));
    }
}

void tableview_item::closeEvent(QCloseEvent *e)
{
    QTableView::closeEvent(e);
    if(text_rect) {
        text_rect->close();
    }
}

void tableview_item::showEvent(QShowEvent *e)
{
    QTableView::showEvent(e);
}

void tableview_item::hideEvent(QHideEvent *e)
{
    QTableView::hideEvent(e);
    if(text_rect) {
        text_rect->hide();
    }
}

void tableview_item::togo()
{
#ifdef Q_OS_UNIX
    if(false == QDir("/proc/" +doublewidget::parentPID).exists()) {
        qApp->quit();
        return;
    }
#endif
    ///
    static int run_going = 0;
    static int time_pause_count = 4;
    static int time_pause_k = 0;
    //
    if(false == time_running)
        time_running = true;

    //
    if(timer.interval() == 500) {
        //动画停止倒计时
        if(time_pause_k < time_pause_count) {
            time_pause_k ++;
            return;
        }
        //动画启动
        time_pause_k = 0;
    }

    BG_RUN_SET *run0 = &run_list[run_going];
    int loop = time_step / run0->count();//0 1
    if(loop == 1) {
        run_going = rand()%run_list.count();
        run0 = &run_list[run_going];
        time_step = 0;
        image_step++;
        if(image_filepath_list.count() > 0)
            image_step = image_step % image_filepath_list.count();
        else
            image_step = 0;


        //        default
        timer.setInterval(160);
        //左上-》右上  左下-》右下
        if(0 == run_going) timer.setInterval(160);
        //上-》下
        if(1 == run_going) timer.setInterval(300);
        //左-》右
        if(2 == run_going) timer.setInterval(300);
        //左上-》右下
        if(3 == run_going) timer.setInterval(200);
    }
    ///
    const QBitArray &bit = run0->at(time_step);

    if(pix_path != image_filepath_list.value(image_step)){
        pix_path = image_filepath_list.value(image_step);
        if(pix) delete pix; pix = 0;
        pix = new QPixmap(pix_path);
    }
    //
    for(int k = 0; k < bit.count(); k++) {
        if(bit[k] == true) {
            update(model->index(k/model->columnCount(),k%model->columnCount()));
        }
    }
    //
    //绘制最后一张图
    if(run0->count() == time_step + 1) {
        timer.setInterval(500);
    }

    time_step ++;
}


tableview_item_text::tableview_item_text(QWidget *parent, const tableview_item::textData &textd)
    : QWidget(parent)
{
    setAttribute(Qt::WA_TranslucentBackground, true);
    this->setWindowFlags(Qt::FramelessWindowHint);

    index = 0;
    this->textd = textd;
    QFont font = this->font();
    font.setPointSize(textd.textpt);
    QFontMetrics fm(font);
    textlen = fm.width(textd.text);
    texth = fm.height();
    setFixedHeight(texth);
}

tableview_item_text::~tableview_item_text()
{
}

int tableview_item_text::getTexth()
{
    return texth;
}

void tableview_item_text::paintEvent(QPaintEvent *e)
{
    QPainter p(this);
    QFont font = p.font();
    font.setPointSize(textd.textpt);
    p.setFont(font);

    p.setPen(QColor(textd.textcolor));


    p.setPen(Qt::NoPen);
    p.fillRect(this->rect(), QColor(255, 0, 0, 100));

    p.setPen(QColor(textd.textcolor));
    p.drawText(QRect(800 + textlen - index, 0, 800, texth), textd.text, Qt::AlignVCenter | Qt::AlignLeft);

}

void tableview_item_text::togo()
{
    index += 50;
    if(index > 800 + textlen + textlen) {
        index = 0;
    }
    update();
}

tableview_item_delegate::tableview_item_delegate(QTableView *tableview, QObject *parent)
    : QItemDelegate(parent)
{
    this->tableview = tableview;
}

void tableview_item_delegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(time_running == false) return;

    if(pix && !pix->isNull()) {
        //
        qreal tableview_radio = tableview->viewport()->width() * 1.0 / tableview->viewport()->height();
        qreal p_radio = pix->width() * 1.0 / pix->height();

        //
        QRect prect;
        if(tableview_radio > p_radio) {
            prect = QRect(0, 0, tableview->viewport()->height() * p_radio, tableview->viewport()->height());
        } else {
            prect = QRect(0, 0, tableview->viewport()->width(), tableview->viewport()->width()/p_radio);
        }
        qreal radio = pix->width()*1.0 / prect.width();
        //
        QPoint pos_origin = QPoint((tableview->viewport()->width() - prect.width()) / 2, (tableview->viewport()->height() - prect.height()) / 2);
        prect.translate(pos_origin);
        prect =  prect.intersected(option.rect);
        if(!prect.isEmpty()) {
            painter->drawPixmap(prect, pix->copy(QRect((prect.topLeft() - pos_origin) * radio, prect.size() * radio)));
        }
    }
}

int tableview_item_model::rowCount(const QModelIndex &parent) const
{
    return 6;
}

int tableview_item_model::columnCount(const QModelIndex &parent) const
{
    return 8;
}

QVariant tableview_item_model::data(const QModelIndex &index, int role) const
{
    return QVariant();
}

QVariant tableview_item_model::headerData(int, Qt::Orientation, int role) const
{
    return QVariant();
}
