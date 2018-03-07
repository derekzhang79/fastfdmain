#include "ftableview_delegate.h"
#include <QPainter>
#include <QtDebug>
#include "ftableview.h"
#include "lds.h"
#include "fexpandmain.h"
#include <QApplication>
#include "n_func.h"

 const int ftableview_delegate::CELL_BORDER_SPACE = 2;
ftableview_delegate::ftableview_delegate(QObject *parent):QItemDelegate(parent)
{
    textColor= 0;
    bgNormalColor10 = 0;
    bgBorderColor = 0;
    border_left = border_top = border_right = border_bottom = CELL_BORDER_SPACE;
    indexisEmptystillDraw = false;
    icon_style = AutoIcon;

    cellMarginLeft = 0;
    cellMarginTop = 0;
    cellMarginRight = 0;
    cellMarginBottom = 0;

    colorLight = 2;
    imageMargin = 5;
    pay_unit_prefix = n_func::f_get_sysparm("pay_unit_prefix", " ");

}

ftableview_delegate::~ftableview_delegate()
{
    qDebug() << __FUNCTION__;
}

void ftableview_delegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStringList textprice = index.data(Qt::UserRole).toString().split("\n");
    QString ch_dishno = index.data(Qt::UserRole+1).toString();
    QVariant image = index.data(Qt::UserRole+2);
    int ch_groupno_count = index.data(Qt::UserRole+3/*组数量*/).toInt();//库存剩余、空：不记库存，不空：计算库存
    QString st_remain = index.data(Qt::UserRole+4).toString();//库存剩余、空：不记库存，不空：计算库存
    double  dishcount = index.data(Qt::UserRole+5).toDouble();// "" 0, 1
    QString ischecked = index.data(Qt::UserRole+6).toString();// "" 0, 1
    QString num_item_warn = index.data(Qt::UserRole+7).toString();// "" 0, 1
    QStringList item_inventory_list = index.data(Qt::UserRole+8).toString().split(",", QString::SkipEmptyParts);// "" 0, 1
    QString vch_color = index.data(Qt::UserRole+9).toString();// "" 0, 1
    QString item_inventory;
    int item_inventory_off = 0;
    const QAbstractItemModel *model = index.model();


    //绘制的边框
    int left = border_left;
    int right = border_right;
    int top = border_top;
    int bottom = border_bottom;
    if(b_hideOuterBorder) {
        if(index.row() == 0) top = 0;
        if(index.column() == 0) left = 0;
        if(index.row() == model->rowCount() - 1) bottom = 0;
        if(index.column() == model->columnCount() - 1) right = 0;
    }
    QRect parent_rect = option.rect.adjusted(left, top, -right, -bottom);


    if(false == indexisEmptystillDraw && ch_dishno == "-3") {//新增
        painter->setPen(QPen(*bgNormalColor10, 3, Qt::DashDotDotLine));

        parent_rect.adjust(1,1,-1,-1);
        double woff = parent_rect.width()/10.0;
        double hoff = parent_rect.height()/10.0;
        painter->drawRoundedRect(parent_rect, woff, hoff);

        painter->setPen(Qt::NoPen);
        painter->setBrush(*bgNormalColor10);
        painter->drawRoundedRect(parent_rect.adjusted(woff*1.5, hoff*4, -woff*1.5, -hoff*4), hoff/1.2, hoff/1.2);
        painter->drawRoundedRect(parent_rect.adjusted(woff*4, hoff*1.5, -woff*4, -hoff*1.5), woff/1.2, woff/1.2);
        return;
    }

    QString text = textprice.value(0);
    QString price = textprice.value(1);
    QFont font = painter->font();
    int textheight = QFontMetrics(font).height();
    QRect rect2(parent_rect.topLeft(), QSize(parent_rect.width(), textheight));
