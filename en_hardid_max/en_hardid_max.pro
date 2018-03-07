#-------------------------------------------------
#
# Project created by QtCreator 2017-04-07T15:46:52
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = en_hardid_max
TEMPLATE = app


SOURCES += main.cpp\
        en_hardid_max.cpp \
    ../lds/lds.cpp

HEADERS  += en_hardid_max.h \
    ../lds/lds.h

FORMS    += en_hardid_max.ui

RC_FILE = fastfd.rc

INCLUDEPATH += ../lds/

