#include "lds_tableview.h"
#include <QItemSelectionModel>
#include <QDebug>
#include <QHeaderView>
#include <QScrollBar>
#include <QMouseEvent>
#include <QPainter>
#include "lds_tableview_delegate_virtual.h"
#include "lds_tableview_header.h"
#include "lds.h"
#include <QTimer>

lds_tableView::lds_tableView(QWidget *parent) :
    lds_tableview_virtual(parent)
{
    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->setSelectionMode(QAbstractItemView::SingleSelection);
    currow=-1;

    stack_row.push(-1);
    stack_row.push(-1);

    this->setAlternatingRowColors(true);

    QFont f = horizontalHeader()->font();
    f.setPointSize(12);
    horizontalHeader()->setFont(f);

    this->setItemDelegate(new lds_tableview_delegate_virtual(this));
}

lds_tableView::~lds_tableView()
{
    qDebug() << __FUNCTION__ ;
}

//!可拖动---------------end
void lds_tableView::restore(int defrow)
{
    qDebug() << "currow:" << currow;
    this->selectRow(currow);
    if(this->currentIndex().row() < 0 && defrow >= 0) {
        this->selectRow(defrow);
    }
}

void lds_tableView::restore_rows()
{

}

int lds_tableView::getcurrow()
{

}

QSet<int> lds_tableView::getcurrows()
{

}

void lds_tableView::select_beforerow()
{
    this->selectRow(stack_row.last());
}

int lds_tableView::get_beforerow()
{
    return stack_row.last();
}

void lds_tableView::saverow(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(deselected)
    //有选中
    foreach(const QModelIndex &index, selected.indexes()){
        currows.insert(index.row());
    }
    foreach(const QModelIndex &index, deselected.indexes()){
        currows.remove(index.row());
    }
    if(selected.indexes().count() > 0) {
        currow=selected.indexes().last().row();
        stack_row.push_front(currow);
        stack_row.pop_back();
    }
}

void lds_tableView::toselectrow(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous)
    emit selectChanged(current.row());
    emit selectChanged(current);
}

void lds_tableView::setModel(QAbstractItemModel *model)
{
    QItemSelectionModel *selectmodel = this->selectionModel();
    QTableView::setModel(model);
    //    if(selectmodel)
    //        delete selectmodel;

    selectmodel=new QItemSelectionModel(model);
    this->setSelectionModel(selectmodel);
    connect(this->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(toselectrow(QModelIndex, QModelIndex)));
}

void lds_tableView::setSelectionModel(QItemSelectionModel *selectionModel)
{
    //1若是有则会disconnet and delete
    if(this->selectionModel()){
        disconnect(this->selectionModel(),SIGNAL(selectionChanged(QItemSelection,QItemSelection)),this,SLOT(saverow(QItemSelection,QItemSelection)));
        delete this->selectionModel();
    }
    //2
    QTableView::setSelectionModel(selectionModel);
    connect(selectionModel,SIGNAL(selectionChanged(QItemSelection,QItemSelection)),this,SLOT(saverow(QItemSelection,QItemSelection)));
}

QString lds_tableView::dg_paint_transfer(const QVariant &dd, const QString &column)
{
    int c = column.toInt();
    lds_tableview_delegate_virtual *d = qobject_cast<lds_tableview_delegate_virtual *>(itemDelegateForColumn(c));
    if(d == 0) {
        d = qobject_cast<lds_tableview_delegate_virtual *>(itemDelegate());
    }

    return d->paint_transfer(dd, this->model()->headerData(c, Qt::Horizontal).toString());
}

QString lds_tableView::dg_paint_transfer(int row, int column)
{
    return dg_paint_transfer(this->model()->data(this->model()->index(row, column)), QString::number(column));
}

QString lds_tableView::hh_paint_transfer(int logicalIndex)
{
    lds_tableview_header *h = qobject_cast<lds_tableview_header *>(horizontalHeader());
    QString d;
    if(h == 0) {
        d = this->model()->headerData(logicalIndex, Qt::Horizontal).toString();
    } else {
        d = h->paint_transfer(logicalIndex);
    }
    return d;
}

lds_tableview_delegate_virtual *lds_tableView::item_delegate_virtual()
{
    return qobject_cast<lds_tableview_delegate_virtual *>(itemDelegate());
}

void lds_tableView::setTransferHheader()
{
    QList<int> hide_column_list;
    bool hhide = false;
    for(int k = 0; k < this->horizontalHeader()->count(); k ++) {
        if(this->horizontalHeader()->isSectionHidden(k)) {
            hide_column_list << k;
        }
    }
    hhide = this->horizontalHeader()->isHidden();
    //
    lds_tableview_header *h = new lds_tableview_header(Qt::Horizontal, this);
    this->setHorizontalHeader(h);
    this->horizontalHeader()->setHidden(hhide);
    QFont f = h->font();
    f.setPointSize(12);
    h->setFont(f);
    //
    for(int k = hide_column_list.count() - 1; k >= 0; k --) {
        h->hideSection(hide_column_list[k]);
    }
}


void lds_tableView::resizeColumnsToContents(bool scan_all_hhead)
{
    resizeColumnsToContents2(scan_all_hhead);
}

void lds_tableView::resizeColumnsToContents2(bool scan_all_hhead)
{
    lds_tableview_delegate_virtual::delay_tableview_resize_column_to_content(this, rePecifyHeaderList, scan_all_hhead);
}

void lds_tableView::mousePressEvent(QMouseEvent *event)
{
    QTableView::mousePressEvent(event);
    QModelIndex index = indexAt(event->pos());
    if(do_double_clicked.press_event_do_double_clicked(index)) {
        emit double_clicked(index);
        return;
    }
}

void lds_tableView::mouseDoubleClickEvent(QMouseEvent *event)
{
    emit double_clicked(indexAt(event->pos()));
    QTableView::mouseDoubleClickEvent(event);
}

void lds_tableView::resizeColumnsToContentsDelay(int mesc)
{
    QTimer::singleShot(mesc, this, SLOT(resizeColumnsToContents2()));
}

void lds_tableView::lds_tableview_delegate_virtual_enableFieldName2k0v1()
{
    lds_tableview_delegate_virtual *d = qobject_cast<lds_tableview_delegate_virtual *>(this->itemDelegate());
    d->enableFieldName2k0v1();
}

void lds_tableView::clearSelection()
{
    currows.clear();
    QTableView::clearSelection();
    this->setCurrentIndex(QModelIndex());
}

int lds_tableView::gonext()
{
    this->selectRow(currow+1);
    return this->currentIndex().row();
}

int lds_tableView::goprevious()
{
    this->selectRow(currow-1);
    return this->currentIndex().row();
}
