#ifndef PRINTER_NORMAL_58_H
#define PRINTER_NORMAL_58_H

#include "printer_normal.h"

class Printer_Normal_58 : public Printer_Normal
{
public:
    Printer_Normal_58();
    virtual ~Printer_Normal_58();
    virtual blockIns tran(const blockIns &bins, int xinye_beep);
    virtual QString name();
};

Q_DECLARE_METATYPE(Printer_Normal_58)

#endif // PRINTER_NORMAL_58_H
