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

    StackView {
        id: stack
        anchors.fill: parent
        initialItem: "LoginPage.qml"
    }

    // After a successful login, navigate to the test page.
    Connections {
        target: loginService
        function onLoginSucceeded() {
            stack.replace("TestPage.qml")
        }
    }
}

