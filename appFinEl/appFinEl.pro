#-------------------------------------------------
#
# Project created by QtCreator 2017-07-04T12:44:28
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = appFinEl
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    model/Cell.cpp \
    model/Element.cpp \
    model/Node.cpp \
    model/Tissue.cpp \
    model/utilitarian.cpp \
    view/KeyPoint.cpp \
    view/KeyLine.cpp \
    view/KeyCell.cpp \
    view/KeyTissue.cpp \
    model/Matrix.cpp

HEADERS  += mainwindow.h \
    model/Cell.h \
    model/Element.h \
    model/Matrix.h \
    model/Node.h \
    model/Tissue.h \
    model/utilitarian.h \
    view/KeyPoint.h \
    view/KeyLine.h \
    view/KeyCell.h \
    view/KeyTissue.h

FORMS    += mainwindow.ui
