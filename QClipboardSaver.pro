#-------------------------------------------------
#
# Project created by QtCreator 2012-06-26T13:42:16
#
#-------------------------------------------------
# https://github.com/trdm/Clint
# Предок: https://github.com/sje397/Clint

QT       += core gui network
#widgets

TARGET = QClipboardSaver
TEMPLATE = app

CONFIG(debug,debug|release ) {
    UI_HEADERS_DIR = debug
} else {
    UI_HEADERS_DIR = release
}


SOURCES += main.cpp\
        mainwindow.cpp

QMAKE_INFO_PLIST=Info.plist

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

RESOURCES += \
    res.qrc

OTHER_FILES += \
    resources/clipboard.png


#message($$UI_HEADERS_DIR)
