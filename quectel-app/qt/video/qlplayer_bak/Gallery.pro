#-------------------------------------------------
#
# Project created by QtCreator 2019-01-28T11:06:43
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qlplayer
TEMPLATE = app


SOURCES += main.cpp\
	audplayer.cpp \
	vidplayer.cpp \
    gallery.cpp \
    filelist.cpp \
	videoplayer.cpp \
	glyuv420spwidget.cpp \
	util.c \

HEADERS  += gallery.h \
            filelist.h \
            vidplayer.h \
            audplayer.h \
            videoplayer.h \
            video_codec.h \
            glyuvwidget.h \
            util.h \

INCLUDEPATH += \
        $$NDK_ROOT/../../prebuilts/ffmpeg/include \
        $$NDK_ROOT/../../hardware/libhardware/include

unix|win32: LIBS += -lavformat -lswscale -lavcodec -lswresample -lavutil -lqcamera-omx -lEGL_adreno -lGLESv2_adreno -lGLESv1_CM_adreno

FORMS    += gallery.ui \
            filelist.ui

CONFIG += mobility
MOBILITY = 

RESOURCES += \
    icon.qrc
