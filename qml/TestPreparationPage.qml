import QtQuick
import QtQuick.Controls

Page {
    id: prepPage
    property string title: qsTr("Test Preparation")

    // Test standard fields.
    property string standardName: "GB1234"
    property double workPressure: 25
    property double testPressure: 37.5
    property int holdTime: 60
    property double rateLimit: 3.0

    // Sample info array (index 1..4).
    property var sampleData: [
        { model: "", manufacturer: "", serialNo: "", volume: 6.8 },
        { model: "", manufacturer: "", serialNo: "", volume: 6.8 },
        { model: "", manufacturer: "", serialNo: "", volume: 6.8 },
        { model: "", manufacturer: "", serialNo: "", volume: 6.8 }
    ]

    Flickable {
        anchors.fill: parent
        contentHeight: column.implicitHeight + 24
        clip: true

        Column {
            id: column
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: 12
            spacing: 10

            Label {
                text: qsTr("Test Preparation")
                font.pixelSize: 20
                font.bold: true
            }

            GroupBox {
                width: parent.width
                title: qsTr("Test Standard")
                Grid {
                    columns: 2
                    spacing: 8
                    Label { text: qsTr("Standard name"); width: 140 }
                    TextField { id: stdName; text: prepPage.standardName }
                    Label { text: qsTr("Working pressure (MPa)"); width: 140 }
                    TextField { id: stdWork; text: prepPage.workPressure }
                    Label { text: qsTr("Test pressure (MPa)"); width: 140 }
                    TextField { id: stdTest; text: prepPage.testPressure }
                    Label { text: qsTr("Holding time (s)"); width: 140 }
                    TextField { id: stdHold; text: prepPage.holdTime }
                    Label { text: qsTr("Residual rate limit (%)"); width: 140 }
                    TextField { id: stdRate; text: prepPage.rateLimit }
                }
            }

            GroupBox {
                width: parent.width
                title: qsTr("Samples (1-4)")
                Repeater {
                    model: 4
                    Rectangle {
                        width: parent.width
                        height: 34
                        color: "transparent"
                        Row {
                            spacing: 6
                            Label { text: qsTr("S%1").arg(index + 1); width: 26; anchors.verticalCenter: parent.verticalCenter }
                            TextField { width: 90; placeholderText: qsTr("Model"); text: sampleData[index].model
                                onTextChanged: sampleData[index].model = text }
                            TextField { width: 100; placeholderText: qsTr("Manufacturer"); text: sampleData[index].manufacturer
                                onTextChanged: sampleData[index].manufacturer = text }
                            TextField { width: 90; placeholderText: qsTr("SerialNo"); text: sampleData[index].serialNo
                                onTextChanged: sampleData[index].serialNo = text }
                            SpinBox { width: 80; from: 1; to: 100; value: sampleData[index].volume; editable: true
                                onValueModified: sampleData[index].volume = value }
                        }
                    }
                }
            }

            Row {
                spacing: 10
                Button {
                    text: qsTr("Save & Start Test")
                    onClicked: {
                        // Apply standard to the controller.
                        hydro.setWorkingPressure(parseFloat(stdWork.text))
                        hydro.setTestingPressure(parseFloat(stdTest.text))
                        // Apply sample info.
                        for (var i = 0; i < 4; i++) {
                            var d = sampleData[i]
                            hydro.setSample(i + 1, d.model, d.manufacturer, d.serialNo, d.volume)
                        }
                        stack.push("TestPage.qml")
                    }
                }
                Button { text: qsTr("Back"); onClicked: stack.pop() }
            }
        }
    }
}
