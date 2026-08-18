import QtQuick
import QtQuick.Controls

Page {
    id: testPage

    Column {
        anchors.centerIn: parent
        spacing: 14
        width: 380

        Label {
            text: qsTr("Hydrostatic Test")
            font.pixelSize: 20
            font.bold: true
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Row {
            spacing: 8
            width: parent.width
            Label { text: qsTr("Working pressure (MPa)"); width: parent.width - 90 - parent.spacing }
            SpinBox {
                width: 90
                from: 1; to: 100; value: 25; editable: true
                onValueModified: hydro.setWorkingPressure(value)
            }
        }
        Row {
            spacing: 8
            width: parent.width
            Label { text: qsTr("Test pressure (MPa)"); width: parent.width - 90 - parent.spacing }
            SpinBox {
                width: 90
                from: 1; to: 150; value: 37; editable: true
                onValueModified: hydro.setTestingPressure(value)
            }
        }

        Row {
            spacing: 10
            width: parent.width
            Button {
                text: qsTr("Start")
                enabled: !hydro.running
                onClicked: hydro.startTest()
            }
            Button {
                text: qsTr("Stop")
                enabled: hydro.running
                onClicked: hydro.stopTest()
            }
        }

        Label {
            text: qsTr("State: ") + hydro.state
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: 14
            font.bold: true
        }
        Label {
            text: hydro.status
            width: parent.width
            wrapMode: Text.Wrap
            horizontalAlignment: Text.AlignHCenter
            color: "#666"
        }

        Label {
            id: resultLabel
            color: "#2e7d32"
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: 14
            text: ""
        }

        Connections {
            target: hydro
            function onTestFinished(ok, passed, failed) {
                resultLabel.text = qsTr("Finished ok=%1, passed=%2, failed=%3").arg(ok).arg(passed).arg(failed)
            }
        }

        Button {
            text: qsTr("Log out")
            anchors.horizontalCenter: parent.horizontalCenter
            onClicked: stack.replace("LoginPage.qml")
        }
    }
}
