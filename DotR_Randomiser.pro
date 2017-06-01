QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DotR_Randomiser
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

INCLUDEPATH += $$PWD/include

QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3

SOURCES += src/main.cpp \
        src/mainwindow.cpp \
        src/decks.cpp \
        src/iologic.cpp

HEADERS  += include/mainwindow.hpp \
        include/decks.hpp \
        include/iologic.hpp

RC_ICONS = Red_Rose.ico

FORMS    += forms/mainwindow.ui

CONFIG += c++14
