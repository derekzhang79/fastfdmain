#include "public_toolbuton_wrap_v.h"

public_toolbuton_wrap_v::public_toolbuton_wrap_v(QWidget *parent) :
    public_toolbuton_wrap(parent)
{
    xp1 = 0;
    yp1 = 30;
    xp2 = 0;
    yp2 = 0;
    textflags =int(Qt::TextWrapAnywhere|Qt::AlignCenter);
    textfont_size = 11;
}
