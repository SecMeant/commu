TEMPLATE = app
TARGET = client

QT = core gui

greaterThan(QT_MAJOR_VERSION,4): QT += widgets

SOURCES += \
    main.cpp \
    initwindow.cpp

HEADERS += \
    initwindow.h
