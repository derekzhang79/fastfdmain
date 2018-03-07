#ifndef LDS_KEYWITHOK_H
#define LDS_KEYWITHOK_H

#include <QFrame>

namespace Ui {
class lds_keywithok;
}

class lds_keywithok : public QFrame
{
    Q_OBJECT

public:
    explicit lds_keywithok(QWidget *parent = 0);
    ~lds_keywithok();
signals:
    void signal_click(const QString &value);
public slots:
    void tocenter();

private:
    Ui::lds_keywithok *ui;
};

#endif // LDS_KEYWITHOK_H
