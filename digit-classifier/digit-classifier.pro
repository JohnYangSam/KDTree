# -------------------------------------------------
# Project created by QtCreator 2010-05-16T20:51:29
# -------------------------------------------------
CONFIG = qt release properties

properties {
TARGET = digit-classifier
TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp \
    CanvasWidget.cpp \
    ../KDTree.h \
    autounlock.cpp
HEADERS += mainwindow.h \
    CanvasWidget.h \
    grid.h \
    ../KDTree.h \
    ../BoundedPQueue.h \
    autounlock.h
}
