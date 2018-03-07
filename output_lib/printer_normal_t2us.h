#ifndef PRINTER_NORMAL_T2US_H
#define PRINTER_NORMAL_T2US_H

#include "printer_normal.h"

class  Printer_Normal_T2US : public Printer_Normal
{
public:
    Printer_Normal_T2US();
    virtual ~Printer_Normal_T2US();
    QString vidpid;
    virtual blockIns tran(const blockIns &bins, int xinye_beep);
    virtual QString name();
};
Q_DECLARE_METATYPE(Printer_Normal_T2US)

#endif // PRINTER_NORMAL_T2US_H
