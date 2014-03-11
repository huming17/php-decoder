QT       += core gui widgets network

TARGET = phpdecoder
TEMPLATE = app


SOURCES += main.cpp\
    mainwindow.cpp \
    deevalwidget.cpp \
    dezendwidget.cpp \
    util.cpp \
    about.cpp

HEADERS  += mainwindow.h \
    deevalwidget.h \
    dezendwidget.h \
    util.h \
    about.h

FORMS    += mainwindow.ui \
    deevalwidget.ui \
    dezendwidget.ui \
    about.ui

LIBS += -lshell32
