#-------------------------------------------------
#
# Project created by QtCreator 2018-08-22T11:45:28
#
#-------------------------------------------------

QT       += core gui xml serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TRANSLATIONS += cn.ts en.ts

TARGET = MusicPlay
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    mydialog.cpp \
    music/music.cpp \
    cJSON/cJSON.c \
    music/scanning.cpp \
    music/user_port.cpp \
    music/Firmware.cpp \
    music/mploader.cpp \
    music/process.cpp

HEADERS += \
        mainwindow.h \
    mydialog.h \
    music/music.h \
    cJSON/cJSON.h \
    music/scanning.h \
    music/user_port.h \
    music/Firmware.h \
    music/mploader.h \
    music/process.h

FORMS += \
        mainwindow.ui \
    mydialog.ui
