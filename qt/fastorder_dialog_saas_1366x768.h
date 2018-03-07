#ifndef FASTORDER_DIALOG_SAAS_1366X768_H
#define FASTORDER_DIALOG_SAAS_1366X768_H

#include <QDialog>

namespace Ui {
class fastorder_dialog_saas_1366x768;
}

class fastorder_dialog_saas_1366x768 : public QDialog
{
    Q_OBJECT

public:
    explicit fastorder_dialog_saas_1366x768(QWidget *parent = 0);
    ~fastorder_dialog_saas_1366x768();

private:
    Ui::fastorder_dialog_saas_1366x768 *ui;
};

#endif // FASTORDER_DIALOG_SAAS_1366X768_H
