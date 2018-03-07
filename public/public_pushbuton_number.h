#ifndef PUBLIC_PUSHBUTON_NUMBER_H
#define PUBLIC_PUSHBUTON_NUMBER_H

#include <QPushButton>

class public_pushbuton_number : public QPushButton
{
    Q_OBJECT
public:
    public_pushbuton_number(QWidget *parent);
    void setNumber(double number);
    void setNumberUnit(const QString &unit);
    double number();
    void setText(const QString &text);
    void paintEvent(QPaintEvent *e);

private:
    QString _numberStr;
    QString _text;
    QString _unit;
};

#endif // PUBLIC_PUSHBUTON_NUMBER_H
