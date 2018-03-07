#include "pushbutton_underline.h"

pushbutton_underline::pushbutton_underline(QWidget *parent) : lds_pushbutton_wrap(parent)
{
    this->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    this->setStyleSheet("QPushButton{color:#ff650b;text-decoration: underline;}"
                        "QPushButton:disabled{color:gray;}");
    this->setFlat(true);
    this->setFocusPolicy(Qt::NoFocus);

    xp1 = 0;
    yp1 = 3;
    xp2 = -3;
    yp2 = -3;
    textflags = int(Qt::AlignLeft|Qt::AlignVCenter|Qt::TextWordWrap);
    b_enable_frame = false;
}

