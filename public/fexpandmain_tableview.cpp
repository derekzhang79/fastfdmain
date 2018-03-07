#include "fexpandmain_tableview.h"
#include <QtDebug>
#include <QHeaderView>
#include <QPainter>
#include <QApplication>
#include "lds_model_sqltablemodel.h"
#include "fexpandmain_2.h"
#include "math.h"
#include "fexpandmain_model_sqltablemodel.h"

QRect fexpandmain_tableview::static_rect0;
QRect fexpandmain_tableview::static_rect1;

QRect fexpandmain_tableview::static_rect2;
QRect fexpandmain_tableview::static_rect3;

QRect fexpandmain_tableview::static_rect4;
QRect fexpandmain_tableview::static_rect5;

QRect fexpandmain_tableview::static_rect6;
QRect fexpandmain_tableview::static_rect7;

QRect fexpandmain_tableview::static_rect8;
QRect fexpandmain_tableview::static_rect9;
QRect fexpandmain_tableview::static_rect10;
QRect fexpandmain_tableview::static_rect11;
QRect fexpandmain_tableview::static_rect12;



fexpandmain_tableview::fexpandmain_tableview(QWidget *parent): fexpandmain_table(parent)
{
    this->setProperty("outer_stylesheet", "fexpandmain_tableview");
    fexpandmain_delegate *d = new fexpandmain_delegate(this);
    d->keepConnect(this);
    this->setItemDelegate(d);
    row_before_press = -1;
}

fexpandmain_tableview::~fexpandmain_tableview()
{
}

void fexpandmain_tableview::scaleHeight(int oldRow, int newRow)
{
    if(this->rowHeight(oldRow) > tableRowHeight) {
        this->setRowHeight(oldRow, tableRowHeight);
    }
    this->setRowHeight(newRow, 90);
}

void fexpandmain_tableview::mousePressEvent(QMouseEvent *event)
{
    row_before_press = this->currentIndex().row();
    lds_tableview_timeout::mousePressEvent(event);
    scaleHeight(row_before_press, pressIndex.row());
}

void fexpandmain_tableview::mouseReleaseEvent(QMouseEvent *event)
{
    lds_tableview_timeout::mouseReleaseEvent(event);
    int row = pressIndex.row();
    if(row_before_press == row//row before press
            && pressIndex.isValid()) {
        QPoint p_off = event->pos() - QPoint(columnViewportPosition(pressIndex.column()), rowViewportPosition(row));
        if(static_rect7.contains(p_off)) {
            //            qDebug() << "suitflag";
            lds_model_sqltablemodel *model = qobject_cast<lds_model_sqltablemodel *>(this->model());
            if("P" == model->model_data(row, "ch_suitflag").toString()) {
                emit signal_suit_fold(pressIndex);
            }
        }
        //
        fexpandmain_model_sqltablemodel_data  *tablemodel = qobject_cast<fexpandmain_model_sqltablemodel_data *>(this->model());
        if(tablemodel->opera(row) == lds_model_sqltablemodel::OpInsert && tablemodel->model_data(row, "ch_suitflag").toString() != "Y"){//新点菜品 且 非子套菜
            if(static_rect8.contains(p_off)) {//+
                emit signal_add(pressIndex);
            }
            if(static_rect10.contains(p_off)) {//*
                emit signal_change(pressIndex);
            }
            if(static_rect9.contains(p_off)) {//-
                emit signal_sub(pressIndex);
            }
        }
        if(tablemodel->model_data(row, "ch_suitflag").toString() != "Y") {
            if(static_rect11.contains(p_off)) {//delete
                emit signal_delete(pressIndex);
            }
        }
        if(static_rect12.contains(p_off)) {//...
            emit signal_detail(pressIndex);
        }
    }
}

