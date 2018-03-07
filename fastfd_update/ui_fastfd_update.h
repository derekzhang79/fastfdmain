/********************************************************************************
** Form generated from reading UI file 'fastfd_update.ui'
**
** Created by: Qt User Interface Compiler version 4.8.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FASTFD_UPDATE_H
#define UI_FASTFD_UPDATE_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QProgressBar>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_fastfd_update
{
public:
    QVBoxLayout *verticalLayout;
    QGridLayout *gridLayout;
    QLabel *label;
    QLineEdit *lineEdit_cur_version;
    QLabel *label_newest_version;
    QLineEdit *lineEdit_newest_version;
    QProgressBar *progressBar;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushButton_update;
    QPushButton *pushButton_cancel;

    void setupUi(QDialog *fastfd_update)
    {
        if (fastfd_update->objectName().isEmpty())
            fastfd_update->setObjectName(QString::fromUtf8("fastfd_update"));
        fastfd_update->resize(471, 149);
        verticalLayout = new QVBoxLayout(fastfd_update);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label = new QLabel(fastfd_update);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 0, 0, 1, 1);

        lineEdit_cur_version = new QLineEdit(fastfd_update);
        lineEdit_cur_version->setObjectName(QString::fromUtf8("lineEdit_cur_version"));

        gridLayout->addWidget(lineEdit_cur_version, 0, 1, 1, 1);

        label_newest_version = new QLabel(fastfd_update);
        label_newest_version->setObjectName(QString::fromUtf8("label_newest_version"));

        gridLayout->addWidget(label_newest_version, 1, 0, 1, 1);

        lineEdit_newest_version = new QLineEdit(fastfd_update);
        lineEdit_newest_version->setObjectName(QString::fromUtf8("lineEdit_newest_version"));

        gridLayout->addWidget(lineEdit_newest_version, 1, 1, 1, 1);


        verticalLayout->addLayout(gridLayout);

        progressBar = new QProgressBar(fastfd_update);
        progressBar->setObjectName(QString::fromUtf8("progressBar"));

        verticalLayout->addWidget(progressBar);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        pushButton_update = new QPushButton(fastfd_update);
        pushButton_update->setObjectName(QString::fromUtf8("pushButton_update"));

        horizontalLayout->addWidget(pushButton_update);

        pushButton_cancel = new QPushButton(fastfd_update);
        pushButton_cancel->setObjectName(QString::fromUtf8("pushButton_cancel"));

        horizontalLayout->addWidget(pushButton_cancel);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(fastfd_update);

        QMetaObject::connectSlotsByName(fastfd_update);
    } // setupUi

    void retranslateUi(QDialog *fastfd_update)
    {
        fastfd_update->setWindowTitle(QApplication::translate("fastfd_update", "Dialog", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("fastfd_update", "\345\275\223\345\211\215\347\211\210\346\234\254", 0, QApplication::UnicodeUTF8));
        label_newest_version->setText(QApplication::translate("fastfd_update", "\346\234\200\346\226\260\347\211\210\346\234\254", 0, QApplication::UnicodeUTF8));
        lineEdit_newest_version->setText(QString());
        pushButton_update->setText(QApplication::translate("fastfd_update", "\345\215\207\347\272\247", 0, QApplication::UnicodeUTF8));
        pushButton_cancel->setText(QApplication::translate("fastfd_update", "\345\217\226\346\266\210", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class fastfd_update: public Ui_fastfd_update {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FASTFD_UPDATE_H
