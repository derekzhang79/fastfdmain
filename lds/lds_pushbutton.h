#ifndef LDS_PUSHBUTTON_H
#define LDS_PUSHBUTTON_H

#include <QPushButton>
class QFocusEvent;
class lds_pushbutton : public QPushButton
{
    Q_OBJECT
public:
    explicit lds_pushbutton(QWidget *parent = 0);
    QWidget *_widget;
signals:
    void signalfocusin();
    void signalfocusout();
protected:
    void focusInEvent(QFocusEvent *e);
    void focusOutEvent(QFocusEvent *e);
    bool eventFilter(QObject */*o*/, QEvent *e);

};

#endif // LDS_PUSHBUTTON_H
