#ifndef TABLEVIEW_ITEM_H
#define TABLEVIEW_ITEM_H

#include <QString>
#include <QImage>
#include <QBitArray>
#include <QTableView>
#include <QItemDelegate>
#include <QTimer>

class tableview_item_text;

class tableview_item : public QTableView
{
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
    tableview_item(QWidget *parent = 0);
    ~tableview_item();

    void initScene(const QString & imagepath, const textData &textd);
    void showPixmap(const QString &mpath);//之前要手动停止timer
    void showPixmap(const QPixmap &m);//之前要手动停止timer
    void start(int msec);
    void start();
    void stop();

signals:
    void signal_pixmap_go();
public slots:
    void text_rect_rasie();

protected:
    virtual void closeEvent(QCloseEvent *e);
    virtual void showEvent(QShowEvent *e);
    virtual void hideEvent(QHideEvent *e);
private slots:
    void togo();
private:
    QAbstractTableModel *model;
    tableview_item_text *text_rect;
    QTimer timer;
    QTimer timer_text;
};

class tableview_item_text : public QWidget
{
    Q_OBJECT
public:
    tableview_item_text(QWidget *parent,
                        const tableview_item::textData &textd);

    ~tableview_item_text();

    int getTexth();
protected:
    void paintEvent(QPaintEvent *e);

public slots:
    void togo();//
private:
    int index;//
    int textlen;
    int texth;
    tableview_item::textData textd;
};

class tableview_item_delegate : public QItemDelegate
{
public:
    tableview_item_delegate(QTableView *tableview, QObject *parent);
    // painting
    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const ;
    QTableView *tableview;

};

class tableview_item_model : public QAbstractTableModel{
    Q_OBJECT
public:
    tableview_item_model(QObject *parent = 0):QAbstractTableModel(parent){}

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const ;
    QVariant headerData(int /*section*/, Qt::Orientation /*orientation*/, int role = Qt::DisplayRole) const ;

};
#endif // TABLEVIEW_ITEM_H
