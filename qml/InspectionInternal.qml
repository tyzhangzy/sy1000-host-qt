import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material

// 内部检查 group component.
Rectangle {
    id: root
    property var inspection: ({})
    width: parent.width
    radius: 8
    border.color: "#DDDDDD"
    color: "white"
    implicitHeight: col.implicitHeight

    Column {
        id: col
        width: parent.width
        spacing: 14
        Rectangle {
            width: parent.width; height: 44; radius: 8; color: "#303F9F"
            Label {
                anchors.left: parent.left; anchors.leftMargin: 16
                anchors.verticalCenter: parent.verticalCenter
                text: qsTr("Internal")
                color: "white"; font.pixelSize: 18; font.bold: true
            }
        }
        CheckBox { id: intSmell; text: qsTr("Smell present") }
        Row {
            spacing: 8
            Label { text: qsTr("Debris") + ":"; width: 160; font.pixelSize: 16; color: "#333"; anchors.bottom: parent.bottom; anchors.bottomMargin: 9 }
            TextField { id: intDebris; width: parent.width - 168; height: 40; Material.background: "transparent" }
        }
        Row {
            spacing: 8
            Label { text: qsTr("Surface condition") + ":"; width: 160; font.pixelSize: 16; color: "#333"; anchors.bottom: parent.bottom; anchors.bottomMargin: 9 }
            TextField { id: intSurface; width: parent.width - 168; height: 40; Material.background: "transparent" }
        }
        Row {
            spacing: 8
            Label { text: qsTr("Defect location") + ":"; width: 160; font.pixelSize: 16; color: "#333"; anchors.bottom: parent.bottom; anchors.bottomMargin: 9 }
            TextField { id: intDefect; width: parent.width - 168; height: 40; Material.background: "transparent" }
        }
        Row {
            spacing: 8
            Label { text: qsTr("Other") + ":"; width: 160; font.pixelSize: 16; color: "#333"; anchors.bottom: parent.bottom; anchors.bottomMargin: 9 }
            TextField { id: intOther; width: parent.width - 168; height: 40; Material.background: "transparent" }
        }
        Row {
            spacing: 20
            Label { text: qsTr("Result") + ":"; width: 160; anchors.verticalCenter: parent.verticalCenter; font.pixelSize: 16; color: "#333" }
            RadioButton { id: intR0; text: qsTr("Qualified") }
            RadioButton { id: intR1; text: qsTr("To Repair") }
            RadioButton { id: intR2; text: qsTr("Scrapped") }
        }
    }

    function load() {
        var it = inspection
        intR0.checked = (it.internal === undefined || it.internal === 0); intR1.checked = it.internal === 1; intR2.checked = it.internal === 3
        intSmell.checked = !!it.internalSmell
        intDebris.text = it.internalDebris || ""
        intSurface.text = it.internalSurfaceCondition || ""
        intDefect.text = it.internalDefectLocation || ""
        intOther.text = it.internalOther || ""
    }

    function save(it) {
        it.internal = intR0.checked ? 0 : intR1.checked ? 1 : 3
        it.internalSmell = intSmell.checked
        it.internalDebris = intDebris.text
        it.internalSurfaceCondition = intSurface.text
        it.internalDefectLocation = intDefect.text
        it.internalOther = intOther.text
    }

    function reset() {
        intR0.checked = true
        intSmell.checked = false
        intDebris.text = intSurface.text = intDefect.text = intOther.text = ""
    }

    Component.onCompleted: root.load()
}
