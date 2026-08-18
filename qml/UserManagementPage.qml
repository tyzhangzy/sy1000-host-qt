import QtQuick
import QtQuick.Controls

Page {
    id: userPage

    function refresh() {
        list.model = userService.users()
    }

    Column {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 8

        Label { text: qsTr("User Management"); font.pixelSize: 20; font.bold: true }

        // Add user form.
        Row {
            spacing: 6
            TextField { id: newUser; placeholderText: qsTr("Username"); width: 110 }
            TextField { id: newCompany; placeholderText: qsTr("Company"); width: 120 }
            TextField { id: newPwd; placeholderText: qsTr("Password"); width: 110 }
            CheckBox { id: newAdmin; text: qsTr("Admin") }
            Button {
                text: qsTr("Add")
                onClicked: {
                    if (userService.addUser(newUser.text, newCompany.text, newPwd.text, newAdmin.checked)) {
                        newUser.text = ""; newPwd.text = ""; newAdmin.checked = false
                        userPage.refresh()
                    }
                }
            }
        }

        ListView {
            id: list
            width: parent.width
            height: parent.height - 100
            clip: true
            model: []
            header: Rectangle {
                width: parent.width; height: 28; color: "#eee"
                Row {
                    anchors.fill: parent; anchors.margins: 8
                    Label { text: qsTr("ID"); width: 40; font.bold: true }
                    Label { text: qsTr("Username"); width: 120; font.bold: true }
                    Label { text: qsTr("Company"); width: 160; font.bold: true }
                    Label { text: qsTr("Admin"); width: 70; font.bold: true }
                }
            }
            delegate: Rectangle {
                width: parent.width; height: 36
                color: index % 2 ? "#fafafa" : "#ffffff"
                Row {
                    anchors.fill: parent; anchors.margins: 8
                    Label { text: modelData.id; width: 40 }
                    Label { text: modelData.username; width: 120 }
                    Label { text: modelData.company; width: 160 }
                    Label { text: modelData.isAdmin ? "Y" : "-"; width: 70 }
                    Button {
                        text: qsTr("Reset Pwd")
                        height: 26
                        onClicked: {
                            var dlg = resetDialog.createObject(userPage)
                            dlg.userId = modelData.id
                            dlg.open()
                        }
                    }
                }
            }
        }

        Row {
            spacing: 10
            Button { text: qsTr("Refresh"); onClicked: userPage.refresh() }
            Button { text: qsTr("Back"); onClicked: stack.pop() }
        }
    }

    Component {
        id: resetDialog
        Dialog {
            property int userId: 0
            title: qsTr("Reset Password")
            modal: true
            anchors.centerIn: parent
            standardButtons: Dialog.Ok | Dialog.Cancel
            TextField { id: newPwdField; placeholderText: qsTr("New password") }
            onAccepted: {
                if (userService.resetPassword(userId, newPwdField.text))
                    userPage.refresh()
            }
        }
    }

    Component.onCompleted: refresh()
}
