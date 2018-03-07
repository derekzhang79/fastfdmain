#ifndef LDS_COMBOBOX_H
#define LDS_COMBOBOX_H

#include <QComboBox>
#include <QPixmap>


//combobox 的下拉高度，比寻常的高一些





class lds_ComboBox : public QComboBox
{
    Q_OBJECT
    Q_PROPERTY(QVariant comusrdata READ comusrdata WRITE setcomusrdata USER true)
public:
    struct PAIRLIST{
        inline PAIRLIST(const QString &_k, const QVariant &_u):
            key(_k),
            usrvalue(_u){}
        QString key;
        QVariant usrvalue;
    };

public:
    explicit lds_ComboBox(QWidget *parent = 0);
    void addItems(const QStringList &texts);
    void addItems(const QMap<QString, QVariant> *map);//顺序非是正序
    void addItems1(const  QList<PAIRLIST>&texts);//顺序是正序
    void addItems1(const  QMap<QString, QVariant>&texts);//顺序非是正序
    void addItemsBySql(const QString &key_value_sql);
    void addItemsByModel(QAbstractItemModel * key_value_model);

    void setindex_c1(const QString c1_text);//xx(c1_text)
    void setcomusrdata(const QVariant &usrtext);//这个model有usrdata和textdata的内容， 会根据usrdata来检索
    QVariant comusrdata(int comindex = -1);//返回usrdata
    QVariant curusrdata();//返回usrdata
    QString comText(const QVariant &d);

    void enableEmitColumn(int emitColumn);
    QVariant model_data(int row, int column, int role = Qt::DisplayRole);
private slots:
    void toEmitColumn(int index);
signals:
    void signalEmitColumn(const QString text);
private:
    int emitColumn;
};

#endif // LDS_COMBOBOX_H
