#include "lds_dialog.h"
#include <QApplication>
#include <QFontDatabase>
#include <QtDebug>
#include "w_scr_dish_pay_module_select.h"
#include "w_bt_dish.h"
#include "w_i_inventory.h"
#include "public_sql.h"
#include <QApplication>
#include "w_m_member.h"
#include "w_sys_manage_outer_pay_set.h"
#include "restauranttableview.h"
#include "w_rpt_report_frame.h"

pointerAutoDel<QTranslator> lds_Dialog::curTranslate;
pointerAutoDel<QTranslator> lds_Dialog::printTranslate;

lds_Dialog::lds_Dialog(QWidget *parent)
    : QDialog(parent)
{
}

void lds_Dialog::comIndexSaveClear(QComboBox *com)
{
    int idx = com->currentIndex();
    if(com->isEditable()) {
        if(com->itemText(idx) != com->currentText()){
            com_index.insert(com, com->currentText());
            return;
        }
    }
    com_index.insert(com, idx);
    com->clear();
}

void lds_Dialog::comIndexRestore(QComboBox *com)
{
    QVariant d = com_index.value(com);
    if(d.type() == QVariant::Int) {
        com->setCurrentIndex(d.toInt());
        return;
    }
    if(d.type() == QVariant::String) {
        com->setEditText(d.toString());
        return;
    }
    com->setCurrentIndex(-1);
}

void lds_Dialog::comIndexRestoreAll()
{
    for(QMap<QComboBox*, QVariant>::iterator k = com_index.begin();
        k != com_index.end();
        k++) {
        comIndexRestore(k.key());
    }
}

void lds_Dialog::clearCache()
{
    w_scr_dish_pay_module_select::clearCache();
    w_bt_dish::clearCache();
    w_i_inventory::clearCache();
    w_m_member::clearCache();
    w_sys_manage_outer_pay_set::clearCache();
    restraurantDelegate::clearCache();
    w_rpt_report_frame::clearCache();
}

QTranslator *lds_Dialog::get_printTranslate(const QString &languageKey)
{
    static QString preKey;

    if(preKey != languageKey) {
        preKey = languageKey;
        printTranslate.delPointer();
    }
    if( 0 == printTranslate.getPointer() ) {
        printTranslate.setPointer(new QTranslator);
    }
    if(printTranslate.getPointer()->isEmpty()) {
        printTranslate.getPointer()->load("userdata/settings/language/"+languageKey+"_fastfd.qm");
    }
    return printTranslate.getPointer();
}

void lds_Dialog::del_printTranslate()
{
    printTranslate.delPointer();
}

void lds_Dialog::retranslateSystem(const QString &key)
{
    if(0 != curTranslate.getPointer()
            &&!curTranslate.getPointer()->isEmpty()) {
        qApp->removeTranslator(curTranslate.getPointer());
        curTranslate.delPointer();
    }
    if( 0 == curTranslate.getPointer() ) {
        curTranslate.setPointer(new QTranslator);
    }

    QString qmpath = QString("userdata/settings/language/")+key+"_fastfd.qm";
    if(curTranslate.getPointer()->load(qmpath)) {
        qApp->installTranslator(curTranslate.getPointer());
    }
    lds::sysconf->setValue("system_setting/SYS_LANGUAGE", key);
    lds_Dialog::clearCache();
}
