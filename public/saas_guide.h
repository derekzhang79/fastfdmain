#ifndef SAAS_GUIDE_H
#define SAAS_GUIDE_H

#include <QDialog>
#include <QItemDelegate>
#include <QStandardItemModel>
#include "w_sys_manage_cloudsync_transfer.h"
#include "lds_messagebox.h"
class QState;
class QLineEdit;

class saas_guide : public QDialog
{
    Q_OBJECT

public:
    explicit saas_guide(QWidget *parent = 0);
    ~saas_guide();

    static cJSONHttpData getTerminal(QWidget *parent);
};



#endif // SAAS_GUIDE_H
