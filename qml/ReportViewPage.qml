import QtQuick
import QtQuick.Controls

// Test report view: renders the report content (A4-styled preview) from the saved
// result and can generate/open the actual PDF file. A2 (report viewing) fallback
// for environments without QPdfView.
Page {
    id: reportPage
    property string title: qsTr("Test Report")
    property int resultId: 0
    property var d: ({})

    function val(v, unit) {
        if (v === undefined || v === null) return "-"
        return (unit === undefined) ? String(v) : v.toFixed(2) + " " + unit
    }
    function inspection(i) {
        var labels = [qsTr("Qualified"), qsTr("To Repair"), qsTr("To Replace"), qsTr("Scrapped")]
        return (i === undefined || i < 0 || i >= labels.length) ? "-" : labels[i]
    }
    function overall(i) {
        var labels = [qsTr("Not Tested"), qsTr("In Progress"), qsTr("Passed"), qsTr("Failed"),
                      qsTr("Qualified"), qsTr("To Repair"), qsTr("To Replace"), qsTr("Scrapped")]
        return (i === undefined || i < 0 || i >= labels.length) ? "-" : labels[i]
    }

    Column {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 10

        Label {
            text: qsTr("Test Report") + "  #" + d.id
            font.pixelSize: 20
            font.bold: true
        }

        // A4-styled report sheet.
        Flickable {
            width: parent.width
            height: parent.height - 90
            contentHeight: sheet.implicitHeight + 24
            clip: true

            Rectangle {
                id: sheet
                width: Math.min(parent.width - 24, 780)
                anchors.horizontalCenter: parent.horizontalCenter
                radius: 6
                border.color: "#CCCCCC"
                color: "white"
                implicitHeight: col.implicitHeight + 30
                Column {
                    id: col
                    anchors.fill: parent
                    anchors.margins: 20
                    spacing: 10

                    Label {
                        text: qsTr("Hydrostatic Test Report")
                        font.pixelSize: 18
                        font.bold: true
                        anchors.horizontalCenter: parent.horizontalCenter
                        color: "#303F9F"
                    }
                    Label {
                        text: qsTr("Serial: %1").arg(d.serial || "-")
                        anchors.horizontalCenter: parent.horizontalCenter
                        color: "#666"
                    }
                    Rectangle { width: parent.width; height: 1; color: "#ddd" }

                    Text { text: qsTr("Tester: %1    Company: %2    Date: %3")
                               .arg(d.tester || "-", d.company || "-", d.date || "-"); color: "#333" }

                    // Standard
                    Label { text: qsTr("Test Standard"); font.bold: true; font.pixelSize: 14 }
                    Grid { columns: 2; spacing: 6; columnSpacing: 24
                        Label { text: qsTr("Name:"); color: "#666" }
                        Label { text: d.standardName || "-" }
                        Label { text: qsTr("Working (MPa):"); color: "#666" }
                        Label { text: val(d.workPressure) }
                        Label { text: qsTr("Test (MPa):"); color: "#666" }
                        Label { text: val(d.testPressure) }
                        Label { text: qsTr("Rate limit (%):"); color: "#666" }
                        Label { text: val(d.rateLimit) }
                    }

                    // Sample
                    Label { text: qsTr("Sample"); font.bold: true; font.pixelSize: 14 }
                    Grid { columns: 2; spacing: 6; columnSpacing: 24
                        Label { text: qsTr("Model:"); color: "#666" }
                        Label { text: d.sampleModel || "-" }
                        Label { text: qsTr("Manufacturer:"); color: "#666" }
                        Label { text: d.manufacturer || "-" }
                        Label { text: qsTr("Serial No:"); color: "#666" }
                        Label { text: d.sampleSerial || "-" }
                        Label { text: qsTr("Volume (L):"); color: "#666" }
                        Label { text: val(d.volume) }
                    }

                    // Hydrostatic data
                    Label { text: qsTr("Hydrostatic Data"); font.bold: true; font.pixelSize: 14 }
                    Grid { columns: 3; spacing: 8
                        Label { text: qsTr("Initial W"); color: "#666" }
                        Label { text: qsTr("Pressure W"); color: "#666" }
                        Label { text: qsTr("Final W"); color: "#666" }
                        Label { text: val(d.initialWeight) }
                        Label { text: val(d.pressureWeight) }
                        Label { text: val(d.finalWeight) }
                        Label { text: qsTr("Full Def"); color: "#666" }
                        Label { text: qsTr("Resid Def"); color: "#666" }
                        Label { text: qsTr("Rate (%)"); color: "#666" }
                        Label { text: val(d.fullDeformation) }
                        Label { text: val(d.residualDeformation) }
                        Label { text: val(d.rate) }
                    }

                    // Appearance inspection
                    Label { text: qsTr("Appearance Inspection"); font.bold: true; font.pixelSize: 14 }
                    Grid { columns: 2; spacing: 6; columnSpacing: 24
                        Label { text: qsTr("External:"); color: "#666" }
                        Label { text: inspection(d.external) }
                        Label { text: qsTr("Internal:"); color: "#666" }
                        Label { text: inspection(d.internal) }
                        Label { text: qsTr("Thread:"); color: "#666" }
                        Label { text: inspection(d.thread) }
                        Label { text: qsTr("Valve:"); color: "#666" }
                        Label { text: inspection(d.valve) }
                    }

                    Rectangle { width: parent.width; height: 1; color: "#ddd" }
                    Label {
                        text: qsTr("Overall Result: ") + overall(d.overall)
                        font.bold: true
                        font.pixelSize: 16
                        color: (d.overall === 4 || d.overall === 2) ? "#2e7d32" : "#c62828"
                    }
                }
            }
        }

        Row {
            spacing: 10
            Button {
                text: qsTr("Generate & Open PDF")
                font.bold: true
                onClicked: {
                    var p = resultService.generatePdf(resultId)
                    pdfPath.text = (p === "") ? qsTr("Generate failed") : p
                    if (p !== "") Qt.openUrlExternally("file:///" + p)
                }
            }
            Button { text: qsTr("Back"); onClicked: stack.pop() }
        }
        Label { id: pdfPath; color: "#666"; width: parent.width; wrapMode: Text.Wrap; text: "" }
    }

    Component.onCompleted: d = resultService.reportData(resultId)
}

