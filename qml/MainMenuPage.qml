import QtQuick
import QtQuick.Controls

Page {
    id: menuPage
    property string title: qsTr("Main Menu")

    // Center: function card buttons (matches the WPF menu grid layout).
    Grid {
        anchors.centerIn: parent
        columns: 2
        spacing: 24

        Rectangle {
            width: 260; height: 150; radius: 12
            border.color: "#CCCCCC"; color: "white"
            Rectangle {
                width: 52; height: 52; radius: 26; color: "#303F9F"
                anchors.top: parent.top; anchors.topMargin: 18
                anchors.horizontalCenter: parent.horizontalCenter
                Label { text: "▶"; color: "white"; font.pixelSize: 26; anchors.centerIn: parent }
            }
            Label {
                text: qsTr("Start Hydrostatic Test")
                font.pixelSize: 18; font.bold: true
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom; anchors.bottomMargin: 20
            }
            MouseArea {
                anchors.fill: parent
                onClicked: stack.push("TestPreparationPage.qml")
            }
        }

        Rectangle {
            width: 260; height: 150; radius: 12
            border.color: "#CCCCCC"; color: "white"
            Rectangle {
                width: 52; height: 52; radius: 26; color: "#303F9F"
                anchors.top: parent.top; anchors.topMargin: 18
                anchors.horizontalCenter: parent.horizontalCenter
                Label { text: "≡"; color: "white"; font.pixelSize: 26; anchors.centerIn: parent }
            }
            Label {
                text: qsTr("Result Management")
                font.pixelSize: 18; font.bold: true
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom; anchors.bottomMargin: 20
            }
            MouseArea {
                anchors.fill: parent
                onClicked: stack.push("ResultManagementPage.qml")
            }
        }

        Rectangle {
            width: 260; height: 150; radius: 12
            border.color: "#CCCCCC"; color: "white"
            Rectangle {
                width: 52; height: 52; radius: 26; color: "#303F9F"
                anchors.top: parent.top; anchors.topMargin: 18
                anchors.horizontalCenter: parent.horizontalCenter
                Label { text: "☺"; color: "white"; font.pixelSize: 26; anchors.centerIn: parent }
            }
            Label {
                text: qsTr("User Management")
                font.pixelSize: 18; font.bold: true
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom; anchors.bottomMargin: 20
            }
            MouseArea {
                anchors.fill: parent
                onClicked: stack.push("UserManagementPage.qml")
            }
        }

        Rectangle {
            width: 260; height: 150; radius: 12
            border.color: "#CCCCCC"; color: "white"
            Rectangle {
                width: 52; height: 52; radius: 26; color: "#303F9F"
                anchors.top: parent.top; anchors.topMargin: 18
                anchors.horizontalCenter: parent.horizontalCenter
                Label { text: "⚙"; color: "white"; font.pixelSize: 26; anchors.centerIn: parent }
            }
            Label {
                text: qsTr("System Maintain")
                font.pixelSize: 18; font.bold: true
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom; anchors.bottomMargin: 20
            }
            MouseArea {
                anchors.fill: parent
                onClicked: stack.push("SystemMaintainPage.qml")
            }
        }
    }

    // Bottom-right: connection status + log out buttons (matches WPF MenuWindow).
    Button {
        id: connBtn
        text: qsTr("Connection Status")
        width: 200; height: 60
        anchors.right: logoutBtn.left; anchors.rightMargin: 20
        anchors.bottom: parent.bottom; anchors.bottomMargin: 50
        font.pixelSize: 18; font.bold: true
        onClicked: menuStatus.text = deviceService.connectDevices()
    }
    Button {
        id: logoutBtn
        text: qsTr("Log out")
        width: 200; height: 60
        anchors.right: parent.right; anchors.bottom: parent.bottom
        anchors.margins: 50
        font.pixelSize: 20; font.bold: true
        onClicked: stack.replace("LoginPage.qml")
    }

    Label {
        id: menuStatus
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom; anchors.bottomMargin: 20
        color: "#888"
        text: ""
    }
}

