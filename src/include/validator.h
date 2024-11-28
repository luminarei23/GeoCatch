#ifndef VALIDATOR_H
#define VALIDATOR_H

#include <QObject>
#include <QString>
#include <QUrl>

#include "networkManager.h"

/**
 * @class Validator
 * @brief Validates user input, interacts with the NetworkManager for online API calls, and handles database operations.
 *
 * The Validator class provides methods for validating user input as IP addresses or URLs,
 * querying the database for cached data, and invoking network calls for fetching information online.
 */
class Validator : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Constructor for Validator.
     * @param parent Optional parent QObject.
     */
    explicit Validator(QObject *parent = nullptr);

    /**
     * @brief Validate the given input as an IP address or URL.
     * @param input The input string to validate.
     *
     * This function determines if the input is a valid IP address or URL, and processes it accordingly.
     */
    Q_INVOKABLE void validateInput(const QString &input);

    /**
     * @brief Retrieve all saved data from the database.
     * @return A list of saved addresses as strings.
     */
    Q_INVOKABLE QList<QString> retrieveData();

    /**
     * @brief Clear all data in the database.
     * @return True if the operation was successful, false otherwise.
     */
    Q_INVOKABLE bool clearDatabase();

    /**
     * @brief Copy the specified text to the system clipboard.
     * @param text The text to copy.
     */
    Q_INVOKABLE void copyToClipboard(const QString &text);

private:
    NetworkManager *networkManager; ///< Pointer to the NetworkManager for online API calls.

    /**
     * @brief Check if the given string is a valid IP address.
     * @param ip The input string to validate.
     * @return True if valid, false otherwise.
     */
    bool isValidIpAddress(const QString &ip);

    /**
     * @brief Check if the given string is a valid URL.
     * @param url The input string to validate.
     * @return True if valid, false otherwise.
     */
    bool isValidUrl(const QString &url);

    /**
     * @brief Normalize the given URL by ensuring it includes a proper scheme.
     * @param input The input URL string.
     * @return A normalized URL string.
     */
    QString normalizeUrl(const QString &input);

    /**
     * @brief Handle special case for resolving "localhost".
     */
    void handleLocalhost();

    /**
     * @brief Query the database for information related to the given input.
     * @param input The input string to query.
     */
    void queryDatabase(const QString &input);

signals:
    /**
     * @brief Signal emitted when resolving "localhost".
     */
    void requestLocalhost();

    /**
     * @brief Signal emitted for processing an IP address.
     * @param ip The IP address to process.
     */
    void requestIpAddress(const QString &ip);

    /**
     * @brief Signal emitted for processing a URL.
     * @param url The URL to process.
     */
    void requestUrl(const QString &url);

    /**
     * @brief Signal emitted for debug messages.
     * @param message The debug message.
     */
    void debugMessage(const QString &message);

    /**
     * @brief Signal emitted when validation results are ready.
     * @param success Whether the validation was successful.
     * @param message A message describing the result.
     * @param resolvedValue The resolved value (e.g., an IP or host).
     */
    void validationResult(bool success, const QString &message, const QString &resolvedValue);

    /**
     * @brief Signal emitted when a database error occurs.
     * @param error The error message.
     */
    void databaseError(const QString &error);

    /**
     * @brief Signal emitted when a request finishes processing.
     */
    void requestFinished();

    /**
     * @brief Signal emitted when API response data is ready.
     * @param ip The queried IP address.
     * @param hostname
     * @param city
     * @param region
     * @param country
     * @param loc
     * @param postal
     * @param timezone
     */
    void apiResponseReceived(const QString &ip, const QString &hostname, const QString &city,
                             const QString &region, const QString &country, const QString &loc,
                             const QString &postal, const QString &timezone);

private slots:
    /**
     * @brief Handle API response and forward results.
     * @param ip The queried IP address.
     * @param hostname
     * @param city
     * @param region
     * @param country
     * @param loc The geographical location
     * @param postal
     * @param timezone
     */
    void handleApiResponse(const QString &ip, const QString &hostname, const QString &city,
                           const QString &region, const QString &country, const QString &loc,
                           const QString &postal, const QString &timezone);
};

#endif // VALIDATOR_H
