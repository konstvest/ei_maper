#-------------------------------------------------
#
# Project created by QtCreator 2017-08-05T13:01:38
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ei_maper
LIBS += -lglu32 -lopengl32
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
    glwidget.cpp \
    figure.cpp \
    scene.cpp \
    particle.cpp \
    sound.cpp \
    object.cpp \
    unit.cpp \
    trap.cpp \
    mob.cpp \
    resfile.cpp \
    MapUtils/mpfile.cpp \
    MapUtils/mprfile.cpp \
    MapUtils/secfile.cpp

HEADERS  += mainwindow.h \
    glwidget.h \
    scene.h \
    ei_types.h \
    ei_object.h \
    ei_map.h \
    ei_mob.h \
    ei_vectors.h \
    resfile.h \
    MapUtils/mpfile.h \
    MapUtils/mprfile.h \
    MapUtils/secfile.h \
    figure.h

FORMS    += mainwindow.ui

RESOURCES += \
    shaders.qrc \
    textures.qrc
