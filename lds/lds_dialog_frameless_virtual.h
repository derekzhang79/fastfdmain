#ifndef LDS_DIALOG_FRAMELESS_VIRTUAL_H
#define LDS_DIALOG_FRAMELESS_VIRTUAL_H

#include <QDialog>
class QLabel;
class lds_dialog_frameless_virtual : public QDialog
{
    Q_OBJECT
public:
    explicit lds_dialog_frameless_virtual(QWidget *parent = 0);
    void setWindowTitle(const QString &title);
    QLabel *label_title;
};

#endif // LDS_DIALOG_FRAMELESS_VIRTUAL_H
