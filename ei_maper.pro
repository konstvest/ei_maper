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
    createobjectform.cpp \
    key_manager.cpp \
    mobparameters.cpp \
    objects/lever.cpp \
    objects/light.cpp \
    log.cpp \
    objects/magictrap.cpp \
    main.cpp \
    mainwindow.cpp \
    math_utils.cpp \
    objects/object_base.cpp \
    ogl_utils.cpp \
    operationmanager.cpp \
    options.cpp \
    part.cpp \
    objects/particle.cpp \
    progressview.cpp \
    resourcemanager.cpp \
    sector.cpp \
    select_window.cpp \
    settings.cpp \
    objects/sound.cpp \
    tablemanager.cpp \
    objects/torch.cpp \
    objects/unit.cpp \
    types.cpp \
    ui_connectors.cpp \
    undo.cpp \
    view.cpp \
    figure.cpp \
    res_file.cpp \
    node.cpp \
    utils.cpp \
    landscape.cpp \
    view_keybinding.cpp \
    camera.cpp \
    mob.cpp \
    objects/worldobj.cpp

HEADERS += \
    createobjectform.h \
    key_manager.h \
    mobparameters.h \
    objects/lever.h \
    objects/light.h \
    log.h \
    objects/magictrap.h \
    mainwindow.h \
    math_utils.h \
    objects/object_base.h \
    ogl_utils.h \
    operationmanager.h \
    options.h \
    part.h \
    objects/particle.h \
    progressview.h \
    resourcemanager.h \
    sector.h \
    select_window.h \
    settings.h \
    objects/sound.h \
    tablemanager.h \
    objects/torch.h \
    objects/unit.h \
    ui_connectors.h \
    undo.h \
    view.h \
    figure.h \
    types.h \
    vectors.h \
    res_file.h \
    node.h \
    utils.h \
    landscape.h \
    camera.h \
    mob.h \
    objects/worldobj.h

FORMS += \
        createobjectform.ui \
        mainwindow.ui \
        mobParameters.ui \
        select_window.ui \
        settings.ui

LIBS += -lglu32 -lopengl32
# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RC_ICONS = icon.ico

RESOURCES += \
    data.qrc \
    shaders.qrc \
    textures.qrc
