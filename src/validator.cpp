#include <QUrl>
#include <QTimer>
#include <QClipboard>
#include <QGuiApplication>
#include <QHostInfo>

#include "validator.h"
#include "networkManager.h"
#include "databaseManager.h"

Validator::Validator(QObject *parent) : QObject(parent), networkManager(new NetworkManager(this)) {
    connect(networkManager, &NetworkManager::apiResponseReceived,
            this, &Validator::handleApiResponse);

    if (!DatabaseManager::instance().initializeDatabase()) {
        emit databaseError("Failed to initialize the database. Please check your setup.");
    }
}

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

bool Validator::isValidUrl(const QString &url) {
    QUrl parsedUrl(url);
    return parsedUrl.isValid() && !parsedUrl.host().isEmpty();
}

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
    return QString();
}

void Validator::validateInput(const QString &input) {
    emit debugMessage("Validation triggered for input: " + input);

    QString trimmedInput = input.trimmed();
    emit debugMessage("Trimmed Input: " + trimmedInput);

    // Resolve "localhost"
    if (trimmedInput.compare("localhost", Qt::CaseInsensitive) == 0) {
        emit debugMessage("Step 1: Resolving 'localhost' to public IP...");
        connect(networkManager, &NetworkManager::localhostResolved, this, [this](const QString &publicIP) {
            emit debugMessage("Resolved localhost to public IP: " + publicIP);
            networkManager->makeApiCall(publicIP);
            QTimer::singleShot(1000, this, &Validator::requestFinished);
        });
        networkManager->resolveLocalhost();
        return;
    }

    // Validate IP address
    if (isValidIpAddress(trimmedInput)) {
        emit debugMessage("Step 2: Detected as a valid IP address.");
        if (networkManager->isOnline()) {
            emit debugMessage("Online mode: Calling NetworkManager::makeApiCall.");
            networkManager->makeApiCall(trimmedInput);
            QTimer::singleShot(1000, this, &Validator::requestFinished);
        } else {
            queryDatabase(trimmedInput);
        }
        return;
    }

    // Validate and resolve URL
    QString normalizedUrl = normalizeUrl(trimmedInput);
    if (!normalizedUrl.isEmpty() && isValidUrl(normalizedUrl)) {
        QUrl url(normalizedUrl);
        emit debugMessage("Step 3: Detected as a valid URL. Host: " + url.host());
        if (networkManager->isOnline()) {
            emit debugMessage("Online mode: Resolving URL to IP and making API call.");
            QHostInfo::lookupHost(url.host(), this, [this, normalizedUrl](const QHostInfo &host) {
                if (host.error() == QHostInfo::NoError) {
                    for (const QHostAddress &address : host.addresses()) {
                        if (address.protocol() == QAbstractSocket::IPv4Protocol) {
                            QString ip = address.toString();
                            emit debugMessage("Step 4: URL resolved to IP: " + ip);
                            emit validationResult(true, "Valid URL. Resolved IP: " + ip, ip);
                            networkManager->makeApiCall(ip); // Use the resolved IP for the API call
                            QTimer::singleShot(1000, this, &Validator::requestFinished);
                            return;
                        }
                    }
                    emit debugMessage("Step 4: No valid IPv4 address found for host.");
                    emit validationResult(false, "No IPv4 address found for the host.", "");
                } else {
                    emit debugMessage("Step 4: Host resolution error: " + host.errorString());
                    emit validationResult(false, "Failed to resolve host: " + host.errorString(), "");
                }
                QTimer::singleShot(1000, this, &Validator::requestFinished);
            });
        } else {
            queryDatabase(url.host());
        }
        return;
    }

    // Invalid input
    emit debugMessage("Step 4: Invalid input. Not an IP address or valid URL.");
    emit validationResult(false, "Invalid input. Not an IP address or valid URL.", "");
    QTimer::singleShot(1000, this, &Validator::requestFinished);
}

void Validator::queryDatabase(const QString &input) {
    emit debugMessage("Offline mode: Searching database for input: " + input);
    QVariantMap data = DatabaseManager::instance().getSpecificAddressData(input);
    if (data.isEmpty()) {
        emit debugMessage("No data found in database for input: " + input);
        emit validationResult(false, "No data found in the database.", input);
    } else {
        emit debugMessage("Data retrieved from database for input: " + input);
        emit networkManager->apiResponseReceived(data["address"].toString(),
                                 data["hostname"].toString(),
                                 data["city"].toString(),
                                 data["region"].toString(),
                                 data["country"].toString(),
                                 data["loc"].toString(),
                                 data["postal"].toString(),
                                 data["timezone"].toString());
    }
    QTimer::singleShot(1000, this, &Validator::requestFinished);
}

QList<QString> Validator::retrieveData() {
    return DatabaseManager::instance().getAddressData();
}

bool Validator::clearDatabase() {
    return DatabaseManager::instance().dropDatabase();
}

void Validator::copyToClipboard(const QString &text) {
    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setText(text, QClipboard::Clipboard);
    emit debugMessage("Copied to clipboard: " + text);
}

void Validator::handleApiResponse(const QString &ip, const QString &hostname, const QString &city,
                                  const QString &region, const QString &country, const QString &loc,
                                  const QString &postal, const QString &timezone) {
    // Forward the signal to QML
    emit debugMessage("Validator forwarding API response to QML.");
    emit apiResponseReceived(ip, hostname, city, region, country, loc, postal, timezone);
}
