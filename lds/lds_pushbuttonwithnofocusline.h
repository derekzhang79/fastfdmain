#ifndef LDS_PUSHBUTTONWITHNOFOCUSLINE_H
#define LDS_PUSHBUTTONWITHNOFOCUSLINE_H

#include <QPushButton>
class lds_pushbuttonwithnofocusline : public QPushButton
{
public:
    lds_pushbuttonwithnofocusline(QWidget *parent = 0);
    QWidget *_widget;
protected:
    bool eventFilter(QObject */*o*/, QEvent *e);
};

#endif // LDS_PUSHBUTTONWITHNOFOCUSLINE_H