//    painter->setFont(font);

    QLinearGradient linearGrad;
    painter->setRenderHint(QPainter::Antialiasing);
    //若是索引值为空则是否依旧绘制
    if(false == indexisEmptystillDraw && ch_dishno.isEmpty()){
        return;
    }
    //点击效果
    /////////////

    linearGrad = getLinerBypropety(ch_dishno, option.state, index, vch_color);
    linearGrad.setStart(parent_rect.topLeft());
    linearGrad.setFinalStop(parent_rect.bottomLeft());
    painter->fillRect(parent_rect, *bgExpand0);

    if(option.state & QStyle::State_Selected) {
        painter->fillRect(parent_rect, linearGrad);
    } else {
        //按键
        if(bgPlain_colorLight_set.contains(posKey(index.row(), index.column()))) {
            painter->fillRect(parent_rect, linearGrad);
        } else {
            painter->fillRect(parent_rect.adjusted(cellMarginLeft, cellMarginTop, -cellMarginRight, -cellMarginBottom), linearGrad);
        }
    }

    if(bgBorderColor)
    {
        if(bgBorderColor->isValid()) {
            painter->setPen(QPen(*bgBorderColor, 0.5));
            painter->drawRect(parent_rect);
        }
    }
    QString text0 = text;
    QRect rect0 = parent_rect;
    int align0 = int(Qt::AlignCenter|Qt::TextWordWrap);
    if(image.canConvert<QString>()) {
        if(!price.isEmpty()) {
            text0 = text+'\n'+ pay_unit_prefix + lds::septNumber(price.toDouble(), 2);
        }
    } else if(image.canConvert<QPixmap>()){

        QPixmap pixmap = qvariant_cast<QPixmap>(image);

        if(icon_style == AutoIcon) {
            align0 = int(Qt::AlignCenter|Qt::TextWordWrap);

            int textheight = parent_rect.height()*1.0/3;
            QRect rectpixmap = parent_rect.adjusted(imageMargin,imageMargin,-imageMargin,-imageMargin-textheight);
            if(!pixmap.isNull())
                pixmap = pixmap.scaled(rectpixmap.size(), Qt::KeepAspectRatio);

            painter->drawPixmap(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, pixmap.size(), rectpixmap), pixmap);
            rect0 = parent_rect.adjusted(0,  parent_rect.height()-textheight,  0, 0);
        } else if(icon_style == TextIcon){
            align0 = int(Qt::AlignVCenter|Qt::AlignLeft);

            QRect rectpixmap = parent_rect;
            rectpixmap = rectpixmap.adjusted(imageMargin, imageMargin, -imageMargin, -imageMargin);
            if(!pixmap.isNull())
                pixmap = pixmap.scaled(rectpixmap.size(), Qt::KeepAspectRatio);

            painter->drawPixmap(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignVCenter | Qt::AlignRight, pixmap.size(), rectpixmap), pixmap);
            rect0 = parent_rect.adjusted(imageMargin, 0, 0, 0);
        }
    }
    if(textColor)painter->setPen(*textColor);
    painter->drawText(rect0, align0,  text0);

    if(ch_groupno_count > 1) {
        painter->setPen(QColor("white"));
        painter->drawText(rect2, Qt::AlignVCenter|Qt::AlignLeft, QString::number(ch_groupno_count));
    }

    if("1" == ischecked) {
        QPixmap pg(":/image/widgets_pic/radio_on.png");
        painter->drawPixmap(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignRight | Qt::AlignTop, pg.size(), parent_rect), pg);
    }


    if(ch_dishno == "-3") {//菜品类别，新增
        painter->setPen(QPen(*textColor, 3, Qt::DashDotDotLine));
        font.setPixelSize(parent_rect.height()*2/3);
        painter->setFont(font);
        painter->drawText(parent_rect, Qt::AlignCenter, "+");
    }

    if(lds::sysconf->value("system_setting/inventory_qt_show_info", false).toBool()
            && !st_remain.isEmpty()) {
        QString table_desc;
        QString table_color;
        if(st_remain.toFloat() > 0) {
            if(st_remain.toFloat() <= num_item_warn.toFloat()) {
                table_color = "yellow";//restraurantDelegate::gettableState(tableState_YUDA).colorname1;
                table_desc = tr("库存")+":"+QString::number(st_remain.toDouble())+tr("请补仓");
            } else {
                QString skin_name = lds::sysconf->value("system_setting/skin_name", "obsidian").toString();
                if("skyblue" == skin_name) {
                    table_color = "green";
                }
                if("obsidian" == skin_name) {
                    table_color = "cyan";
                }
                table_desc = tr("库存")+":"+QString::number(st_remain.toDouble());
            }
        } else {
            table_color = "red";
            table_desc = tr("售完");
        }

        painter->setPen(table_color);
        painter->drawText(rect2, Qt::AlignVCenter|Qt::AlignRight, table_desc);
    }
    /////////////



    /// 原料库存预警
    //13012131231213312
    if(lds::sysconf->value("system_setting/inventory_qt_show_info", false).toBool()) {
        foreach(const QString &k, item_inventory_list) {
            QString k0 = k.split("/").value(1);
            QString k1 = k.split("/").value(2);
            if(k0.isEmpty()) {
                item_inventory += "3";
                continue;
            }
            if(k0.toFloat() < 0) {
                item_inventory += "0";
                continue;
            }
            if(k0.toFloat() < k1.toFloat()) {
                item_inventory += "1";
                continue;
            }
            if(k0.toFloat() >= k1.toFloat()) {
                item_inventory += "2";
                continue;
            }
        }
        qSort(item_inventory.begin(), item_inventory.end(), qLess<QChar>());
        foreach(const QChar &c, item_inventory) {
            QColor color;
            //若是y小于top
            int item_inventory_y = parent_rect.bottom() - item_inventory_off * 5 - 5;
            if(item_inventory_y < parent_rect.top()) {
                break;
            }
            if(c == '0') {
                color = QColor("red");
            }
            if(c == '1') {
                color = QColor("yellow");
            }
            if(c == '2') {
                color = QColor("green");
            }
            if(c == '3') {
                color = QColor("gray");
            }
            painter->fillRect(QRect(parent_rect.right() - 5, item_inventory_y, 5, 5), color);
            item_inventory_off++;
        }
        ///~原料库存预警
    }
}

