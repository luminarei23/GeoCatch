#ifndef VALIDATOR_H
#define VALIDATOR_H

#include <QObject>
#include <QString>
#include <QNetworkAccessManager>
#include <QHostInfo>
#include <QTimer>
#include <QUrl>

class Validator : public QObject {
    Q_OBJECT

public:
    explicit Validator(QObject *parent = nullptr);

    Q_INVOKABLE void validateInput(const QString &input); // Validate both IP and URL inputs
    Q_INVOKABLE QVariantMap retrieveData(const QString &address);
    Q_INVOKABLE bool clearDatabase();

private:
    QNetworkAccessManager *networkManager;

    bool isValidIpAddress(const QString &ip); // Check for IPv4 addresses
    bool isValidUrl(const QString &url);      // Check for valid URLs
    QString normalizeUrl(const QString &input);
    void makeApiCall(const QString &ip);
    void handleLocalhost();

signals:
    void debugMessage(const QString &message);
    void validationResult(bool success, const QString &message, const QString &resolvedValue);
    void apiResponseReceived(const QString &ip, const QString &hostname, const QString &city,
                             const QString &region, const QString &country, const QString &loc,
                             const QString &postal, const QString &timezone);
    void requestFinished();
};

#endif // VALIDATOR_H
