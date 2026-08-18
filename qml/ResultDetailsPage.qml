import QtQuick
import QtQuick.Controls

Page {
    id: detailPage
    property int resultId: 0
    property var d: ({})

    Flickable {
        anchors.fill: parent
        contentHeight: col.implicitHeight + 24
        clip: true
        Column {
            id: col
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: 12
            spacing: 8

            Label {
                text: qsTr("Result Detail") + "  #" + d.id
                font.pixelSize: 18
                font.bold: true
            }

            Grid {
                columns: 2
                spacing: 6
                function row(label, value) {
                    Label { text: label + ":"; width: 170; color: "#666" }
                    Label { text: value === undefined ? "-" : value }
                }
            }

            Label { text: qsTr("Test"); font.bold: true; font.pixelSize: 14 }
            Row { spacing: 40
                Column { Label{text: qsTr("Serial")}; Label{text:d.serial||"-"} }
                Column { Label{text: qsTr("Date")}; Label{text:d.date||"-"} }
                Column { Label{text: qsTr("Tester")}; Label{text:d.tester||"-"} }
                Column { Label{text: qsTr("Company")}; Label{text:d.company||"-"} }
            }

            Label { text: qsTr("Sample"); font.bold: true; font.pixelSize: 14 }
            Row { spacing: 40
                Column { Label{text: qsTr("Model")}; Label{text:d.sampleModel||"-"} }
                Column { Label{text: qsTr("Manufacturer")}; Label{text:d.manufacturer||"-"} }
                Column { Label{text: qsTr("SerialNo")}; Label{text:d.sampleSerial||"-"} }
                Column { Label{text: qsTr("Volume")}; Label{text:d.volume||"-"} }
            }

            Label { text: qsTr("Standard"); font.bold: true; font.pixelSize: 14 }
            Row { spacing: 40
                Column { Label{text: qsTr("Working MPa")}; Label{text:d.workPressure||"-"} }
                Column { Label{text: qsTr("Test MPa")}; Label{text:d.testPressure||"-"} }
                Column { Label{text: qsTr("Rate %")}; Label{text:d.rate!==undefined ? d.rate.toFixed(2) : "-"} }
            }

            Label { text: qsTr("Hydrostatic Data"); font.bold: true; font.pixelSize: 14 }
            Row { spacing: 40
                Column { Label{text: qsTr("Initial W")}; Label{text:d.initialWeight||"-"} }
                Column { Label{text: qsTr("Pressure W")}; Label{text:d.pressureWeight||"-"} }
                Column { Label{text: qsTr("Final W")}; Label{text:d.finalWeight||"-"} }
                Column { Label{text: qsTr("Full Def")}; Label{text:d.fullDeformation||"-"} }
                Column { Label{text: qsTr("Resid Def")}; Label{text:d.residualDeformation||"-"} }
            }

            Button { text: qsTr("Back"); onClicked: stack.pop() }
        }
    }

    Component.onCompleted: d = resultService.details(resultId)
}
