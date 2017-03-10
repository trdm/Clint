#-------------------------------------------------
#
# Project created by QtCreator 2012-06-26T13:42:16
#
#-------------------------------------------------
# https://github.com/trdm/Clint
# Предок: https://github.com/sje397/Clint
# Пример командной строки для запуска просмотра файла истории.
# -h "C:\Users\Settingth\QClipSaver\histori2017_03.dat"

QT       += core gui network sql
#widgets

TARGET = QClipSaver
TEMPLATE = app

CONFIG(debug,debug|release ) {
    UI_HEADERS_DIR = debug
} else {
    UI_HEADERS_DIR = release
}

win32:RC_FILE = QClipboardSaver.rc

SOURCES += main.cpp\
        mainwindow.cpp \
    settingthdlg.cpp

QMAKE_INFO_PLIST=Info.plist

HEADERS  += mainwindow.h \
    settingthdlg.h

FORMS    += mainwindow.ui \
    settingthdlg.ui

RESOURCES += \
    QClipboardSaver.qrc

OTHER_FILES += \
    resources/clipboard.png


#message($$UI_HEADERS_DIR)
