TEMPLATE = lib
CONFIG += c++11 qt plugin
QT += qml quick

SOURCES += \
    fluidsim.cpp

RESOURCES += qml.qrc

HEADERS += \
    fluidsim.h \
    qmlplugin.h

DESTDIR = imports/zydd/FluidSim
TARGET = fluidsim

QML_IMPORT_PATH =
DEFINES += QT_DEPRECATED_WARNINGS

target.path = imports/zydd/FluidSim
pluginfiles.files = qmldir
pluginfiles.path = imports/zydd/FluidSim

INSTALLS += target pluginfiles

DISTFILES += \
    qmldir \
    main.qml
