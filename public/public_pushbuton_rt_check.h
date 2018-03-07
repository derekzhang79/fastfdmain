#ifndef PUBLIC_PUSHBUTON_RT_CHECK_H
#define PUBLIC_PUSHBUTON_RT_CHECK_H

#include "lds_toolbar_button.h"
#include "fexpandmain_model_sqltablemodel.h"
class public_pushbuton_rt_check : public lds_toolbar_button
{
    Q_OBJECT
public:
    enum checkState{
        none = 0,
        info,
        yunm,
        warn,
    };

public:
    explicit public_pushbuton_rt_check(QWidget *parent = 0);
    void set_rt_checked(const vch_membernoData &m);
    void set_rt_checked(bool f);
    void set_rt_checked_warn(bool f);
    void paintEvent(QPaintEvent *e);
private:
    QString get_m_belong_state();
    QString m_belong_state;
    checkState  _state;
};

#endif // PUBLIC_PUSHBUTON_RT_CHECK_H
