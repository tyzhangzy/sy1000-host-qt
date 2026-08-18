import QtQuick
import QtQuick.Controls

ApplicationWindow {
    id: root
    title: qsTr("SY1000 - Hydrostatic Test System")
    width: 900
    height: 640
    minimumWidth: 800
    minimumHeight: 560
    visible: true

    // 顶部标题栏
    header: ToolBar {
        Label {
            anchors.centerIn: parent
            text: qsTr("SY1000 Hydrostatic Test System")
            font.bold: true
        }
    }

    // 登录页内容（后续：登录成功 → 主菜单 → 测试页）
    Loader {
        anchors.fill: parent
        source: "LoginPage.qml"
    }

    footer: Label {
        text: qsTr("core / devices / dao / report / ui")
        horizontalAlignment: Text.AlignHCenter
        color: "#aaa"
        padding: 6
    }
}
