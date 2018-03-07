#ifndef W_SYS_MANAGE_OUTER_SETTINGS_H
#define W_SYS_MANAGE_OUTER_SETTINGS_H

#include <QDialog>
#include <QModelIndex>
#include <QSet>
#include "lds_dialog.h"
#include <QTimer>

class lds_model_sqltablemodel;
class QStandardItemModel;
class ElectronicCountPrice;
class ftableview_standmodel_sql_none;
namespace Ui {
class w_sys_manage_outer_settings;
}

class w_sys_manage_outer_settings : public lds_Dialog
{
    Q_OBJECT

public:
    explicit w_sys_manage_outer_settings(QWidget *parent = 0, int fisrtIndex = 0);
    ~w_sys_manage_outer_settings();
    virtual void retranslateView();
private slots:
    void toupdateSection(const QModelIndex &index);
    virtual void took();
    void tocancel();
    void tobillset();
    void todrawerdef();
    void todrawertest();
    void toprintermanager();
    void tokexiantest();
    void toshow_kexianruninfo(int index);
    void tofupingreset();
    void tofuping_adv();
    void tofuping_dish();
    void tojijia_test();

    void towxsavestart(bool f);
    void towxdel();
    void towxdel2();
    void towxnew();
    void towxnew2();
    void towxinit();
    void towxstartjz();
    void towxclose();
    void towxsave();

    void tolabel_print();
    void tolabel_print_naicai_test();
    void tobar_scale();

    void toimport();

    void tomobile_connect_max_value();
    void tofontapp();
    void toupdatefontexample(const QString &font);

    void toSaaSCheck(int index);
    void toSaaSCheck(bool check);

    void tocolorPicker();

    void to_read_tel_test(); //电话盒
    void to_read_tel_readdata();

    void toLabelDishCheckUn();
    void toLabelDishCheckAll();

    void tofuping_4_3cun(int index);
    void tofuping_screen_index(int index);
private:
    QTimer read_tel_timer;
public:
    static void SYS_FONT_APP_byconf();
protected:
    bool saveData(int index, bool issave = false);
    void setEditText(QComboBox *com, const QString &editText);
private:
    QSet<int> index_set;
    ElectronicCountPrice *countprice_scale;

    QStandardItemModel *wx_model;
    QStandardItemModel *wx_model_2;
    QString wx_dir;
    void wx_init_jizhan();
    void wx_init_diancaiji();
    bool wxsavejizhan();
    bool wxsavediancaiji();

    enum modelType{
        model_normal_print = 0,
        model_kitchen_print,
        model_label_print,
        model_screen_print,
        model_price_scale,

        model_bar_scale,
        model_wireless_machine,
        model_tablet,
        model_font,
    };
    QMap<modelType, int> map_view_stack;
public:
    Ui::w_sys_manage_outer_settings *ui;
    ftableview_standmodel_sql_none * backlist_model;
    lds_model_sqltablemodel *tablemodel_dish_subtype;
};

#endif // W_SYS_MANAGE_OUTER_SETTINGS_H
