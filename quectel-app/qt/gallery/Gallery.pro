#-------------------------------------------------
#
# Project created by QtCreator 2019-01-28T11:06:43
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Gallery
TEMPLATE = app


SOURCES += main.cpp\
        gallery.cpp \
    filelist.cpp

HEADERS  += gallery.h \
    filelist.h

FORMS    += gallery.ui \
    filelist.ui

CONFIG += mobility
MOBILITY = 

RESOURCES += \
    icon.qrc

