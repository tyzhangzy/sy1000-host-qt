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
                    // Test-function card (WPF RaisedSecondaryButton = Amber).
                    Repeater {
                        model: [
                            { icon: "▶", text: qsTr("Start Hydrostatic Test"), page: "TestPreparationPage.qml" }
                        ]
                        delegate: ShadowCard {
                            title: modelData.text
                            icon: modelData.icon
                            accent: "#FFC107"
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
                                { icon: "👥", text: qsTr("User Management"), page: "UserManagementPage.qml" },
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

    // Bottom-right: log-out-menu button (WPF LogoutButton: RaisedDarkButton + ExitToApp).
    Button {
        id: logoutBtn
        width: 240; height: 60
        anchors.right: parent.right; anchors.bottom: parent.bottom
        anchors.margins: 50
        onClicked: stack.replace("LoginPage.qml")

        contentItem: Item {
            anchors.fill: parent
            Row {
                anchors.centerIn: parent
                spacing: 8
                Label { text: "⇤"; color: "white"; font.pixelSize: 26 }
                Label { text: qsTr("Log out Menu"); color: "white"; font.pixelSize: 20; font.bold: true }
            }
        }
        background: Rectangle {
            radius: 6
            color: logoutBtn.hovered ? "#1A237E" : "#283593"
            Behavior on color { ColorAnimation { duration: 150 } }
        }
    }
}

