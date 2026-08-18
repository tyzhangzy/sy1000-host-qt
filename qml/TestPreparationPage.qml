import QtQuick
import QtQuick.Controls

// Test preparation page: matches the WPF layout (Standard card + 4 Sample cards in a row).
Page {
    id: prepPage
    property string title: qsTr("Test Preparation")

    property var sampleData: [
        { model: "", manufacturer: "", serialNo: "", volume: 6.8 },
        { model: "", manufacturer: "", serialNo: "", volume: 6.8 },
        { model: "", manufacturer: "", serialNo: "", volume: 6.8 },
        { model: "", manufacturer: "", serialNo: "", volume: 6.8 }
    ]

    Flickable {
        anchors.fill: parent
        contentHeight: row.implicitHeight + 80
        clip: true

        Column {
            anchors.fill: parent
            spacing: 12

            // Row of 5 cards: standard + 4 samples (matches WPF).
            Row {
                id: row
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: 12
                spacing: 12

                // Standard card.
                Rectangle {
                    width: 260
                    height: 340
                    radius: 10
                    border.color: "#CCCCCC"
                    color: "white"
                    Column {
                        anchors.fill: parent
                        anchors.margins: 14
                        spacing: 8
                        Label { text: qsTr("Test Standard"); font.bold: true; font.pixelSize: 18 }
                        TextField { id: stdName; width: parent.width; placeholderText: qsTr("Name"); text: "GB1234" }
                        TextField { id: stdWork; width: parent.width; placeholderText: qsTr("Working MPa"); text: "25" }
                        TextField { id: stdTest; width: parent.width; placeholderText: qsTr("Test MPa"); text: "37.5" }
                        TextField { id: stdHold; width: parent.width; placeholderText: qsTr("Hold (s)"); text: "60" }
                        TextField { id: stdRate; width: parent.width; placeholderText: qsTr("Rate limit %"); text: "3.0" }
                    }
                }

                // 4 sample cards.
                Repeater {
                    model: 4
                    Rectangle {
                        width: 260
                        height: 340
                        radius: 10
                        border.color: "#CCCCCC"
                        color: "white"
                        Column {
                            anchors.fill: parent
                            anchors.margins: 14
                            spacing: 8
                            Label { text: qsTr("Sample %1").arg(index + 1); font.bold: true; font.pixelSize: 18 }
                            TextField { width: parent.width; placeholderText: qsTr("Model")
                                text: sampleData[index].model
                                onTextChanged: sampleData[index].model = text }
                            TextField { width: parent.width; placeholderText: qsTr("Manufacturer")
                                text: sampleData[index].manufacturer
                                onTextChanged: sampleData[index].manufacturer = text }
                            TextField { width: parent.width; placeholderText: qsTr("Serial No")
                                text: sampleData[index].serialNo
                                onTextChanged: sampleData[index].serialNo = text }
                            SpinBox {
                                width: parent.width; from: 1; to: 100
                                value: sampleData[index].volume; editable: true
                                onValueModified: sampleData[index].volume = value
                            }
                            Row {
                                spacing: 8
                                Button {
                                    text: qsTr("Inspect")
                                    onClicked: {
                                        inspectDlg.sampleIndex = index
                                        inspectDlg.target = sampleData[index]
                                        inspectDlg.open()
                                    }
                                }
                                Label {
                                    text: sampleData[index].inspected ? qsTr("Done") : qsTr("Not inspected")
                                    color: sampleData[index].inspected ? "#2e7d32" : "#c62828"
                                    anchors.verticalCenter: parent.verticalCenter
                                }
                            }
                        }
                    }
                }
            }

            // Shared appearance inspection dialog.
            AppearanceInspectionDialog {
                id: inspectDlg
            }

            // Bottom actions.
            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 12
                Button {
                    text: qsTr("Save & Start Test")
                    font.bold: true
                    onClicked: {
                        hydro.setWorkingPressure(parseFloat(stdWork.text))
                        hydro.setTestingPressure(parseFloat(stdTest.text))
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

