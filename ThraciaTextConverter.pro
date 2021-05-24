#-------------------------------------------------
#
# Project created by QtCreator 2015-12-12T11:57:20
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ThraciaTextConverter
TEMPLATE = app

CONFIG += c++11

SOURCES += main.cpp\
        mainwindow.cpp \
    thraciatextconverter.cpp \
    snesdatafinder.cpp \
    scriptparser.cpp

HEADERS  += mainwindow.h \
    thraciatextconverter.h \
    snesdatafinder.h \
    scriptparser.h

FORMS    += mainwindow.ui

RESOURCES += \
    resources.qrc

RC_FILE = icon.rc
