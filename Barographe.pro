#-------------------------------------------------
#
# Project created by QtCreator 2019-02-06T08:15:48
#
#-------------------------------------------------

QT       += core gui serialport printsupport xml network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Barographe
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    fenconfig.cpp \
    qcustomplot.cpp \
    graphe.cpp \
    sensordialog.cpp \
    xmlmanager.cpp

HEADERS += \
        mainwindow.h \
    fenconfig.h \
    qcustomplot.h \
    graphe.h \
    sensordialog.h \
    xmlmanager.h

FORMS += \
        mainwindow.ui \
    fenconfig.ui \
    graphe.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    icons_kde.qrc

RC_ICONS = icon.ico

QMAKE_CXXFLAGS += "-fno-sized-deallocation"
