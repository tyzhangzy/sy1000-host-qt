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
                id: testRect
                width: parent.width
                implicitHeight: testCol.implicitHeight + 40
                radius: 10
                color: "white"
                border.color: "#DDDDDD"
                Column {
                    id: testCol
                    width: parent.width
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.top: parent.top
                    anchors.margins: 20
                    spacing: 12
                    Label {
                        text: qsTr("Test Functions")
                        font.pixelSize: 24
                        font.bold: true
                        color: "#303F9F"
                    }
                    Rectangle { width: parent.width; height: 1; color: "#303F9F" }
                    // Test-function card (reused component, data-driven).
                    Repeater {
                        model: [
                            { icon: "▶", text: qsTr("Start Hydrostatic Test"), page: "TestPreparationPage.qml" }
                        ]
                        delegate: ShadowCard {
                            title: modelData.text
                            icon: modelData.icon
                            onClicked: stack.push(modelData.page)
                        }
                    }
                }
            }

            // ===== Management System section =====
            Rectangle {
                id: mgmtRect
                width: parent.width
                implicitHeight: mgmtCol.implicitHeight + 40
                radius: 10
                color: "white"
                border.color: "#DDDDDD"
                Column {
                    id: mgmtCol
                    width: parent.width
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.top: parent.top
                    anchors.margins: 20
                    spacing: 12
                    Label {
                        text: qsTr("Management System")
                        font.pixelSize: 24
                        font.bold: true
                        color: "#303F9F"
                    }
                    Rectangle { width: parent.width; height: 1; color: "#303F9F" }
                    // Management cards (reused component, data-driven by index).
                    Row {
                        spacing: 20
                        Repeater {
                            model: [
                                { icon: "≡", text: qsTr("Result Management"), page: "ResultManagementPage.qml" },
                                { icon: "☺", text: qsTr("User Management"), page: "UserManagementPage.qml" },
                                { icon: "⚙", text: qsTr("System Maintain"), page: "SystemMaintainPage.qml" }
                            ]
                            delegate: ShadowCard {
                                title: modelData.text
                                icon: modelData.icon
                                onClicked: stack.push(modelData.page)
                            }
                        }
                    }
                }
            }
        }
    }

    // Bottom-right: return-to-main-menu button (WPF LogoutButton, with arrow).
    Button {
        id: logoutBtn
        text: "→  " + qsTr("Return to Main Menu")
        width: 240; height: 60
        anchors.right: parent.right; anchors.bottom: parent.bottom
        anchors.margins: 50
        font.pixelSize: 20; font.bold: true
        onClicked: stack.replace("LoginPage.qml")
    }
}

