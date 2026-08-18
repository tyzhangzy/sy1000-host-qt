import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

// System maintain page: matches the WPF SystemMaintainWindow layout.
// Left = device controls (water jacket locks, pumps), right = system info.
Page {
    id: sysPage
    property string title: qsTr("System Maintain")

    // Top bar: back button + title.
    Rectangle {
        id: topBar
        width: parent.width
        height: 56
        color: "#303F9F"
        Row {
            anchors.left: parent.left
            anchors.leftMargin: 12
            anchors.verticalCenter: parent.verticalCenter
            Button {
                text: qsTr("Back")
                font.bold: true
                onClicked: stack.pop()
            }
        }
        Label {
            anchors.centerIn: parent
            text: qsTr("System Maintain")
            color: "white"
            font.pixelSize: 24
            font.bold: true
        }
    }

    // Main content: left controls, right system info.
    Row {
        anchors.top: topBar.bottom
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 16
        spacing: 12

        // Left: device control card.
        Rectangle {
            width: parent.width * 2 / 3
            height: parent.height
            radius: 10
            border.color: "#CCCCCC"
            color: "white"
            clip: true

            Column {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 14

                Label {
                    text: qsTr("Device Controls")
                    font.pixelSize: 20
                    font.bold: true
                }

                // Connect button.
                Row {
                    spacing: 10
                    Button {
                        text: qsTr("Connect Devices")
                        onClicked: connLabel.text = deviceService.connectDevices()
                    }
                    Label {
                        id: connLabel
                        anchors.verticalCenter: parent.verticalCenter
                        color: "#666"
                        text: ""
                    }
                }

                // Water jacket locks 1-4.
                Label { text: qsTr("Water Jacket Locks"); font.bold: true; font.pixelSize: 16 }
                Grid {
                    columns: 2
                    spacing: 10
                    Repeater {
                        model: 4
                        Row {
                            spacing: 8
                            Label { text: qsTr("Lock %1").arg(index + 1); width: 70; anchors.verticalCenter: parent.verticalCenter }
                            Switch {
                                onToggled: deviceService.setWaterJacketLock(index + 1, checked)
                            }
                        }
                    }
                }

                // Pumps & water inlet.
                Label { text: qsTr("Pumps"); font.bold: true; font.pixelSize: 16 }
                Column {
                    spacing: 8
                    Row {
                        spacing: 8
                        Label { text: qsTr("Water Inlet"); width: 90; anchors.verticalCenter: parent.verticalCenter }
                        Switch { onToggled: deviceService.setWaterInlet(checked) }
                    }
                    Row {
                        spacing: 8
                        Label { text: qsTr("Fast Pump"); width: 90; anchors.verticalCenter: parent.verticalCenter }
                        Switch { onToggled: deviceService.setFastPump(checked) }
                    }
                    Row {
                        spacing: 8
                        Label { text: qsTr("Slow Pump"); width: 90; anchors.verticalCenter: parent.verticalCenter }
                        Switch { onToggled: deviceService.setSlowPump(checked) }
                    }
                }
            }
        }

        // Right: system info card.
        Rectangle {
            width: parent.width / 3
            height: parent.height
            radius: 10
            border.color: "#CCCCCC"
            color: "white"
            clip: true
            Column {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 12
                Label {
                    text: qsTr("System Information")
                    font.pixelSize: 20
                    font.bold: true
                    anchors.horizontalCenter: parent.horizontalCenter
                }
                Rectangle {
                    width: parent.width
                    height: 150
                    radius: 6
                    border.color: "#DDDDDD"
                    color: "#FAFAFA"
                    Label {
                        anchors.fill: parent
                        anchors.margins: 12
                        text: deviceService.systemInfo()
                        wrapMode: Text.Wrap
                        font.pixelSize: 14
                        color: "#333"
                    }
                }
            }
        }
    }
}
