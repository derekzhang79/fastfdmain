#ifndef LDS_KEY_H
#define LDS_KEY_H

#include <QFrame>

namespace Ui {
class lds_key;
}

class lds_key : public QFrame
{
    Q_OBJECT

public:
    explicit lds_key(QWidget *parent = 0);
    ~lds_key();
signals:
    void signal_click(const QString &value);
public slots:
    void tocenter();
private:
    Ui::lds_key *ui;
};

#endif // LDS_KEY_H
