QT += core gui widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SlideMenuDemo
TEMPLATE = app

CONFIG += c++11

SOURCES += \
    main.cpp \
    slidemenu.cpp

HEADERS += \
    slidemenu.h
