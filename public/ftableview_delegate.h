#ifndef FTABLEVIEW_DELEGATE_H
#define FTABLEVIEW_DELEGATE_H

#include <QWidget>
#include <QItemDelegate>
#include <QLinearGradient>
#include "restauranttableview.h"
#include <QSettings>

class ftableview;
class ftableview_delegate : public QItemDelegate
{
    Q_OBJECT
public:
    enum IconStyle{
//        IconOnly,
//        TextOnly,
        TextIcon,
//        IconText,
//        TextUnderIcon,
        AutoIcon,// icon isvalid? TextUnderIcon : TextOnly;
    };

public:
    ftableview_delegate(QObject *parent = 0);
    ~ftableview_delegate();
    // painting
    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const;
    void setIconStyle(IconStyle style);
public:
    QLinearGradient getLinerBypropety(const QString &key, QStyle::State state , const QModelIndex &index, const QString &vch_color) const;
    void keepConnect(ftableview *tableview);
    void setMargin(int m);
    void setMargin(int border_top_bottom, int border_left_right);
    void setMargin(int left, int top, int right, int bottom);
    void enableBgexpand0(int row, int column);
    QString posKey(int row, int column) const;
    void hideOuterBorder();
    void setCellBorder(int left, int top, int right, int bottom);
    void setColorLight(double light);
    void setFontNumFamily(int row, int column);
    void setBgPlain_colorLight(int row, int column);
    void setFontNumFamilygPlain_color(int row, int column);
    void setImageMargin(int m);
private:
    QColor *bgNormalColor10;
    QColor *textColor;
    QColor *bgBorderColor;
    QColor *bgExpand0;
    QString tableview_property;
public:
    bool indexisEmptystillDraw;
    static const int CELL_BORDER_SPACE;
private:

    int border_left;
    int border_top;
    int border_right;
    int border_bottom;

    QSet<QString> bgPlain_Expand0_set;//如付款界面的，金额
    QSet<QString> fontNumFamily_set;
    QSet<QString> bgPlain_colorLight_set;

    lds_bool_default_false b_hideOuterBorder;
    int cellMarginLeft;
    int cellMarginTop;
    int cellMarginRight;
    int cellMarginBottom;

    IconStyle icon_style;
    double colorLight;
    int imageMargin;

    QString pay_unit_prefix;
};
#endif // FTABLEVIEW_DELEGATE_H
