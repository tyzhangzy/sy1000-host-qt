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

        ShadowCard {
            title: qsTr("Start Hydrostatic Test")
            icon: "▶"
            onClicked: stack.push("TestPreparationPage.qml")
        }
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

