#ifndef LDS_KEYTOOLBUTTON_H
#define LDS_KEYTOOLBUTTON_H

#include <QToolButton>
class QLineEdit;
class lds_keytoolbutton : public QToolButton
{
    Q_OBJECT
public:
    enum KEYTYPE{
        NOKEY,
        NUMKEY,
        SYSKEY
    };

public:
    explicit lds_keytoolbutton(QWidget *parent = 0,KEYTYPE type = NUMKEY);
    void setKeyType(KEYTYPE type);
    KEYTYPE Keytype();
private slots:
    void tocenter(const QString &value);
    void showpop();
    void showpop_time();
private:
    QLineEdit*parentl;
    KEYTYPE _type;
    QString oldstylesheet;
};

#endif // LDS_KEYTOOLBUTTON_H
