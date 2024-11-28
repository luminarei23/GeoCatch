#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QStandardPaths>
#include <QDebug>

#include "validator.h"
#include "databaseManager.h"
#include "networkManager.h"

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);

    // Initialize DatabaseManager
    if (!DatabaseManager::instance().initializeDatabase()) {
        qWarning() << "Failed to initialize the database!";
    }

    // Register Validator and NetworkManager for QML
    qmlRegisterType<Validator>("validator", 1, 0, "Validator");
    qmlRegisterType<NetworkManager>("networkmanager", 1, 0, "NetworkManager");

    QQmlApplicationEngine engine;

    // Handle object creation failure in QML
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    // Load the QML module
    engine.loadFromModule("GeoCatch", "Main");

    return app.exec();
}
