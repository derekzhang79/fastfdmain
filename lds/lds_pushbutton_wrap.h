#ifndef LDS_PUSHBUTTON_WRAP_H
#define LDS_PUSHBUTTON_WRAP_H

#include <QPushButton>
#include "lds_bool.h"
#include <QMap>
#include <QVariant>
#include "lds_record_do_longpress.h"

class lds_pushbutton_wrap : public QPushButton
{
    Q_OBJECT
public:
    explicit lds_pushbutton_wrap(QWidget *parent = 0);
    virtual ~lds_pushbutton_wrap();
    void paintEvent(QPaintEvent *);
    void show_flag_top_right(bool f);//云端未上传时,会有用
    void show_flag_bottom_right(bool f, const QColor &show_bottom_right_color = QColor("#0070a1"));//云端未上传时,会有用
    void show_text_right_bottom(const QString &mainColor);//左上角/右下角
    void show_right_pixmap(const QPixmap &right_pixmap);
    int textflags;
    int xp1, yp1, xp2, yp2;


    void disableFrame();
    lds_bool_default_true b_enable_frame;
protected:
    lds_bool_default_false show_top_right;
    lds_bool_default_false show_bottom_right;
    lds_bool_default_false b_text_right_bottom;
    QColor show_bottom_right_color;
    QPixmap right_pixmap;
    /////////////////
public:
    QVariant getData(int role, const QVariant &def = QVariant());
    void setData(int role, const QVariant &d);
private:
    QMap<int, QVariant> role_value_map;

    /////////////////////////////////////
public:
    void enableLongPress(bool flag);
signals:
    void timeout();
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
protected:
    lds_record_do_longpress *do_longpress;
};

#endif // LDS_PUSHBUTTON_WRAP_H
