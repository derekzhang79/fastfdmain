#include "table_management_desk.h"
#include "math.h"
#include "table_management_pad.h"
#include "table_management_graphicsview.h"
#include <QGraphicsDropShadowEffect>
#include <QStyleOptionGraphicsItem>
#include "fexpandmain.h"
#include "n_func.h"
#include "restauranttableview.h"
#include <QtDebug>
#include "lds.h"

static QPointF get_pop_point(QSizeF popsize, QRectF box, QRectF screen)
{
    QPointF pos = box.bottomLeft();
    QPointF pos2 =  box.topRight();

    //        pos.setX(pos.x()-popsize.width());
    pos2.setX(pos2.x()-popsize.width());
    if (pos.x() < screen.left()) {
        pos.setX(qMax(pos.x(), screen.left()));
    } else if (pos.x()+popsize.width() > screen.right()) {
        pos.setX(qMax(pos.x()-popsize.width(), screen.right()-popsize.width()));
    }

    if (pos.y() + popsize.height() > screen.bottom()) {
        pos.setY(pos2.y() - popsize.height());
    } else if (pos.y() < screen.top()) {
        pos.setY(screen.top());
    }
    if (pos.y() < screen.top()) {
        pos.setY(screen.top());
    }
    if (pos.y()+popsize.height() > screen.bottom()) {
        pos.setY(screen.bottom()-popsize.height());
    }
    return pos;
}


int table_management_desk::scale_add_value = 20;
int table_management_desk::rotate_add_value = 10;
table_management_desk::table_management_desk(table_management_pad *pad, shapeType stype)
{
    this->pad = pad;
    setFlags(ItemIsMovable | ItemIsSelectable| ItemIsFocusable);
    QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect;
    effect->setYOffset(0);
    effect->setXOffset(0);
    effect->setBlurRadius(20);
    effect->setColor(QColor(0, 0, 0, 180));
#ifdef Q_OS_WIN
    setGraphicsEffect(effect);
#endif

    setData(item_hscale_min, 1);
    setData(item_vscale, 1);

    setData(item_rotate, 0);
    setData(item_boundingRect, QRectF(-50, -50, 100, 100));
    setData(item_ch_areano, QString());
    setData(item_ch_tableno, QString());

    setData(item_shapetype, stype);
    setData(item_pos, QPointF());

    textcolor = QColor("black");

    desk_preprint_map = 0;
}

table_management_desk::~table_management_desk()
{
}

QRectF table_management_desk::boundingRect() const
{
    return data(item_boundingRect).toRectF();
}

QPainterPath table_management_desk::shape() const
{
    qreal w = boundingRect().width();
    qreal h = boundingRect().height();
    qreal w_2 = w / 2;
    qreal h_2 = h / 2;
    QPainterPath path;
    switch(data(item_shapetype).toInt() ) {
    case shape_rectangle_min:
        path.addRoundedRect(QRectF(-w_2, -h_2, w, h), 5, 5);
        break;
    case shape_triangle:
    {
        qreal sqr30 = pow(3, 0.5);
        path.addPolygon(QPolygonF(
                            QVector<QPointF>()
                            << QPointF(0, -h_2)
                            << QPointF(  h_2/2 * sqr30, h_2/2)
                            << QPointF( -h_2/2 * sqr30, h_2/2)
                            << QPointF(0, -h_2)
                            ));
    }
        break;
    case shape_circle:
        path.addEllipse(QRectF(-w_2, -h_2, w, h));
        break;

    default:
        break;
    }

    return path;
}

void table_management_desk::paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget */*widget*/)
{
    const QPainterPath path = shape();
    const QRectF rect = path.boundingRect();
    if(pay_unit_prefix.isEmpty()) {
        pay_unit_prefix = n_func::f_get_sysparm("pay_unit_prefix", " ");
    }

    QFont f = painter->font();
    f.setPixelSize(rect.height() / 5);//高度
    painter->setFont(f);


    QMap<int, QString> ch_billno_map = restraurantDelegate::get_ch_billno_div_map(ch_billnos);
    bool desk_div = restraurantDelegate::ch_billno_is_div(ch_billno_map);
    QString desk_info;
    table_management_graphicsview *view = deskView;
    if(view->is_desk_show_code){
        desk_info = data(item_ch_tableno).toString();
    } else {
        desk_info = view->table_no_name_map_get().value(data(item_ch_tableno).toString());
    }
    //未开台
    if(ch_billno_map.isEmpty()) {
        if(!order_tableno.isEmpty()) {//预定
            paint_path(painter, path, QPixmap(":/image/desk_order.png"));
        } else {//未预定
            paint_path(painter, path, QPixmap(":/image/desk_unuse.png"));
        }
        //未清台
        if(div_bar.count() > 0) {
            restraurantDelegate::paintDensePattern(painter,
                                                   path,
                                                   QColor(restraurantDelegate::gettableState(tableState_BAR).colorname1),
                                                   Qt::DiagCrossPattern,
                                                   3,
                                                   true
                                                   );
        }
        paint_text(painter, rect.toRect(),  item, desk_info);
        return;
    }
    //未分单
    if(!desk_div) {
        bool isnext = true;
        QString ch_billno = ch_billno_map.value(0);//ch_billno
        if(isnext && desk_preprint_map && desk_preprint_map->value(ch_billno, false)) {
            isnext = false;
            paint_path(painter, path, QPixmap(":/image/desk_preprint.png"));
        }
        //最后
        if(isnext) {
            paint_path(painter, path, QPixmap(":/image/desk_inuse.png"));
        }
        //未清台
        if(div_bar.count() > 0) {
            restraurantDelegate::paintDensePattern(painter,
                                                   path,
                                                   QColor(restraurantDelegate::gettableState(tableState_BAR).colorname1),
                                                   Qt::DiagCrossPattern,
                                                   3,
                                                   true
                                                   );
        }
        paint_text(painter, rect.toRect(),  item, desk_info + "\n" +pay_unit_prefix + QString().sprintf("%.02f", num_cost));
        return;
    }

    //分单
    if(desk_div) {
        paint_path(painter, path, QPixmap(":/image/desk_inuse.png"));
        //未清台
        if(div_bar.count() > 0) {
            restraurantDelegate::paintDensePattern(painter,
                                                   path,
                                                   QColor(restraurantDelegate::gettableState(tableState_BAR).colorname1),
                                                   Qt::DiagCrossPattern,
                                                   3,
                                                   true
                                                   );
        }
        paint_text(painter, rect.toRect(),  item,  desk_info + "\n" +QString::number(ch_billno_map.count())+QObject::tr("订单"));
        return;
    }
}

