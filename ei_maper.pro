#-------------------------------------------------
#
# Project created by QtCreator 2019-01-27T13:50:17
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ei_maper
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
    key_manager.cpp \
    objects/lever.cpp \
    objects/light.cpp \
    log.cpp \
    logwindow.cpp \
    objects/magictrap.cpp \
    main.cpp \
    mainwindow.cpp \
    math_utils.cpp \
    objects/object_base.cpp \
    options.cpp \
    part.cpp \
    objects/particle.cpp \
    sector.cpp \
    settings.cpp \
    objects/sound.cpp \
    texturelist.cpp \
    objects/torch.cpp \
    objects/unit.cpp \
    view.cpp \
    figure.cpp \
    res_file.cpp \
    objectlist.cpp \
    node.cpp \
    utils.cpp \
    landscape.cpp \
    view_keybinding.cpp \
    camera.cpp \
    view_mouse_action.cpp \
    mob.cpp \
    objects/worldobj.cpp

HEADERS += \
    color.h \
    key_manager.h \
    objects/lever.h \
    objects/light.h \
    log.h \
    logwindow.h \
    objects/magictrap.h \
    mainwindow.h \
    math_utils.h \
    objects/object_base.h \
    options.h \
    part.h \
    objects/particle.h \
    sector.h \
    settings.h \
    objects/sound.h \
    texturelist.h \
    objects/torch.h \
    objects/unit.h \
    view.h \
    figure.h \
    types.h \
    vectors.h \
    res_file.h \
    objectlist.h \
    node.h \
    utils.h \
    landscape.h \
    map_mp.h \
    camera.h \
    mob.h \
    objects/worldobj.h

FORMS += \
        mainwindow.ui \
        settings.ui

LIBS += -lglu32 -lopengl32
# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    data.qrc \
    shaders.qrc \
    textures.qrc
