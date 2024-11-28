import QtQuick
import QtQuick.Window
import QtQuick.Controls.Basic
import QtQuick.Layouts
import QtQuick.Dialogs
import Qt5Compat.GraphicalEffects
import validator 1.0
import networkmanager 1.0

ApplicationWindow {
    id: appRoot
    width: 600
    height: 600
    maximumWidth: 600
    maximumHeight: 600
    visible: true
    title: qsTr("GeoCatch")
    color: "#f2f2f2"

    Component.onCompleted: {
        console.log("networkManager:", networkManager);
        if (networkManager) {
            console.log("Connected to networkManager signals.");
        } else {
            console.error("NetworkManager is not properly initialized in QML.");
        }
    }
    // Validator component for input validation and database interactions
    Validator {
        id: ipValidator

        // Debugging messages from Validator
        onDebugMessage: function(message) {
            console.log("Validator Debug:", message);
        }

        // Input validation results
        onValidationResult: function(valid, message, ip) {
            if (!valid) {
                messageDialog.title = "Error";
                messageDialog.text = message;
                messageDialog.open();
            } else {
                console.log(message);
            }
        }

        // Debugging database-related errors
        onDatabaseError: function(error) {
            console.error("Database Error:", error);
            messageDialog.title = "Database Error";
            messageDialog.text = error;
            messageDialog.open();
        }
    }

    Connections {
        target: ipValidator

        // Handle the API response forwarded from Validator
        onApiResponseReceived: function(ip, hostname, city, region, country, loc, postal, timezone) {
            appObject.ip = ip;
            appObject.hostname = hostname || "Unknown";
            appObject.city = city || "Unknown";
            appObject.region = region || "Unknown";
            appObject.country = country || "Unknown";
            appObject.loc = loc || "Unknown";
            appObject.postal = postal || "Unknown";
            appObject.timezone = timezone || "Unknown";

            console.log("API Response Received:", ip, hostname, city, region, country, loc, postal, timezone);
        }
    }

    // NetworkManager component for handling network-related functionality
    NetworkManager {
        id: networkManager

        onDebugMessage: console.log("NetworkManager Debug:", message)

        onApiResponseReceived: function(ip, apiData) {
            appObject.ip = ip;
            appObject.hostname = apiData.hostname || "Unknown";
            appObject.city = apiData.city || "Unknown";
            appObject.region = apiData.region || "Unknown";
            appObject.country = apiData.country || "Unknown";
            appObject.loc = apiData.loc || "Unknown";
            appObject.postal = apiData.postal || "Unknown";
            appObject.timezone = apiData.timezone || "Unknown";

            console.log("API Response: ", ip, apiData);
        }

        // Connection status updates
        onConnectionStatusChanged: function(isOnline) {
            console.log("Connection status updated:", isOnline ? "Online" : "Offline");

            // Update connection icon and label
            connectionIcon.source = isOnline
                ? "qrc:/resources/online.png"
                : "qrc:/resources/offline.png";

            offlineMessage.text = isOnline
                ? ""
                : "Offline Mode: Searching database instead of the internet.";

            offlineMessage.visible = !isOnline;
        }

        // Localhost resolved to public IP
        onLocalhostResolved: function(publicIP) {
            console.log("Resolved Localhost IP:", publicIP);
            // Trigger validation with the resolved IP
            ipValidator.validateInput(publicIP);
        }
    }

    // Define MessageDialog to show the result
    MessageDialog {
        id: messageDialog
        title: "Validation Result"
        text: ""
    }
    // All entries
    QtObject {
        id: appObject

        property string ip: "0.0.0.0"
        property string hostname: "localhost"
        property string city: "Unknown"
        property string region: "Unknown"
        property string country: "Unknown"
        property string loc: "Unknown"
        property string postal: "Unknown"
        property string timezone: "Unknown"
    }

    QtObject {
        id: fontSystem

        property int h1: 24
        property int h2: 18
        property int small: 12
        property int button: 20
        property int button_small: 14
    }

    Pane {
        anchors.fill: parent
        background: appRoot.background

        ColumnLayout {
            width: parent.width
            Layout.fillWidth: true
            anchors.centerIn: parent

            spacing: 16

            Text {

                font.pixelSize: fontSystem.h1
                font.bold: true
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                text: qsTr("The trusted app for IP address data")
                color: "#171c26"
            }
            Text {

                font.pixelSize: fontSystem.h2
                font.bold: false
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                text: qsTr("Accurate IP discovery with one click!")
                color: "#171c26"
            }
            Item {
                id: mainItem
                width: 430
                implicitHeight: 470
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                Behavior on implicitHeight {
                    NumberAnimation { duration: 200; }
                }

                RectangularGlow {
                    id: effect
                    anchors.fill: mainBorder
                    glowRadius: 32
                    spread: 0.1
                    color: "#dfdfdf"
                    cornerRadius: mainBorder.radius + glowRadius
                }

                Rectangle {
                    id: mainBorder
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    anchors.fill: parent
                    color: "#ffffff"
                    border.width: 1
                    border.color: "#dfdfdf"
                    radius: 15
                }

                ColumnLayout {
                    width: parent.width
                    Layout.fillWidth: true
                    spacing: 10

                    Item { height: 20; }

                    RowLayout {
                        width: parent.width
                        Layout.fillWidth: true

                        Item { }

                    }

                    RowLayout {
                        width: parent.width
                        Layout.fillWidth: true
                        spacing: 5
                        Item { width: 30; }

                        Item { width: 2; }

                        Text {

                            font.pixelSize: 14
                            text: qsTr("Hostname:")
                        }
                        Text {

                            font.pixelSize: 14
                            color: "#007aff"
                            text: qsTr(appObject.hostname)
                        }

                        Item { Layout.fillWidth: true; }
                    }

                    RowLayout {
                        width: parent.width
                        Layout.fillWidth: true
                        spacing: 5
                        Item { width: 30; }


                        Item { width: 2; }

                        Text {

                            font.pixelSize: 14
                            text: qsTr("City:")
                        }
                        Text {

                            font.pixelSize: 14
                            color: "#007aff"
                            text: qsTr(appObject.city)
                        }

                        Item { Layout.fillWidth: true; }
                    }

                    RowLayout {
                        width: parent.width
                        Layout.fillWidth: true
                        spacing: 5
                        Item { width: 30; }

                        Item { width: 2; }

                        Text {

                            font.pixelSize: 14
                            text: qsTr("Region:")
                        }
                        Text {

                            font.pixelSize: 14
                            color: "#007aff"
                            text: qsTr(appObject.region)
                        }

                        Item { Layout.fillWidth: true; }
                    }

                    RowLayout {
                        width: parent.width
                        Layout.fillWidth: true
                        spacing: 5
                        Item { width: 30; }

                        Item { width: 2; }

                        Text {

                            font.pixelSize: 14
                            text: qsTr("Country:")
                        }
                        Text {

                            font.pixelSize: 14
                            color: "#007aff"
                            text: qsTr(appObject.country)
                        }

                        Item { Layout.fillWidth: true; }
                    }

                    RowLayout {
                        width: parent.width
                        Layout.fillWidth: true
                        spacing: 5
                        Item { width: 30; }

                        Item { width: 2; }

                        Text {

                            font.pixelSize: 14
                            text: qsTr("Localization:")
                        }
                        Text {

                            font.pixelSize: 14
                            color: "#007aff"
                            text: qsTr(appObject.loc)
                        }

                        Item { Layout.fillWidth: true; }
                    }

                    RowLayout {
                        width: parent.width
                        Layout.fillWidth: true
                        spacing: 5
                        Item { width: 30; }

                        Item { width: 2; }

                        Text {

                            font.pixelSize: 14
                            text: qsTr("Postal code:")
                        }
                        Text {

                            font.pixelSize: 14
                            color: "#007aff"
                            text: qsTr(appObject.postal)
                        }

                        Item { Layout.fillWidth: true; }
                    }


                    RowLayout {
                        width: parent.width
                        Layout.fillWidth: true
                        spacing: 5
                        Item { width: 30; }

                        Item { width: 2; }

                        Text {

                            font.pixelSize: 14
                            text: qsTr("Timezone:")
                        }
                        Text {

                            font.pixelSize: 14
                            color: "#007aff"
                            text: qsTr(appObject.timezone)
                        }

                        Item { Layout.fillWidth: true; }
                    }

                    Item {}

                    Rectangle {
                        width: parent.width - 100
                        height: 38
                        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                        color: "#f1f1f1"
                        radius: 10

                        ColumnLayout {
                            anchors.fill: parent
                            Item { width: 10; }

                            RowLayout {
                                width: parent.width
                                Layout.fillWidth: true
                                spacing: 0
                                Item { width: 15; }

                                // Input box for entering IP address
                                TextInput {
                                    id: ipInput
                                        width: parent.width - 50
                                        height: 50
                                        font.pixelSize: 16
                                        color: "#333"
                                        anchors.fill: parent
                                        anchors.margins: 4
                                        focus: true
                                }

                                Item { Layout.fillWidth: true; }
                            }

                            Item { width: 10; }
                        }
                    }
                    Item{}
                    // detecting the IP call
                    Button {
                        id: shortButton
                        implicitWidth: parent.width / 1.2
                        implicitHeight: 38
                        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                        text: "Detect"

                        layer.enabled: true
                        layer.effect: DropShadow {
                            transparentBorder: true
                            horizontalOffset: 0
                            verticalOffset: 3
                            color: "#ccc"
                            radius: 16
                            samples: 32
                            spread: 0.0
                        }

                        background: Rectangle {
                            id: shortButtonBack
                            anchors.fill: parent
                            color: "#171c26"
                            radius: 6
                            z: 2
                        }

                        contentItem: Text {
                            text: shortButton.text
                            font.bold: false
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignHCenter
                            opacity: enabled ? 1.0 : 0.3
                            color: "#fff"
                            anchors.fill: parent
                            font.pixelSize: fontSystem.button
                            elide: Text.ElideRight
                            scale: shortButton.down ? 0.9 : 1.0
                            z: 3
                            Behavior on scale {
                                NumberAnimation { duration: 70; }
                            }
                        }
                        onClicked: {
                            busyIndicator.running = true;  // Start the busy indicator
                            ipValidator.validateInput(ipInput.text);  // Call the C++ validation function
                            ipInput.text = "";
                            }
                            Connections {
                                target: ipValidator

                                onRequestFinished: {
                                    busyIndicator.running = false;  // Stop the busy indicator
                                }
                            }
                        }
                    // displaying database call
                    Button {
                        id: viewDatabaseButton
                        implicitWidth: parent.width / 1.2
                        implicitHeight: 38
                        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                        text: "View Saved Data"

                        layer.enabled: true
                        layer.effect: DropShadow {
                            transparentBorder: true
                            horizontalOffset: 0
                            verticalOffset: 3
                            color: "#ccc"
                            radius: 16
                            samples: 32
                            spread: 0.0
                        }

                        background: Rectangle {
                            id: viewDatabaseButtonBack
                            anchors.fill: parent
                            color: "#171c26"
                            radius: 6
                            z: 2
                        }

                        contentItem: Text {
                            text: viewDatabaseButton.text
                            font.bold: false
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignHCenter
                            opacity: enabled ? 1.0 : 0.3
                            color: "#fff"
                            anchors.fill: parent
                            font.pixelSize: fontSystem.button_small
                            elide: Text.ElideRight
                            scale: viewDatabaseButton.down ? 0.9 : 1.0
                            z: 3
                            Behavior on scale {
                                NumberAnimation { duration: 70; }
                            }
                        }

                        onClicked: {
                            console.log("View Database button clicked");

                            savedAddressesModel.clear();
                            let addresses = ipValidator.retrieveData();
                            for (let address of addresses) {
                                savedAddressesModel.append({ address: address });
                            }
                            savedAddressesDialog.open();
                        }
                    }
                // clearing database call
                Button {
                    id: clearDatabase
                    implicitWidth: parent.width / 1.2
                    implicitHeight: 38
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    text: "Clear Database"

                    layer.enabled: true
                    layer.effect: DropShadow {
                        transparentBorder: true
                        horizontalOffset: 0
                        verticalOffset: 3
                        color: "#ccc"
                        radius: 16
                        samples: 32
                        spread: 0.0
                    }

                    background: Rectangle {
                        id: clearDatabaseBack
                        anchors.fill: parent
                        color: "#171c26"
                        radius: 6
                        z: 2
                    }

                    contentItem: Text {
                        text: clearDatabase.text
                        font.bold: false
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignHCenter
                        opacity: enabled ? 1.0 : 0.3
                        color: "#fff"
                        anchors.fill: parent
                        font.pixelSize: fontSystem.button_small
                        elide: Text.ElideRight
                        scale: clearDatabase.down ? 0.9 : 1.0
                        z: 3
                        Behavior on scale {
                            NumberAnimation { duration: 70; }
                        }
                    }

                    onClicked: {
                            console.log("Clear Database button clicked");
                            if (ipValidator.clearDatabase()) {
                                console.log("Database cleared successfully.");
                            } else {
                                console.warn("Failed to clear database.");
                            }
                        }
                    }
                Item{}
                }
            }
        }
        // busy logo animation
        BusyIndicator {
            id: busyIndicator
            width: 48
            height: 48
            running: false
            anchors.bottom: parent.bottom

            contentItem: Item {
                implicitWidth: 16
                implicitHeight: 16

                Item {
                    id: itemProgress
                    width: 32
                    height: 32
                    anchors.centerIn: parent
                    opacity: busyIndicator.running ? 1 : 0

                    Behavior on opacity {
                        OpacityAnimator {
                            duration: 250
                        }
                    }

                    RotationAnimator {
                        target: itemProgress
                        running: busyIndicator.visible && busyIndicator.running
                        from: 0
                        to: 360
                        loops: Animation.Infinite
                        duration: 1250
                    }

                    Repeater {
                        id: repeater
                        model: 3

                        Rectangle {
                            implicitWidth: 5
                            implicitHeight: 5
                            radius: implicitWidth / 2
                            color: "#171c26"

                            anchors.centerIn: itemProgress
                            transform: [
                                Translate {
                                    y: -itemProgress.height / 2 + 7
                                },
                                Rotation {
                                    angle: index / repeater.count * 360
                                    origin.x: implicitWidth / 2
                                    origin.y: implicitHeight / 2
                                }
                            ]
                        }
                    }
                }
            }
        }
        // view of the entries saved in database
        Dialog {
            id: savedAddressesDialog
            width: parent.width * 0.8
            height: parent.height * 0.6
            modal: true
            title: "Saved Data"

            ColumnLayout {
                anchors.fill: parent
                spacing: 10

                ListView {
                    id: savedAddressesListView
                    model: savedAddressesModel
                    clip: true
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    delegate: Rectangle {
                        width: parent.width
                        height: 40
                        color: index % 2 === 0 ? "#f2f2f2" : "#ffffff"
                        border.color: "#dfdfdf"
                        radius: 5
                        RowLayout {
                            anchors.fill: parent
                            spacing: 10
                            Text {
                                text: model.address
                                font.pixelSize: 14
                                Layout.fillWidth: true
                            }
                            Button {
                                text: "Copy"
                                Layout.alignment: Qt.AlignRight
                                onClicked: {
                                    Qt.callLater(() => {
                                        ipValidator.copyToClipboard(model.address);
                                        console.log("Address copied:", model.address);
                                    });
                                }
                            }
                        }
                    }
                }

                Button {
                    text: "Close"
                    Layout.alignment: Qt.AlignHCenter
                    onClicked: savedAddressesDialog.close();
                }
            }
        }
        // label displaying information about state of the app
        Label {
            id: offlineMessage
            visible: false  // Default to hidden
            text: ""        // Default empty text
            color: "red"
            anchors.top: connectionIcon.bottom
            anchors.left: parent.left
            anchors.margins: 135
        }

        ListModel {
            id: savedAddressesModel
        }
    }

    // connection indication icon
    Image {
        id: connectionIcon
        source: "qrc:/resources/online.png"
        width: 24
        height: 24
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.margins: 10
        ToolTip {
            text: connectionIcon.source === "qrc:/resources/online.png" ? "Online" : "Offline"
        }
    }
}
