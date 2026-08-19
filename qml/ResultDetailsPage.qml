import QtQuick
import QtQuick.Controls

Page {
    id: detailPage
    property string title: qsTr("Result Detail")
    property int resultId: 0
    property var d: ({})

    function inspection(i) {
        var labels = [qsTr("Qualified"), qsTr("To Repair"), qsTr("To Replace"), qsTr("Scrapped")]
        return (i === undefined || i < 0 || i >= labels.length) ? "-" : labels[i]
    }

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

            // Appearance inspection results (matches WPF UnifiedTestResultDetailsWindow).
            Label { text: qsTr("Appearance Inspection"); font.bold: true; font.pixelSize: 14 }
            Row { spacing: 40
                Column { Label{text: qsTr("External")}; Label{text:detailPage.inspection(d.external)} }
                Column { Label{text: qsTr("Internal")}; Label{text:detailPage.inspection(d.internal)} }
                Column { Label{text: qsTr("Thread")}; Label{text:detailPage.inspection(d.thread)} }
                Column { Label{text: qsTr("Valve")}; Label{text:detailPage.inspection(d.valve)} }
            }

            Row {
                spacing: 10
                Button {
                    text: qsTr("View Report")
                    font.bold: true
                    onClicked: stack.push("ReportViewPage.qml", { resultId: detailPage.resultId })
                }
                Button { text: qsTr("Back"); onClicked: stack.pop() }
            }
        }
    }

    Component.onCompleted: d = resultService.details(resultId)
}
