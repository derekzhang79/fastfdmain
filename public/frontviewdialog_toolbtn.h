#ifndef FRONTVIEWDIALOG_TOOLBTN_H
#define FRONTVIEWDIALOG_TOOLBTN_H

#include <QRadioButton>
class QStylePainter;
//is_muti_draw_self，可以绘制自定义的图片
class frontViewDialog_toolbtn : public QRadioButton
{
    Q_OBJECT
public:
    explicit frontViewDialog_toolbtn(QWidget *parent = 0);
    void set_btn_property(const QString &property);
    void setNum(float n);
    float getNum();
public:
    QString index;
    bool is_use_pixmap;
    QColor presscolor;
    bool isdollar;
signals:
    void thisclicked(const QString &sqr);
    void thispressed(frontViewDialog_toolbtn *rb);
    void thispressed(const QString &sqr);
protected:
    void mousePressEvent(QMouseEvent *e);
    void mouseDoubleClickEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

    void paintEvent(QPaintEvent *e);
private:
    QIcon _icon;
    QSize _icon_size;
    QString _text;
    float num;
    void setPenColor(QStylePainter *p, QColor c);

    QString pay_unit_prefix;
};

#endif // FRONTVIEWDIALOG_TOOLBTN_H