QPainterPath fexpandmain_tableview::pathEllipse(const QRectF &rect)
{
    QPainterPath path;
    path.addEllipse(rect);
    return path;
}
QPainterPath fexpandmain_tableview::pathStar()
{
    qreal off = 1.4;
    QPainterPath path;
    path.moveTo( QPointF(6.9, 13.7) );
    path.lineTo( QPointF(12.78, 13.82) );
    path.lineTo( QPointF(9.46, 8.76) );
    path.lineTo( QPointF(11.38, 8.16) );
    path.lineTo( QPointF(14.7, 13.96) );
    path.lineTo( QPointF(18.1, 8.16) );
    path.lineTo( QPointF(20.02, 8.8) );
    path.lineTo( QPointF(16.7, 13.82) );
    path.lineTo( QPointF(22.5, 13.82) );
    path.lineTo( QPointF(22.5, 15.62) );
    path.lineTo( QPointF(16.7, 15.62) );
    path.lineTo( QPointF(19.98, 21.72) );
    path.lineTo( QPointF(18.1, 22.24) );
    path.lineTo( QPointF(14.78, 16.56) );
    path.lineTo( QPointF(11.38, 22.24) );
    path.lineTo( QPointF(9.5, 21.68) );
    path.lineTo( QPointF(12.86, 15.62) );
    path.lineTo( QPointF(6.9, 15.62) );
    return path;
}

QPainterPath fexpandmain_tableview::pathAdd()
{
    QPainterPath path;
    path.moveTo( QPointF(6.9, 13.5) );
    path.lineTo( QPointF(13.5, 13.5) );
    path.lineTo( QPointF(13.5, 6.9) );
    path.lineTo( QPointF(15.9, 6.9) );
    path.lineTo( QPointF(15.9, 13.5) );
    path.lineTo( QPointF(22.5, 13.5) );
    path.lineTo( QPointF(22.5, 15.9) );
    path.lineTo( QPointF(15.9, 15.9) );
    path.lineTo( QPointF(15.9, 22.5) );
    path.lineTo( QPointF(13.5, 22.5) );
    path.lineTo( QPointF(13.5, 15.9) );
    path.lineTo( QPointF(6.9, 15.9) );


    return path;
}

QPainterPath fexpandmain_tableview::pathSub()
{
    QPainterPath path;
    path.moveTo( QPointF(6.9, 13.5) );
    path.lineTo( QPointF(22.5, 13.5) );
    path.lineTo( QPointF(22.5, 15.9) );
    path.lineTo( QPointF(6.9, 15.9) );
    return path;
}

QPainterPath fexpandmain_tableview::pathDot()
{
    QPainterPath path;
    path.addRect(6.5, 13.5 ,2.8, 2.8);
    path.addRect(13.5, 13.5 ,2.8, 2.8);
    path.addRect(20.5, 13.5 ,2.8, 2.8);
    return path;
}

QPainterPath fexpandmain_tableview::pathDel()
{
    QPainterPath path;

    QRect boundingRect(-9, 0, 30, 20);
    qreal startAngle = 261;
    qreal sweepLength = 110;
    path.moveTo(QPointF(3.6, 20));
    path.arcTo(boundingRect, startAngle, sweepLength);
    QRect boundingRect2(-19, -10, 40, 30);
    qreal startAngle2 = 350;
    qreal sweepLength2 = -72;
    path.arcTo(boundingRect2, startAngle2, sweepLength2);
    return path;
}

QPainterPath fexpandmain_tableview::pathDel2()
{
    QPainterPath path;
    path.moveTo( QPointF(8.66, 10.13) );
    path.lineTo( QPointF(10.33, 8.46) );
    path.lineTo( QPointF(23, 21) );
    path.lineTo( QPointF(21.4, 22.6) );

    return path;
}

fexpandmain_tableview::fexpandmain_delegate::fexpandmain_delegate(QObject *parent) : QStyledItemDelegate(parent)
{
}

void fexpandmain_tableview::fexpandmain_delegate::keepConnect(fexpandmain_tableview *tableview)
{
    divColor = &tableview->divColor;
    nameColor = &tableview->nameColor;
    nameSubColor = &tableview->nameSubColor;
    border = &tableview->border;
    defColor = &tableview->defColor;
    itemBgColor = &tableview->itemBgColor;
}

