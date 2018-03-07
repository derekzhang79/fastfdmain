#ifndef PRINTER_NORMAL_80_H
#define PRINTER_NORMAL_80_H

#include "printer_normal.h"

class  Printer_Normal_80 : public Printer_Normal
{
public:
    Printer_Normal_80();
    virtual ~Printer_Normal_80();
    virtual blockIns tran(const blockIns &bins, int xinye_beep);
    virtual QString name();
};
Q_DECLARE_METATYPE(Printer_Normal_80)

#endif // PRINTER_NORMAL_80_H
