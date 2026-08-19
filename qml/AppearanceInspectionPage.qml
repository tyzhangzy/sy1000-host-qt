import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material

// "复合气瓶外观检查评估表" — a sub-page with a background-less title bar,
// composed of four independent inspection group components.
Page {
    id: page
    title: qsTr("Appearance Inspection")
    property int sampleIndex: 0
    property var target: ({})
    readonly property bool hideGlobalHeader: true

    function insp() {
        if (target.inspection === undefined || target.inspection === null)
            target.inspection = {}
        return target.inspection
    }

    function todayStr() {
        var d = new Date()
        var m = ("0" + (d.getMonth() + 1)).slice(-2)
        var day = ("0" + d.getDate()).slice(-2)
        return d.getFullYear() + "-" + m + "-" + day
    }

    function saveAll() {
        var it = insp()
        extGroup.save(it)
        intGroup.save(it)
        thrGroup.save(it)
        valGroup.save(it)
        it.inspectionDate = todayStr()
        it.inspectionCompleted = true
    }

    function resetAll() {
        extGroup.reset()
        intGroup.reset()
        thrGroup.reset()
        valGroup.reset()
    }

    Column {
        anchors.fill: parent

        // Title bar with no background color (dark text title only).
        Rectangle {
            width: parent.width; height: 64; color: "transparent"
            Label {
                anchors.centerIn: parent
                text: qsTr("Appearance Inspection")
                color: "#303F9F"; font.pixelSize: 24; font.bold: true
            }
        }

        // Content area.
        ScrollView {
            width: parent.width; height: parent.height - 64
            clip: true
            contentWidth: availableWidth
            Column {
                width: parent.width
                anchors.margins: 20
                spacing: 18

                // Inspector info (L7): the report's 检验员/证书号 fields come
                // from here (previously there was no QML entry point at all).
                Rectangle {
                    width: parent.width
                    radius: 8
                    color: "#F5F7FA"
                    border.color: "#DDDDDD"
                    Column {
                        width: parent.width
                        anchors.margins: 14
                        spacing: 10
                        Label { text: qsTr("Inspector"); font.bold: true; font.pixelSize: 16; color: "#303F9F" }
                        Row {
                            spacing: 12
                            Label { text: qsTr("Inspector name"); width: 130; anchors.verticalCenter: parent.verticalCenter; color: "#555" }
                            TextField {
                                width: 280
                                text: page.insp().inspectorName || ""
                                onTextChanged: page.insp().inspectorName = text
                            }
                            Label { text: qsTr("Certificate No"); width: 100; anchors.verticalCenter: parent.verticalCenter; color: "#555" }
                            TextField {
                                width: 220
                                text: page.insp().inspectorCertNo || ""
                                onTextChanged: page.insp().inspectorCertNo = text
                            }
                        }
                    }
                }

                InspectionExternal { id: extGroup; inspection: page.insp() }
                InspectionInternal { id: intGroup; inspection: page.insp() }
                InspectionThread { id: thrGroup; inspection: page.insp() }
                InspectionValve { id: valGroup; inspection: page.insp() }

                // 操作按钮
                Row {
                    anchors.horizontalCenter: parent.horizontalCenter
                    spacing: 16
                    Button {
                        text: qsTr("Save and Close")
                        width: 220; height: 44
                        Material.background: "#303F9F"; Material.foreground: "white"
                        font.pixelSize: 16; font.bold: true
                        onClicked: { page.saveAll(); stack.pop() }
                    }
                    Button {
                        text: qsTr("Reset")
                        width: 220; height: 44
                        Material.background: "#303F9F"; Material.foreground: "white"
                        font.pixelSize: 16; font.bold: true
                        onClicked: page.resetAll()
                    }
                }
            }
        }
    }
}