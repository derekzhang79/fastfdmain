#ifndef FEXPANDMAIN_TABLEVIEW_H
#define FEXPANDMAIN_TABLEVIEW_H


#include <QTableView>
#include <QMouseEvent>
#include "fexpandmain_table.h"
#include <QStyledItemDelegate>

class fexpandmain_tableview : public fexpandmain_table
{
    Q_OBJECT
public:
    explicit fexpandmain_tableview(QWidget *parent = 0) ;
    ~fexpandmain_tableview();

    void scaleHeight(int  oldRow, int newRow);
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private:
    class fexpandmain_delegate : public QStyledItemDelegate{
    public:
        fexpandmain_delegate(QObject *parent = 0);
        void keepConnect(fexpandmain_tableview *tableview);

        // painting
        void paint(QPainter *painter,
                   const QStyleOptionViewItem &option,
                   const QModelIndex &index) const;
        void drawDish(QPainter *painter, const QRect &option_rect, const QModelIndex &index, bool isselected) const;
        QColor getCurNameColor(const QString &ch_suitflag) const;

        int *border;
        QColor *divColor;
        QColor *nameColor;
        QColor *nameSubColor;
        QColor *defColor;
        QColor *itemBgColor;
    };
    int row_before_press;
    //////////////////
    static QRect static_rect0;
    static QRect static_rect1;

    static QRect static_rect2;
    static QRect static_rect3;

    static QRect static_rect4;
    static QRect static_rect5;

    static QRect static_rect6;
    static QRect static_rect7;

    static QRect static_rect8;
    static QRect static_rect9;
    static QRect static_rect10;
    static QRect static_rect11;//delete
    static QRect static_rect12;//detail
    /////////////////////
    static QPainterPath pathEllipse(const QRectF &rect);
    static QPainterPath pathStar();
    static QPainterPath pathAdd();
    static QPainterPath pathSub();
    static QPainterPath pathDot();
    static QPainterPath pathDel();
    static QPainterPath pathDel2();
};

#endif // FEXPANDMAIN_TABLEVIEW_H
