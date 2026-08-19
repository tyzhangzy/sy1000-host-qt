import QtQuick
import QtQuick.Controls

ApplicationWindow {
    id: root
    title: qsTr("SY1000 - Hydrostatic Test System")
    width: 1920
    height: 1080
    visibility: Window.Maximized
    minimumWidth: 1280
    minimumHeight: 800
    visible: true

    // Top title bar (matches WPF MenuWindow ColorZone header). Hidden on the
    // login page (WPF LoginWindow is frameless with no header).
    header: Rectangle {
        height: 60
        color: "#303F9F"
        visible: stack.depth > 1
        Row {
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.verticalCenter: parent.verticalCenter
            Label {
                text: qsTr("SY1000")
                color: "white"
                font.pixelSize: 26
                font.bold: true
            }
            Label {
                id: titleLabel
                text: "  |  " + stack.currentItem.title
                color: "#EEEEEE"
                font.pixelSize: 20
                anchors.verticalCenter: parent.verticalCenter
            }
        }
        Label {
            anchors.right: parent.right
            anchors.rightMargin: 20
            anchors.verticalCenter: parent.verticalCenter
            text: loginService.username() !== "" ? loginService.username() : ""
            color: "white"
            font.pixelSize: 18
        }
    }

    StackView {
        id: stack
        anchors.fill: parent
        initialItem: "LoginPage.qml"
    }

    // After a successful login, navigate to the main menu.
    Connections {
        target: loginService
        function onLoginSucceeded() {
            stack.replace("MainMenuPage.qml")
        }
    }
}


