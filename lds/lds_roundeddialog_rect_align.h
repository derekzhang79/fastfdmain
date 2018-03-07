#ifndef LDS_ROUNDEDDIALOG_RECT_ALIGN_H
#define LDS_ROUNDEDDIALOG_RECT_ALIGN_H

#include <QDialog>
#include "lds_dialog_drog.h"

class QVBoxLayout;
class QLineEdit;
class lds_roundeddialog_title;

class lds_roundeddialog_rect_align : public lds_dialog_drog
{
public:

    Q_OBJECT
    Q_PROPERTY(QColor bgColor READ getbgColor WRITE setbgColor DESIGNABLE true)
    Q_PROPERTY(QColor titleColor READ gettitleColor WRITE settitleColor DESIGNABLE true)
public:
    void setbgColor(const QColor &color);
    QColor getbgColor() const;
    QColor bgColor;

    void settitleColor(const QColor &color);
    QColor gettitleColor() const;
    QColor titleColor;

    void setbgPixmap(const QPixmap &pixmap);
    QPixmap getbgPixmap();
    QPixmap bgPixmap;
public:
    lds_roundeddialog_rect_align(QDialog *buddy, bool show_title=true, bool clike_empty_reject = false);
    ~lds_roundeddialog_rect_align();

    void setTitle(const QString &title);
    void disableAutoDestory();
    void enable_clike_empty_reject();

    static int titleHeight;
    QDialog *get_buddy();
    void buddyResize();
public Q_SLOTS:
    int exec();
protected:
    void paintEvent(QPaintEvent *e);
    bool eventFilter(QObject *o, QEvent *e);
    void showEvent(QShowEvent *e);
    void resizeEvent(QResizeEvent *e);

private slots:
    void todone(int result);
    void toBuddyResize();
private:
    void printerPath(QPainter *painter, const QList<QPoint> &pointlist, const QColor &color);

    QDialog *buddy;
    bool _show_title;

    QWidget *old_parent;
    Qt::WindowFlags old_w_flags;

    lds_roundeddialog_title *label_title;

    bool isdestroy;
    bool clike_empty_reject;
protected:
    void keyPressEvent(QKeyEvent *e);
    void  mousePressEvent(QMouseEvent *e);
};

#endif // LDS_ROUNDEDDIALOG_RECT_ALIGN_H
