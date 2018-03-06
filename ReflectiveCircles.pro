
#CONFIG += debug
CONFIG += release
CONFIG -= debug_and_release debug_and_release_target

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


CONFIG(release, debug|release){
    DESTDIR = ./bin/release
    OBJECTS_DIR = ./build
    MOC_DIR = ./build
}

CONFIG(debug, debug|release){
    DESTDIR = ./bin/debug
    OBJECTS_DIR = ./build
    MOC_DIR = ./build
}

#release:DESTDIR = ./bin/release
#debug:DESTDIR = ./bin/debug
##DESTDIR = ./bin
#OBJECTS_DIR = ./build
#MOC_DIR = ./build


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
