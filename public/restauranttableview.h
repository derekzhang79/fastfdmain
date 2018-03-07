#ifndef RESTAURANTTABLEVIEW_H
#define RESTAURANTTABLEVIEW_H
#include "ftableview.h"
#include <QItemDelegate>
#include "lds_bool.h"
#include "fexpandmain.h"
#include "lds_tableview_delegate_virtual.h"
#include "lds_model_sqltablemodel.h"
#include "fexpandmain_model_sqltablemodel.h"

class restraurantDelegate;

struct tableSTATE{
    tableSTATE(){}
    inline tableSTATE(const QString &_key, tableState _state, const QString &_colorname1)
        :key(_key), state(_state), colorname1(_colorname1){}
    QString key;
    tableState state;
    QString colorname1;
    lds_bool_default_false isdisabled;
};
struct cell_content{
    QString showed_color;
    lds_bool_default_false ispressed;
    lds_bool_default_false ischecked;
    lds_bool_default_false isdisabled;
    lds_bool_default_false isDensePattern;
};

class restaurantTableView : public ftableview
{
    Q_OBJECT
    //////////////////////////////////////样式表//////////////////////////////////////
    Q_PROPERTY(QString textColor READ getTextColor WRITE setTextColor DESIGNABLE true)
public:
    void setTextColor(const QString &c);
    QString getTextColor();
public:
    QString textColor;
public:
    restaurantTableView(QWidget *parent = 0);
    void setCurrentIndex(const QString &ch_tableno);
    void setCurrentIndex ( const QModelIndex & index );

    bool find_selected_ch_tableno(const QString &ch_tableno);
    QModelIndex getIndexByChtableno(const QString &ch_tableno);
    void setItemDelegate(restraurantDelegate *delegate);
    restraurantDelegate *get_delegate();

    QStringList getCheckList(const QModelIndex &index);
    QString getChbillnoBySqr(const QModelIndex &index, int sqr);

    ///////////////////////////1////////////////////////////
    void enableMutiSelected(bool f = true);
    QSet<QString> tablenolist;
    void loadTableList(const QStringList &tableList);
public slots:
    void tocurindex_checklist(const QString &checklist);
signals:
    void signalSaveTableFinished();
    void siganlmodelcheck(const QModelIndex & index);
private slots:
    void toSaveCurTable(const QModelIndex &index);
    void toloadTableNo();
    ///////////////////////////1////////////////////////////

private:
    restraurantDelegate *d;

    ///////////////////////////2////////////////////////////
protected slots:
    void toshowpop(const QModelIndex &index);
    ///////////////////////////2////////////////////////////

protected:
    void keyPressEvent(QKeyEvent *e);
};

class restraurantDelegate : public QItemDelegate{
    Q_OBJECT
public:
    enum selectEffect{
        effect_radio = 0,
        effect_border,
    };
public:
    ///////////////////////////1////////////////////////////
    static tableSTATE gettableState(tableState state);
    static QPixmap getPixmapByStateList(QList<tableState> statelist);
    static QPixmap getPixmapByStateList(tableState state);

    static QList<tableSTATE> &getTablestatelist();
    static void clearCache();
private:
    static QList<tableSTATE> tablestatelist;
    ///////////////////////////1////////////////////////////
public:
    static  QColor color_light(const QString &c, double light, int a = 255);//0 1 2 3 原 亮 很亮 最亮
    static  QColor color_light(const QColor &c, double light, int a = 255);//0 1 2 3 原 亮 很亮 最亮
    static  int chbillno2index(int v);
    static  int index2chbillno(int v);
    const static int table_border;

    static void paintDensePattern(QPainter *painter, const QPainterPath &path, const QColor &color, Qt::BrushStyle pattern, qreal pattern_scale, bool drawPen);
    static void paintDensePattern(QPainter *painter, const QRectF &rect, const QColor &color, Qt::BrushStyle pattern, qreal pattern_scale, bool drawPen);

    static void painterCell(QPainter *painter, cell_content cell[][3], int x, int y, const QRect &rect, bool painter_linear, bool gray_mask, qreal xyRadius, qreal  pattern_scale);
    static void cell_init_showed_color(cell_content cell[][3], const QString &showed);
public:
    restraurantDelegate(QObject *parent = 0);
    ~restraurantDelegate();
    // painting
    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const;
    QMap<QString/*ch_tableno*/, bool> *preprinttablemap;//预打印状态
    void keepConnect(restaurantTableView *tableview);
//    void update_select_effect(selectEffect effect_type);

    static QMap<int/*sqr*/, QString/*ch_billno*/> get_ch_billno_div_map(const QString &ch_billnos) ;
    static bool ch_billno_is_div(const QMap<int, QString> &map) ;
    static bool ch_billno_is_div(const QString &ch_billnos);

    static const QString sql_ch_billnos;
    //酒吧
    static bool bar_is_div(const QStringList &int_div_id_list) ;
private:
    QString *textColor;
//    selectEffect effect_type;
//    QString pay_unit_prefix;
    fexpandmain_model_sqltablemodel *model_cache;
};



//九宫格
class restraurant9BoxDelegate : public QItemDelegate{
    Q_OBJECT
public:
    restraurant9BoxDelegate(QObject *parent = 0);
    // painting
    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const;

    void tryCombine();

public:
    QMap<QString/*ch_tableno*/, bool> *preprinttablemap;//预打印状态
private:
    QString pay_unit_prefix;
    lds_bool_default_false combine_state;
    int border;
};

//九宫格
class restraurantDelegateTableState : public lds_tableview_delegate_virtual{
    Q_OBJECT
public:
    restraurantDelegateTableState(QObject *parent = 0)
        : lds_tableview_delegate_virtual(parent){}
    virtual QString paint_transfer(const QVariant &dd) const ;
};
#endif // RESTAURANTTABLEVIEW_H
