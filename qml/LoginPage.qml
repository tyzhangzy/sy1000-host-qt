import QtQuick
import QtQuick.Controls

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
            spacing: 24

            // Logo placeholder (original uses dklogo.png).
            Rectangle {
                width: 260
                height: 150
                radius: 10
                color: "#193660"
                anchors.horizontalCenter: parent.horizontalCenter
                Label {
                    text: "DKSY"
                    color: "white"
                    font.pixelSize: 52
                    font.bold: true
                    anchors.centerIn: parent
                }
            }

            Label {
                text: qsTr("SY1000 Hydrostatic Test System")
                font.pixelSize: 56
                font.bold: true
                color: "#193660"
                horizontalAlignment: Text.AlignHCenter
            }

            Label {
                text: qsTr("Quanshen")
                font.pixelSize: 34
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

        Rectangle {
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

                ComboBox {
                    id: userBox
                    width: 300
                    height: 44
                    model: loginService.usernames()
                    font.pixelSize: 18
                }

                TextField {
                    id: passwordField
                    width: 300
                    height: 44
                    placeholderText: qsTr("Password")
                    echoMode: TextInput.Password
                    font.pixelSize: 18
                    onAccepted: signInButton.clicked()
                }

                Button {
                    id: signInButton
                    width: 320
                    height: 52
                    text: qsTr("Login")
                    font.pixelSize: 22
                    font.bold: true
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
                        onActivated: (index) => lang.setLanguage(index === 0 ? "en" : "zh_CN")
                    }
                }
            }
        }
    }
}


