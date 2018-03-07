#ifndef FTABLEVIEW_STANDMODEL_VIRTUAL_H
#define FTABLEVIEW_STANDMODEL_VIRTUAL_H

#include <QStandardItemModel>
#include <QSqlRecord>

class QItemSelectionModel;
class QTableView;
class ftableview_standmodel_virtual : public QStandardItemModel
{
    Q_OBJECT
    Q_PROPERTY(int step READ step WRITE setstep USER true)

public:
    enum TYPE{
        TYPE_icon,
        TYPE_list
    };
public:
    explicit ftableview_standmodel_virtual(QObject *parent, TYPE t);

    QString imagedishdir;//  D:/Qt/projects/untitled151/image_dish_dir
    QItemSelectionModel *selectionmodel;

    bool model_data_set(int row, int column, const QVariant &value, int role);
    void set_row_column(int row_count, int column_count);
public:
    virtual void togenerate(int index, int updownum, bool hasnext) = 0;
    virtual void togenerateNext() = 0;
    virtual void togeneratePre() = 0;
public slots:
    void setstep(int value);
    void refreshcur();
signals:
    void signalstepstart();
    void signalstepstop();
    void signalFinished();
protected:

    int step() const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    virtual void updatedata(int r, int c, const QMap<int, QVariant> &map);
    virtual void updatedata(int r, int c, const QVariant &edata, const QVariant &udata, const QVariant &ddata);
    virtual void updatedata(int r,int c, const QSqlRecord &record);
    void updatep2pdelay();//path2pixmap
    void indexBegin();
public:
    void clearData(int r, int c);
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
protected:
    int curindex;
    int preindex;
    TYPE _type;

protected:
    //////////////////////////////1//////////////////////////////
    QVariantList get_datalist_by_allpage_deep_1();
    QVariantList get_datalist_by_allpage_deep(int deep);
    QVariantList get_datalist_by_page_deep_1(int page);
    QVariantList get_datalist_by_page_deep(int page, int deep);
    QVariantList get_datalist_by_rect_deep(const QList<QVariantList> &datarect, int deep);
    //page_list_select:0, list[0, "12", 0x03, 12.5]不包括-1【<<】, -2【>>】
    void refresh_cur_select();
    void restore_cur_select(int uesrcheck = -1);
    void clear_cur_select();
    QMap< int /*page*/, QList< QVariantList  /*data:usr+*/ > > page_list_select;
    int user_role_equal_max;
    //////////////////////////////1//////////////////////////////
private:
    int valuestep;
    int valuestep_pre;
};

#endif // FTABLEVIEW_STANDMODEL_VIRTUAL_H
