#ifndef BTNLANGUAGESWICTH_H
#define BTNLANGUAGESWICTH_H

#include "lds_pushbutton_wrap.h"

class btnlanguageswicth : public lds_pushbutton_wrap
{
    Q_OBJECT
public:
    btnlanguageswicth(QWidget *parent = 0) ;
    void initKeys(const QString &first_key, const QString &second_key);
    void initKeys_conf();
    QString firstKey();
    QString firstValue();
    QString secondKey();
    QString secondValue();
    void enabledSwitchKey(bool enabled);

    static void getvaluesConf(QString &first_value, QString &value2);
signals:
    void languageChanged();
private slots:
    void switchKey();
protected:
    void paintEvent(QPaintEvent *e);
private:
    void enabledKey1();
    void enabledKey2();
    void updateInitkeys();
    bool is_switch_language;
    QString key1;
    QString key2;
    QString value1;
    QString value2;
    int cur_index;
};

#endif // BTNLANGUAGESWICTH_H
