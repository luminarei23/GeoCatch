#include "databaseManager.h"

#include <QCoreApplication>
#include <QDir>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

DatabaseManager::DatabaseManager(QObject *parent) : QObject(parent) {
    QString connectionName = "GeoCatchDB";

    if (QSqlDatabase::contains(connectionName)) {
        db = QSqlDatabase::database(connectionName);
    } else {
        db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
        db.setDatabaseName(QCoreApplication::applicationDirPath() + "/api_responses.db");
    }

    if (!db.isOpen() && !db.open()) {
        qWarning() << "Database failed to open:" << db.lastError().text();
    } else {
        qDebug() << "Database successfully opened.";
    }
}

DatabaseManager::~DatabaseManager() {
    QString connectionName = "GeoCatchDB";
    if (QSqlDatabase::contains(connectionName)) {
        QSqlDatabase db = QSqlDatabase::database(connectionName);

        if (db.isOpen()) {
            db.close();
        }

        // Ensure no active queries are left
        QSqlDatabase::removeDatabase(connectionName);
        qDebug() << "Database connection removed.";
    } else {
        qDebug() << "No database connection to remove.";
    }
}


bool DatabaseManager::initializeDatabase() {
    QString connectionName = "GeoCatchDB";

    if (!QSqlDatabase::contains(connectionName)) {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
        db.setDatabaseName(QCoreApplication::applicationDirPath() + "/api_responses.db");

        if (!db.open()) {
            qWarning() << "Failed to open database:" << db.lastError().text();
            return false;
        }
        qDebug() << "Database successfully opened.";
    }

    QSqlDatabase db = QSqlDatabase::database(connectionName);
    if (!db.isOpen()) {
        qWarning() << "Database is not open!";
        return false;
    }

    QSqlQuery query(db);
    QString createTable = R"(
        CREATE TABLE IF NOT EXISTS api_responses (
            id INTEGER PRIMARY KEY,
            address TEXT UNIQUE,
            hostname TEXT,
            city TEXT,
            region TEXT,
            country TEXT,
            loc TEXT,
            postal TEXT,
            timezone TEXT
        )
    )";

    if (!query.exec(createTable)) {
        qWarning() << "Failed to create table:" << query.lastError().text();
    } else {
        qDebug() << "Table created or already exists.";
    }

    // Log existing tables in the database
    qDebug() << "Tables in database:" << db.tables();
    return true;
}

bool DatabaseManager::addressExists(const QString &address) {
    QSqlDatabase db = getDatabase();
    if (!db.isOpen()) {
        qWarning() << "Database is not open.";
        return false;
    }

    QSqlQuery query(db);
    if (!query.prepare("SELECT COUNT(*) FROM api_responses WHERE address = :address")) {
        qWarning() << "Failed to prepare query for checking address:" << query.lastError().text();
        return false;
    }

    query.bindValue(":address", address);

    if (!query.exec()) {
        qWarning() << "Failed to execute query for checking address:" << query.lastError().text();
        return false;
    }

    if (query.next()) {
        return query.value(0).toInt() > 0;
    }

    return false;
}

bool DatabaseManager::addAddress(const QString &address, const QVariantMap &data) {
    QSqlDatabase db = getDatabase();
    if (!db.isOpen()) {
        qWarning() << "Database is not open.";
        return false;
    }

    QSqlQuery query(db);
    if (!query.prepare(R"(
        INSERT INTO api_responses (address, hostname, city, region, country, loc, postal, timezone)
        VALUES (:address, :hostname, :city, :region, :country, :loc, :postal, :timezone)
    )")) {
        qWarning() << "Failed to prepare query for adding address:" << query.lastError().text();
        return false;
    }

    query.bindValue(":address", address);
    query.bindValue(":hostname", data.value("hostname").toString());
    query.bindValue(":city", data.value("city").toString());
    query.bindValue(":region", data.value("region").toString());
    query.bindValue(":country", data.value("country").toString());
    query.bindValue(":loc", data.value("loc").toString());
    query.bindValue(":postal", data.value("postal").toString());
    query.bindValue(":timezone", data.value("timezone").toString());

    if (!query.exec()) {
        qWarning() << "Failed to insert new address:" << query.lastError().text();
        return false;
    }

    qDebug() << "Address added successfully:" << address;
    return true;
}

bool DatabaseManager::saveUniqueAddress(const QString &address, const QVariantMap &data) {
    // Check if the address already exists
    if (addressExists(address)) {
        qDebug() << "Address already exists in the database:" << address;
        return false;
    }

    // Insert new address
    return addAddress(address, data);
}

QList<QString> DatabaseManager::getAddressData() {
    QSqlDatabase db = getDatabase();
    QList<QString> addresses;

    if (!db.isOpen()) {
        qWarning() << "Database is not open!";
        return addresses;
    }

    if (!tableExists("api_responses")) {
        qWarning() << "Table 'api_responses' does not exist!";
        return addresses;
    }

    QSqlQuery query(db);
    if (!query.exec("SELECT address FROM api_responses")) {
        qWarning() << "Failed to fetch saved addresses:" << query.lastError().text();
        return addresses;
    }

    while (query.next()) {
        addresses.append(query.value("address").toString());
    }

    return addresses;
}

bool DatabaseManager::tableExists(const QString &tableName) {
    if (!db.isOpen()) {
        qWarning() << "Database is not open!";
        return false;
    }

    return db.tables().contains(tableName);
}


void DatabaseManager::logTables() {
    QSqlDatabase db = QSqlDatabase::database("GeoCatchDB");
    if (!db.isOpen()) {
        qWarning() << "Database is not open!";
        return;
    }

    qDebug() << "Tables in database:" << db.tables();
}

QSqlDatabase DatabaseManager::getDatabase() const {
    QString connectionName = "GeoCatchDB";

    if (QSqlDatabase::contains(connectionName)) {
        return QSqlDatabase::database(connectionName);
    } else {
        qWarning() << "Database connection" << connectionName << "does not exist!";
        return QSqlDatabase(); // Return a null database if connection doesn't exist
    }
}

bool DatabaseManager::dropDatabase() {
    QSqlDatabase db = getDatabase();

    if (!db.isOpen()) {
        qWarning() << "Database is not open when clearing data.";
        return false;
    }

    if (!tableExists("api_responses")) {
        qWarning() << "Table 'api_responses' does not exist.";
        return false;
    }

    QSqlQuery query(db);
    if (!query.exec("DELETE FROM api_responses")) {
        qWarning() << "Failed to clear database:" << query.lastError().text();
        return false;
    }

    qDebug() << "Database cleared successfully.";
    return true;
}

QVariantMap DatabaseManager::getSpecificAddressData(const QString &address) {
    QSqlDatabase db = getDatabase();
    if (!db.isOpen()) {
        qDebug() << "Database is not open!";
        return {};
    }

    QSqlQuery query(db);
    query.prepare("SELECT * FROM api_responses WHERE address = :address");
    query.bindValue(":address", address);

    if (!query.exec()) {
        qDebug() << "Database query failed:" << query.lastError().text();
        return {};
    }

    if (query.next()) {
        QVariantMap data;
        data["address"] = query.value("address").toString();
        data["hostname"] = query.value("hostname").toString();
        data["city"] = query.value("city").toString();
        data["region"] = query.value("region").toString();
        data["country"] = query.value("country").toString();
        data["loc"] = query.value("loc").toString();
        data["postal"] = query.value("postal").toString();
        data["timezone"] = query.value("timezone").toString();
        qDebug() << "Retrieved data from database:" << data;
        return data;
    }

    qDebug() << "No data found for address:" << address;
    return {};
}
