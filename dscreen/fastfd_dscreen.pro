#-------------------------------------------------
#
# Project created by QtCreator 2015-12-06T22:56:01
#
#-------------------------------------------------

QT       += core gui sql network xml concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
win32 {
TARGET = ../../fastfdd#window下的有debug目录
}
unix {
TARGET = ../fastfdd
}
TEMPLATE = app

DESTDIR = .


HEADERS += \
    doublewidget.h \
    ../lds/cJSON.h \
    ../lds/lds.h \
    ../lds/lds_graphicsview.h \
    ../lds/lds_tablewidget.h \
    ../lds/lds_tableview_repecify.h \
    ../lds/lds_query.h \
    ../testwx/liberweima/bitstream.h \
    ../testwx/liberweima/config.h \
    ../testwx/liberweima/CreatQr.h \
    ../testwx/liberweima/erweima.h \
    ../testwx/liberweima/mask.h \
    ../testwx/liberweima/mmask.h \
    ../testwx/liberweima/mqrspec.h \
    ../testwx/liberweima/qrencode.h \
    ../testwx/liberweima/qrinput.h \
    ../testwx/liberweima/qrspec.h \
    ../testwx/liberweima/rscode.h \
    ../testwx/liberweima/split.h \
    ../public/fexpandmain_table.h \
    ../lds/lds_dialog_drog.h \
    lds_picshow_item.h \
    ../lds/lds_tableview_timeout.h \
    ../testwx/testwx_help.h \
    doublewidget_1024x600.h \
    ../lds/lds_model_sqltablemodel.h \
    ../lds/lds_expression2rpn.h \
    ../lds/lds_tableview_virtual.h \
    ../lds/lds_thread.h \
    ../lds/lds_model_sqltablemodel_d.h \
    ../lds/lds_record_do_longpress.h \
    rect_item.h \
    ../lds/lds_bool.h \
    ../lds/lds_query_db.h \
    tableview_item.h \
    doublewidget_normal.h \


SOURCES += \
    main.cpp \
    doublewidget.cpp \
    ../lds/lds.cpp \
    ../lds/cJSON.c \
    ../lds/lds_graphicsview.cpp \
    ../lds/lds_tablewidget.cpp \
    ../lds/lds_tableview_repecify.cpp \
    ../lds/lds_query.cpp \
    ../testwx/liberweima/erweima.cpp \
    ../testwx/liberweima/bitstream.c \
    ../testwx/liberweima/CreatQr.c \
    ../testwx/liberweima/mask.c \
    ../testwx/liberweima/mmask.c \
    ../testwx/liberweima/mqrspec.c \
    ../testwx/liberweima/qrencode.c \
    ../testwx/liberweima/qrinput.c \
    ../testwx/liberweima/qrspec.c \
    ../testwx/liberweima/rscode.c \
    ../testwx/liberweima/split.c \
    ../public/fexpandmain_table.cpp \
    ../lds/lds_dialog_drog.cpp \
    lds_picshow_item.cpp \
    ../lds/lds_tableview_timeout.cpp \
    ../testwx/testwx_help.cpp \
    doublewidget_1024x600.cpp \
    ../lds/lds_model_sqltablemodel.cpp \
    ../lds/lds_expression2rpn.cpp \
    ../lds/lds_tableview_virtual.cpp \
    ../lds/lds_thread.cpp \
    ../lds/lds_model_sqltablemodel_d.cpp \
    ../lds/lds_record_do_longpress.cpp \
    rect_item.cpp \
    ../lds/lds_bool.cpp \
    ../lds/lds_query_db.cpp \
    tableview_item.cpp \
    doublewidget_normal.cpp \


FORMS += \
    doublewidget.ui \
    ../public/fexpandmain_payover.ui \
    ../back/w_rpt_report_frame.ui

RESOURCES +=

UI_DIR += .

RC_FILE = ../fastfd.rc

OTHER_FILES += \

INCLUDEPATH +=  ../lds \
                                ../public \
                                ../testwx/liberweima \
                                ../qt \
                                ../testwx \
                                ../back \
