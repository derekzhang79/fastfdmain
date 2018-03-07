#ifndef LDS_TABLEVIEW_SUM_H
#define LDS_TABLEVIEW_SUM_H
#include <QWidget>
#include <QStandardItemModel>
#include <QItemSelection>
#include <QItemDelegate>
#include <QPainter>
#include <QtGlobal>
#include <QAbstractItemView>
#include "lds_model_sqlstandardmodel.h"
#include "lds_tableview_delegate_virtual.h"
#include "lds_tableview_header.h"

//有汇总的tableview





class QTableView;
class lds_tableView;
class QLineEdit;
class QAbstractItemModel;
class QScrollBar;
class QHeaderView;

class lds_tableview_sum : public QWidget
{
    Q_OBJECT
public:
    explicit lds_tableview_sum(QWidget *parent = 0);
    void setModel(QAbstractItemModel *model);
    lds_tableView *tableView;//tableview 本身的函数在这里操作
    lds_tableView *lineView;

    QStringList updateSumRange(int from, int to);
    QString updateSum(int column);//用的是普通model，即model和linemodel的列数一致
    QString updateSum(int column, const QVariant &value);//用的是普通model，即model和linemodel的列数一致

    void setEditTriggers(QAbstractItemView::EditTriggers triggers);

    void refreshSizeForContent();
    void setTransferHheader();

private slots:
    void update_tableview_bar(int min, int max);
    void update_lineview_header(int logicalIndex, int oldSize, int newSize);
    void update_lineview_selection(const QItemSelection &selected, const QItemSelection &deselected);
    void update_tableview_verticalheader();
    void linerefresh();
    void scrollbarinfo(int step);
    void linemodel_remove_and_filldata();
private:
    class RedModel:public QStandardItemModel{
    public:
        RedModel(QObject *parent = 0):
            QStandardItemModel(parent){}
        QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    };
    class ItemDelegate : public lds_tableview_delegate_virtual
    {
    public:
        explicit ItemDelegate(QObject *parent = 0):lds_tableview_delegate_virtual(parent){}
        virtual void paint(QPainter * painter,const QStyleOptionViewItem & option, const QModelIndex & index) const;
    };
    class lineViewHeader : public lds_tableview_header{
    public:
        lineViewHeader(Qt::Orientation orientation, QWidget * parent = 0)
            :lds_tableview_header(orientation, parent) {}

        //from lds_tableview_header
        QString paint_transfer(int logicalIndex) const ;
    };

    lds_model_sqlstandardmodel *linemodel;
    QAbstractItemModel *parentmodel;
    void updatelinemodel();
};
#endif // LDS_TABLEVIEW_SUM_H
