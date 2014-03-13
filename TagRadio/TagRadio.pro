#-------------------------------------------------
#
# Project created by QtCreator 2014-03-03T19:06:33
#
#-------------------------------------------------

QT       += core gui bluetooth

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TagRadio
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    bluetooth.cpp

HEADERS  += mainwindow.h \
    bluetooth.h

FORMS    += mainwindow.ui
