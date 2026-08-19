import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material

// Label (left) + a long underline line (input field), label bottom-aligned
// with the underline. Used inside the inspection group components.
Row {
    id: root
    property string label
    property alias text: fld.text
    spacing: 8
    Label {
        text: root.label
        width: 160
        font.pixelSize: 16
        color: "#333"
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 9
    }
    TextField {
        id: fld
        width: root.width - 168
        height: 40
        Material.background: "transparent"
        padding: 4
        // The long underline line.
        Rectangle {
            anchors.left: parent.left; anchors.right: parent.right
            anchors.bottom: parent.bottom
            height: 1
            color: "#9E9E9E"
        }
    }
}
