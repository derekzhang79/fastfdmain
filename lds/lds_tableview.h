#ifndef LDS_TABLEVIEW_H
#define LDS_TABLEVIEW_H

#include "lds_tableview_virtual.h"
#include <QStack>

class lds_tableview_delegate_virtual;

class lds_tableView : public lds_tableview_virtual
{
    Q_OBJECT
public:
    explicit lds_tableView(QWidget *parent = 0);
    ~lds_tableView();
    void restore(int defrow = -1);
    void restore_rows();

    int getcurrow();
    QSet<int> getcurrows();

    void select_beforerow();
    int get_beforerow();
    virtual void setModel(QAbstractItemModel *model);
    virtual void setSelectionModel(QItemSelectionModel *selectionModel);

    QString dg_paint_transfer(const QVariant &dd, const QString & column);
    QString dg_paint_transfer(int row, int column);

    QString hh_paint_transfer(int logicalIndex);
    lds_tableview_delegate_virtual *item_delegate_virtual();
    void setTransferHheader();


    void lds_tableview_delegate_virtual_enableFieldName2k0v1();
    void resizeColumnsToContentsDelay(int mesc = 10);
public slots:
    void clearSelection();
    int gonext();
    int goprevious();

    void resizeColumnsToContents(bool scan_all_hhead = true);
    void resizeColumnsToContents2(bool scan_all_hhead = true);
signals:
    void selectChanged(int crow);
    void selectChanged(const QModelIndex &index);
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
private slots:
    void saverow(const QItemSelection &selected, const QItemSelection &deselected);
    void toselectrow(const QModelIndex &current, const QModelIndex &previous);
private:
    int currow;
    QStack<int> stack_row;
    QSet<int> currows;
private:
    lds_record_do_double_clicked do_double_clicked;
};

#endif // LDS_TABLEVIEW_H
