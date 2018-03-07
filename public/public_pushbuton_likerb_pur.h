#ifndef PUBLIC_PUSHBUTON_LIKERB_PUR_H
#define PUBLIC_PUSHBUTON_LIKERB_PUR_H

#include <QPushButton>
#include "lds_record_do_longpress.h"

class public_pushbuton_likerb_pur : public QPushButton
{
    Q_OBJECT
public:
    explicit public_pushbuton_likerb_pur(QWidget *parent = 0);

    void setChecked1(bool f);
    void setExclusive(bool isexclusive);
    void enableLongPress(bool flag);
signals:
    void signalpressed(bool execPress);
    void timeout();
protected:
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);

    void focusInEvent(QFocusEvent *e);
    void focusOutEvent(QFocusEvent *e);

private:
    void scan_all_except_self();
    bool isexclusive;
    lds_record_do_longpress *do_longpress;
};

class public_pushbuton_likerb_pur_auto_release {
public:
    public_pushbuton_likerb_pur_auto_release(public_pushbuton_likerb_pur *b);
    ~public_pushbuton_likerb_pur_auto_release();

    public_pushbuton_likerb_pur *rb() {return b;}
    static public_pushbuton_likerb_pur *current_checked_rb(QWidget *parent);
    public_pushbuton_likerb_pur *b;
};

class public_pushbuton_likerb_pur_unexclusive : public public_pushbuton_likerb_pur {

    Q_OBJECT
public:
    explicit public_pushbuton_likerb_pur_unexclusive(QWidget *parent = 0);
};

#endif // PUBLIC_PUSHBUTON_LIKERB_PUR_H
