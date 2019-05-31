#-------------------------------------------------
#
# Project created by QtCreator 2019-05-16T11:23:12
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = camerademo
TEMPLATE = app


SOURCES += main.cpp\
           qcamdemo.cpp\
           qcamdata.cpp  \
           quecrecorder.cpp \
           qcamera_mp4.c \

HEADERS  += qcamdemo.h\
            qcamdata.h  \
            qcamera_mp4.h \
            quecrecorder.h \

FORMS    += qcamdemo.ui

#CONFIG += mobility
#MOBILITY = 

RESOURCES += \
    qcamdemores.qrc

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

INCLUDEPATH += \
        $(WORKSPACE)/vendor/qcom/opensource/libroboticscamera/libcamera/inc \
        $(WORKSPACE)/vendor/qcom/opensource/libroboticscamera/libcamera/src/camera_hal1 \


#  camera_test_LDFLAGS  = -lutils -lcutils -llog -lmm-qcamera -lmmcamera_interface -lmmjpeg_interface -lcamera -lhardware
#unix|win32:

dependency_libs=' -lbinder -ldl -lrt -lhardware -lmmcamera_interface -lmmjpeg_interface -lcamera_client -lmm-qcamera'
LIBS += -L$(PKG_CONFIG_SYSROOT_DIR)/usr/lib -lcamera -lbinder -ldl -lrt -lhardware -lmmcamera_interface -lmmjpeg_interface -lcamera_client -lmm-qcamera -lpthread -lqcamera-omx -lavformat -lswscale -lavcodec -lswresample -lavutil -llog

target.path = /camerademo
INSTALLS += target
