#include "lds_tableview_delegate_com.h"
#include <QDateTimeEdit>
#include <QComboBox>
#include <QtDebug>

lds_tableview_delegate_com::lds_tableview_delegate_com(QStandardItemModel *m, int c, QObject *parent) :
    lds_tableview_delegate_virtual(parent),typemodel(m),column(c),_onlyk(false)
{
    qDebug() << __LINE__;
}

lds_tableview_delegate_com::lds_tableview_delegate_com(const QMap<QString, QVariant> &map, QObject *parent) :
    lds_tableview_delegate_virtual(parent)
{
    column = -1;
    _onlyk = true;
    qDebug() << __LINE__;

    typemodel = new QStandardItemModel(this);
    for(QMap<QString, QVariant>::const_iterator k = map.begin(); k != map.end(); k++) {
        qDebug() << __LINE__;
        typemodel->appendRow(QList<QStandardItem*>()
                             << new QStandardItem(k.key()) << new QStandardItem(k.value().toString())
                             );
    }
}
QWidget *lds_tableview_delegate_com::createEditor(QWidget *aParent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    qDebug() << __LINE__;
    if ( -1 == column || (column == index.column()) ) {
        qDebug() << __LINE__;
        QComboBox *editor = new QComboBox(aParent);
        QString k;
        QString v;
        editor->setModel(typemodel);
        editor->setCurrentIndex(-1);
        if(_onlyk) editor->setModelColumn(0);
        else editor->setModelColumn(1);
        qDebug() << __LINE__;
        for(int row = 0, rowcount = typemodel->rowCount(); row < rowcount; row++){
            qDebug() << __LINE__;
            k = typemodel->item(row, 0)->text();
            v = typemodel->item(row, 1)->text();
            if(k == index.data(Qt::EditRole).toString()){// key == key
                editor->setCurrentIndex(row);
                break;
            }
            qDebug() << __LINE__;
        }
        qDebug() << __LINE__;
        return editor;
    }
    qDebug() << __LINE__;
    return lds_tableview_delegate_virtual::createEditor(aParent, option, index);
}

void lds_tableview_delegate_com::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    lds_tableview_delegate_virtual::paint(painter, option, index);

}

void lds_tableview_delegate_com::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if(typemodel){
        QComboBox *dateEditor = qobject_cast<QComboBox *>(editor);
        if(dateEditor){
            QString k;
            QString v;
            int row = dateEditor->currentIndex();
            if(row > -1){
                k = typemodel->item(row, 0)->text();
                v = typemodel->item(row, 1)->text();
            }
            model->setData(index ,k, Qt::EditRole);
            return;
        }
    }
    lds_tableview_delegate_virtual::setModelData(editor, model, index);
}

void lds_tableview_delegate_com::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QComboBox *dateEditor = qobject_cast<QComboBox *>(editor);
    if(dateEditor){

    } else {
        lds_tableview_delegate_virtual::setEditorData(editor, index);
    }
}

void lds_tableview_delegate_com::setonlyk(bool f)
{
    _onlyk = f;
}
