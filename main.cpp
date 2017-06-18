#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "fluidsim.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    qmlRegisterType<FluidSim>("zydd", 1, 0, "FluidSim");

    QQmlApplicationEngine engine;
    engine.load(":/main.qml");

    return app.exec();
}
