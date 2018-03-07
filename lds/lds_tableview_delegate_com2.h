#ifndef LDS_TABLEVIEW_DELEGATE_COM2_H
#define LDS_TABLEVIEW_DELEGATE_COM2_H

#include <QItemDelegate>
#include <QMap>
#include "lds_tableview_delegate_virtual.h"

class lds_tableview_delegate_com2 : public lds_tableview_delegate_virtual
{
public:
    explicit lds_tableview_delegate_com2(const QMap<QString/*key*/, QVariant/*value*/> & map, QObject *parent = 0);

    QWidget *createEditor(QWidget *aParent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual void paint(QPainter * painter,const QStyleOptionViewItem & option, const QModelIndex & index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;

    QMap<QString/*key*/, QVariant/*value*/> map;
};

#endif // LDS_TABLEVIEW_DELEGATE_COM2_H
