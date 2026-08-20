import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material

// 瓶口螺纹检查 group component.
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
                text: qsTr("Thread")
                color: "white"; font.pixelSize: 18; font.bold: true
            }
        }
        Row {
            spacing: 8
            Label { text: qsTr("Specification") + ":"; font.pixelSize: 16; color: "#333"; anchors.verticalCenter: parent.verticalCenter }
            TextField { id: thrSpec; width: 220; height: 40; background: Rectangle { color: "transparent" }; padding: 4
                Rectangle { anchors.left: parent.left; anchors.right: parent.right; anchors.bottom: parent.bottom; height: 1; color: "#9E9E9E" }
            }
            Label { text: qsTr("Condition") + ":"; font.pixelSize: 16; color: "#333"; anchors.verticalCenter: parent.verticalCenter }
            TextField { id: thrCond; width: 220; height: 40; background: Rectangle { color: "transparent" }; padding: 4
                Rectangle { anchors.left: parent.left; anchors.right: parent.right; anchors.bottom: parent.bottom; height: 1; color: "#9E9E9E" }
            }
        }
        FieldRow { id: thrEval; label: qsTr("Evaluation") + ":" }
        FieldRow { id: thrOther; label: qsTr("Other") + ":" }
        Row {
            spacing: 20
            Label { text: qsTr("Result") + ":"; width: 160; anchors.verticalCenter: parent.verticalCenter; font.pixelSize: 16; color: "#333" }
            RadioButton { id: thrR0; text: qsTr("Qualified") }
            RadioButton { id: thrR1; text: qsTr("To Repair") }
            RadioButton { id: thrR2; text: qsTr("Scrapped") }
        }
    }

    function load() {
        var it = inspection
        thrR0.checked = (it.thread === undefined || it.thread === 0); thrR1.checked = it.thread === 1; thrR2.checked = it.thread === 3
        thrSpec.text = it.threadSpecification || ""
        thrCond.text = it.threadCondition || ""
        thrEval.text = it.threadEvaluation || ""
        thrOther.text = it.threadOther || ""
    }

    function save(it) {
        it.thread = thrR0.checked ? 0 : thrR1.checked ? 1 : 3
        it.threadSpecification = thrSpec.text
        it.threadCondition = thrCond.text
        it.threadEvaluation = thrEval.text
        it.threadOther = thrOther.text
    }

    function reset() {
        thrR0.checked = true
        thrSpec.text = thrCond.text = thrEval.text = thrOther.text = ""
    }

    Component.onCompleted: root.load()
}
