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

    // Top title bar (matches WPF MenuWindow ColorZone header).
    header: Rectangle {
        height: 60
        color: "#303F9F"
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
            anchors.right: quitBtn.left
            anchors.rightMargin: 16
            anchors.verticalCenter: parent.verticalCenter
            text: loginService.username() !== "" ? loginService.username() : ""
            color: "white"
            font.pixelSize: 18
        }

        // Power-style quit button, window top-right (matches WPF LoginWindow
        // QuitButton). Shown only on the login page (first stack item).
        Button {
            id: quitBtn
            width: 44
            height: 44
            visible: stack.depth === 1
            anchors.right: parent.right
            anchors.rightMargin: 16
            // Align the button's bottom with the title label's bottom (top-left).
            anchors.bottom: titleLabel.bottom
            onClicked: Qt.quit()

            contentItem: Label {
                text: "⏻"
                color: "white"
                font.pixelSize: 26
                anchors.centerIn: parent
            }
            background: Rectangle {
                radius: height / 2
                color: quitBtn.hovered ? "#193660" : "#1E2A5A"
                border.color: "white"
                border.width: 2
            }
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


