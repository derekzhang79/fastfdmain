#ifndef BANK_CARD_H
#define BANK_CARD_H

#include <QDialog>
//其他收银操作
namespace Ui {
class bank_card;
}

class bank_card : public QDialog
{
    Q_OBJECT
    
public:
    explicit bank_card(const QString & num,QWidget *parent);
    ~bank_card();
    void init_control();
    bool btn_t_f;
    double shishoujine;
    QString pingzhenghao;
    QString vch_memeo;
    QString time;
private slots:
    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void todelayfocus();
private:
    Ui::bank_card *ui;
    QString _num;
};

#endif // BANK_CARD_H
