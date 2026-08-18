import QtQuick
import QtQuick.Controls

Page {
    id: resultPage
    property string title: qsTr("Result Management")

    function refresh() {
        list.model = resultService.results()
    }

    Column {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 8

        Label {
            text: qsTr("Saved Test Results")
            font.pixelSize: 20
            font.bold: true
        }

        ListView {
            id: list
            width: parent.width
            height: parent.height - 80
            clip: true
            model: []

            header: Rectangle {
                width: parent.width
                height: 28
                color: "#eee"
                Row {
                    anchors.fill: parent
                    anchors.margins: 8
                    Label { text: qsTr("Serial"); width: 150; font.bold: true }
                    Label { text: qsTr("Tester"); width: 100; font.bold: true }
                    Label { text: qsTr("Manufacturer"); width: 120; font.bold: true }
                    Label { text: qsTr("Rate(%)"); width: 80; font.bold: true }
                }
            }

            delegate: Rectangle {
                width: parent.width
                height: 36
                color: index % 2 ? "#fafafa" : "#ffffff"
                Row {
                    anchors.fill: parent
                    anchors.margins: 8
                    Label { text: modelData.serial; width: 150 }
                    Label { text: modelData.tester; width: 100 }
                    Label { text: modelData.manufacturer; width: 120 }
                    Label { text: modelData.rate.toFixed(2); width: 80 }
                    Button {
                        text: qsTr("Detail")
                        anchors.verticalCenter: parent.verticalCenter
                        onClicked: stack.push("ResultDetailsPage.qml", { resultId: modelData.id })
                    }
                }
            }
        }

        Row {
            spacing: 10
            Button { text: qsTr("Refresh"); onClicked: resultPage.refresh() }
            Button { text: qsTr("Back"); onClicked: stack.pop() }
        }
    }

    Component.onCompleted: refresh()
}
