import QtQuick
import QtQuick.Controls
import SyCharts 1.0

Page {
    id: testPage
    property string title: qsTr("Hydrostatic Test")

    Column {
        anchors.centerIn: parent
        spacing: 14
        width: 420

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
            function onPressureSample(value) {
                pressureChart.addValue(value)
            }
            function onWeightSample(index, value) {
                pressureChart.addSeriesValue(index, value)
            }
            function onConfirmRequested(title, message) {
                messageDlg.showRequest(title, message)
            }
        }

        // Realtime dual-axis chart: left = pressure (MPa), right = 4 sample
        // weights/deformation (g). Series 0 is the pressure curve; series 1..4
        // are the per-sample weight curves on the right Y axis.
        Rectangle {
            width: parent.width
            height: 320
            color: "#fafafa"
            border.color: "#ddd"
            radius: 6
            clip: true
            RealTimeChart {
                id: pressureChart
                anchors.fill: parent
                anchors.margins: 6
                title: qsTr("Pressure / Deformation")
                yAxisLabel: qsTr("Pressure (MPa)")
                rightYAxisLabel: qsTr("Weight (g)")
                lineColor: "#3f51b5"
                yMax: 60
                rightYMin: 0
                rightYMax: 2000
                maxPoints: 600
                showGrid: true

                // Register the 4 sample weight series once (right axis).
                Component.onCompleted: {
                    addSeries(qsTr("Sample 1"), "#e57373", true)
                    addSeries(qsTr("Sample 2"), "#81c784", true)
                    addSeries(qsTr("Sample 3"), "#ffb74d", true)
                    addSeries(qsTr("Sample 4"), "#64b5f6", true)
                }
            }

            // Legend.
            Row {
                anchors.top: parent.top
                anchors.topMargin: 8
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 14
                Repeater {
                    model: [
                        { c: "#3f51b5", t: qsTr("Pressure") },
                        { c: "#e57373", t: qsTr("S1") },
                        { c: "#81c784", t: qsTr("S2") },
                        { c: "#ffb74d", t: qsTr("S3") },
                        { c: "#64b5f6", t: qsTr("S4") }
                    ]
                    Row {
                        spacing: 4
                        Rectangle {
                            width: 12; height: 12; radius: 3
                            color: modelData.c
                            anchors.verticalCenter: parent.verticalCenter
                        }
                        Label { text: modelData.t; anchors.verticalCenter: parent.verticalCenter }
                    }
                }
            }
        }

        Button {
            text: qsTr("Clear chart")
            anchors.horizontalCenter: parent.horizontalCenter
            onClicked: pressureChart.clear()
        }

        Button {
            text: qsTr("Back to menu")
            anchors.horizontalCenter: parent.horizontalCenter
            onClicked: stack.pop()
        }
    }

    // Operator instruction/confirmation dialog (overlays the whole page).
    HydroTestMessageDialog {
        id: messageDlg
    }
}
