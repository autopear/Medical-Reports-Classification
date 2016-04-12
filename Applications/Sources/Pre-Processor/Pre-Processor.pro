#-------------------------------------------------
#
# Project created by QtCreator 2016-04-10T18:28:22
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = "Pre-Processor"
TEMPLATE = app


SOURCES += main.cpp\
           mainwindow.cpp \
           Porter2/porter2_stemmer.cpp \
           documentsloader.cpp \
           exportsvmdialog.cpp \
           idfcalculator.cpp \
           reportsparser.cpp \
           stemmer.cpp \
           svmexporter.cpp \
           tfcalculator.cpp \
           wordsloader.cpp

HEADERS += mainwindow.h \
           Porter2/porter2_stemmer.h \
           documentsloader.h \
           exportsvmdialog.h \
           idfcalculator.h \
           reportsparser.h \
           stemmer.h \
           svmexporter.h \
           tfcalculator.h \
           wordsloader.h
