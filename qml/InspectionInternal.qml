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
        Row {
            spacing: 12
            CheckBox { id: intSmell; text: qsTr("Smell present"); anchors.verticalCenter: parent.verticalCenter }
            Label { text: qsTr("Debris") + ":"; font.pixelSize: 16; color: "#333"; anchors.verticalCenter: parent.verticalCenter }
            TextField {
                id: intDebris
                width: 340
                height: 40
                background: Rectangle { color: "transparent" }
                padding: 4
                Rectangle {
                    anchors.left: parent.left; anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    height: 1
                    color: "#9E9E9E"
                }
            }
        }
        FieldRow { id: intSurface; label: qsTr("Surface condition") + ":" }
        FieldRow { id: intDefect; label: qsTr("Defect location") + ":" }
        FieldRow { id: intOther; label: qsTr("Other") + ":" }
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
