#-------------------------------------------------
#
# Project created by QtCreator 2016-10-23T09:16:10
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MultiEncryption
TEMPLATE = app


SOURCES += main.cpp\
        multiencryption.cpp \
    playfair.cpp \
    hill.cpp

HEADERS  += multiencryption.h \
    playfair.h \
    hill.h \
    encryptable.h

FORMS    += multiencryption.ui
