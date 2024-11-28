import QtQuick
import QtQuick.Window
import QtQuick.Controls.Basic
import QtQuick.Layouts
import QtQuick.Dialogs
import Qt5Compat.GraphicalEffects
import validator 1.0

ApplicationWindow {
    id: appRoot
    width: 600
    height: 600
    maximumWidth: 600
    maximumHeight: 600
    visible: true
    title: qsTr("GeoCatch")
    color: "#f2f2f2"

    // Create an instance of Validator
    Validator {
        id: ipValidator

        onDebugMessage: console.log("Validator Debug:", message) // Print debug messages

        onValidationResult: function(valid, message, ip) {
            if (!valid) {
                messageDialog.title = "Error"
                messageDialog.text = message
                messageDialog.open()
            } else {
                console.log(message);  // Can show success message in the UI
            }
        }

        onApiResponseReceived: function(ip, hostname, city, region, country, loc, postal, timezone) {
            // Update appObject properties with the API response data
            appObject.ip = ip;
            appObject.hostname = hostname;
            appObject.city = city;
            appObject.region = region;
            appObject.country = country;
            appObject.loc = loc;
            appObject.postal = postal;
            appObject.timezone = timezone;

            // Log the response to the console for debugging
            console.log("API Response: ", ip, hostname, city, region, country, loc, postal, timezone);
        }

        onDatabaseError: function(error) {
               console.error("Database Error:", error);
               messageDialog.title = "Database Error";
               messageDialog.text = error;
               messageDialog.open();
           }

        onConnectionStatusChanged: function(isOnline) {
            console.log("Connection status updated:", isOnline ? "Online" : "Offline");

            // Update the connection icon
            connectionIcon.source = isOnline
                ? "qrc:/resources/online.png"
                : "qrc:/resources/offline.png";

            // Update label text
            offlineMessage.text = isOnline
                ? ""
                : "Offline Mode: Searching database instead of the internet.";

            // Update visibility of the label
            offlineMessage.visible = !isOnline;
        }
    }

    // Define MessageDialog to show the result
    MessageDialog {
        id: messageDialog
        title: "Validation Result"
        text: ""
        // icon: MessageDialog.Information
        // standardButton: MessageDialog.Ok
    }

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

    function stringFixer(variable) {
        return variable.replace(/['"]+/g, '\n');
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
                            text: qsTr("Loc:")
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
                                        focus: true  // Automatically focus when the user clicks the input box
                                }

                                Item { Layout.fillWidth: true; }
                            }

                            Item { width: 10; }
                        }
                    }
                    Item{}

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

    Image {
        id: connectionIcon
        source: "qrc:/resources/online.png" // Default to offline
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
