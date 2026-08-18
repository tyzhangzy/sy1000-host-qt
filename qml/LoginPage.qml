import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

/**
 * Login page (login window content). Matches the original WPF layout:
 * left 65% = logo + device title, right 35% = centered login card.
 */
Item {
    id: loginPage
    property string title: qsTr("Login")

    // Left brand panel (65%).
    Rectangle {
        width: parent.width * 0.65
        height: parent.height
        color: "#E0E0E0"

        Column {
            anchors.centerIn: parent
            spacing: 16

            // Logo placeholder (original uses dklogo.png).
            Rectangle {
                width: 220
                height: 130
                radius: 8
                color: "#193660"
                anchors.horizontalCenter: parent.horizontalCenter
                Label {
                    text: qsTr("DKSY")
                    color: "white"
                    font.pixelSize: 44
                    font.bold: true
                    anchors.centerIn: parent
                }
            }

            Label {
                text: qsTr("SY1000 Hydrostatic Test System")
                font.pixelSize: 44
                font.bold: true
                color: "#193660"
                horizontalAlignment: Text.AlignHCenter
            }
            Label {
                text: qsTr("Quanshen")
                font.pixelSize: 28
                font.bold: true
                color: "#444"
                horizontalAlignment: Text.AlignHCenter
            }
        }
    }

    // Right login card (35%).
    Rectangle {
        width: parent.width * 0.35
        height: parent.height
        color: "#F5F5F5"

        Rectangle {
            width: 380
            height: 420
            radius: 15
            border.color: "#CCCCCC"
            color: "white"
            anchors.centerIn: parent

            Column {
                anchors.centerIn: parent
                spacing: 14

                Label {
                    text: qsTr("User Login")
                    font.pixelSize: 30
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
                    width: 300
                    height: 52
                    text: qsTr("Login")
                    font.pixelSize: 20
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
                    font.pixelSize: 13
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
                        width: 140
                        model: ["English", "中文"]
                        onActivated: (index) => lang.setLanguage(index === 0 ? "en" : "zh_CN")
                    }
                }
            }
        }
    }
}

