#ifndef LDS_DIALOG_H
#define LDS_DIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QTranslator>
#include "pointerautodel.h"

class lds_Dialog : public QDialog
{
public:
    static void retranslateSystem(const QString &key);
    static void clearCache();
    static QTranslator *get_printTranslate(const QString &languageKey);
    static void del_printTranslate();
private:
    static pointerAutoDel<QTranslator> printTranslate;
    static pointerAutoDel<QTranslator> curTranslate;
//    static QTranslator printTranslate;
//    static QTranslator curTranslate;
public:
    lds_Dialog(QWidget *parent);
    virtual void retranslateView() = 0;

    void comIndexSaveClear(QComboBox *com);
    void comIndexRestore(QComboBox *com);
    void comIndexRestoreAll();
    QMap<QComboBox*, QVariant> com_index;
};

#endif // LDS_DIALOG_H
