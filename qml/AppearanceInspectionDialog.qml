import QtQuick
import QtQuick.Controls

// Appearance inspection dialog (simplified): external/internal/thread/valve results.
// `target` is a reference to the sample's data object (results are written back).
Dialog {
    id: dlg
    property int sampleIndex: 0
    property var target: ({})

    title: qsTr("Appearance Inspection - Sample %1").arg(sampleIndex + 1)
    modal: true
    anchors.centerIn: parent
    width: 380

    property var resultOptions: [qsTr("Qualified"), qsTr("To Repair"), qsTr("To Replace"), qsTr("Scrapped")]

    Column {
        spacing: 10
        width: parent.width

        Row {
            spacing: 10
            Label { text: qsTr("External"); width: 90; anchors.verticalCenter: parent.verticalCenter }
            ComboBox { id: ext; width: 200; model: dlg.resultOptions }
        }
        Row {
            spacing: 10
            Label { text: qsTr("Internal"); width: 90; anchors.verticalCenter: parent.verticalCenter }
            ComboBox { id: intl; width: 200; model: dlg.resultOptions }
        }
        Row {
            spacing: 10
            Label { text: qsTr("Thread"); width: 90; anchors.verticalCenter: parent.verticalCenter }
            ComboBox { id: thr; width: 200; model: dlg.resultOptions }
        }
        Row {
            spacing: 10
            Label { text: qsTr("Valve"); width: 90; anchors.verticalCenter: parent.verticalCenter }
            ComboBox { id: val; width: 200; model: dlg.resultOptions }
        }
    }

    standardButtons: Dialog.Ok | Dialog.Cancel

    onOpened: {
        ext.currentIndex = Math.max(0, ["Qualified","To Repair","To Replace","Scrapped"].indexOf(target.external || ""))
        intl.currentIndex = Math.max(0, ["Qualified","To Repair","To Replace","Scrapped"].indexOf(target.internal || ""))
        thr.currentIndex = Math.max(0, ["Qualified","To Repair","To Replace","Scrapped"].indexOf(target.thread || ""))
        val.currentIndex = Math.max(0, ["Qualified","To Repair","To Replace","Scrapped"].indexOf(target.valve || ""))
    }
    onAccepted: {
        target.external = ext.currentText
        target.internal = intl.currentText
        target.thread = thr.currentText
        target.valve = val.currentText
        target.inspected = true
    }
}
