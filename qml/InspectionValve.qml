import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material

// 气瓶阀检查 group component.
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
                text: qsTr("Valve")
                color: "white"; font.pixelSize: 18; font.bold: true
            }
        }
        Row {
            spacing: 8
            Label { text: qsTr("Valve No") + ":"; width: 160; font.pixelSize: 16; color: "#333"; anchors.bottom: parent.bottom; anchors.bottomMargin: 9 }
            TextField { id: valNo; width: parent.width - 168; height: 40; Material.background: "transparent" }
        }
        Row {
            spacing: 8
            Label { text: qsTr("Thread condition") + ":"; width: 160; font.pixelSize: 16; color: "#333"; anchors.bottom: parent.bottom; anchors.bottomMargin: 9 }
            TextField { id: valThreadCond; width: parent.width - 168; height: 40; Material.background: "transparent" }
        }
        Row {
            spacing: 8
            Label { text: qsTr("Air tightness") + ":"; width: 160; font.pixelSize: 16; color: "#333"; anchors.bottom: parent.bottom; anchors.bottomMargin: 9 }
            TextField { id: valAirtight; width: parent.width - 168; height: 40; Material.background: "transparent" }
        }
        CheckBox { id: valDiaphragm; text: qsTr("Diaphragm replaced") }
        Row {
            spacing: 8
            Label { text: qsTr("Other") + ":"; width: 160; font.pixelSize: 16; color: "#333"; anchors.bottom: parent.bottom; anchors.bottomMargin: 9 }
            TextField { id: valOther; width: parent.width - 168; height: 40; Material.background: "transparent" }
        }
        Row {
            spacing: 20
            Label { text: qsTr("Result") + ":"; width: 160; anchors.verticalCenter: parent.verticalCenter; font.pixelSize: 16; color: "#333" }
            RadioButton { id: valR0; text: qsTr("Qualified") }
            RadioButton { id: valR1; text: qsTr("To Repair") }
            RadioButton { id: valR2; text: qsTr("Scrapped") }
        }
    }

    function load() {
        var it = inspection
        valR0.checked = (it.valve === undefined || it.valve === 0); valR1.checked = it.valve === 1; valR2.checked = it.valve === 3
        valNo.text = it.valveNo || ""
        valThreadCond.text = it.valveThreadCondition || ""
        valAirtight.text = it.valveAirTightness || ""
        valDiaphragm.checked = !!it.valveDiaphragmReplaced
        valOther.text = it.valveOther || ""
    }

    function save(it) {
        it.valve = valR0.checked ? 0 : valR1.checked ? 1 : 3
        it.valveNo = valNo.text
        it.valveThreadCondition = valThreadCond.text
        it.valveAirTightness = valAirtight.text
        it.valveDiaphragmReplaced = valDiaphragm.checked
        it.valveOther = valOther.text
    }

    function reset() {
        valR0.checked = true
        valNo.text = valThreadCond.text = valAirtight.text = valOther.text = ""
        valDiaphragm.checked = false
    }

    Component.onCompleted: root.load()
}
