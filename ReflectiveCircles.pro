
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

Release:DESTDIR = release
Debug:DESTDIR = debug
