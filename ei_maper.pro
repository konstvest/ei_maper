#-------------------------------------------------
#
# Project created by QtCreator 2014-10-06T13:57:27
#
#-------------------------------------------------

QT       += core gui
QT       += opengl

LIBS += -lglu32 -lopengl32

TARGET = ei_maper
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    mainscene.cpp

HEADERS  += mainwindow.h \
    mainscene.h

FORMS    += mainwindow.ui

RESOURCES += \
    resource.qrc
