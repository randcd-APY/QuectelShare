QT += widgets

VERSION = $$QT_VERSION

HEADERS += \
           demoapplication.h \
           quec_launcher.h \
           pictureflow.h \
           slideshow.h

SOURCES += \
           demoapplication.cpp \
           quec_launcher.cpp \
           main.cpp \
           pictureflow.cpp \
           slideshow.cpp

RESOURCES = quec_launcher.qrc

target.path = /quec_launcher
INSTALLS += target
