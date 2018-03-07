#ifndef QCHECKBOXLONGPRESS_H
#define QCHECKBOXLONGPRESS_H

#include <QCheckBox>
#include "lds_bool.h"
#include <QPointer>
#include "lds_record_do_longpress.h"

class QLabel;
class QTimeLine;
class QEventLoop;

class QCheckBoxLongPress : public QCheckBox
{
    Q_OBJECT
public:
    QCheckBoxLongPress(QWidget *parent = 0);
    void enableLongPress(bool enabled);
signals:
    void timeout();
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
private:
    lds_record_do_longpress *do_longpress;
};

#endif // QCHECKBOXLONGPRESS_H
