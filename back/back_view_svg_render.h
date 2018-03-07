#ifndef BACK_VIEW_SVG_RENDER_H
#define BACK_VIEW_SVG_RENDER_H

#include <QRectF>
#include <QSvgRenderer>
#include <QPainter>

class back_view_svg_render
{
public:
    back_view_svg_render();
    static bool svg_render_by_rect(QSvgRenderer *render, QPainter *p, const QRectF &r);
    static bool svg_render_by_rect(QSvgRenderer *render, QPainter *p, const QRectF &r, const QRectF &rectp);

};

#endif // BACK_VIEW_SVG_RENDER_H