void fexpandmain_tableview::fexpandmain_delegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(index.column() == 0) {
        drawDish(painter, option.rect, index, option.state & QStyle::State_Selected);
    } else {
        QStyledItemDelegate::paint(painter, option, index);
    }
}

void fexpandmain_tableview::fexpandmain_delegate::drawDish(QPainter *painter, const QRect &option_rect, const QModelIndex &index, bool isselected) const
{
    const fexpandmain_model_sqltablemodel_data *tablemodel = static_cast<const fexpandmain_model_sqltablemodel_data *>(index.model());
    int row = index.row();
    QString vch_print_memo = tablemodel->model_data(row, "vch_print_memo").toString();
    double num_price_add = tablemodel->model_data(row, "num_price_add").toDouble();
    QString ch_suitflag = tablemodel->model_data(row, "ch_suitflag").toString();
    QString dish_desc = tablemodel->get_dish_desc(row);

    const QRect back_rect = option_rect.adjusted(*border, 0, *border*-1, *border*-1);
    const QRect rect = back_rect.adjusted(0, 0, 0, tableRowHeight - option_rect.height());
    int circle_rb = 30;

    ////////////////////
    if(static_rect0.isNull()) static_rect0 = rect.adjusted(circle_rb, 0, 0 , 0);//base
    if(static_rect1.isNull()) static_rect1 = QRect(0                              ,0                            ,circle_rb                                       , rect.height());//序号

    if(static_rect2.isNull()) static_rect2 = QRect(circle_rb                                                 , 0                            ,static_rect0.width()*7.0/10.0            , static_rect0.height()*2.0/6.0);//菜品
    if(static_rect3.isNull()) static_rect3 = QRect(circle_rb+static_rect0.width()*7.0/10.0, 0                            ,static_rect0.width()*3.0/10.0 - *border*2, static_rect0.height()*2.0/6.0);//金额

    if(static_rect5.isNull()) static_rect5 = QRect(circle_rb                                                , static_rect0.height()*2.0/6.0,static_rect0.width()*7.0/10.0            , static_rect0.height()*2.0/6.0);//做法
    if(static_rect6.isNull()) static_rect6 = QRect(circle_rb+static_rect0.width()*7.0/10.0, static_rect0.height()*2.0/6.0,static_rect0.width()*3.0/10.0 - *border*2, static_rect0.height()*2.0/6.0);//做法加价

    if(static_rect4.isNull()) static_rect4 = QRect(circle_rb                                               , static_rect0.height()*4.0/6.0,static_rect0.width()/**7.0/10.0*/            , static_rect0.height()*2.0/6.0);//数量，单价,标志
    if(static_rect7.isNull()) static_rect7 = QRect(circle_rb+static_rect0.width()*7.0/10.0, static_rect0.height()*4.0/6.0,static_rect0.width()*3.0/10.0 - *border*2, static_rect0.height()*2.0/6.0);//折叠

    if(static_rect8.isNull()) static_rect8   = QRect(rect.width() - 120 - circle_rb + 0                                              , static_rect0.height(), circle_rb, circle_rb);//+
    if(static_rect9.isNull()) static_rect9   = QRect(rect.width() - 120 - circle_rb + 30                                            , static_rect0.height(), circle_rb, circle_rb);//-
    if(static_rect10.isNull()) static_rect10 = QRect(rect.width() - 120 - circle_rb + 60                                            , static_rect0.height(), circle_rb, circle_rb);//*
    if(static_rect11.isNull()) static_rect11 = QRect(rect.width() - 120 - circle_rb + 90                                            , static_rect0.height(), circle_rb, circle_rb);// !
    if(static_rect12.isNull()) static_rect12 = QRect(rect.width() - 120 - circle_rb + 120                                            , static_rect0.height(), circle_rb, circle_rb);// !
    ////////////////////
    QRect rect1 = static_rect1.translated(rect.topLeft());//序号
    QRect rect2 = static_rect2.translated(rect.topLeft());//菜品

    QRect rect3 = static_rect3.translated(rect.topLeft());//金额
    QRect rect4 = static_rect4.translated(rect.topLeft());//赠送等标识数量，单价
    QRect rect5 = static_rect5.translated(rect.topLeft());//做法
    QRect rect6 = static_rect6.translated(rect.topLeft());//做法加价
    QRect rect7 = static_rect7.translated(rect.topLeft());//折叠

    QRect rect8 = static_rect8.translated(rect.topLeft());//+
    QRect rect9 = static_rect9.translated(rect.topLeft());//-
    QRect rect10 = static_rect10.translated(rect.topLeft());//*
    QRect rect11 = static_rect11.translated(rect.topLeft());// !
    QRect rect12 = static_rect12.translated(rect.topLeft());// !


    //    //没有做法，有赠送标识
    if(vch_print_memo.isEmpty() && num_price_add == 0) {
        rect2.adjust(0, 0,                     0, rect.height()*2.0/6.0);//菜品
        rect3.adjust(0, 0,                     0, rect.height()*2.0/6.0);//数量，单价
        rect4.adjust(0, -rect.height()*1.0/6.0, 0, 0);//金额

        rect5 = QRect();//做法
        rect6 = QRect();//做法加价
    }


    painter->setRenderHint(QPainter::Antialiasing);
    QFontMetrics fontm(painter->font());

    //background
    if(isselected) {
        painter->fillRect(back_rect, QColor("#ff650b"));
    } else {
        painter->fillRect(rect, *itemBgColor);
    }
    // more Dense7Pattern
    if(back_rect.height() > tableRowHeight) {
        QString colorname;
        if(lds::sysconf->value("system_setting/skin_name", "obsidian").toString() == "obsidian") {
            colorname = "#484848";
        } else {
            colorname = "lightgray";
        }
        restraurantDelegate::paintDensePattern(painter,
                                               back_rect.adjusted(0, rect.height(), 0, 0),
                                               QColor(colorname),
                                               Qt::Dense7Pattern,
                                               1,
                                               false
                                               );
    }
    //int_id
    QString int_id = tablemodel->model_data(row, "int_id").toString();
    if(tablemodel->opera(row) != lds_model_sqltablemodel::OpInsert) {
        int_id += ".";
    }
    //
    QColor brush_color(getCurNameColor(ch_suitflag));
    //
    if(back_rect.height() > tableRowHeight) {
        if(tablemodel->opera(row) == lds_model_sqltablemodel::OpInsert && tablemodel->model_data(row, "ch_suitflag").toString() != "Y"){//新点菜品 且 非子套菜
            painter->fillPath(pathEllipse(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, QSize(circle_rb, circle_rb), rect8).adjusted(2,2,-2,-2)), brush_color);//+
            painter->fillPath(pathEllipse(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, QSize(circle_rb, circle_rb), rect9).adjusted(2,2,-2,-2)), brush_color);//-
            painter->fillPath(pathEllipse(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, QSize(circle_rb, circle_rb), rect10).adjusted(2,2,-2,-2)), brush_color);//x
        }
        if(tablemodel->model_data(row, "ch_suitflag").toString() != "Y") {
            painter->fillPath(pathEllipse(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, QSize(circle_rb, circle_rb), rect11).adjusted(2,2,-2,-2)), QColor("red"));//delete
        }
        painter->fillPath(pathEllipse(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, QSize(circle_rb, circle_rb), rect12).adjusted(2,2,-2,-2)), brush_color);//详细
    }
    //(1)
    if(isselected) {
        painter->fillPath(pathEllipse(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, QSize(circle_rb, circle_rb), rect1).adjusted(2,2,-2,-2)), brush_color);//序号
        painter->setPen("white");
    } else {
        painter->setPen(*defColor);
    }
    //
    painter->drawText(rect1, Qt::AlignCenter, int_id);//序号
    if(back_rect.height() > tableRowHeight) {
        if(tablemodel->opera(row) == lds_model_sqltablemodel::OpInsert && tablemodel->model_data(row, "ch_suitflag").toString() != "Y"){//新点菜品 且 非子套菜
            painter->fillPath(pathAdd().translated(rect8.x(), rect8.y()), QColor("white"));//+
            painter->fillPath(pathSub().translated(rect9.x(), rect9.y()), QColor("white"));//-
            painter->fillPath(pathStar().translated(rect10.x(), rect10.y()), QColor("white"));//*
        }
        if(tablemodel->model_data(row, "ch_suitflag").toString() != "Y") {//delete
            painter->fillPath(pathDel().translated(rect11.x(), rect11.y()), QColor("white"));
            painter->fillPath(pathDel2().translated(rect11.x(), rect11.y()), QColor("white"));
        }
        painter->fillPath(pathDot().translated(rect12.x(), rect11.y()), QColor("white"));//...
    }

    //菜品
    painter->drawText(rect2, Qt::AlignVCenter | Qt::AlignLeft,
                      tablemodel->get_cey_bt_dish_value(tablemodel->model_data(row, "ch_dishno").toString(), "vch_dishname").toString()
                      );
    //金额
    painter->drawText(rect3, Qt::AlignVCenter | Qt::AlignRight, QString("%1%2")
                      .arg(tablemodel->get_cey_sys_parameter_value("pay_unit_prefix").toString())
                      .arg(lds::septNumber(tablemodel->model_data_IF(row, "if(ch_presentflag = 'Y', 0, (num_num-num_back)*num_price*int_discount*0.01)"), tablemodel->get_cey_sys_parameter_value("bp_fmt_amount_dec", 2).toInt()))
                      );

    QString priceXnum = QString("%1%2x%3")
            .arg(tablemodel->get_cey_sys_parameter_value("pay_unit_prefix").toString())
            .arg(lds::septNumber(tablemodel->model_data(row, "num_price").toDouble(), tablemodel->get_cey_sys_parameter_value("bp_fmt_price_dec", 2).toInt()))
            .arg(lds::septNumber(tablemodel->model_data_NIBOLAN(row, "num_num-num_back"), tablemodel->get_cey_sys_parameter_value("bp_fmt_num_dec", 3).toInt()))
            ;
    int AlignLeft_width = qMax(fontm.width(priceXnum), rect.width()/2);
    ///单价数量、特价标识
    painter->drawText(rect4, Qt::AlignVCenter | Qt::AlignLeft, priceXnum);
    painter->drawText(rect4.adjusted(AlignLeft_width+2, 0, 0, 0), Qt::AlignVCenter | Qt::AlignLeft, dish_desc);
    ///单价数量、特价标识
    //做法
    if(ch_suitflag == "P") {
    } else {
        if(!vch_print_memo.isEmpty()) {
            painter->drawText(rect5, Qt::AlignVCenter | Qt::AlignLeft, tablemodel->get_desc_vch_print_memo_by_map(vch_print_memo));
        }
    }
    //做法加价
    if(ch_suitflag != "Y") {//子套菜不打印做法合计
        painter->drawText(rect6, Qt::AlignVCenter | Qt::AlignRight, QString("%1%2")
                          .arg(tablemodel->get_cey_sys_parameter_value("pay_unit_prefix").toString())
                          .arg(lds::septNumber(tablemodel->model_data_NIBOLAN(row, "num_price_add"), tablemodel->get_cey_sys_parameter_value("bp_fmt_amount_dec", 2).toInt()))
                          );
    }
    //折叠

    if(ch_suitflag == "P") {
        QPixmap p;
        if(tablemodel->row_fold_map.value(row, tablemodel->get_cey_sys_parameter_value("qt_Y_dish_fold") == "1")){
            p =  QPixmap(":/image/widgets_pic/xxu.png");
        } else {
            p =  QPixmap(":/image/widgets_pic/xxd.png");
        }
        painter->drawPixmap(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignBottom | Qt::AlignRight, p.size(), rect7), p);
    }
}

QColor fexpandmain_tableview::fexpandmain_delegate::getCurNameColor(const QString &ch_suitflag) const
{
    return ch_suitflag == "Y" ? *nameSubColor : *nameColor;
}
