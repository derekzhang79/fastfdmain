#include "public_pushbutton_numkey.h"
#include "lds.h"
#include <QFontDatabase>
#include <QtDebug>

public_pushbutton_numkey::public_pushbutton_numkey(QWidget *parent) : QPushButton(parent)
{
    QFont font = this->font();
    font.setFamily(lds::FONT_NUM_Family);
    font.setPointSize(17);
    font.setBold(true);
    this->setFont(font);
}

