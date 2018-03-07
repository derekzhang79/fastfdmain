#include "lds_datawidgetmapper.h"
#include <QAbstractItemModel>
#include <QWidget>
#include <QtDebug>
#include <QAbstractItemDelegate>


lds_dataWidgetMapper::lds_dataWidgetMapper(QObject *parent)
    : QDataWidgetMapper(parent)
{
}

void lds_dataWidgetMapper::addMapping(QWidget *widget, int section)
{
    mapperData d;
    //    d.propertyName = propertyName;
    d.section = section;
    d.widget = widget;
    widgetList.append(d);
    QDataWidgetMapper::addMapping(widget, section);
}

void lds_dataWidgetMapper::addMapping(QWidget *widget, int section, const QByteArray &propertyName)
{
    mapperData d;
    d.propertyName = propertyName;
    d.section = section;
    d.widget = widget;
    widgetList.append(d);
    QDataWidgetMapper::addMapping(widget, section, propertyName);
}

void lds_dataWidgetMapper::submit(int row)
{
    foreach(const mapperData &d, widgetList) {
        if(d.propertyName.isEmpty())
            this->itemDelegate()->setModelData(d.widget, this->model(), this->model()->index(row, d.section));
        else
            this->model()->setData(this->model()->index(row, d.section), d.widget->property(d.propertyName), Qt::EditRole);
    }
}
