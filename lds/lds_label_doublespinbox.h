#ifndef LDS_LABEL_DOUBLESPINBOX_H
#define LDS_LABEL_DOUBLESPINBOX_H

#include <QWidget>
#include "lds_bool.h"

struct lds_label_doublespinbox_data{
    lds_label_doublespinbox_data();
    //
    void setValue(double value);
    double value();
    void setDecimals(int decimals);
    QString text();
    //
    double v;
    int decimals;
    //
    QString edit_value;
    QString labelText;
    QColor bgColor;
    QString prefix;
    bool isselectAll;
    bool enableSelectAll;
};

class lds_label_doublespinbox : public QWidget
{
    Q_OBJECT
public:
    lds_label_doublespinbox(QWidget *parent = 0);

    void setDecimals(int decimals);
    int decimals();

    void setValue(double v);
    void setValue(const QString &v);
    double value();
    double value_if_prefix(double else_value = 0);

    void setLabelText(const QString &labelText);

    void setBgColor(const QColor &bgColor);

    void setPrefix(const QString &prefix);
    QString prefix();

    void clear();


    void enableSelectAll();
    void selectAll();
    void selectCLear();
    bool isselectAll();

    void enableClickable();

    void setTipText(const QString &tip_text);
    void enableShowNegativeValue();

    QString falseText();
signals:
    void valueChanged(double);
    void signalClicked();
protected:
    virtual void paintEvent(QPaintEvent *);
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);
private:
    QString text();
private:
    lds_label_doublespinbox_data p_data;
    lds_bool_default_false isclickable;
    lds_bool_default_false ispressed;
    lds_bool_default_false bShowNegativeValue;
    QString tip_text;
};

#endif // LDS_LABEL_DOUBLESPINBOX_H
