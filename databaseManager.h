#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QString>
#include <QList>
#include <QVariantMap>
#include <QMutex>

class DatabaseManager : public QObject {
    Q_OBJECT

public:
    static DatabaseManager& instance() {
        static DatabaseManager instance;
        return instance;
    }

    // Public functions
    bool initializeDatabase();
    bool addressExists(const QString &address);
    bool addAddress(const QString &address, const QVariantMap &data);
    bool saveUniqueAddress(const QString &address, const QVariantMap &data);
    bool dropDatabase();
    QList<QString> getAddressData();
    QSqlDatabase getDatabase() const;
    QVariantMap getSpecificAddressData(const QString &address);
    void logTables();


private:
    explicit DatabaseManager(QObject *parent = nullptr);
    ~DatabaseManager();

    QSqlDatabase db;
    QMutex dbMutex;

    bool tableExists(const QString &tableName);

    // Disable copying
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;
};

#endif // DATABASEMANAGER_H
