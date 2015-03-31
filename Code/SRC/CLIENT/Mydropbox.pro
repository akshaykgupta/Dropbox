#-------------------------------------------------
#
# Project created by QtCreator 2015-02-27T13:50:05
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Mydropbox
TEMPLATE = app


SOURCES += main.cpp\
        ipwindow.cpp \
    client.cpp \
    loginwindow.cpp \
    middleend.cpp \
    userinterface.cpp \
    registration.cpp \
    sharewindow.cpp \
    sharedwindow.cpp

HEADERS  += ipwindow.h \
    client.h \
    loginwindow.h \
    middleend.h \
    userinterface.h \
    registration.h \
    sharewindow.h \
    sharedwindow.h

FORMS    += ipwindow.ui \
    loginwindow.ui \
    userinterface.ui \
    registration.ui \
    sharewindow.ui \
    sharedwindow.ui

LIBS += -lssl -lcrypto -lboost_system -lboost_filesystem

QMAKE_CXXFLAGS += -std=c++11
