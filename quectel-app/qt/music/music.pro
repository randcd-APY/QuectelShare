#-------------------------------------------------
#
# Project created by QtCreator 2019-05-16T11:05:53
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MusicDemo
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
        music_select.cpp \
        music_thread.cpp \
        music_recard_thread.cpp 

HEADERS += \
        mainwindow.h \
        music_select.h \
        music_thread.h \
        qlaudio_api.h \
        music_recard_thread.h \

FORMS += \
        mainwindow.ui

#INCLUDEPATH += .\

unix|win32:  LIBS += -lql_audio_api

INCLUDEPATH = \\172.18.105.32\work_3_workspace\sc20_project\SC20_Linux_R09_r00320.1\hardware\qcom\audio\qahw_api\test
INCLUDEPATH += \\172.18.105.32\work_3_workspace\sc20_project\SC20_Linux_R09_r00320.1\poky\build\tmp-glibc\work\msm8909-oe-linux-gnueabi\audiohal\1.0-r0\image\usr\include
DEPENDPATH += \\172.18.105.32\work_3_workspace\sc20_project\SC20_Linux_R09_r00320.1\poky\build\tmp-glibc\work\msm8909-oe-linux-gnueabi\audiohal\1.0-r0\image\usr\include