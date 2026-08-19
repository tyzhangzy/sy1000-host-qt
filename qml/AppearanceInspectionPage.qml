import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material

// "复合气瓶外观检查评估表" — a sub-page (uses the global app header), composed
// of four independent inspection group components (External/Internal/Thread/Valve).
Page {
    id: page
    title: qsTr("Appearance Inspection")
    property int sampleIndex: 0
    property var target: ({})

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

    // Content area (below the global app header).
    ScrollView {
        anchors.fill: parent
        clip: true
        contentWidth: availableWidth
        Column {
            width: parent.width
            anchors.margins: 20
            spacing: 18

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