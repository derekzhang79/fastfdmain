#ifndef LDS_LABEL_TEXT_H
#define LDS_LABEL_TEXT_H

#include <QWidget>
class lds_label_text : public QWidget
{
public:
    lds_label_text(QWidget *parent = 0);

    void setLabelText(const QString &labelText);
    void setText(const QString &text);
    void setValue(double value);
    void setBgColor(const QColor &bgColor);
    double value();
    QString text();
private:
    QString txt;
    QString labelText;
    QColor bgColor;
protected:
    virtual void paintEvent(QPaintEvent *);
};

#endif // LDS_LABEL_TEXT_H
