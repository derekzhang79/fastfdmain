#-------------------------------------------------
#
# Project created by QtCreator 2015-12-06T22:56:01
#
#-------------------------------------------------

DEFINES += QT_NEW_SYSTEM
INCLUDEPATH += $$PWD/inputnew/

QT       += core gui sql network xml concurrent svg phonon

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SoftPos

TEMPLATE = app

#with system\ chinese
#/home/cz/Qt5.2.1/5.2.1/gcc_64/bin/lupdate ~/视频/fastfdmain/fastfdmain_new.pro
# /home/cz/Qt5.2.1/5.2.1/gcc_64/bin/linguist

TRANSLATIONS +=ts/EN_fastfd.ts
TRANSLATIONS +=ts/FA_fastfd.ts
TRANSLATIONS +=ts/DE_fastfd.ts

TRANSLATIONS +=ts/FR_fastfd.ts
TRANSLATIONS +=ts/RU_fastfd.ts
TRANSLATIONS +=ts/ES_fastfd.ts
TRANSLATIONS +=ts/TR_fastfd.ts
TRANSLATIONS +=ts/AR_fastfd.ts

TRANSLATIONS +=ts/KO_fastfd.ts
TRANSLATIONS +=ts/JA_fastfd.ts
TRANSLATIONS +=ts/IW_fastfd.ts
TRANSLATIONS +=ts/IT_fastfd.ts
TRANSLATIONS +=ts/PT_fastfd.ts

TRANSLATIONS +=ts/HI_fastfd.ts
TRANSLATIONS +=ts/BN_fastfd.ts
TRANSLATIONS +=ts/PA_fastfd.ts
TRANSLATIONS +=ts/MM_fastfd.ts

DESTDIR = .

include(qt/qt.pri)

include(back/back.pri)

include(public/public.pri)

include(peripheral/peripheral.pri)

include(testwx/testwx.pri)

include(lds/lds.pri)

include(fastfd_update/fastfd_update.pri)

include(output_lib/output_lib.pri)

include(fileviewer/fileviewer.pri)

include(mwrf/mwrf.pri)

include(inputnew/inputnew.pri)

SOURCES += main.cpp\
#        mainwindow.cpp

#HEADERS  += mainwindow.h

#FORMS    += \
#    mainwindow.ui

RESOURCES += \
    res.qrc

RC_FILE = fastfd.rc


unix{
#libcurl
CONFIG(debug,debug|release):INCLUDEPATH += /usr/local/curl/include
CONFIG(release,debug|release):INCLUDEPATH += /usr/local/libcurl/include
CONFIG(debug,debug|release):LIBS += /usr/local/curl/lib/libcurl.a -lz -lrt
CONFIG(release,debug|release):LIBS +=-lcurl
#~libcurl
#openssl
INCLUDEPATH +=/usr/local/openssl/include
LIBS += -L/usr/local/openssl/lib -lssl -lcrypto -ldl -lpthread
#~openssl
#lusb
CONFIG(debug,debug|release):INCLUDEPATH += /usr/local/include
CONFIG(release,debug|release):INCLUDEPATH += /usr/local/usb-arm/include
CONFIG(debug,debug|release):LIBS +=-L/usr/local/lib -lusb-1.0
CONFIG(release,debug|release):LIBS +=  -lusb-1.0  /lib/arm-linux-gnueabihf/libudev.so.1
#~lusb
}

win32:LIBS +=-lwinspool
