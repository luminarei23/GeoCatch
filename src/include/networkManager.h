#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QString>
#include <QTimer>

/**
 * @class NetworkManager
 * @brief Handles network-related functionality, including API calls, connection status checks, and localhost resolution.
 *
 * The NetworkManager class provides functionality for making API calls to fetch IP-related data,
 * resolving localhost to public IP, and monitoring the device's connection status.
 */
class NetworkManager : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Constructor for NetworkManager.
     * @param parent Optional parent QObject.
     */
    explicit NetworkManager(QObject *parent = nullptr);

    /**
     * @brief Make an API call to fetch data for a given IP address.
     * @param ip The IP address to query.
     */
    void makeApiCall(const QString &ip);

    /**
     * @brief Resolve the localhost address to the public IP.
     */
    void resolveLocalhost();

    /**
     * @brief Check if the network is currently online.
     * @return True if online, false otherwise.
     */
    bool isOnline() const;

    /**
     * @brief Set the online status and emit a signal if the status changes.
     * @param onlineStatus The new online status.
     */
    void setOnline(bool onlineStatus);

    /**
     * @brief Check the current connection status and emit updates.
     *
     * This function is invokable from QML.
     */
    Q_INVOKABLE void checkConnectionStatus();

private:
    QNetworkAccessManager *networkManager; ///< Manages network requests.
    QTimer *connectionTimer;               ///< Timer for periodically checking connection status.
    bool online = true;                    ///< Stores the current online status.

signals:
    /**
     * @brief Signal emitted when an API response is received.
     * @param ip The IP address queried.
     * @param hostname The hostname associated with the IP.
     * @param city
     * @param region
     * @param country
     * @param loc The geographical location
     * @param postal The postal code
     * @param timezone
     */
    void apiResponseReceived(const QString &ip, const QString &hostname, const QString &city,
                             const QString &region, const QString &country, const QString &loc,
                             const QString &postal, const QString &timezone);

    /**
     * @brief Signal emitted for debug messages.
     * @param message The debug message.
     */
    void debugMessage(const QString &message);

    /**
     * @brief Signal emitted when the connection status changes.
     * @param isOnline The new connection status (true for online, false for offline).
     */
    void connectionStatusChanged(bool isOnline);

    /**
     * @brief Signal emitted when the localhost is resolved to a public IP.
     * @param publicIP The resolved public IP address.
     */
    void localhostResolved(const QString &publicIP);

private slots:
    /**
     * @brief Handle the response from a connection status check.
     * @param reply The network reply containing the connection status result.
     */
    void handleConnectionStatus(QNetworkReply *reply);
};

#endif // NETWORKMANAGER_H
