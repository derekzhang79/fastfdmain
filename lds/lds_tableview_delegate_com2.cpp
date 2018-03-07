#include "lds_tableview_delegate_com2.h"
#include <QComboBox>
#include <QtDebug>

lds_tableview_delegate_com2::lds_tableview_delegate_com2(const QMap<QString, QVariant> &map, QObject *parent)
    : lds_tableview_delegate_virtual(parent)
{
    this->map = map;
}

QWidget *lds_tableview_delegate_com2::createEditor(QWidget *aParent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
        QComboBox *editor = new QComboBox(aParent);
        for(QMap<QString/*key*/, QVariant/*value*/>::const_iterator k = map.begin(); k != map.end(); k++) {
            editor->addItem(k.key(), k.value());
        }
        editor->setCurrentIndex(editor->findData(index.data(Qt::UserRole)));
        return editor;
}

void lds_tableview_delegate_com2::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    lds_tableview_delegate_virtual::paint(painter, option, index);
}

void lds_tableview_delegate_com2::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox *com = qobject_cast<QComboBox *>(editor);
    model->setData(index ,com->itemData(com->currentIndex()), Qt::UserRole);
    model->setData(index ,com->itemText(com->currentIndex()), Qt::EditRole);

    qDebug() << __LINE__ << model->data(index, Qt::EditRole);
    qDebug() << __LINE__ << model->data(index, Qt::UserRole);
}

void lds_tableview_delegate_com2::setEditorData(QWidget *editor, const QModelIndex &index) const
{

}
