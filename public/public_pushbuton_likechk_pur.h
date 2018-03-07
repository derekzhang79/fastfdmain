#ifndef PUBLIC_PUSHBUTON_LIKECHK_PUR_H
#define PUBLIC_PUSHBUTON_LIKECHK_PUR_H

#include <QPushButton>

class public_pushbuton_likechk_pur : public QPushButton
{
public:
    public_pushbuton_likechk_pur(QWidget *parent = 0);
    void setChecked1(bool f);
protected:
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
};

#endif // PUBLIC_PUSHBUTON_LIKECHK_PUR_H
