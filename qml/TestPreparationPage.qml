import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material

// Test preparation page, layout matched to the WPF TestPreparationPage:
// one StandardCardUserControl + four SampleCardUserControls in a single row.
Page {
    id: prepPage
    title: qsTr("Test Preparation")

    // 4 sample records (model/manufacturer/volume/company/serial + inspection result).
    property var sampleData: [
        { model: "", manufacturer: "", userCompany: "", serialNo: "", volume: 6.8, inspection: {} },
        { model: "", manufacturer: "", userCompany: "", serialNo: "", volume: 6.8, inspection: {} },
        { model: "", manufacturer: "", userCompany: "", serialNo: "", volume: 6.8, inspection: {} },
        { model: "", manufacturer: "", userCompany: "", serialNo: "", volume: 6.8, inspection: {} }
    ]

    // Test-standard fields (WPF StandardCardUserControl).
    property string standardName: "GB/T9251-2022"
    property int workingPressure: 30
    property int testingPressure: 45
    property int holdTime: 30
    property int residualRate: 5

    // Amber title bar (WPF ColorZone Mode="SecondaryMid").
    component TitleBar: Rectangle {
        property string text: ""
        height: 56
        color: "#FFC107"
        topLeftRadius: 8; topRightRadius: 8; bottomLeftRadius: 0; bottomRightRadius: 0
        Row {
            anchors.fill: parent
            anchors.leftMargin: 16
            spacing: 8
            Label { text: "☰"; color: "#212121"; font.pixelSize: 22; anchors.verticalCenter: parent.verticalCenter }
            Label { text: TitleBar.text; color: "#212121"; font.bold: true; font.pixelSize: 20; anchors.verticalCenter: parent.verticalCenter }
        }
    }

    // Dark raised button (WPF MaterialDesignRaisedDarkButton).
    component DarkButton: Button {
        property alias btnText: darkBtnText.text
        width: 200; height: 60
        Material.background: hovered ? "#1A237E" : "#283593"
        Material.foreground: "white"
        Material.elevation: 2
        contentItem: Label {
            id: darkBtnText
            text: ""
            color: "white"; font.pixelSize: 20; font.bold: true
            horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter
        }
    }

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

                // Standard card (WPF StandardCardUserControl).
                Rectangle {
                    width: 300; height: 720
                    radius: 8; color: "white"; border.color: "#E0E0E0"

                    TitleBar {
                        text: qsTr("Input Test Standard")
                        anchors.top: parent.top; anchors.left: parent.left; anchors.right: parent.right
                    }

                    Column {
                        anchors.top: parent.top; anchors.topMargin: 76
                        anchors.left: parent.left; anchors.right: parent.right
                        anchors.leftMargin: 16; anchors.rightMargin: 16
                        spacing: 14
                        Column { spacing: 4; width: parent.width
                            Label { text: qsTr("Test Standard"); font.pixelSize: 15 }
                            TextField { width: parent.width; text: standardName; onTextChanged: standardName = text }
                        }
                        Column { spacing: 4; width: parent.width
                            Label { text: qsTr("Working Pressure"); font.pixelSize: 15 }
                            Row { spacing: 6
                                SpinBox { width: 196; from: 1; to: 60; value: workingPressure; editable: true; onValueModified: workingPressure = value }
                                Label { text: "MPa"; font.pixelSize: 15; anchors.verticalCenter: parent.verticalCenter }
                            }
                        }
                        Column { spacing: 4; width: parent.width
                            Label { text: qsTr("Testing Pressure"); font.pixelSize: 15 }
                            Row { spacing: 6
                                SpinBox { width: 196; from: 1; to: 60; value: testingPressure; editable: true; onValueModified: testingPressure = value }
                                Label { text: "MPa"; font.pixelSize: 15; anchors.verticalCenter: parent.verticalCenter }
                            }
                        }
                        Column { spacing: 4; width: parent.width
                            Label { text: qsTr("Hold Time"); font.pixelSize: 15 }
                            Row { spacing: 6
                                SpinBox { width: 196; from: 20; to: 60; value: holdTime; editable: true; onValueModified: holdTime = value }
                                Label { text: qsTr("sec"); font.pixelSize: 15; anchors.verticalCenter: parent.verticalCenter }
                            }
                        }
                        Column { spacing: 4; width: parent.width
                            Label { text: qsTr("Residual Deformation Rate"); font.pixelSize: 15 }
                            Row { spacing: 6
                                SpinBox { width: 196; from: 0; to: 100; value: residualRate; editable: true; onValueModified: residualRate = value }
                                Label { text: "%"; font.pixelSize: 15; anchors.verticalCenter: parent.verticalCenter }
                            }
                        }
                    }

                    DarkButton {
                        btnText: qsTr("Confirm")
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.bottom: parent.bottom; anchors.bottomMargin: 16
                        onClicked: {
                            hydro.setWorkingPressure(workingPressure)
                            hydro.setTestingPressure(testingPressure)
                        }
                    }
                }

                // 4 sample cards (WPF SampleCardUserControl).
                Repeater {
                    model: 4
                    Rectangle {
                        width: 300; height: 720
                        radius: 8; color: "white"; border.color: "#E0E0E0"

                        TitleBar {
                            text: qsTr("Input Cylinder %1 Info").arg(index + 1)
                            anchors.top: parent.top; anchors.left: parent.left; anchors.right: parent.right
                        }

                        Column {
                            anchors.top: parent.top; anchors.topMargin: 76
                            anchors.left: parent.left; anchors.right: parent.right
                            anchors.leftMargin: 16; anchors.rightMargin: 16
                            spacing: 12
                            Column { spacing: 4; width: parent.width
                                Label { text: qsTr("Cylinder Model"); font.pixelSize: 15 }
                                TextField { width: parent.width
                                    text: sampleData[index].model
                                    onTextChanged: sampleData[index].model = text }
                            }
                            Column { spacing: 4; width: parent.width
                                Label { text: qsTr("Manufacturer"); font.pixelSize: 15 }
                                TextField { width: parent.width
                                    text: sampleData[index].manufacturer
                                    onTextChanged: sampleData[index].manufacturer = text }
                            }
                            Column { spacing: 4; width: parent.width
                                Label { text: qsTr("Cylinder Volume"); font.pixelSize: 15 }
                                Row { spacing: 6
                                    TextField {
                                        width: 196
                                        text: sampleData[index].volume
                                        validator: DoubleValidator { bottom: 0; top: 10 }
                                        onEditingFinished: {
                                            var v = parseFloat(text)
                                            sampleData[index].volume = isNaN(v) ? 0 : v
                                        }
                                    }
                                    Label { text: "L"; font.pixelSize: 15; anchors.verticalCenter: parent.verticalCenter }
                                }
                            }
                            Column { spacing: 4; width: parent.width
                                Label { text: qsTr("User Company"); font.pixelSize: 15 }
                                TextField { width: parent.width
                                    text: sampleData[index].userCompany
                                    onTextChanged: sampleData[index].userCompany = text }
                            }
                            Column { spacing: 4; width: parent.width
                                Label { text: qsTr("Serial No"); font.pixelSize: 15 }
                                TextField { width: parent.width
                                    text: sampleData[index].serialNo
                                    onTextChanged: sampleData[index].serialNo = text }
                            }
                        }

                        Column {
                            anchors.horizontalCenter: parent.horizontalCenter
                            anchors.bottom: parent.bottom; anchors.bottomMargin: 16
                            spacing: 12
                            DarkButton {
                                btnText: qsTr("Appearance Inspection")
                                onClicked: { inspectDlg.sampleIndex = index; inspectDlg.target = sampleData[index]; inspectDlg.open() }
                            }
                            DarkButton {
                                btnText: qsTr("Confirm")
                                onClicked: prepPage.saveSample(index)
                            }
                        }
                    }
                }
            }

            // Shared appearance inspection dialog (WPF AppearenceInspectionWindow).
            AppearanceInspectionDialog { id: inspectDlg }

            // Validation warning popup.
            Popup {
                id: warnPopup
                function msg(s) { warnText.text = s; warnPopup.open() }
                modal: true; focus: true
                anchors.centerIn: parent
                width: 420
                padding: 20
                Column { spacing: 12; width: parent.width
                    Label { id: warnText; text: ""; wrapMode: Text.Wrap; font.pixelSize: 16; width: parent.width }
                    Button {
                        text: qsTr("OK")
                        anchors.horizontalCenter: parent.horizontalCenter
                        onClicked: warnPopup.close()
                    }
                }
            }

            // Bottom actions: start the test / back.
            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 16
                DarkButton {
                    btnText: qsTr("Start Test")
                    width: 260
                    onClicked: {
                        for (var i = 0; i < 4; i++)
                            hydro.setSampleInspection(i + 1, sampleData[i].inspection)
                        stack.push("TestPage.qml")
                    }
                }
                DarkButton {
                    btnText: qsTr("Back")
                    width: 160
                    onClicked: stack.pop()
                }
            }
        }
    }

    // Validate + save a single sample (mirrors WPF SampleCard ValidateSampleData).
    function saveSample(idx) {
        var d = sampleData[idx]
        if (!d.model) { warnPopup.msg(qsTr("Please enter cylinder model.")); return }
        if (!d.manufacturer) { warnPopup.msg(qsTr("Please enter manufacturer.")); return }
        if (!(d.volume > 0)) { warnPopup.msg(qsTr("Please enter a valid cylinder volume.")); return }
        if (!d.userCompany) { warnPopup.msg(qsTr("Please enter user company.")); return }
        if (!d.serialNo) { warnPopup.msg(qsTr("Please enter serial No.")); return }
        if (!(d.inspection && d.inspection.inspectionCompleted)) { warnPopup.msg(qsTr("Please complete appearance inspection before saving.")); return }
        hydro.setSample(idx + 1, d.model, d.manufacturer, d.serialNo, d.volume)
    }
}

