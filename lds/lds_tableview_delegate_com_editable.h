#ifndef LDS_TABLEVIEW_DELEGATE_COM_EDITABLE_H
#define LDS_TABLEVIEW_DELEGATE_COM_EDITABLE_H


#include <QItemDelegate>
#include <QMap>
#include <QStandardItemModel>

class lds_tableview_delegate_com_editable : public QItemDelegate
{
    Q_OBJECT
public:
    explicit lds_tableview_delegate_com_editable(QStandardItemModel *m/*k_v*/, int c, QObject *parent = 0);

    QWidget *createEditor(QWidget *aParent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    QStandardItemModel *typemodel;
private:
    int column;
};

#endif // LDS_TABLEVIEW_DELEGATE_COM_EDITABLE_H
