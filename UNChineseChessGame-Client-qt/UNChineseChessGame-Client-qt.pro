#-------------------------------------------------
#
# Project created by QtCreator 2016-05-02T18:22:28
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = UNChineseChessGame-Client-qt
TEMPLATE = app


SOURCES += main.cpp\
        MainWindow.cpp \
    ClientSocket.cpp \
    Square.cpp

HEADERS  += MainWindow.h \
    ClientSocket.h \
    Define.h \
    Square.h

FORMS    += MainWindow.ui

RESOURCES += \
    src.qrc
