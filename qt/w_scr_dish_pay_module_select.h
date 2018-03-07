#ifndef W_SCR_DISH_PAY_MODULE_SELECT_H
#define W_SCR_DISH_PAY_MODULE_SELECT_H

#include <QDialog>
#include <QModelIndex>

namespace Ui {
class w_scr_dish_pay_module_select;
}

class w_scr_dish_pay_module_select : public QDialog
{
    Q_OBJECT
public:
    enum moduleType{
        module_name = 0,
        modeule_key,
    };
    enum DialogType{
        dialog_remove = Accepted + 1,
        dialog_default = Accepted + 2
    };

public:
    explicit w_scr_dish_pay_module_select(const QStringList &cur_select_items, int operatMode, QWidget *parent = 0);
    ~w_scr_dish_pay_module_select();
    static QList<QPair<QString, QString> >   &get_pay_module_list(int operatMode);
    static void pay_module_list_clear();
    static QString   pay_module_list_value(const QString &key, int operatMode);
    static int   pay_module_list_index(const QString &key, int operatMode);
    QString get_select_module_key();

    static void clearCache();
private slots:
    void took();
    void tocancel();
    void tocheckremove(const QModelIndex &index);
    void todefault();
private:
    Ui::w_scr_dish_pay_module_select *ui;
    static QList<QPair<QString, QString> >  pay_module_list;
    QString select_module_key;
};

#endif // W_SCR_DISH_PAY_MODULE_SELECT_H
