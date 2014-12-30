
CONFIG += release

TARGET = circles

TEMPLATE = app


SOURCES += src/main.cpp \
           src/ui.cpp \
           src/geometry.cpp \
           src/renderer.cpp

HEADERS += src/ui.h \
           src/geometry.h \
           src/renderer.h

#FORMS  += src/ReflectiveCircles.ui

release:DESTDIR = release
debug:DESTDIR = debug

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
