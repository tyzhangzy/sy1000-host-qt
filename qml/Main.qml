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
        // Custom Item: outer circle and inner icon both center on the same
        // parent so their y coordinates align exactly.
        Item {
            id: quitBtn
            width: 44
            height: 44
            property bool hovered: false
            visible: stack.depth === 1
            anchors.right: parent.right
            anchors.rightMargin: 16
            // Align the button's bottom with the title label's bottom (top-left),
            // then shift it down so its top stays inside the header.
            anchors.bottom: titleLabel.bottom
            transform: Translate { y: 6 }

            // Outer circle (background) - fills the Item.
            Rectangle {
                anchors.fill: parent
                radius: height / 2
                color: quitBtn.hovered ? "#193660" : "#1E2A5A"
                border.color: "white"
                border.width: 2
            }
            // Inner power glyph - centered on the same Item; small -1px lift to
            // optically center the glyph (its font metrics sit slightly low).
            Label {
                anchors.centerIn: parent
                anchors.verticalCenterOffset: -1
                text: "⏻"
                color: "white"
                font.pixelSize: 26
            }

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor
                onEntered: quitBtn.hovered = true
                onExited: quitBtn.hovered = false
                onClicked: Qt.quit()
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


