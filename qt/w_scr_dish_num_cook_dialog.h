#ifndef W_SCR_DISH_NUM_COOK_DIALOG_H
#define W_SCR_DISH_NUM_COOK_DIALOG_H

#include <QDialog>
#include <QStandardItemModel>

#include <QTextEdit>

class frontViewDialog_toolbtn;
class fexpandmain_model_sqltablemodel;
class fexpandmain_model_sqltablemodel_data;
namespace Ui {
class w_scr_dish_num_cook_Dialog;
}

class w_scr_dish_num_cook_Dialog : public QDialog
{
    Q_OBJECT
public:
    static void static_to_present(QWidget *parent,
                                  fexpandmain_model_sqltablemodel *tablemodel,
                                  int row,
                                  float num_num);
public:
    explicit w_scr_dish_num_cook_Dialog(QWidget *parent);
    w_scr_dish_num_cook_Dialog(const fexpandmain_model_sqltablemodel_data *model, int row, QWidget *parent);

    ~w_scr_dish_num_cook_Dialog();
    void retranslateView();


    void commitModel(fexpandmain_model_sqltablemodel *frommodel, int row);

    void intData();
    void refresh(const fexpandmain_model_sqltablemodel_data *model, int row);
    double get_cur_dish_num();
protected:
    void refresh_sub(int row);//_frommodel
    void enable_cook(bool f) const;
private slots:
    void toallcook();
    void toclearcook();
    void took();
    void tocancel();
    void topresent();
    void totakeweight();
    void tocookset();
    void tomore();

    void toupdatelabelreturn();
    void toupdatelabelreturn(frontViewDialog_toolbtn *rb);
    void togenaratcook(const QString &no);
private:
    Ui::w_scr_dish_num_cook_Dialog *ui;
    fexpandmain_model_sqltablemodel *tablemodel;
    const fexpandmain_model_sqltablemodel_data *frommodel;
    QTextEdit *label_info;

    /////////
protected:
    bool eventFilter(QObject *o, QEvent *e);
};

#endif // W_SCR_DISH_NUM_COOK_DIALOG_H
