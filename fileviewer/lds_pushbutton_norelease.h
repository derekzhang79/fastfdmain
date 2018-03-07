#ifndef LDS_PUSHBUTTON_NORELEASE_H
#define LDS_PUSHBUTTON_NORELEASE_H

#include <QPushButton>

class lds_pushbutton_norelease : public QPushButton
{
    Q_OBJECT
public:
    explicit lds_pushbutton_norelease(QWidget *parent = 0);

    void mouseReleaseEvent(QMouseEvent *e);


};

#endif // LDS_PUSHBUTTON_NORELEASE_H
