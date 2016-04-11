#-------------------------------------------------
#
# Project created by QtCreator 2016-04-10T18:28:22
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ReportsStemmer
TEMPLATE = app


SOURCES += main.cpp\
           mainwindow.cpp \
           Porter2/porter2_stemmer.cpp \
           reportsparser.cpp \
           stemmer.cpp

HEADERS += mainwindow.h \
           Porter2/porter2_stemmer.h \
           reportsparser.h \
           stemmer.h
