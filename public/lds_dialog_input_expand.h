#ifndef Lds_DIALOG_INPUT_EXPAND_H
#define Lds_DIALOG_INPUT_EXPAND_H

#include <QDialog>
#include <QStringList>
class QFile;

namespace Ui {
class lds_dialog_input_expand;
}

class lds_dialog_input_expand : public QDialog
{
    Q_OBJECT

public:
    explicit lds_dialog_input_expand(QWidget *parent = 0);
    ~lds_dialog_input_expand();
    QString retreason;

private slots:
    void tosub();
    void toadd();
    void took();
    void tocancel();

    void updatereason();
    void savereason();
private:
    Ui::lds_dialog_input_expand *ui;
    QStringList defreasons;
    QFile *file;
};

#endif // Lds_DIALOG_INPUT_EXPAND_H
