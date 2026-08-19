import QtQuick
import QtQuick.Controls

// Appearance inspection dialog, matched to WPF AppearenceInspectionWindow.xaml
// ("复合气瓶外观检查评估表"). Result int values match InspectionResult enum:
// 0=Qualified, 1=ToRepair, 2=ToReplace, 3=Scrapped.
Dialog {
    id: dlg
    property int sampleIndex: 0
    property var target: ({})

    title: qsTr("Appearance Inspection")
    modal: true
    anchors.centerIn: parent
    width: 860
    height: Math.min(parent.height - 40, 820)

    property var resultLabels: [qsTr("Qualified"), qsTr("To Repair"), qsTr("Scrapped")]

    // Reset all fields (WPF "重置" button).
    function reset() {
        extR0.checked = true; intR0.checked = true; thrR0.checked = true; valR0.checked = true
        extThermal.checked = extScratch.checked = extWear.checked = extDela.checked = extDeform.checked = false
        extDefect.text = extOther.text = ""
        intSmell.checked = false; intDebris.text = intSurface.text = intDefect.text = intOther.text = ""
        thrSpec.text = thrCond.text = thrEval.text = thrOther.text = ""
        valNo.text = valThreadCond.text = valAirtight.text = valOther.text = ""
        valDiaphragm.checked = false
    }

    // Safe access to the inspection object (create it lazily if missing).
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

    ScrollView {
        anchors.fill: parent
        anchors.margins: 12
        clip: true
        contentWidth: availableWidth

        Column {
            spacing: 14
            width: parent.width

            // ===== External (外部检查) =====
            SectionBox {
                title: qsTr("External")
                Column {
                    spacing: 10
                    width: parent.width
                    Row {
                        spacing: 20
                        CheckBox { id: extThermal; text: qsTr("Thermal damage") }
                        CheckBox { id: extScratch; text: qsTr("Scratch") }
                        CheckBox { id: extWear; text: qsTr("Wear") }
                        CheckBox { id: extDela; text: qsTr("Delamination") }
                        CheckBox { id: extDeform; text: qsTr("Deformation") }
                    }
                    Row {
                        spacing: 8
                        Label { text: qsTr("Defect location") + ":"; width: 160; anchors.verticalCenter: parent.verticalCenter }
                        TextField { id: extDefect; width: parent.width - 160 }
                    }
                    Row {
                        spacing: 8
                        Label { text: qsTr("Other") + ":"; width: 160; anchors.verticalCenter: parent.verticalCenter }
                        TextField { id: extOther; width: parent.width - 160 }
                    }
                    Row {
                        spacing: 20
                        Label { text: qsTr("Result") + ":"; width: 160; anchors.verticalCenter: parent.verticalCenter }
                        RadioButton { id: extR0; text: dlg.resultLabels[0] }
                        RadioButton { id: extR1; text: dlg.resultLabels[1] }
                        RadioButton { id: extR2; text: dlg.resultLabels[2] }
                    }
                }
            }

            // ===== Internal (内部检查) =====
            SectionBox {
                title: qsTr("Internal")
                Column {
                    spacing: 10
                    width: parent.width
                    Row {
                        spacing: 20
                        CheckBox { id: intSmell; text: qsTr("Smell present") }
                        Label { text: qsTr("Debris") + ":"; anchors.verticalCenter: parent.verticalCenter }
                        TextField { id: intDebris; width: 320 }
                    }
                    Row {
                        spacing: 8
                        Label { text: qsTr("Surface condition") + ":"; width: 160; anchors.verticalCenter: parent.verticalCenter }
                        TextField { id: intSurface; width: parent.width - 160 }
                    }
                    Row {
                        spacing: 8
                        Label { text: qsTr("Defect location") + ":"; width: 160; anchors.verticalCenter: parent.verticalCenter }
                        TextField { id: intDefect; width: parent.width - 160 }
                    }
                    Row {
                        spacing: 8
                        Label { text: qsTr("Other") + ":"; width: 160; anchors.verticalCenter: parent.verticalCenter }
                        TextField { id: intOther; width: parent.width - 160 }
                    }
                    Row {
                        spacing: 20
                        Label { text: qsTr("Result") + ":"; width: 160; anchors.verticalCenter: parent.verticalCenter }
                        RadioButton { id: intR0; text: dlg.resultLabels[0] }
                        RadioButton { id: intR1; text: dlg.resultLabels[1] }
                        RadioButton { id: intR2; text: dlg.resultLabels[2] }
                    }
                }
            }

            // ===== Thread (瓶口螺纹检查) =====
            SectionBox {
                title: qsTr("Thread")
                Column {
                    spacing: 10
                    width: parent.width
                    Row {
                        spacing: 8
                        Label { text: qsTr("Specification") + ":"; anchors.verticalCenter: parent.verticalCenter }
                        TextField { id: thrSpec; width: 220 }
                        Label { text: qsTr("Condition") + ":"; anchors.verticalCenter: parent.verticalCenter }
                        TextField { id: thrCond; width: 220 }
                    }
                    Row {
                        spacing: 8
                        Label { text: qsTr("Evaluation") + ":"; width: 160; anchors.verticalCenter: parent.verticalCenter }
                        TextField { id: thrEval; width: parent.width - 160 }
                    }
                    Row {
                        spacing: 8
                        Label { text: qsTr("Other") + ":"; width: 160; anchors.verticalCenter: parent.verticalCenter }
                        TextField { id: thrOther; width: parent.width - 160 }
                    }
                    Row {
                        spacing: 20
                        Label { text: qsTr("Result") + ":"; width: 160; anchors.verticalCenter: parent.verticalCenter }
                        RadioButton { id: thrR0; text: dlg.resultLabels[0] }
                        RadioButton { id: thrR1; text: dlg.resultLabels[1] }
                        RadioButton { id: thrR2; text: dlg.resultLabels[2] }
                    }
                }
            }

            // ===== Valve (气瓶阀检查) =====
            SectionBox {
                title: qsTr("Valve")
                Column {
                    spacing: 10
                    width: parent.width
                    Row {
                        spacing: 8
                        Label { text: qsTr("Valve No") + ":"; anchors.verticalCenter: parent.verticalCenter }
                        TextField { id: valNo; width: 220 }
                        Label { text: qsTr("Thread condition") + ":"; anchors.verticalCenter: parent.verticalCenter }
                        TextField { id: valThreadCond; width: 220 }
                    }
                    Row {
                        spacing: 8
                        Label { text: qsTr("Air tightness") + ":"; width: 160; anchors.verticalCenter: parent.verticalCenter }
                        TextField { id: valAirtight; width: parent.width - 160 }
                    }
                    CheckBox { id: valDiaphragm; text: qsTr("Diaphragm replaced") }
                    Row {
                        spacing: 8
                        Label { text: qsTr("Other") + ":"; width: 160; anchors.verticalCenter: parent.verticalCenter }
                        TextField { id: valOther; width: parent.width - 160 }
                    }
                    Row {
                        spacing: 20
                        Label { text: qsTr("Result") + ":"; width: 160; anchors.verticalCenter: parent.verticalCenter }
                        RadioButton { id: valR0; text: dlg.resultLabels[0] }
                        RadioButton { id: valR1; text: dlg.resultLabels[1] }
                        RadioButton { id: valR2; text: dlg.resultLabels[2] }
                    }
                }
            }
        }
    }

    standardButtons: Dialog.NoButton
    footer: DialogButtonBox {
        Button { text: qsTr("Save and Close"); DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole }
        Button { text: qsTr("Reset"); DialogButtonBox.buttonRole: DialogButtonBox.ResetRole; onClicked: dlg.reset() }
    }

    onOpened: {
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

    onAccepted: {
        var it = insp()
        it.external = extR0.checked ? 0 : extR1.checked ? 1 : 3
        it.internal = intR0.checked ? 0 : intR1.checked ? 1 : 3
        it.thread = thrR0.checked ? 0 : thrR1.checked ? 1 : 3
        it.valve = valR0.checked ? 0 : valR1.checked ? 1 : 3
        it.inspectionDate = dlg.todayStr()
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

    // Reusable titled section container (WPF GroupBox).
    component SectionBox: Rectangle {
        id: sectionRoot
        property string title
        default property alias content: contentCol.data
        width: parent.width
        radius: 6
        border.color: "#CCCCCC"
        color: "#fafafa"
        implicitHeight: contentCol.implicitHeight + 20
        Column {
            id: contentCol
            anchors.fill: parent
            anchors.margins: 10
            spacing: 6
            Label {
                text: sectionRoot.title
                font.bold: true
                font.pixelSize: 15
                color: "#303F9F"
            }
        }
    }
}