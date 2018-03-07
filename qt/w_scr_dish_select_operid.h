#ifndef W_SCR_DISH_SELECT_OPERID_H
#define W_SCR_DISH_SELECT_OPERID_H

#include <QDialog>
#include "ui_w_scr_dish_special_cook.h"

class w_scr_dish_select_operid : public QDialog
{
    Q_OBJECT
public:
    explicit w_scr_dish_select_operid(QWidget *parent);
    ~w_scr_dish_select_operid();
    void setOperid(const QString &id);
    QString selectOperid();
private slots:
    void took();
    void toexit();
private:
    QStringList hasSelected();
    QStringList typenolist;
    Ui::w_scr_dish_special_cook *ui;
};

#endif // W_SCR_DISH_SELECT_OPERID_H
