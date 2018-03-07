#ifndef LDS_DATAWIDGETMAPPER_H
#define LDS_DATAWIDGETMAPPER_H

#include <QDataWidgetMapper>
#include <QMap>

class lds_dataWidgetMapper : public QDataWidgetMapper
{
public:
    lds_dataWidgetMapper(QObject *parent = 0);
    void addMapping(QWidget *widget, int section);
    void addMapping(QWidget *widget, int section, const QByteArray &propertyName);

    void submit(int row);
private:
    struct mapperData{
        QWidget * widget;
        int section;
        QByteArray propertyName;
    };
    QList<mapperData> widgetList;
};

#endif // LDS_DATAWIDGETMAPPER_H
