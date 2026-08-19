import QtQuick
import QtQuick.Controls

Page {
    id: menuPage
    title: qsTr("Main Menu")

    // Scrollable content: two sections (Test / Management), like WPF MenuWindow.
    Flickable {
        anchors.fill: parent
        anchors.topMargin: 24
        contentHeight: col.implicitHeight + 160
        clip: true

        Column {
            id: col
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: 40
            spacing: 20

            // ===== Test Functions section =====
            Rectangle {
                width: parent.width
                radius: 10
                color: "white"
                border.color: "#DDDDDD"
                Column {
                    anchors.fill: parent
                    anchors.margins: 20
                    spacing: 12
                    Label {
                        text: qsTr("Test Functions")
                        font.pixelSize: 24
                        font.bold: true
                        color: "#303F9F"
                    }
                    Rectangle { width: parent.width; height: 1; color: "#303F9F" }
                    ShadowCard {
                        title: qsTr("Start Hydrostatic Test")
                        icon: "▶"
                        onClicked: stack.push("TestPreparationPage.qml")
                    }
                }
            }

            // ===== Management System section =====
            Rectangle {
                width: parent.width
                radius: 10
                color: "white"
                border.color: "#DDDDDD"
                Column {
                    anchors.fill: parent
                    anchors.margins: 20
                    spacing: 12
                    Label {
                        text: qsTr("Management System")
                        font.pixelSize: 24
                        font.bold: true
                        color: "#303F9F"
                    }
                    Rectangle { width: parent.width; height: 1; color: "#303F9F" }
                    Row {
                        spacing: 20
                        ShadowCard {
                            title: qsTr("Result Management")
                            icon: "≡"
                            onClicked: stack.push("ResultManagementPage.qml")
                        }
                        ShadowCard {
                            title: qsTr("User Management")
                            icon: "☺"
                            onClicked: stack.push("UserManagementPage.qml")
                        }
                        ShadowCard {
                            title: qsTr("System Maintain")
                            icon: "⚙"
                            onClicked: stack.push("SystemMaintainPage.qml")
                        }
                    }
                }
            }
        }
    }

    // Bottom-right: log out button (WPF LogoutButton).
    Button {
        id: logoutBtn
        text: qsTr("Log out")
        width: 200; height: 60
        anchors.right: parent.right; anchors.bottom: parent.bottom
        anchors.margins: 50
        font.pixelSize: 20; font.bold: true
        onClicked: stack.replace("LoginPage.qml")
    }
    // Connection status button (WPF header ConnectionStatusButton).
    Button {
        id: connBtn
        text: qsTr("Connection Status")
        width: 200; height: 60
        anchors.right: logoutBtn.left; anchors.rightMargin: 20
        anchors.bottom: parent.bottom; anchors.bottomMargin: 50
        font.pixelSize: 18; font.bold: true
        onClicked: menuStatus.text = deviceService.connectDevices()
    }

    Label {
        id: menuStatus
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom; anchors.bottomMargin: 20
        color: "#888"
        text: ""
    }
}

