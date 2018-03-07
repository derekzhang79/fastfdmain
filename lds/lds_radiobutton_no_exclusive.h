#ifndef LDS_RADIOBUTTON_NO_EXCLUSIVE_H
#define LDS_RADIOBUTTON_NO_EXCLUSIVE_H
#include <QRadioButton>

class lds_radiobutton_no_exclusive : public QRadioButton
{
    Q_OBJECT
public:
    lds_radiobutton_no_exclusive(QWidget *parent = 0);
signals:
    void signal_checked();
public slots:
    void tochecked();
    void tounchecked();
    void tocheckstate(bool f);
};

#endif // LDS_RADIOBUTTON_NO_EXCLUSIVE_H