int table_management_desk::type() const
{
    return Type;
}

void table_management_desk::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mousePressEvent(event);
    //
    if(deskView->press_event_do_double_clicked(this->data(item_ch_tableno).toString())) {
        deskView->emit_signal_table_double_clicked(this->data(item_ch_tableno).toString());
        return;
    }
    //
    ismove = false;
    pad->hide();
}

void table_management_desk::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseMoveEvent(event);
    ismove = true;
}

void table_management_desk::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseReleaseEvent(event);
    if(flags() & ItemIsMovable) {
        if(!ismove) {
            QRectF boundrect = boundingRect().translated(this->pos());//.adjusted(10, 10, -10, -10);
            this->setSelected(false);
            QPointF ppos = get_pop_point(pad->get_control_size(), boundrect, pad->boundingRect());
            pad->setPos(ppos +
                        QPointF(pad->get_control_size().width()/2,
                                pad->get_control_size().height()/2
                                ));
            pad->set_app_item(this);
            pad->setFocus();
            pad->show();
        }
        return;
    }
    deskView->emit_signal_table_clicked(this->data(item_ch_tableno ).toString());
}

void table_management_desk::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    deskView->emit_signal_table_double_clicked(this->data(item_ch_tableno ).toString());
    QGraphicsItem::mouseDoubleClickEvent(event);
}

void table_management_desk::focusOutEvent(QFocusEvent *event)
{
    QGraphicsItem::focusOutEvent(event);
    deskView->emit_signal_table_clicked("");
}

void table_management_desk::set_shapetype(table_management_desk::shapeType stype)
{
    prepareGeometryChange();
    setData(item_shapetype, stype);
}

table_management_desk::shapeType table_management_desk::get_shapetype()
{
    return (shapeType)data(item_shapetype).toInt();
}

void table_management_desk::update_boundrect(int stype, int add_sub_key)
{
    prepareGeometryChange();

    QRectF rect = data(item_boundingRect).toRectF();
    if(stype == item_hscale_min) {
        rect.adjust(-scale_add_value*add_sub_key, 0, scale_add_value*add_sub_key, 0);
    }
    if(stype == item_vscale) {
        rect.adjust(0, -scale_add_value*add_sub_key, 0, scale_add_value*add_sub_key);
    }
    setData(item_boundingRect, rect);
}

void table_management_desk::updaste_textcolor(QColor textcolor)
{
    this->textcolor = textcolor;
    update();
}

QString table_management_desk::get_text()
{
    return data(item_ch_tableno).toString();
}

void table_management_desk::paint_cell(const QString &ch_billnos, QMap<QString, bool> *desk_preprint_map, float num_cost, const QString &order_tableno, const QString &vch_tablename, const QString &div_bar)
{
    this->ch_billnos = ch_billnos;
    if(this->desk_preprint_map == 0) {
        this->desk_preprint_map = desk_preprint_map;
    }
    this->num_cost = num_cost;
    this->order_tableno = order_tableno;
    if(2 == lds::sysconf->value("system_setting/yingyemoshi", 0).toInt()) {//酒吧模式
        this->div_bar = div_bar;
    }


    deskView->table_no_name_map_insert(data(item_ch_tableno).toString(), vch_tablename);

    update();
}

void table_management_desk::paint_path(QPainter *painter, const QPainterPath &path, const QPixmap &pixmap)
{
    painter->setPen(QPen(QColor(0,0,0,100), 1));
    painter->setBrush(pixmap);
    painter->drawPath(path);
}

void table_management_desk::paint_text(QPainter *painter, const QRect &rect, const QStyleOptionGraphicsItem *item, const QString &text)
{
    if(item->state & QStyle::State_Selected) {
        painter->setPen(QColor("#ff650b"));
    } else {
        painter->setPen(textcolor);
    }
    painter->drawText(rect, Qt::AlignCenter, text);
}
