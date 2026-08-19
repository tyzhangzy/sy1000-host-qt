import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material

// Test preparation page, layout matched to the WPF TestPreparationPage:
// one StandardCardUserControl + four SampleCardUserControls in a single row.
Page {
    id: prepPage
    title: qsTr("Test Preparation")
    // This page draws its own top bar (WPF MainTestWindow), so the app-wide
    // header (device/user/quit) is hidden. Top bar height 80 == Main.qml header.
    readonly property bool hideGlobalHeader: true

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

    Column {
        anchors.fill: parent

        // Top status bar (WPF MainTestWindow ColorZone header): left / center / right.
        Rectangle {
            id: topBar
            width: parent.width; height: 80; color: "#303F9F"
            // Left: title + connection status box.
            Row {
                anchors.left: parent.left; anchors.leftMargin: 24
                anchors.verticalCenter: parent.verticalCenter
                spacing: 20
                Label { text: qsTr("Hydrostatic Test"); color: "white"; font.pixelSize: 22; font.bold: true; anchors.verticalCenter: parent.verticalCenter }
                Rectangle {
                    width: 300; height: 40; radius: 4; color: "white"
                    Label {
                        id: connLabel
                        anchors.fill: parent; anchors.margins: 6
                        text: deviceService.connectDevices()
                        color: "#333"; font.pixelSize: 16; font.bold: true; verticalAlignment: Text.AlignVCenter
                    }
                    MouseArea { anchors.fill: parent; cursorShape: Qt.PointingHandCursor
                        onClicked: connLabel.text = deviceService.connectDevices() }
                }
            }
            // Center: test status box.
            Rectangle {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                width: 340; height: 40; radius: 4; color: "white"
                Label { anchors.fill: parent; anchors.margins: 6
                    text: hydro.status === "" ? qsTr("Status: ") + hydro.state : hydro.status
                    color: "#333"; font.pixelSize: 16; font.bold: true; verticalAlignment: Text.AlignVCenter }
            }
            // Right: cylinder pressure label + value.
            Row {
                anchors.right: parent.right; anchors.rightMargin: 24
                anchors.verticalCenter: parent.verticalCenter
                spacing: 10
                Label { text: qsTr("Cylinder pressure"); color: "white"; font.pixelSize: 20; font.bold: true; anchors.verticalCenter: parent.verticalCenter }
                Rectangle { width: 170; height: 40; radius: 4; color: "white"
                    TextField { anchors.fill: parent; anchors.margins: 2; readOnly: true; horizontalAlignment: Text.AlignHCenter
                        font.pixelSize: 20; font.bold: true; color: "#333"
                        text: isNaN(hydro.currentPressure) ? "N/A" : hydro.currentPressure.toFixed(2) + " MPa" } }
            }
        }

        Row {
            width: parent.width; height: parent.height - 80

        // Left column (WPF MainTestWindow left column).
        Rectangle {
            width: 320; height: parent.height; color: "#F5F7FA"
            Column {
                anchors.top: parent.top; anchors.topMargin: 12
                anchors.left: parent.left; anchors.right: parent.right
                anchors.leftMargin: 10; anchors.rightMargin: 10
                spacing: 10
                TextArea { id: leftStd; width: parent.width; height: 128; readOnly: true; font.pixelSize: 14; wrapMode: Text.Wrap; text: hydro.testStandardInfo() }
                TextArea { id: leftS1; width: parent.width; height: 104; readOnly: true; font.pixelSize: 14; wrapMode: Text.Wrap; text: hydro.sampleInfo(1) }
                TextArea { id: leftS2; width: parent.width; height: 104; readOnly: true; font.pixelSize: 14; wrapMode: Text.Wrap; text: hydro.sampleInfo(2) }
                TextArea { id: leftS3; width: parent.width; height: 104; readOnly: true; font.pixelSize: 14; wrapMode: Text.Wrap; text: hydro.sampleInfo(3) }
                TextArea { id: leftS4; width: parent.width; height: 104; readOnly: true; font.pixelSize: 14; wrapMode: Text.Wrap; text: hydro.sampleInfo(4) }
                Button {
                    text: qsTr("Start Hydrostatic Test")
                    width: 240; height: 56
                    anchors.horizontalCenter: parent.horizontalCenter
                    Material.background: "#FFC107"; Material.foreground: "#212121"; Material.elevation: 2
                    font.pixelSize: 17; font.bold: true
                    onClicked: { for (var i = 0; i < 4; i++) hydro.setSampleInspection(i + 1, sampleData[i].inspection); stack.push("TestPage.qml") }
                }
            }
            Column {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom; anchors.bottomMargin: 14
                spacing: 10
                DarkButton { btnText: qsTr("Save Test Result"); enabled: false }
                DarkButton { btnText: qsTr("View Test Report"); enabled: false }
                DarkButton { btnText: qsTr("Return to Main Menu"); onClicked: stack.pop() }
            }
        }

        Rectangle { width: 2; color: "#CCCCCC"; height: parent.height }

        // Right: prep cards (standard + 4 samples) in a scrollable area.
        Flickable {
            width: parent.width - 322
            height: parent.height
            contentWidth: row.implicitWidth + 24
            contentHeight: cardsCol.implicitHeight
            clip: true

            Column {
                id: cardsCol
                width: parent.width
                spacing: 12
                Row {
                    id: row
                    width: parent.width
                    anchors.margins: 12
                    spacing: 12

                // Standard card (WPF StandardCardUserControl).
                Rectangle {
                    width: 300; height: 720
                    radius: 8; color: "white"; border.color: "#E0E0E0"

                    TitleBar {
                        text: qsTr("Test Standard")
                        anchors.top: parent.top; anchors.left: parent.left; anchors.right: parent.right
                    }

                    Column {
                        anchors.top: parent.top; anchors.topMargin: 76
                        anchors.left: parent.left; anchors.right: parent.right
                        anchors.leftMargin: 16; anchors.rightMargin: 16
                        spacing: 14
                        TextField { width: parent.width; text: standardName; onTextChanged: standardName = text }
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
                            hydro.setTestStandard(standardName, holdTime, residualRate)
                            leftStd.text = hydro.testStandardInfo()
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
                            text: qsTr("Cylinder Model %1").arg(index + 1)
                            anchors.top: parent.top; anchors.left: parent.left; anchors.right: parent.right
                        }

                        Column {
                            anchors.top: parent.top; anchors.topMargin: 76
                            anchors.left: parent.left; anchors.right: parent.right
                            anchors.leftMargin: 16; anchors.rightMargin: 16
                            spacing: 12
                            TextField { width: parent.width
                                text: sampleData[index].model
                                onTextChanged: sampleData[index].model = text }
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
        // Refresh the left-column cylinder summary box.
        var areas = [leftS1, leftS2, leftS3, leftS4]
        areas[idx].text = hydro.sampleInfo(idx + 1)
    }
}

