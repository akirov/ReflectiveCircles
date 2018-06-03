
##CONFIG += debug
#CONFIG += release
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
    OBJECTS_DIR = ./build/release
    MOC_DIR = ./build/release
}

CONFIG(debug, debug|release){
    DESTDIR = ./bin/debug
    OBJECTS_DIR = ./build/debug
    MOC_DIR = ./build/debug
}

#release:DESTDIR = ./bin/release
#debug:DESTDIR = ./bin/debug


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
