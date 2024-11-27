#include "validator.h"
#include "databasemanager.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QHostInfo>
#include <QNetworkInterface>
#include <QDebug>
#include <QProcess>
#include <QTimer>

Validator::Validator(QObject *parent) : QObject(parent) {
    networkManager = new QNetworkAccessManager(this);
}

// Check if the input is a valid IPv4 address
bool Validator::isValidIpAddress(const QString &ip) {
    QStringList parts = ip.split('.');
    if (parts.size() != 4) return false;

    for (const QString &part : parts) {
        bool ok;
        int number = part.toInt(&ok);
        if (!ok || number < 0 || number > 255) return false;
    }

    return true;
}

// Check if the input is a valid URL
bool Validator::isValidUrl(const QString &url) {
    QUrl parsedUrl(url);
    return parsedUrl.isValid() && !parsedUrl.host().isEmpty();
}

// Normalize the input to a valid URL with https:// or https://www. if necessary
QString Validator::normalizeUrl(const QString &input) {
    QString trimmedInput = input.trimmed();
    emit debugMessage("Trimmed Input: " + trimmedInput);

    if (!trimmedInput.startsWith("http://") && !trimmedInput.startsWith("https://")) {
        if (trimmedInput.startsWith("www.")) {
            trimmedInput = "https://" + trimmedInput;
        } else {
            trimmedInput = "https://www." + trimmedInput;
        }
    }
    emit debugMessage("Normalized Input with Scheme: " + trimmedInput);

    QUrl url(trimmedInput);
    if (url.isValid() && !url.host().isEmpty()) {
        return url.toString();
    }

    emit debugMessage("Invalid URL after normalization: " + trimmedInput);
    return QString(); // Return an empty string if normalization fails
}

void Validator::validateInput(const QString &input) {
    emit debugMessage("Validation triggered for input: " + input);

    QString trimmedInput = input.trimmed();
    emit debugMessage("Trimmed Input: " + trimmedInput);

    // Step 1: Easter egg for "localhost"
    if (trimmedInput.compare("localhost", Qt::CaseInsensitive) == 0) {
        emit debugMessage("Step 1: Easter egg triggered: Resolving public IP...");
        handleLocalhost();
        QTimer::singleShot(1000, this, &Validator::requestFinished);
        return;
    }

    // Step 2: Check if it's a valid IP address
    if (isValidIpAddress(trimmedInput)) {
        emit debugMessage("Step 2: Detected as a valid IP address.");
        emit validationResult(true, "Valid IP address.", trimmedInput);
        makeApiCall(trimmedInput); // Directly make the API call for the IP address
        QTimer::singleShot(1000, this, &Validator::requestFinished);
        return;
    }

    // Step 3: Normalize and validate as a URL
    QString normalizedUrl = normalizeUrl(trimmedInput);
    if (!normalizedUrl.isEmpty() && isValidUrl(normalizedUrl)) {
        QUrl url(normalizedUrl);
        emit debugMessage("Step 3: Detected as a valid URL. Host: " + url.host());

        // Resolve the host to an IP address
        QHostInfo::lookupHost(url.host(), this, [this, normalizedUrl](const QHostInfo &host) {
            if (host.error() == QHostInfo::NoError) {
                for (const QHostAddress &address : host.addresses()) {
                    if (address.protocol() == QAbstractSocket::IPv4Protocol) {
                        QString ip = address.toString();
                        emit debugMessage("Step 4: URL resolved to IP: " + ip);
                        emit validationResult(true, "Valid URL. Resolved IP: " + ip, ip);
                        makeApiCall(ip); // Use the resolved IP for the API call
                        QTimer::singleShot(1000, this, &Validator::requestFinished);
                        return;
                    }
                }
                emit debugMessage("Step 4: No valid IPv4 address found for host.");
                emit validationResult(false, "No IPv4 address found for the host.", "");
                QTimer::singleShot(1000, this, &Validator::requestFinished);
            } else {
                emit debugMessage("Step 4: Host resolution error: " + host.errorString());
                emit validationResult(false, "Failed to resolve host: " + host.errorString(), "");
                QTimer::singleShot(1000, this, &Validator::requestFinished);
            }
        });
        return;
    }

    // Step 4: Invalid input
    emit debugMessage("Step 4: Invalid input. Not an IP address or valid URL.");
    emit validationResult(false, "Invalid input. Not an IP address or valid URL.", "");
    QTimer::singleShot(1000, this, &Validator::requestFinished);
}



// Make an API call to fetch information for the given IP address
void Validator::makeApiCall(const QString &ip) {
    QString apiUrl = "https://ipinfo.io/";
    QString token = "dbd9e958df8fb5"; // Your token
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

void Validator::handleLocalhost() {
    QString publicIpApi = "https://api.ipify.org?format=json"; // API to get public IP

    QNetworkRequest request;
    request.setUrl(QUrl(publicIpApi)); // Explicitly set the URL

    QNetworkReply *reply = networkManager->get(request);

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            QJsonDocument jsonResponse = QJsonDocument::fromJson(reply->readAll());
            QJsonObject jsonObj = jsonResponse.object();

            QString publicIP = jsonObj["ip"].toString();
            if (!publicIP.isEmpty()) {
                emit debugMessage("Resolved Public IP: " + publicIP);
                makeApiCall(publicIP); // Send the public IP to your geolocation API
            } else {
                emit debugMessage("Failed to resolve public IP.");
                emit validationResult(false, "Could not resolve public IP for localhost.", "");
            }
        } else {
            emit debugMessage("Error resolving public IP: " + reply->errorString());
            emit validationResult(false, "Error resolving public IP: " + reply->errorString(), "");
        }
        reply->deleteLater();
    });
}

QVariantMap Validator::retrieveData(const QString &address) {
    return DatabaseManager::instance().getAddressData(address);
}

bool Validator::clearDatabase() {
    return DatabaseManager::instance().dropDatabase();
}
