import QtQuick
import QtQuick.Controls

Page {
    id: menuPage

    Column {
        anchors.centerIn: parent
        spacing: 16
        width: 320

        Label {
            text: qsTr("SY1000 Main Menu")
            font.pixelSize: 22
            font.bold: true
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Button {
            text: qsTr("Start Hydrostatic Test")
            width: parent.width
            onClicked: stack.push("TestPage.qml")
        }
        Button {
            text: qsTr("Result Management")
            width: parent.width
            onClicked: stack.push("ResultManagementPage.qml")
        }
        Button {
            text: qsTr("Log out")
            width: parent.width
            onClicked: stack.replace("LoginPage.qml")
        }
    }
}
