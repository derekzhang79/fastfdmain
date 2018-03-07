#include "btnlanguageswicth.h"
#include <QPainter>
#include <QPaintEvent>
#include "w_sys_manage_outer_pay_set.h"
#include "lds.h"
#include "lds_dialog.h"
#include "lds_messagebox.h"
#include "saas_pay_detail.h"


btnlanguageswicth::btnlanguageswicth(QWidget *parent)
    : lds_pushbutton_wrap(parent){

    enabledSwitchKey(true);
    enableLongPress(true);
#ifdef QT_DEBUG
    initKeys("CN", "EN");
#endif
}

void btnlanguageswicth::initKeys(const QString &first_key, const QString &second_key)
{
    key1 = first_key;
    key2 = second_key;
    cur_index = 1;

    if(key1 == "CN") {
        value1 = w_sys_manage_outer_pay_set::getLanguageValue(key1, true);
        value2 = w_sys_manage_outer_pay_set::getLanguageValue(key2, true);
    } else {
        value1 = (key1);
        value2 = (key2);
    }
}

void btnlanguageswicth::initKeys_conf()
{
    initKeys(lds::get_soft_curent_language_first(), lds::get_soft_curent_language_second());
}

void btnlanguageswicth::enabledKey1()
{
    cur_index = 1;
    updateInitkeys();
}

void btnlanguageswicth::enabledKey2()
{
    cur_index = 2;
    updateInitkeys();
}

void btnlanguageswicth::enabledSwitchKey(bool enabled)
{
    disconnect(this, SIGNAL(clicked()),this, SLOT(switchKey()));
    if(enabled) {
        connect(this, SIGNAL(clicked()),this, SLOT(switchKey()));
    }
}

void btnlanguageswicth::getvaluesConf(QString &value1, QString &value2)
{
    QString key1 = lds::get_soft_curent_language_first();
    QString key2 = lds::get_soft_curent_language_second();

    if(key1 == "CN") {
        value1 = w_sys_manage_outer_pay_set::getLanguageValue(key1, true);
        value2 = w_sys_manage_outer_pay_set::getLanguageValue(key2, true);
    } else {
        value1 = (key1);
        value2 = (key2);
    }
}

void btnlanguageswicth::switchKey()
{
    if(!saas_pay_detail::SaaSFuncCheck(this, "0512"/*多语言*/)) {
        return ;
    }

    if(cur_index == 1) {lds_Dialog::retranslateSystem(key2); lds::set_soft_curent_language_second(key1); enabledKey2(); emit languageChanged(); return;}
    if(cur_index == 2) {lds_Dialog::retranslateSystem(key1); lds::set_soft_curent_language_second(key2); enabledKey1(); emit languageChanged(); return;}
}

void btnlanguageswicth::paintEvent(QPaintEvent *e)
{
    QPushButton::paintEvent(e);
    QPainter p(this);
    QFont font = p.font();
    QString firstV;


    font.setPixelSize(height()*1.2/5);
//    font.setBold(true);
    p.setFont(font);

    firstV = firstValue();
    if(firstV.isEmpty()) firstV = "["+tr("主")+"]";
    QFontMetrics fm(p.font());
    if(fm.width(firstV) >= width()) {
        p.drawText(rect(), Qt::AlignLeft | Qt::AlignVCenter, firstV);
    } else {
        p.drawText(rect(), Qt::AlignCenter, firstV);
    }

    font.setPixelSize(height()*0.8/5);
//    font.setBold(false);
    p.setFont(font);
    QString secondV = secondValue();
    if(secondV.isEmpty()) {
        secondV = "["+tr("次")+"]";
    }
    p.drawText(rect(), Qt::AlignRight | Qt::AlignBottom, secondV);
}

void btnlanguageswicth::updateInitkeys()
{
    update();
    initKeys(firstKey(), secondKey());
}

QString btnlanguageswicth::firstValue()
{
    if(cur_index == 1) {
        return value1;
    }
    if(cur_index == 2) {
        return value2;
    }
    return value1;
}

QString btnlanguageswicth::secondValue()
{
    if(cur_index == 1) {
        return value2;
    }
    if(cur_index == 2) {
        return value1;
    }
    return value2;
}

QString btnlanguageswicth::firstKey()
{
    if(cur_index == 1) {
        return key1;
    }
    if(cur_index == 2) {
        return key2;
    }
    return key1;
}

QString btnlanguageswicth::secondKey()
{
    if(cur_index == 1) {
        return key2;
    }
    if(cur_index == 2) {
        return key1;
    }
    return key2;
}
