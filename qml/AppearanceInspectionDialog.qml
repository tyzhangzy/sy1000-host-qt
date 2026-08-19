import QtQuick
import QtQuick.Controls

// Appearance inspection dialog (full WPF-style form).
// `target.inspection` is the sample's inspection object (all fields written back
// on accept). Result values are stored as int matching InspectionResult enum:
// 0=Qualified, 1=ToRepair, 2=ToReplace, 3=Scrapped.
Dialog {
    id: dlg
    property int sampleIndex: 0
    property var target: ({})

    title: qsTr("Appearance Inspection - Sample %1").arg(sampleIndex + 1)
    modal: true
    anchors.centerIn: parent
    width: 640
    height: Math.min(parent.height - 60, 780)

    property var resultLabels: [qsTr("Qualified"), qsTr("To Repair"), qsTr("To Replace"), qsTr("Scrapped")]

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
        anchors.leftMargin: 12
        anchors.rightMargin: 12
        anchors.topMargin: 12
        clip: true
        contentWidth: availableWidth

        Column {
            spacing: 12
            width: parent.width

            // Inspector information.
            Label { text: qsTr("Inspector"); font.bold: true; font.pixelSize: 16 }
            Grid {
                columns: 2
                spacing: 8
                columnSpacing: 12
                Label { text: qsTr("Name"); width: 90 }
                TextField { id: inspName; width: 220; placeholderText: qsTr("Inspector name") }
                Label { text: qsTr("Cert No"); width: 90 }
                TextField { id: inspCert; width: 220; placeholderText: qsTr("Certificate No") }
                Label { text: qsTr("Date"); width: 90 }
                TextField { id: inspDate; width: 220; placeholderText: "yyyy-MM-dd" }
            }

            // ============ External (appearance) ============
            SectionBox {
                title: qsTr("External")
                Column {
                    spacing: 8
                    width: parent.width
                    Row {
                        spacing: 10
                        Label { text: qsTr("Result"); width: 90; anchors.verticalCenter: parent.verticalCenter }
                        ComboBox { id: extCb; width: 220; model: dlg.resultLabels }
                    }
                    Row {
                        spacing: 14
                        CheckBox { id: extThermal; text: qsTr("Thermal damage") }
                        CheckBox { id: extScratch; text: qsTr("Scratch") }
                        CheckBox { id: extWear; text: qsTr("Wear") }
                    }
                    Row {
                        spacing: 14
                        CheckBox { id: extDela; text: qsTr("Delamination") }
                        CheckBox { id: extDeform; text: qsTr("Deformation") }
                    }
                    Label { text: qsTr("Defect location"); font.bold: true }
                    TextField { id: extDefect; width: parent.width; placeholderText: qsTr("Defect location") }
                    Label { text: qsTr("Other"); font.bold: true }
                    TextField { id: extOther; width: parent.width; placeholderText: qsTr("Other notes") }
                }
            }

            // ============ Internal ============
            SectionBox {
                title: qsTr("Internal")
                Column {
                    spacing: 8
                    width: parent.width
                    Row {
                        spacing: 10
                        Label { text: qsTr("Result"); width: 90; anchors.verticalCenter: parent.verticalCenter }
                        ComboBox { id: intCb; width: 220; model: dlg.resultLabels }
                    }
                    CheckBox { id: intSmell; text: qsTr("Smell present") }
                    Label { text: qsTr("Debris"); font.bold: true }
                    TextField { id: intDebris; width: parent.width; placeholderText: qsTr("Debris description") }
                    Label { text: qsTr("Surface condition"); font.bold: true }
                    TextField { id: intSurface; width: parent.width; placeholderText: qsTr("Surface condition") }
                    Label { text: qsTr("Defect location"); font.bold: true }
                    TextField { id: intDefect; width: parent.width; placeholderText: qsTr("Defect location") }
                    Label { text: qsTr("Other"); font.bold: true }
                    TextField { id: intOther; width: parent.width; placeholderText: qsTr("Other notes") }
                }
            }

            // ============ Thread ============
            SectionBox {
                title: qsTr("Thread")
                Column {
                    spacing: 8
                    width: parent.width
                    Row {
                        spacing: 10
                        Label { text: qsTr("Result"); width: 90; anchors.verticalCenter: parent.verticalCenter }
                        ComboBox { id: thrCb; width: 220; model: dlg.resultLabels }
                    }
                    Label { text: qsTr("Specification"); font.bold: true }
                    TextField { id: thrSpec; width: parent.width; placeholderText: qsTr("Thread specification") }
                    Label { text: qsTr("Condition"); font.bold: true }
                    TextField { id: thrCond; width: parent.width; placeholderText: qsTr("Thread condition") }
                    Label { text: qsTr("Evaluation"); font.bold: true }
                    TextField { id: thrEval; width: parent.width; placeholderText: qsTr("Thread evaluation") }
                    Label { text: qsTr("Other"); font.bold: true }
                    TextField { id: thrOther; width: parent.width; placeholderText: qsTr("Other notes") }
                }
            }

            // ============ Valve ============
            SectionBox {
                title: qsTr("Valve")
                Column {
                    spacing: 8
                    width: parent.width
                    Row {
                        spacing: 10
                        Label { text: qsTr("Result"); width: 90; anchors.verticalCenter: parent.verticalCenter }
                        ComboBox { id: valCb; width: 220; model: dlg.resultLabels }
                    }
                    Label { text: qsTr("Valve No"); font.bold: true }
                    TextField { id: valNo; width: parent.width; placeholderText: qsTr("Valve number") }
                    Label { text: qsTr("Thread condition"); font.bold: true }
                    TextField { id: valThreadCond; width: parent.width; placeholderText: qsTr("Valve thread condition") }
                    Label { text: qsTr("Air tightness"); font.bold: true }
                    TextField { id: valAirtight; width: parent.width; placeholderText: qsTr("Air tightness") }
                    CheckBox { id: valDiaphragm; text: qsTr("Diaphragm replaced") }
                    Label { text: qsTr("Other"); font.bold: true }
                    TextField { id: valOther; width: parent.width; placeholderText: qsTr("Other notes") }
                }
            }

        }
    }

    standardButtons: Dialog.Ok | Dialog.Cancel

    onOpened: {
        var it = insp()
        extCb.currentIndex = it.external !== undefined ? it.external : 0
        intCb.currentIndex = it.internal !== undefined ? it.internal : 0
        thrCb.currentIndex = it.thread !== undefined ? it.thread : 0
        valCb.currentIndex = it.valve !== undefined ? it.valve : 0
        inspName.text = it.inspectorName || ""
        inspCert.text = it.inspectorCertNo || ""
        inspDate.text = it.inspectionDate || ""
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
        it.external = extCb.currentIndex
        it.internal = intCb.currentIndex
        it.thread = thrCb.currentIndex
        it.valve = valCb.currentIndex
        it.inspectorName = inspName.text
        it.inspectorCertNo = inspCert.text
        it.inspectionDate = inspDate.text !== "" ? inspDate.text : dlg.todayStr()
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

    // Reusable titled section container. Children provided by the caller are
    // appended to the inner Column (below the title) via the default alias.
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