void ftableview_delegate::setIconStyle(ftableview_delegate::IconStyle style)
{
    icon_style = style;
}

QLinearGradient ftableview_delegate::getLinerBypropety(const QString &key, QStyle::State state, const QModelIndex &index, const QString &vch_color) const
{
    QLinearGradient retv;
    QColor color;
    double color_light1;
    double color_light0;

    //点击
    if(state &QStyle::State_Selected || !(state & QStyle::State_Enabled)){
        color = QColor("#ff650b");
        color_light1 = 0;
        color_light0 = 0;
        goto end;
    }

    //外部单色
    if(bgPlain_Expand0_set.contains(posKey(index.row(), index.column()))) {
        color = *bgExpand0;
        color_light1 = 0;
        color_light0 = 0;
        goto end;
    }
    //按键
    if(bgPlain_colorLight_set.contains(posKey(index.row(), index.column()))) {
        color = *bgNormalColor10;
        color_light1 = colorLight;
        color_light0 = colorLight;
        goto end;
    }

    //默认
    {
        QColor color0(vch_color);
        if(color0.isValid()) {
            color = color0;
        } else {
            color = *bgNormalColor10;
        }
    }
    color_light1 = colorLight;
    color_light0 = 0;


end:
    retv.setColorAt(0.0, restraurantDelegate::color_light(color, color_light1));
    retv.setColorAt(1.0, restraurantDelegate::color_light(color, color_light0));

    return retv;
}

void ftableview_delegate::keepConnect(ftableview *tableview)
{
    bgNormalColor10 = &tableview->bgNormalColor10;
    textColor = &tableview->textColor;
    bgBorderColor = &tableview->bgBorderColor;
    bgExpand0 = &tableview->bgExpand0;
    tableview_property = tableview->property("outer_stylesheet").toString();
}

void ftableview_delegate::setMargin(int m)
{
    border_top = border_bottom = m;
    border_left = border_right = m;
}

void ftableview_delegate::setMargin(int border_top_bottom, int border_left_right)
{
    border_top = border_bottom = border_top_bottom;
    border_left = border_right = border_left_right;
}

void ftableview_delegate::setMargin(int left, int top, int right, int bottom)
{
    border_top =  top;  border_bottom = bottom;
    border_left = left; border_right = right;
}

void ftableview_delegate::enableBgexpand0(int row, int column)
{
    bgPlain_Expand0_set.insert(posKey(row, column));
}

QString ftableview_delegate::posKey(int row, int column) const
{
    return QString("(%1,%2)").arg(row).arg(column);
}

void ftableview_delegate::hideOuterBorder()
{
    b_hideOuterBorder = true;
}

void ftableview_delegate::setCellBorder(int left, int top, int right, int bottom)
{
    cellMarginLeft = left;
    cellMarginTop = top;
    cellMarginRight = right;
    cellMarginBottom = bottom;
}

void ftableview_delegate::setColorLight(double light)
{
    colorLight = light;
}

void ftableview_delegate::setFontNumFamily(int row, int column)
{
    fontNumFamily_set.insert(posKey(row, column));
}

void ftableview_delegate::setBgPlain_colorLight(int row, int column)
{
    bgPlain_colorLight_set.insert(posKey(row, column));
}

void ftableview_delegate::setFontNumFamilygPlain_color(int row, int column)
{
    setFontNumFamily(row, column);
    setBgPlain_colorLight(row, column);

}

void ftableview_delegate::setImageMargin(int m)
{
    imageMargin = m;
}
