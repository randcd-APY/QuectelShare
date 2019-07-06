TEMPLATE = app
TARGET = qlfunc

QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

HEADERS = \
		ExamplePaintSurface.h \
		mainwindow.h \
		OpenGlOffscreenSurface.h

SOURCES = \
    main.cpp \
	mainwindow.cpp \
	OpenGlOffscreenSurface.cpp \
	ExamplePaintSurface.cpp

QT+=widgets
