#include "lds_tableview_delegate_com_editable.h"
#include <QDateTimeEdit>
#include <QComboBox>

lds_tableview_delegate_com_editable::lds_tableview_delegate_com_editable(QStandardItemModel *m, int c, QObject *parent) :
    QItemDelegate(parent),typemodel(m),column(c)
{
}

QWidget *lds_tableview_delegate_com_editable::createEditor(QWidget *aParent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (typemodel && column == index.column()) {
        QComboBox *editor = new QComboBox(aParent);
        QString k;
        QString v;
        editor->setEditable(true);
        editor->setModel(typemodel);
        for(int row = 0, rowcount = typemodel->rowCount(); row < rowcount; row++){
            k = typemodel->item(row, 0)->text();
            v = typemodel->item(row, 1)->text();
            if(k == index.data(Qt::EditRole).toString()){// key == key
                editor->setCurrentIndex(row);
                return editor;
            }
        }
        editor->setEditText(index.data(Qt::EditRole).toString());
        return editor;
    }
    return QItemDelegate::createEditor(aParent, option, index);
}

void lds_tableview_delegate_com_editable::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if(typemodel){
        QComboBox *dateEditor = qobject_cast<QComboBox *>(editor);
        if(dateEditor){
            model->setData(index, dateEditor->currentText());
            return;
        }
    }
    QItemDelegate::setModelData(editor, model, index);
}

void lds_tableview_delegate_com_editable::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QComboBox *dateEditor = qobject_cast<QComboBox *>(editor);
    if(dateEditor){

    } else {
        QItemDelegate::setEditorData(editor, index);
    }
}
