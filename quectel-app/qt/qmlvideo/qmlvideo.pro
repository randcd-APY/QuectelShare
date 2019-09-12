TEMPLATE = app
TARGET = qmlvideo

QT += quick multimedia multimediawidgets

LOCAL_SOURCES = main.cpp
LOCAL_HEADERS = trace.h

SOURCES += $$LOCAL_SOURCES
HEADERS += $$LOCAL_HEADERS
RESOURCES += qmlvideo.qrc

LIBS +=-L/usr/local/lib -lQt5Multimedia

#SNIPPETS_PATH = ../snippets
#include($$SNIPPETS_PATH/performancemonitor/performancemonitordeclarative.pri)

target.path = $$[QT_INSTALL_EXAMPLES]/multimedia/video/qmlvideo
INSTALLS += target

EXAMPLE_FILES += \
    qmlvideo.png \
    qmlvideo.svg
