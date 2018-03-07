#include "public_pushbuton_likechk_pur.h"

public_pushbuton_likechk_pur::public_pushbuton_likechk_pur(QWidget *parent)
    : QPushButton(parent)
{
}

void public_pushbuton_likechk_pur::setChecked1(bool f)
{
    this->setDown(f);
}

void public_pushbuton_likechk_pur::mousePressEvent(QMouseEvent *e)
{
    this->setDown(!isDown());
}

void public_pushbuton_likechk_pur::mouseReleaseEvent(QMouseEvent *e)
{
}

void public_pushbuton_likechk_pur::mouseMoveEvent(QMouseEvent *e)
{

}
