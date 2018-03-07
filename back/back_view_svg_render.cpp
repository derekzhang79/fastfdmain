#include "back_view_svg_render.h"

back_view_svg_render::back_view_svg_render()
{
}

bool back_view_svg_render::svg_render_by_rect(QSvgRenderer *render, QPainter *p, const QRectF &r)
{
    return svg_render_by_rect(render, p, r, QRectF(0,0, p->device()->width(), p->device()->height()));
}

bool back_view_svg_render::svg_render_by_rect(QSvgRenderer *render, QPainter *p, const QRectF &r, const QRectF &rectp)
{
    if(render->isValid()) {
        QRectF rect2 = r;

        QRectF rectd = QRectF(rectp.topLeft(), render->defaultSize());

        QRectF rect1 = QRectF(-rect2.x()*rectd.width()/rect2.width(),

                              -rect2.y()*rectd.height()/rect2.height(),

                              (rectd.width()*rectp.width())/rect2.width(),

                              (rectd.height()*rectp.height())/rect2.height()

                              );

        render->setViewBox(rect1);
        render->render(p, QRectF(0,0,rectp.width(), rectp.height()));
        return true;
    }
    return false;
}
