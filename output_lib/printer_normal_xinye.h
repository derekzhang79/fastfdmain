#ifndef PRINTER_NORMAL_XINYE_H
#define PRINTER_NORMAL_XINYE_H

#include "printer_normal.h"


class  Printer_Normal_XINYE  : public Printer_Normal
{
public:
    Printer_Normal_XINYE();
    virtual blockIns tran(const blockIns &bins, int xinye_beep_count);
    virtual  QString name();
};

Q_DECLARE_METATYPE(Printer_Normal_XINYE)

#endif // PRINTER_NORMAL_XINYE_H
