#include "lds_roundeddialog_title_frame.h"
#include <QPainter>
#include "lds.h"
#include "saas_pay_detail.h"

lds_roundeddialog_title_frame::lds_roundeddialog_title_frame(QWidget *parent)
    : QFrame(parent){
}

void lds_roundeddialog_title_frame::paintEvent(QPaintEvent *e)
{
    QFrame::paintEvent(e);

    QPainter p(this);
    QRect rect0 = this->rect();
    //top left
    saas_pay_detail::paintLeftTop(this, rect0, &p);
}
