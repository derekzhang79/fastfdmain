#ifndef W_SYS_MANAGE_OPERATORRIGHT_H
#define W_SYS_MANAGE_OPERATORRIGHT_H

#include <QDialog>
#include "w_sys_manage.h"
#include "w_rpt_report_frame.h"

class Ui_w_sys_manage_operatorright_Dialog;
class QSqlQueryModel;
class QStandardItemModel;
class Ui_w_bt_dish_unitset_Dialog;

class w_sys_manage_operatorright : public QDialog
{
    Q_OBJECT
public:    
    explicit w_sys_manage_operatorright(QWidget *parent = 0);

    QSqlQueryModel *querymodel;
    Ui_w_sys_manage_operatorright_Dialog *ui;

public slots:
    void torefresh();
private slots:
    void toSelectAll();
    void toselectno();
    void todishright();
    void torightcopy();
    void topwdchange();
    void tosave();
    void toexit();
    void torightmodelupdate();
    void torightfind(const QString &text);
private:

    QStandardItemModel *standmodel;
};

//权限copy
class Ui_w_sys_manage_operatorright_rightcopy_Dialog;
class w_sys_manage_operatorright_rightcopy : public QDialog
{
    Q_OBJECT
public:
    explicit w_sys_manage_operatorright_rightcopy(w_sys_manage_operatorright *parent = 0);
private slots:
    void tocopy();
    void toexit();
private:
    Ui_w_sys_manage_operatorright_rightcopy_Dialog *ui;
    w_sys_manage_operatorright *_parent;
    report_querymodel *querymodel;
    QString vch_operidfrom;
};

//密码修改
class Ui_w_m_member_pwdchange_Dialog;
class w_sys_manage_operatorright_pwdchange : public QDialog{
    Q_OBJECT
public:
    explicit w_sys_manage_operatorright_pwdchange(w_sys_manage_operatorright *parent = 0);

    Ui_w_m_member_pwdchange_Dialog *ui;
private slots:
    void took();
    void toexit();
private:
};
//菜品大类权限
class w_sys_manage_operatorright_dish_type_right : public QDialog{
    Q_OBJECT
public:
    explicit w_sys_manage_operatorright_dish_type_right(const QString &vch_operid, QWidget *parent = 0);
    ~w_sys_manage_operatorright_dish_type_right();
private slots:
    void tonew();
    void todel();
    void took();
    void tocancel();
    void refresh();
private:
    lds_tableView *tableview;
    lds_ComboBox *combobox;
    lds_toolbar_button *pushbutton_new;
    lds_toolbar_button *pushbutton_del;
    QStandardItemModel *standmodel;
    QLabel *label_desc;
};

class w_sys_manage_operatorright_dish_type_right_new_type : public QDialog{
    Q_OBJECT
public:
    explicit w_sys_manage_operatorright_dish_type_right_new_type(const QString &vch_operid, QWidget *parent = 0);
    ~w_sys_manage_operatorright_dish_type_right_new_type();
    QStringList  get_dish_type_list();
private:
    Ui_w_bt_dish_unitset_Dialog *ui;
    report_querymodel *querymodel;
};

#endif // W_SYS_MANAGE_OPERATORRIGHT_H
