import QtQuick
import QtQuick.Controls

// Result management (B5): split into two tabs matching the WPF windows.
//   Tab 1 "Test Results"       -> TestResultManagementWindow columns
//   Tab 2 "Unified Results"    -> UnifiedTestResultManagementWindow columns
Page {
    id: resultPage
    title: qsTr("Result Management")
    property var data: []

    function refresh() { data = resultService.results() }
    function statusLabel(i) {
        var labels = [qsTr("Not Tested"), qsTr("In Progress"), qsTr("Passed"), qsTr("Failed"),
                      qsTr("Qualified"), qsTr("To Repair"), qsTr("To Replace"), qsTr("Scrapped")]
        return (i === undefined || i < 0 || i >= labels.length) ? "-" : labels[i]
    }

    Column {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 8

        Label {
            text: qsTr("Result Management")
            font.pixelSize: 20
            font.bold: true
        }

        TabBar {
            id: tabBar
            width: parent.width
            TabButton { text: qsTr("Test Results") }
            TabButton { text: qsTr("Unified Results") }
        }

        StackLayout {
            width: parent.width
            height: parent.height - tabBar.height - 78
            currentIndex: tabBar.currentIndex

            // ===== Tab 1: TestResultManagementWindow-style =====
            Item {
                Column {
                    anchors.fill: parent
                    spacing: 4
                    Rectangle {  // header
                        width: parent.width; height: 28; color: "#eee"
                        Row { anchors.fill: parent; anchors.margins: 8
                            Label { text: qsTr("Tester"); width: 120; font.bold: true }
                            Label { text: qsTr("Date"); width: 130; font.bold: true }
                            Label { text: qsTr("Model"); width: 120; font.bold: true }
                            Label { text: qsTr("Serial No"); width: 150; font.bold: true }
                            Label { text: qsTr("Manufacturer"); width: 160; font.bold: true }
                        }
                    }
                    ListView {
                        width: parent.width; height: parent.height - 32; clip: true
                        model: resultPage.data
                        delegate: Rectangle {
                            width: parent.width; height: 34
                            color: index % 2 ? "#fafafa" : "#ffffff"
                            Row { anchors.fill: parent; anchors.margins: 8
                                Label { text: modelData.tester || "-"; width: 120; anchors.verticalCenter: parent.verticalCenter }
                                Label { text: modelData.date || "-"; width: 130; anchors.verticalCenter: parent.verticalCenter }
                                Label { text: modelData.sampleModel || "-"; width: 120; anchors.verticalCenter: parent.verticalCenter }
                                Label { text: modelData.sampleSerial || "-"; width: 150; anchors.verticalCenter: parent.verticalCenter }
                                Label { text: modelData.manufacturer || "-"; width: 160; anchors.verticalCenter: parent.verticalCenter }
                                Button {
                                    text: qsTr("Delete")
                                    anchors.verticalCenter: parent.verticalCenter
                                    onClicked: { resultService.removeResult(modelData.id); resultPage.refresh() }
                                }
                            }
                        }
                    }
                }
            }


            // ===== Tab 2: UnifiedTestResultManagementWindow-style =====
            Item {
                Column {
                    anchors.fill: parent
                    spacing: 4
                    Rectangle {  // header
                        width: parent.width; height: 28; color: "#eee"
                        Row { anchors.fill: parent; anchors.margins: 8
                            Label { text: qsTr("Manufacturer"); width: 150; font.bold: true }
                            Label { text: qsTr("Cylinder No"); width: 140; font.bold: true }
                            Label { text: qsTr("Date"); width: 140; font.bold: true }
                            Label { text: qsTr("Tester"); width: 120; font.bold: true }
                            Label { text: qsTr("Result"); width: 120; font.bold: true }
                        }
                    }
                    ListView {
                        width: parent.width; height: parent.height - 32; clip: true
                        model: resultPage.data
                        delegate: Rectangle {
                            width: parent.width; height: 34
                            color: index % 2 ? "#fafafa" : "#ffffff"
                            Row { anchors.fill: parent; anchors.margins: 8
                                Label { text: modelData.manufacturer || "-"; width: 150; anchors.verticalCenter: parent.verticalCenter }
                                Label { text: modelData.sampleSerial || "-"; width: 140; anchors.verticalCenter: parent.verticalCenter }
                                Label { text: modelData.date || "-"; width: 140; anchors.verticalCenter: parent.verticalCenter }
                                Label { text: modelData.tester || "-"; width: 120; anchors.verticalCenter: parent.verticalCenter }
                                Label { text: resultPage.statusLabel(modelData.overall); width: 120; anchors.verticalCenter: parent.verticalCenter }
                                Button {
                                    text: qsTr("Details")
                                    anchors.verticalCenter: parent.verticalCenter
                                    onClicked: stack.push("ResultDetailsPage.qml", { resultId: modelData.id })
                                }
                            }
                        }
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

