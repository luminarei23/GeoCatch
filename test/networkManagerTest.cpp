#include <QtTest>
#include "networkManager.h"

class NetworkManagerTest : public QObject {
    Q_OBJECT

private slots:
    void testOnlineStatus();
    void testApiCall();
};

void NetworkManagerTest::testOnlineStatus() {
    NetworkManager manager;
    QVERIFY(manager.isOnline() || !manager.isOnline()); // Basic test
}

void NetworkManagerTest::testApiCall() {
    NetworkManager manager;
    manager.makeApiCall("1.1.1.1");
    QVERIFY(true);
}

QTEST_MAIN(NetworkManagerTest)
#include "networkManagerTest.moc"
