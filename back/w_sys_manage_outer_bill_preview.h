#ifndef W_SYS_MANAGE_OUTER_BILL_PREVIEW_H
#define W_SYS_MANAGE_OUTER_BILL_PREVIEW_H

#include <QDialog>
#include <QModelIndex>
#include "lds_model_sqltablemodel.h"
#include <QDataWidgetMapper>
#include "lds_query.h"
#include "lds_model_mapwidget_vertical.h"

namespace Ui {
class w_sys_manage_outer_bill_preview;
}

class w_sys_manage_outer_bill_preview : public lds_model_mapwidget_vertical
{
    Q_OBJECT
public:
    explicit w_sys_manage_outer_bill_preview(QWidget *parent = 0);
    w_sys_manage_outer_bill_preview(QString quick_input);
    ~w_sys_manage_outer_bill_preview();

    static void if_bill_empty_import_58_def();
public:
    bool billexport(QWidget *parent);
    bool billimport(QWidget *parent);
    static bool import(const QString &filepath);
public slots:
    void took();
    void tocancel();
    void toupdateSection(const QModelIndex &index);
    void togetlogo();
    void tobillexport();
    void tobillimport();
    void to_rb_input();


    void toHideExSet();
    void toShowExSet();
protected:
    virtual bool insert_sql_objectname(const QString &objectname);

private:
    bool saveData();
private:
    Ui::w_sys_manage_outer_bill_preview *ui;
};

#endif // W_SYS_MANAGE_OUTER_BILL_PREVIEW_H
