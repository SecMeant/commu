TEMPLATE = app
TARGET = client
CONFIG += static

QT = core gui

greaterThan(QT_MAJOR_VERSION,4): QT += widgets

SOURCES += \
    main.cpp \
    initwindow.cpp \
    chatwindow.cpp \
    mlp.cpp \
    threadfunctions.cpp

HEADERS += \
    initwindow.h \
    threadfunctions.h \
    chatwindow.h \
    mlp.h

LIBS += -lws2_32 -luser32
