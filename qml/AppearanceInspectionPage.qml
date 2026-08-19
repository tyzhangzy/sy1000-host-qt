import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts

// "复合气瓶外观检查评估表" — a full PAGE (not a dialog), matched to WPF
// AppearenceInspectionWindow.xaml: title bar + inspection groups.
Page {
    id: page
    title: qsTr("Appearance Inspection")
    property int sampleIndex: 0
    property var target: ({})
    readonly property bool hideGlobalHeader: true

    property var resultLabels: [qsTr("Qualified"), qsTr("To Repair"), qsTr("Scrapped")]

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

    function reset() {
        extR0.checked = true; intR0.checked = true; thrR0.checked = true; valR0.checked = true
        extThermal.checked = extScratch.checked = extWear.checked = extDela.checked = extDeform.checked = false
        extDefect.text = extOther.text = ""
        intSmell.checked = false; intDebris.text = intSurface.text = intDefect.text = intOther.text = ""
        thrSpec.text = thrCond.text = thrEval.text = thrOther.text = ""
        valNo.text = valThreadCond.text = valAirtight.text = valOther.text = ""
        valDiaphragm.checked = false
    }

    // A titled group: deep-blue header bar + content below.
    component GroupCard: Rectangle {
        property string title
        default property alias content: cardCol.data
        width: parent.width
        radius: 8
        border.color: "#DDDDDD"
        color: "white"
        implicitHeight: cardCol.implicitHeight
        Column {
            id: cardCol
            width: parent.width
            spacing: 14
            Rectangle {
                width: parent.width; height: 44
                radius: 8
                color: "#303F9F"
                Label {
                    anchors.left: parent.left; anchors.leftMargin: 16
                    anchors.verticalCenter: parent.verticalCenter
                    text: title
                    color: "white"; font.pixelSize: 18; font.bold: true
                }
            }
        }
    }

    // A row: label (bottom-aligned with the underline) + underlined input field.
    component FieldRow: Row {
        property string label
        default property alias field: fieldSlot.data
        spacing: 8
        Label {
            text: label
            font.pixelSize: 16
            width: 160
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 9
            color: "#333"
        }
        Item { id: fieldSlot; width: parent.width - 168; height: 40 }
    }

    Column {
        anchors.fill: parent

        // Title bar: "复合气瓶外观检查评估表"
        Rectangle {
            width: parent.width; height: 64; color: "#303F9F"
            Label {
                anchors.centerIn: parent
                text: qsTr("Appearance Inspection")
                color: "white"; font.pixelSize: 26; font.bold: true
            }
        }

        // Content
        ScrollView {
            width: parent.width; height: parent.height - 64
            clip: true
            contentWidth: availableWidth
            Column {
                width: parent.width
                anchors.margins: 20
                spacing: 18

                // ===== External (外部检查) =====
                GroupCard {
                    title: qsTr("External")
                    Column {
                        spacing: 14
                        width: parent.width
                        RowLayout {
                            width: parent.width
                            CheckBox { id: extThermal; text: qsTr("Thermal damage"); Layout.fillWidth: true }
                            CheckBox { id: extScratch; text: qsTr("Scratch"); Layout.fillWidth: true }
                            CheckBox { id: extWear; text: qsTr("Wear"); Layout.fillWidth: true }
                            CheckBox { id: extDela; text: qsTr("Delamination"); Layout.fillWidth: true }
                            CheckBox { id: extDeform; text: qsTr("Deformation"); Layout.fillWidth: true }
                        }
                        FieldRow { label: qsTr("Defect location") + ":"
                            TextField { width: parent.width; height: 40; Material.background: "transparent"; id: extDefect }
                        }
                        FieldRow { label: qsTr("Other") + ":"
                            TextField { width: parent.width; height: 40; Material.background: "transparent"; id: extOther }
                        }
                        Row {
                            spacing: 20
                            Label { text: qsTr("Result") + ":"; width: 160; anchors.verticalCenter: parent.verticalCenter; font.pixelSize: 16; color: "#333" }
                            RadioButton { id: extR0; text: page.resultLabels[0] }
                            RadioButton { id: extR1; text: page.resultLabels[1] }
                            RadioButton { id: extR2; text: page.resultLabels[2] }
                        }
                    }
                }

                // ===== Internal (内部检查) =====
                GroupCard {
                    title: qsTr("Internal")
                    Column {
                        spacing: 14
                        width: parent.width
                        CheckBox { id: intSmell; text: qsTr("Smell present") }
                        FieldRow { label: qsTr("Debris") + ":"
                            TextField { width: parent.width; height: 40; Material.background: "transparent"; id: intDebris } }
                        FieldRow { label: qsTr("Surface condition") + ":"
                            TextField { width: parent.width; height: 40; Material.background: "transparent"; id: intSurface } }
                        FieldRow { label: qsTr("Defect location") + ":"
                            TextField { width: parent.width; height: 40; Material.background: "transparent"; id: intDefect } }
                        FieldRow { label: qsTr("Other") + ":"
                            TextField { width: parent.width; height: 40; Material.background: "transparent"; id: intOther } }
                        Row {
                            spacing: 20
                            Label { text: qsTr("Result") + ":"; width: 160; anchors.verticalCenter: parent.verticalCenter; font.pixelSize: 16; color: "#333" }
                            RadioButton { id: intR0; text: page.resultLabels[0] }
                            RadioButton { id: intR1; text: page.resultLabels[1] }
                            RadioButton { id: intR2; text: page.resultLabels[2] }
                        }
                    }
                }

                // ===== Thread (瓶口螺纹检查) =====
                GroupCard {
                    title: qsTr("Thread")
                    Column {
                        spacing: 14
                        width: parent.width
                        FieldRow { label: qsTr("Specification") + ":"
                            TextField { width: parent.width; height: 40; Material.background: "transparent"; id: thrSpec } }
                        FieldRow { label: qsTr("Condition") + ":"
                            TextField { width: parent.width; height: 40; Material.background: "transparent"; id: thrCond } }
                        FieldRow { label: qsTr("Evaluation") + ":"
                            TextField { width: parent.width; height: 40; Material.background: "transparent"; id: thrEval } }
                        FieldRow { label: qsTr("Other") + ":"
                            TextField { width: parent.width; height: 40; Material.background: "transparent"; id: thrOther } }
                        Row {
                            spacing: 20
                            Label { text: qsTr("Result") + ":"; width: 160; anchors.verticalCenter: parent.verticalCenter; font.pixelSize: 16; color: "#333" }
                            RadioButton { id: thrR0; text: page.resultLabels[0] }
                            RadioButton { id: thrR1; text: page.resultLabels[1] }
                            RadioButton { id: thrR2; text: page.resultLabels[2] }
                        }
                    }
                }

                // ===== Valve (气瓶阀检查) =====
                GroupCard {
                    title: qsTr("Valve")
                    Column {
                        spacing: 14
                        width: parent.width
                        FieldRow { label: qsTr("Valve No") + ":"
                            TextField { width: parent.width; height: 40; Material.background: "transparent"; id: valNo } }
                        FieldRow { label: qsTr("Thread condition") + ":"
                            TextField { width: parent.width; height: 40; Material.background: "transparent"; id: valThreadCond } }
                        FieldRow { label: qsTr("Air tightness") + ":"
                            TextField { width: parent.width; height: 40; Material.background: "transparent"; id: valAirtight } }
                        CheckBox { id: valDiaphragm; text: qsTr("Diaphragm replaced") }
                        FieldRow { label: qsTr("Other") + ":"
                            TextField { width: parent.width; height: 40; Material.background: "transparent"; id: valOther } }
                        Row {
                            spacing: 20
                            Label { text: qsTr("Result") + ":"; width: 160; anchors.verticalCenter: parent.verticalCenter; font.pixelSize: 16; color: "#333" }
                            RadioButton { id: valR0; text: page.resultLabels[0] }
                            RadioButton { id: valR1; text: page.resultLabels[1] }
                            RadioButton { id: valR2; text: page.resultLabels[2] }
                        }
                    }
                }

                // 操作按钮
                Row {
                    anchors.horizontalCenter: parent.horizontalCenter
                    spacing: 16
                    Button {
                        text: qsTr("Save and Close")
                        width: 220; height: 44
                        Material.background: "#303F9F"; Material.foreground: "white"
                        font.pixelSize: 16; font.bold: true
                        onClicked: { page.saveInspection(); stack.pop() }
                    }
                    Button {
                        text: qsTr("Reset")
                        width: 220; height: 44
                        Material.background: "#303F9F"; Material.foreground: "white"
                        font.pixelSize: 16; font.bold: true
                        onClicked: page.reset()
                    }
                }
            }
        }
    }

    // Save inspection back to target.inspection (called by "Save and Close").
    function saveInspection() {
        var it = insp()
        it.external = extR0.checked ? 0 : extR1.checked ? 1 : 3
        it.internal = intR0.checked ? 0 : intR1.checked ? 1 : 3
        it.thread = thrR0.checked ? 0 : thrR1.checked ? 1 : 3
        it.valve = valR0.checked ? 0 : valR1.checked ? 1 : 3
        it.inspectionDate = todayStr()
        it.externalThermalDamage = extThermal.checked
        it.externalScratch = extScratch.checked
        it.externalWear = extWear.checked
        it.externalDelamination = extDela.checked
        it.externalDeformation = extDeform.checked
        it.externalDefectLocation = extDefect.text
        it.externalOther = extOther.text
        it.internalSmell = intSmell.checked
        it.internalDebris = intDebris.text
        it.internalSurfaceCondition = intSurface.text
        it.internalDefectLocation = intDefect.text
        it.internalOther = intOther.text
        it.threadSpecification = thrSpec.text
        it.threadCondition = thrCond.text
        it.threadEvaluation = thrEval.text
        it.threadOther = thrOther.text
        it.valveNo = valNo.text
        it.valveThreadCondition = valThreadCond.text
        it.valveAirTightness = valAirtight.text
        it.valveDiaphragmReplaced = valDiaphragm.checked
        it.valveOther = valOther.text
        it.inspectionCompleted = true
    }

    Component.onCompleted: {
        var it = insp()
        extR0.checked = (it.external === undefined || it.external === 0); extR1.checked = it.external === 1; extR2.checked = it.external === 3
        intR0.checked = (it.internal === undefined || it.internal === 0); intR1.checked = it.internal === 1; intR2.checked = it.internal === 3
        thrR0.checked = (it.thread === undefined || it.thread === 0); thrR1.checked = it.thread === 1; thrR2.checked = it.thread === 3
        valR0.checked = (it.valve === undefined || it.valve === 0); valR1.checked = it.valve === 1; valR2.checked = it.valve === 3
        extThermal.checked = !!it.externalThermalDamage
        extScratch.checked = !!it.externalScratch
        extWear.checked = !!it.externalWear
        extDela.checked = !!it.externalDelamination
        extDeform.checked = !!it.externalDeformation
        extDefect.text = it.externalDefectLocation || ""
        extOther.text = it.externalOther || ""
        intSmell.checked = !!it.internalSmell
        intDebris.text = it.internalDebris || ""
        intSurface.text = it.internalSurfaceCondition || ""
        intDefect.text = it.internalDefectLocation || ""
        intOther.text = it.internalOther || ""
        thrSpec.text = it.threadSpecification || ""
        thrCond.text = it.threadCondition || ""
        thrEval.text = it.threadEvaluation || ""
        thrOther.text = it.threadOther || ""
        valNo.text = it.valveNo || ""
        valThreadCond.text = it.valveThreadCondition || ""
        valAirtight.text = it.valveAirTightness || ""
        valDiaphragm.checked = !!it.valveDiaphragmReplaced
        valOther.text = it.valveOther || ""
    }
}