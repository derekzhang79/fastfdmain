#ifndef LDS_CALENDARWIDGET_H
#define LDS_CALENDARWIDGET_H

#include <QDialog>
#include <QDateTime>
class QDateTimeEdit;
namespace Ui {
class lds_calendarwidget;
}

class lds_calendarwidget : public QDialog
{
    Q_OBJECT

public:
    explicit lds_calendarwidget(QDateTimeEdit *_dtedit, QWidget *parent = 0);
    ~lds_calendarwidget();

    void resetFocus();
    void updateEnable();
public slots:
    void setDateTime(const QDateTime &dt);
private:
    QDateTime dateTime();
private slots:
    void set_num();
private:
    Ui::lds_calendarwidget *ui;
    QDateTimeEdit *_dtedit;
};

#endif // LDS_CALENDARWIDGET_H
