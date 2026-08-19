import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import SyCharts 1.0

// Hydrostatic test page, matched to the WPF MainTestWindow layout: a top status
// bar (test status + cylinder pressure) and a left column (standard + 1..4
// cylinder info + start button; bottom: save/report/menu buttons).
Page {
    id: testPage
    title: qsTr("Hydrostatic Test")

    component DarkBtn: Button {
        property alias btnText: _t.text
        width: 220; height: 56
        Material.background: hovered ? "#1A237E" : "#283593"
        Material.foreground: "white"; Material.elevation: 2
        contentItem: Label { id: _t; text: ""; color: "white"; font.pixelSize: 18; font.bold: true; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
    }

    Column {
        anchors.fill: parent

        // Top status bar (WPF MainTestWindow ColorZone header).
        Rectangle {
            width: parent.width; height: 64; color: "#303F9F"
            Row {
                anchors.left: parent.left; anchors.leftMargin: 20
                anchors.verticalCenter: parent.verticalCenter
                spacing: 18
                Label { text: qsTr("Hydrostatic Test"); color: "white"; font.pixelSize: 22; font.bold: true; anchors.verticalCenter: parent.verticalCenter }
                // Test status display box (WPF StatusButton).
                Rectangle {
                    width: 340; height: 40; radius: 4; color: "white"
                    Label {
                        anchors.fill: parent; anchors.margins: 6
                        text: hydro.status === "" ? qsTr("Status: ") + hydro.state : hydro.status
                        color: "#333"; font.pixelSize: 16; font.bold: true; verticalAlignment: Text.AlignVCenter
                    }
                }
                // Cylinder pressure label + value edit box (WPF HPTextBox).
                Label { text: qsTr("Cylinder pressure"); color: "white"; font.pixelSize: 20; font.bold: true; anchors.verticalCenter: parent.verticalCenter }
                Rectangle {
                    width: 170; height: 40; radius: 4; color: "white"
                    TextField {
                        anchors.fill: parent; anchors.margins: 2
                        readOnly: true; horizontalAlignment: Text.AlignHCenter
                        font.pixelSize: 20; font.bold: true; color: "#333"
                        text: isNaN(hydro.currentPressure) ? "N/A" : hydro.currentPressure.toFixed(2) + " MPa"
                    }
                }
            }
        }

        Row {
            width: parent.width; height: parent.height - 64

            // Left column: standard + 1..4 cylinders + start (top); menu buttons (bottom).
            Rectangle {
                width: 320; height: parent.height; color: "#F5F7FA"
                Column {
                    anchors.top: parent.top; anchors.topMargin: 12
                    anchors.left: parent.left; anchors.right: parent.right
                    anchors.leftMargin: 10; anchors.rightMargin: 10
                    spacing: 10
                    TextArea { width: parent.width; height: 128; readOnly: true; font.pixelSize: 14; wrapMode: Text.Wrap; text: hydro.testStandardInfo() }
                    Repeater {
                        model: 4
                        TextArea {
                            width: parent.width; height: 104; readOnly: true; font.pixelSize: 14; wrapMode: Text.Wrap
                            text: hydro.sampleInfo(index + 1)
                            placeholderText: qsTr("%1 cylinder").arg(index + 1)
                        }
                    }
                    Button {
                        text: qsTr("Start Hydrostatic Test")
                        width: 240; height: 56
                        anchors.horizontalCenter: parent.horizontalCenter
                        Material.background: "#FFC107"; Material.foreground: "#212121"; Material.elevation: 2
                        font.pixelSize: 17; font.bold: true
                        onClicked: hydro.startTest()
                    }
                }
                Column {
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.bottom: parent.bottom; anchors.bottomMargin: 14
                    spacing: 10
                    DarkBtn { btnText: qsTr("Save Test Result") }
                    DarkBtn { btnText: qsTr("View Test Report") }
                    DarkBtn { btnText: qsTr("Return to Main Menu"); onClicked: stack.pop() }
                }
            }

            Rectangle { width: 2; color: "#CCCCCC"; height: parent.height }

            // Right: realtime chart + start/stop controls.
            Column {
                width: parent.width - 322; height: parent.height
                anchors.margins: 12
                spacing: 10
                Rectangle {
                    width: parent.width; height: parent.height - 92
                    color: "#fafafa"; border.color: "#ddd"; radius: 6; clip: true
                    RealTimeChart {
                        id: pressureChart
                        anchors.fill: parent; anchors.margins: 6
                        title: qsTr("Pressure / Deformation"); yAxisLabel: qsTr("Pressure (MPa)"); rightYAxisLabel: qsTr("Weight (g)")
                        lineColor: "#3f51b5"; yMax: 60; rightYMin: 0; rightYMax: 2000; maxPoints: 600; showGrid: true
                        Component.onCompleted: {
                            addSeries(qsTr("Sample 1"), "#e57373", true); addSeries(qsTr("Sample 2"), "#81c784", true)
                            addSeries(qsTr("Sample 3"), "#ffb74d", true); addSeries(qsTr("Sample 4"), "#64b5f6", true)
                        }
                    }
                }
                Row {
                    width: parent.width
                    spacing: 12
                    Button {
                        text: qsTr("Start"); width: 160; height: 56
                        enabled: !hydro.running; Material.background: "#FFC107"; Material.foreground: "#212121"
                        font.pixelSize: 18; font.bold: true
                        onClicked: hydro.startTest()
                    }
                    Button {
                        text: qsTr("Stop"); width: 160; height: 56
                        enabled: hydro.running; Material.background: "#e53935"; Material.foreground: "white"
                        font.pixelSize: 18; font.bold: true
                        onClicked: hydro.stopTest()
                    }
                    Button { text: qsTr("Clear chart"); height: 56; onClicked: pressureChart.clear() }
                    Label {
                        text: resultLabel.text
                        color: resultLabel.color; anchors.verticalCenter: parent.verticalCenter
                        font.pixelSize: 15; font.bold: true
                    }
                }
            }
        }
    }

    // Hidden label used to relay the completion text into the control row.
    Label { id: resultLabel; visible: false; color: "#2e7d32"; text: "" }

    Connections {
        target: hydro
        function onTestFinished(ok, passed, failed) { resultLabel.text = qsTr("Finished ok=%1, passed=%2, failed=%3").arg(ok).arg(passed).arg(failed) }
        function onPressureSample(value) { pressureChart.addValue(value) }
        function onWeightSample(index, value) { pressureChart.addSeriesValue(index, value) }
        function onConfirmRequested(title, message) { messageDlg.showRequest(title, message) }
    }

    // Operator instruction/confirmation dialog.
    HydroTestMessageDialog { id: messageDlg }
}
