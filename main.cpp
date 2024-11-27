#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QStandardPaths>
#include <Qdebug>

#include "validator.h"
#include "databaseManager.h" // Include the database manager

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);

    // Initialize DatabaseManager
    DatabaseManager::instance().initializeDatabase();

    // Register Validator for QML
    qmlRegisterType<Validator>("validator", 1, 0, "Validator");

    QQmlApplicationEngine engine;
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("GeoCatch", "Main");

    return app.exec();
}
