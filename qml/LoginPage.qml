import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material

/**
 * Login page, matched to the original WPF LoginWindow layout:
 * full-screen light-grey background; left 65% brand panel; right 35% holds a
 * floating white login card.
 */
Item {
    id: loginPage
    property string title: qsTr("Login")

    // Unified light-grey background (matches WPF #E0E0E0 gradient).
    Rectangle {
        anchors.fill: parent
        color: "#E0E0E0"
    }

    // Left brand area (65%).
    Item {
        id: brandArea
        width: parent.width * 0.65
        height: parent.height

        Column {
            anchors.centerIn: parent
            // Vertical spacing matches WPF (Margin top 50 between each item).
            spacing: 50

            // Real product logo (from original WPF Resources/dklogo.png).
            Image {
                id: logoImg
                source: "qrc:/qml/assets/dklogo.png"
                width: 500
                height: 220
                fillMode: Image.PreserveAspectFit
                anchors.horizontalCenter: parent.horizontalCenter
                smooth: true
            }

            // Device name from config.json (matches WPF DeviceName, 64px).
            Label {
                text: deviceService.deviceName()
                width: parent.width
                font.pixelSize: 64
                font.bold: true
                color: "#193660"
                horizontalAlignment: Text.AlignHCenter
            }

            // Manufacturer from config.json (matches WPF Manufacturer, 36px).
            Label {
                text: deviceService.manufacturer()
                width: parent.width
                font.pixelSize: 36
                font.bold: true
                color: "#444"
                horizontalAlignment: Text.AlignHCenter
            }
        }
    }

    // Right login card area (35%), anchored to the right of the brand area.
    Item {
        width: parent.width * 0.35
        height: parent.height
        anchors.left: brandArea.right

        // Simulate WPF MaterialDesign Elevation Dp12 (DropShadowEffect): shadow
        // is visible on all sides, strongest at the bottom. Qt has no
        // DropShadowEffect, so layered translucent rounded rects approximate it.
        // Negative margins extend the shadow beyond every edge of the card.
        Rectangle { radius: 16; color: "#2E000000"; anchors.fill: loginCard; anchors.leftMargin: -6; anchors.rightMargin: -6; anchors.topMargin: -2; anchors.bottomMargin: -18 }
        Rectangle { radius: 16; color: "#26000000"; anchors.fill: loginCard; anchors.leftMargin: -4; anchors.rightMargin: -4; anchors.topMargin: -1; anchors.bottomMargin: -12 }
        Rectangle { radius: 16; color: "#1C000000"; anchors.fill: loginCard; anchors.leftMargin: -2; anchors.rightMargin: -2; anchors.topMargin: 0;  anchors.bottomMargin: -7 }
        Rectangle { radius: 16; color: "#12000000"; anchors.fill: loginCard; anchors.leftMargin: -1; anchors.rightMargin: -1; anchors.topMargin: 0;  anchors.bottomMargin: -3 }
        // Login card (matches WPF Card: 400 wide, radius 15, Elevation Dp12).
        Rectangle {
            id: loginCard
            width: 400
            height: 460
            radius: 15
            color: "white"
            border.color: "#DDDDDD"
            anchors.centerIn: parent

            Column {
                anchors.centerIn: parent
                spacing: 16

                Label {
                    text: qsTr("User Login")
                    font.pixelSize: 32
                    font.bold: true
                    color: "#303F9F"
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                Row {
                    anchors.horizontalCenter: parent.horizontalCenter
                    spacing: 10
                    Label { text: "👤"; font.pixelSize: 22; color: "#303F9F"; anchors.verticalCenter: parent.verticalCenter }
                    ComboBox {
                        id: userBox
                        width: 270
                        height: 44
                        model: loginService.usernames()
                        font.pixelSize: 18
                    }
                }

                Row {
                    anchors.horizontalCenter: parent.horizontalCenter
                    spacing: 10
                    Label { text: "🔒"; font.pixelSize: 22; color: "#303F9F"; anchors.verticalCenter: parent.verticalCenter }
                    TextField {
                        id: passwordField
                        width: 270
                        height: 44
                        placeholderText: qsTr("Password")
                        echoMode: TextInput.Password
                        font.pixelSize: 18
                        onAccepted: signInButton.clicked()
                    }
                }

                Button {
                    id: signInButton
                    width: 320
                    height: 55
                    text: qsTr("Login")
                    font.pixelSize: 22
                    font.bold: true
                    // Raised Material button in the WPF PrimaryHueDark (#303F9F).
                    Material.background: "#303F9F"
                    Material.foreground: "white"
                    Material.elevation: 2
                    ToolTip.visible: hovered
                    ToolTip.text: qsTr("Click to sign in and enter the main menu")
                    onClicked: {
                        if (loginService.tryLogin(userBox.currentText, passwordField.text)) {
                            statusLabel.color = "#2e7d32"
                            statusLabel.text = qsTr("Welcome, ") + loginService.username() +
                                               (loginService.isAdmin() ? qsTr(" (admin)") : "")
                        } else {
                            statusLabel.color = "#c62828"
                            statusLabel.text = loginService.errorMessage()
                        }
                    }
                }

                Label {
                    id: statusLabel
                    width: 300
                    wrapMode: Text.Wrap
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: 14
                    color: "#c62828"
                    text: ""
                }

                Row {
                    anchors.horizontalCenter: parent.horizontalCenter
                    spacing: 8
                    Label {
                        text: qsTr("Language:")
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    ComboBox {
                        width: 150
                        model: ["English", "中文"]
                        currentIndex: lang.current() === "zh_CN" ? 1 : 0
                        onActivated: (index) => lang.setLanguage(index === 0 ? "en" : "zh_CN")
                    }
                }
            }
        }
    }

    // Power quit button, top-right corner (WPF LoginWindow QuitButton). The
    // login page has no header, so the button anchors to the page itself.
    Item {
        id: quitBtn
        width: 44
        height: 44
        property bool hovered: false
        anchors.top: parent.top
        anchors.topMargin: 20
        anchors.right: parent.right
        anchors.rightMargin: 20

        Rectangle {
            anchors.fill: parent
            radius: height / 2
            color: quitBtn.hovered ? "#193660" : "#1E2A5A"
            border.color: "white"
            border.width: 2
        }
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


