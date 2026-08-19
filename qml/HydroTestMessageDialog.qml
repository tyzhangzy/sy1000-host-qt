import QtQuick
import QtQuick.Controls

// Hydrostatic test message dialog: shows the state machine's operation
// instruction and asks the operator to confirm (OK) or cancel. The response is
// forwarded to `hydro.respondConfirm()`, which resumes the waiting sub-task.
Dialog {
    id: dlg
    property string promptTitle: qsTr("Operation")
    property string promptMessage: qsTr("")

    title: promptTitle
    modal: true
    anchors.centerIn: parent
    width: 460
    standardButtons: Dialog.Ok | Dialog.Cancel

    // Show a request; set title/message then open.
    function showRequest(title, message) {
        promptTitle = title
        promptMessage = message
        open()
    }

    contentItem: Column {
        spacing: 12
        width: parent.width
        Label {
            text: dlg.promptMessage
            width: parent.width
            wrapMode: Text.WordWrap
            font.pixelSize: 15
        }
    }

    onAccepted: hydro.respondConfirm(true)
    onRejected: hydro.respondConfirm(false)
}
