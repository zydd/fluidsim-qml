TEMPLATE = lib
CONFIG += c++11 qt plugin
QT += qml quick

SOURCES += \
    fluidsim.cpp

RESOURCES += qml.qrc

HEADERS += \
    fluidsim.h \
    qmlplugin.h

DESTDIR = imports/zydd/fluid
TARGET = fluidsim

QML_IMPORT_PATH =
DEFINES += QT_DEPRECATED_WARNINGS

target.path = imports/zydd/fluid
pluginfiles.files = qmldir
pluginfiles.path = imports/zydd/fluid

INSTALLS += target pluginfiles

DISTFILES += \
    qmldir \
    main.qml \
    display.frag \
    fluid.frag \
    init.frag
