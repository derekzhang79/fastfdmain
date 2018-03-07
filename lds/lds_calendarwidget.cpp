#include "lds_calendarwidget.h"
#include "ui_lds_calendarwidget.h"
#include <QDateTimeEdit>
#include "lds.h"
#include <QKeyEvent>

lds_calendarwidget::lds_calendarwidget(QDateTimeEdit *_dtedit, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::lds_calendarwidget)
{
    ui->setupUi(this);
    this->_dtedit = _dtedit;

    connect(ui->pushButton_num0,SIGNAL(clicked()),this,SLOT(set_num()));
    connect(ui->pushButton_num1,SIGNAL(clicked()),this,SLOT(set_num()));
    connect(ui->pushButton_num2,SIGNAL(clicked()),this,SLOT(set_num()));
    connect(ui->pushButton_num3,SIGNAL(clicked()),this,SLOT(set_num()));
    connect(ui->pushButton_num4,SIGNAL(clicked()),this,SLOT(set_num()));
    connect(ui->pushButton_num5,SIGNAL(clicked()),this,SLOT(set_num()));
    connect(ui->pushButton_num6,SIGNAL(clicked()),this,SLOT(set_num()));
    connect(ui->pushButton_num7,SIGNAL(clicked()),this,SLOT(set_num()));
    connect(ui->pushButton_num8,SIGNAL(clicked()),this,SLOT(set_num()));
    connect(ui->pushButton_num9,SIGNAL(clicked()),this,SLOT(set_num()));
    connect(ui->pushButton_clear,SIGNAL(clicked()),this,SLOT(set_num()));
    connect(ui->pushButton_curyear,SIGNAL(clicked()),this,SLOT(set_num()));
    connect(ui->pushButton_curyear_1,SIGNAL(clicked()),this,SLOT(set_num()));
    connect(ui->pushButton_curyear_2,SIGNAL(clicked()),this,SLOT(set_num()));
    connect(ui->pushButton_today,SIGNAL(clicked()),this,SLOT(set_num()));
    connect(ui->pushButton_ok,SIGNAL(clicked()),this,SLOT(set_num()));
}

lds_calendarwidget::~lds_calendarwidget()
{
    delete ui;
}

void lds_calendarwidget::resetFocus()
{
    ui->lineEdit_year->setFocus();
}

void lds_calendarwidget::updateEnable()
{
    QString disf = _dtedit->displayFormat();
    lds::showWidget(ui->lineEdit_day, disf.contains("d"));ui->label_day->setHidden(ui->lineEdit_day->isHidden());
    lds::showWidget(ui->lineEdit_hour, disf.contains("h"));ui->label_hour->setHidden(ui->lineEdit_hour->isHidden());
    lds::showWidget(ui->lineEdit_minute, disf.contains("m"));ui->label_minute->setHidden(ui->lineEdit_minute->isHidden());

    lds::showWidget(ui->lineEdit_month, disf.contains("M"));ui->label_month->setHidden(ui->lineEdit_month->isHidden());
    lds::showWidget(ui->lineEdit_second, disf.contains("s"));ui->label_second->setHidden(ui->lineEdit_second->isHidden());
    lds::showWidget(ui->lineEdit_year, disf.contains("y"));ui->label_year->setHidden(ui->lineEdit_year->isHidden());
}

QDateTime lds_calendarwidget::dateTime()
{
    int y = lds::widgetIsShow(ui->lineEdit_year) ? ui->lineEdit_year->text().toInt():2000;
    int M = lds::widgetIsShow(ui->lineEdit_month) ? ui->lineEdit_month->text().toInt():1;
    int d = lds::widgetIsShow(ui->lineEdit_day) ? ui->lineEdit_day->text().toInt():1;
    int h = lds::widgetIsShow(ui->lineEdit_hour) ? ui->lineEdit_hour->text().toInt():0;
    int m = lds::widgetIsShow(ui->lineEdit_minute) ? ui->lineEdit_minute->text().toInt():0;
    int s = lds::widgetIsShow(ui->lineEdit_second) ? ui->lineEdit_second->text().toInt():0;

    return QDateTime(QDate(y, M, d) ,QTime(h, m, s));
}

void lds_calendarwidget::setDateTime(const QDateTime &dt)
{
    QDate d = dt.date();
    QTime t = dt.time();
    ui->pushButton_curyear->setText(QString::number(d.year()));
    ui->pushButton_curyear_1->setText(QString::number(d.year()-1));
    ui->pushButton_curyear_2->setText(QString::number(d.year()-2));

    if(lds::widgetIsShow(ui->lineEdit_day))ui->lineEdit_day->setText(QString::number(d.day())); else ui->lineEdit_day->setText(QString::number(0));
    if(lds::widgetIsShow(ui->lineEdit_hour))ui->lineEdit_hour->setText(QString::number(t.hour())); else ui->lineEdit_hour->setText(QString::number(0));
    if(lds::widgetIsShow(ui->lineEdit_minute))ui->lineEdit_minute->setText(QString::number(t.minute())); else ui->lineEdit_minute->setText(QString::number(0));
    if(lds::widgetIsShow(ui->lineEdit_month))ui->lineEdit_month->setText(QString::number(d.month())); else ui->lineEdit_month->setText(QString::number(0));
    if(lds::widgetIsShow(ui->lineEdit_second))ui->lineEdit_second->setText(QString::number(t.second())); else ui->lineEdit_second->setText(QString::number(0));
    if(lds::widgetIsShow(ui->lineEdit_year))ui->lineEdit_year->setText(QString::number(d.year())); else ui->lineEdit_year->setText(QString::number(0));

    if( (lds::widgetIsHide(ui->lineEdit_hour) && lds::widgetIsHide(ui->lineEdit_minute) && lds::widgetIsHide(ui->lineEdit_second))
            || (lds::widgetIsHide(ui->lineEdit_year) && lds::widgetIsHide(ui->lineEdit_month) && lds::widgetIsHide(ui->lineEdit_day)) ) {
        resize(295, 300);
        ui->frame_2->resize(275, 40);
        ui->frame->move(0, 55);
    } else {
        resize(295, 330);
        ui->frame_2->resize(275, 70);
        ui->frame->move(0, 85);
    }
}

void lds_calendarwidget::set_num()
{
    QPushButton *btn=qobject_cast<QPushButton *>(sender());
    if(!btn)return;
    if(btn==ui->pushButton_curyear){
        ui->lineEdit_year->setText(btn->text());
    } else if(btn==ui->pushButton_curyear_1){
        ui->lineEdit_year->setText(btn->text());
    } else if(btn==ui->pushButton_curyear_2){
        ui->lineEdit_year->setText(btn->text());
    } else if(btn==ui->pushButton_today){
        setDateTime(QDateTime::currentDateTime());
    } else if(btn == ui->pushButton_ok){
        _dtedit->setDateTime(dateTime());
        this->close();
    } else {
        QLineEdit *line=qobject_cast<QLineEdit *>(this->focusWidget());
        if(!line)return;
        if(btn==ui->pushButton_clear){
            line->clear();
        }else {
            line->insert(btn->text());
        }
    }
}
