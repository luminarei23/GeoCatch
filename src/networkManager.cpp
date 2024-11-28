#include <QNetworkRequest>
#include <QUrl>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>

#include "networkManager.h"
#include "databaseManager.h"

NetworkManager::NetworkManager(QObject *parent) : QObject(parent) {
    networkManager = new QNetworkAccessManager(this);
    checkConnectionStatus();

    connectionTimer = new QTimer(this);
    connect(connectionTimer, &QTimer::timeout, this, &NetworkManager::checkConnectionStatus);
    connectionTimer->start(3000); // Check the connection each 3 seconds
}

void NetworkManager::makeApiCall(const QString &ip) {
    QString apiUrl = "https://ipinfo.io/";
    QString token = "It's not wise to share this :>"; // Your token
    QString urlString = apiUrl + ip + "/json?token=" + token;

    QUrl url(urlString);
    QNetworkRequest request;
    request.setUrl(url);

    QNetworkReply *reply = networkManager->get(request);

    connect(reply, &QNetworkReply::finished, this, [this, reply, ip]() {
        if (reply->error() == QNetworkReply::NoError) {
            QJsonDocument jsonResponse = QJsonDocument::fromJson(reply->readAll());
            QJsonObject jsonObj = jsonResponse.object();

            QVariantMap apiData;
            apiData["hostname"] = jsonObj["hostname"].toString();
            apiData["city"] = jsonObj["city"].toString();
            apiData["region"] = jsonObj["region"].toString();
            apiData["country"] = jsonObj["country"].toString();
            apiData["loc"] = jsonObj["loc"].toString();
            apiData["postal"] = jsonObj["postal"].toString();
            apiData["timezone"] = jsonObj["timezone"].toString();

            // Save to database
            if (!DatabaseManager::instance().saveUniqueAddress(ip, apiData)) {
                emit debugMessage("Address already exists in the database: " + ip);
            } else {
                emit debugMessage("Address saved successfully: " + ip);
            }

            emit apiResponseReceived(
                ip,
                apiData["hostname"].toString(),
                apiData["city"].toString(),
                apiData["region"].toString(),
                apiData["country"].toString(),
                apiData["loc"].toString(),
                apiData["postal"].toString(),
                apiData["timezone"].toString()
                );
        } else {
            emit debugMessage("Error fetching data: " + reply->errorString());
        }
        reply->deleteLater();
    });
}

void NetworkManager::resolveLocalhost() {
    QNetworkRequest request(QUrl("https://api.ipify.org?format=json"));

    QNetworkReply *reply = networkManager->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            QJsonDocument jsonResponse = QJsonDocument::fromJson(reply->readAll());
            QJsonObject jsonObj = jsonResponse.object();
            QString publicIP = jsonObj["ip"].toString();

            if (!publicIP.isEmpty()) {
                emit localhostResolved(publicIP);
            } else {
                emit debugMessage("Failed to resolve public IP.");
            }
        } else {
            emit debugMessage("Error resolving public IP: " + reply->errorString());
        }
        reply->deleteLater();
    });
}

void NetworkManager::checkConnectionStatus() {
    QNetworkRequest request(QUrl("https://www.google.com")); // Use a lightweight URL for testing
    QNetworkReply *reply = networkManager->head(request);

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        bool onlineStatus  = (reply->error() == QNetworkReply::NoError);

        // Use the setter to update and emit changes
        if (onlineStatus != isOnline()) {
            setOnline(onlineStatus); // Update the online status
        }
        reply->deleteLater();
    });
}

bool NetworkManager::isOnline() const {
    return online;
}

void NetworkManager::setOnline(bool onlineStatus) {
    if (online != onlineStatus) {
        online = onlineStatus;
        emit connectionStatusChanged(online); // Emit the signal for the change
        emit debugMessage("Connection status updated: " + QString(online ? "Online" : "Offline"));
    }
}

void NetworkManager::handleConnectionStatus(QNetworkReply *reply) {
    bool onlineStatus = (reply->error() == QNetworkReply::NoError);

    // Use the setter to update the connection status
    setOnline(onlineStatus);

    reply->deleteLater();
}
