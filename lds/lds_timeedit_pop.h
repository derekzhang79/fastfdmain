#ifndef LDS_TIMEEDIT_POP_H
#define LDS_TIMEEDIT_POP_H

#include <QDialog>
#include <QTime>

namespace Ui {
class lds_timeEdit_pop;
}

class lds_timeEdit_pop : public QDialog
{
    Q_OBJECT

public:
    explicit lds_timeEdit_pop(QWidget *parent = 0);
    ~lds_timeEdit_pop();
    void setTime(const QTime &time);
    QTime getTime();
private slots:
    void set_num();
private:
    Ui::lds_timeEdit_pop *ui;
    QTime rettime;
};

#endif // LDS_TIMEEDIT_POP_H
