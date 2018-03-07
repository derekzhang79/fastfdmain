#ifndef PRINTER_NORMAL_KF58_H
#define PRINTER_NORMAL_KF58_H

#include "printer_normal.h"

class  Printer_Normal_KF58 : public Printer_Normal
{
public:
    Printer_Normal_KF58();
    virtual ~Printer_Normal_KF58();
    virtual blockIns tran(const blockIns &bins, int xinye_beep);
    virtual QString name();
};
Q_DECLARE_METATYPE(Printer_Normal_KF58)

#endif // PRINTER_NORMAL_KF58_H
