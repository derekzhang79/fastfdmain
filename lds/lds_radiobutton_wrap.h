#ifndef LDS_RADIOBUTTON_WRAP_H
#define LDS_RADIOBUTTON_WRAP_H

#include <QObject>
#include <QRadioButton>

class lds_radiobutton_wrap : public QRadioButton
{
    Q_OBJECT

public:
    explicit lds_radiobutton_wrap(QWidget *parent = 0);
protected:
    void paintEvent(QPaintEvent *e);
    bool hitButton(const QPoint &p) const;
private:
    QRect textrect() const;
};

#endif // LDS_RADIOBUTTON_WRAP_H
