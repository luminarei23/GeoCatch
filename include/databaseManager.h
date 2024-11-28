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

/**
 * @class DatabaseManager
 * @brief Singleton class for managing database operations in the application.
 *
 * This class provides functionality to interact with the SQLite database,
 * including initializing the database, adding, retrieving, and deleting data,
 * as well as ensuring thread safety for database operations.
 */
class DatabaseManager : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Get the singleton instance of DatabaseManager.
     * @return Reference to the single DatabaseManager instance.
     */
    static DatabaseManager& instance() {
        static DatabaseManager instance;
        return instance;
    }

    /**
     * @brief Initialize the database and create required tables if they do not exist.
     * @return True if the database was successfully initialized, false otherwise.
     */
    bool initializeDatabase();

    /**
     * @brief Check if a specific address exists in the database.
     * @param address The address to check.
     * @return True if the address exists, false otherwise.
     */
    bool addressExists(const QString &address);

    /**
     * @brief Add a new address and its associated data to the database.
     * @param address The address to add.
     * @param data A QVariantMap containing additional data about the address.
     * @return True if the address was successfully added, false otherwise.
     */
    bool addAddress(const QString &address, const QVariantMap &data);

    /**
     * @brief Save a unique address to the database. If the address already exists, it is not added again.
     * @param address The address to add.
     * @param data A QVariantMap containing additional data about the address.
     * @return True if the address was successfully added or already exists, false otherwise.
     */
    bool saveUniqueAddress(const QString &address, const QVariantMap &data);

    /**
     * @brief Drop all data in the database.
     * @return True if the database was successfully cleared, false otherwise.
     */
    bool dropDatabase();

    /**
     * @brief Retrieve all address data from the database.
     * @return A list of address strings.
     */
    QList<QString> getAddressData();

    /**
     * @brief Retrieve a reference to the QSqlDatabase instance.
     * @return The QSqlDatabase object used by the DatabaseManager.
     */
    QSqlDatabase getDatabase() const;

    /**
     * @brief Retrieve specific address data from the database.
     * @param address The address to look up.
     * @return A QVariantMap containing data associated with the address.
     */
    QVariantMap getSpecificAddressData(const QString &address);

    /**
     * @brief Log all tables in the database for debugging purposes.
     */
    void logTables();

private:
    /**
     * @brief Private constructor for the singleton pattern.
     * @param parent Optional parent QObject.
     */
    explicit DatabaseManager(QObject *parent = nullptr);

    /**
     * @brief Destructor for DatabaseManager.
     */
    ~DatabaseManager();

    QSqlDatabase db; ///< The QSqlDatabase instance for managing database connections.
    QMutex dbMutex;  ///< Mutex for ensuring thread safety in database operations.

    /**
     * @brief Check if a specific table exists in the database.
     * @param tableName The name of the table to check.
     * @return True if the table exists, false otherwise.
     */
    bool tableExists(const QString &tableName);

    // Disable copying
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;
};

#endif // DATABASEMANAGER_H
