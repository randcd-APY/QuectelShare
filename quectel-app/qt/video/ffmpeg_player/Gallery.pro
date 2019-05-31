#-------------------------------------------------
#
# Project created by QtCreator 2019-01-28T11:06:43
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ffmpeg_player
TEMPLATE = app


SOURCES += main.cpp\
        gallery.cpp \
    	filelist.cpp \
		ffmpeg_player.cpp 

HEADERS  += gallery.h \
    		filelist.h \
			ffmpeg_player.h

INCLUDEPATH += \
        $$NDK_ROOT/../../prebuilts/ffmpeg/include

unix|win32: LIBS += -L$$NDK_ROOT/../../prebuilts/ffmpeg/lib -lavformat -lswscale -lavcodec -lswresample -lavutil

FORMS    += gallery.ui \
    filelist.ui

CONFIG += mobility
MOBILITY = 

RESOURCES += \
    icon.qrc

