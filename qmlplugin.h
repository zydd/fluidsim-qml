#ifndef QMLPLUGIN_H
#define QMLPLUGIN_H

#include <QQmlExtensionPlugin>

#include "fluidsim.h"

class QmlPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "zydd.fluidsim")

public:
    inline void registerTypes(const char *uri) {
        Q_ASSERT(uri == QLatin1String("zydd.fluid"));
        qmlRegisterType<FluidSim>(uri, 1, 0, "FluidSim");
    }
};

#endif // QMLPLUGIN_H
