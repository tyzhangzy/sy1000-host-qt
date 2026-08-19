import QtQuick
import QtQuick.Controls

// Material-style elevated card: simulated drop shadow (stacked translucent
// layers, since Qt6 removed GraphicalEffects), rounded corners, and a subtle
// hover highlight. Mirrors the WPF MaterialDesign Card (Elevation, radius 10).
Item {
    id: card
    property string title: ""
    property string icon: "◈"
    property color accent: "#303F9F"
    property int elevation: 4
    property bool hovered: false
    signal clicked()

    implicitWidth: 260
    implicitHeight: 150

    // Simulated drop shadow (bottom-right, alpha decreasing outward).
    Rectangle { z: -4; radius: 14; color: "#26000000"
        anchors.fill: parent; anchors.leftMargin: 6; anchors.topMargin: 6 }
    Rectangle { z: -3; radius: 13; color: "#1C000000"
        anchors.fill: parent; anchors.leftMargin: 4; anchors.topMargin: 4 }
    Rectangle { z: -2; radius: 12; color: "#12000000"
        anchors.fill: parent; anchors.leftMargin: 2; anchors.topMargin: 2 }

    // Card face: filled MaterialDesign-RaisedButton style (accent bg, white icon/text).
    Rectangle {
        id: face
        anchors.fill: parent
        radius: 10
        color: hovered ? Qt.darker(accent, 1.25) : accent
        Behavior on color { ColorAnimation { duration: 150 } }

        Column {
            anchors.fill: parent
            anchors.margins: 12
            spacing: 10

            Label {
                text: card.icon
                color: "white"
                font.pixelSize: 40
                anchors.horizontalCenter: parent.horizontalCenter
            }
            Label {
                text: card.title
                color: "white"
                font.pixelSize: 18
                font.bold: true
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        onEntered: card.hovered = true
        onExited: card.hovered = false
        onClicked: card.clicked()
    }
}
