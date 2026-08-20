import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material

// Label (left) + a long underline line (input field). Used inside the
// inspection group components.
Row {
    id: root
    property string label
    property alias text: fld.text
    property int rowSpacing: 8
    width: parent.width
    spacing: root.rowSpacing
    Label {
        text: root.label
        width: 160
        font.pixelSize: 16
        color: "#333"
        horizontalAlignment: Text.AlignRight
        anchors.verticalCenter: parent.verticalCenter
    }
    TextField {
        id: fld
        width: root.width - 160 - root.rowSpacing
        height: 40
        background: Rectangle { color: "transparent" }
        padding: 4
        // The long underline line (no other border).
        Rectangle {
            anchors.left: parent.left; anchors.right: parent.right
            anchors.bottom: parent.bottom
            height: 1
            color: "#9E9E9E"
        }
    }
}
