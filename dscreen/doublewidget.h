#ifndef DOUBLEWIDGET_H
#define DOUBLEWIDGET_H

#include <QDialog>
#include <QTimer>
#include <QTcpSocket>
#include <QTcpServer>
#include <QStandardItemModel>
#include <QUdpSocket>
#include <QStyledItemDelegate>
#include "lds.h"
#include "rect_item.h"

//当有新的连接是，才会有心跳
class QSettings;
class QFrame;
class fexpandmain_table;
class lds_picshow_item;
class Ui_doublewidget;
class tableview_item;

class doublewidget : public QDialog
{
    Q_OBJECT

public:
    static void showPayinfo(QWidget *tableview,  QFrame *&pay, const lds::DOUBLE_DATA &d);
    static const QString parentPID;
public:
    explicit doublewidget(QSize dsize ,QWidget *parent = 0);
    ~doublewidget();
protected slots:
    virtual void doublewidget_layout() = 0;
    virtual void adc_timer_start();
    virtual void adc_timer_out();
    virtual void standmodel_setdata(int row, const QString &p0, const QString &p1, const QString &p2, const QString &p3, const QString &p4);
    virtual void standmodel_insetdata(int row, const QString &p0, const QString &p1, const QString &p2, const QString &p3, const QString &p4);
    virtual void total_settext(const QString &text);
    virtual void num_settext(const QString &text);
    virtual void adc_show();
    virtual QString class_name();
protected slots:
    void trytoclose();
    void processPendingDatagrams();
    void start();
    void toquit();
protected:
    virtual void resizeEvent(QResizeEvent *e);
    virtual void moveEvent(QMoveEvent *e);
//    tableview_item*expand;
    Ui_doublewidget *ui;
    QTimer *dishtimeouttimer;
    QTimer *trytoclosetimer;
    QString dishimagepath;
    QString heartbeatresponse;
    QUdpSocket *udpSocket;
    QFrame *paywidget;
    QStandardItemModel *standmodel;
};


class delegate : public QStyledItemDelegate{
    Q_OBJECT
public:
    delegate(QObject *parent = 0) : QStyledItemDelegate(parent) {}
    void keepConnect(fexpandmain_table *tableview);

    // painting
    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const;


    int *border;
    QColor *divColor;
    QColor *nameColor;
    QColor *nameSubColor;
    QColor *defColor;
    QColor *itemBgColor;
};

#endif // DOUBLEWIDGET_H
