import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts

// 外部检查 group component (deep-blue title bar + fields + result radios).
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
        // Deep-blue title bar.
        Rectangle {
            width: parent.width; height: 44; radius: 8; color: "#303F9F"
            Label {
                anchors.left: parent.left; anchors.leftMargin: 16
                anchors.verticalCenter: parent.verticalCenter
                text: qsTr("External")
                color: "white"; font.pixelSize: 18; font.bold: true
            }
        }
        // 5 checkboxes evenly spaced.
        RowLayout {
            width: parent.width
            CheckBox { id: extThermal; text: qsTr("Thermal damage"); Layout.fillWidth: true }
            CheckBox { id: extScratch; text: qsTr("Scratch"); Layout.fillWidth: true }
            CheckBox { id: extWear; text: qsTr("Wear"); Layout.fillWidth: true }
            CheckBox { id: extDela; text: qsTr("Delamination"); Layout.fillWidth: true }
            CheckBox { id: extDeform; text: qsTr("Deformation"); Layout.fillWidth: true }
        }
        // Label + underlined field, bottom-aligned.
        Row {
            spacing: 8
            Label { text: qsTr("Defect location") + ":"; width: 160; font.pixelSize: 16; color: "#333"; anchors.bottom: parent.bottom; anchors.bottomMargin: 9 }
            TextField { id: extDefect; width: parent.width - 168; height: 40; Material.background: "transparent" }
        }
        Row {
            spacing: 8
            Label { text: qsTr("Other") + ":"; width: 160; font.pixelSize: 16; color: "#333"; anchors.bottom: parent.bottom; anchors.bottomMargin: 9 }
            TextField { id: extOther; width: parent.width - 168; height: 40; Material.background: "transparent" }
        }
        Row {
            spacing: 20
            Label { text: qsTr("Result") + ":"; width: 160; anchors.verticalCenter: parent.verticalCenter; font.pixelSize: 16; color: "#333" }
            RadioButton { id: extR0; text: qsTr("Qualified") }
            RadioButton { id: extR1; text: qsTr("To Repair") }
            RadioButton { id: extR2; text: qsTr("Scrapped") }
        }
    }

    function load() {
        var it = inspection
        extR0.checked = (it.external === undefined || it.external === 0); extR1.checked = it.external === 1; extR2.checked = it.external === 3
        extThermal.checked = !!it.externalThermalDamage
        extScratch.checked = !!it.externalScratch
        extWear.checked = !!it.externalWear
        extDela.checked = !!it.externalDelamination
        extDeform.checked = !!it.externalDeformation
        extDefect.text = it.externalDefectLocation || ""
        extOther.text = it.externalOther || ""
    }

    function save(it) {
        it.external = extR0.checked ? 0 : extR1.checked ? 1 : 3
        it.externalThermalDamage = extThermal.checked
        it.externalScratch = extScratch.checked
        it.externalWear = extWear.checked
        it.externalDelamination = extDela.checked
        it.externalDeformation = extDeform.checked
        it.externalDefectLocation = extDefect.text
        it.externalOther = extOther.text
    }

    function reset() {
        extR0.checked = true
        extThermal.checked = extScratch.checked = extWear.checked = extDela.checked = extDeform.checked = false
        extDefect.text = extOther.text = ""
    }

    Component.onCompleted: root.load()
}
