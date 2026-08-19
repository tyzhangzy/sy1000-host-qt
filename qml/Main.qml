import QtQuick
import QtQuick.Controls

ApplicationWindow {
    id: root
    title: qsTr("SY1000 - Hydrostatic Test System")
    // Frameless, maximized window to match the WPF LoginWindow (WindowStyle=None).
    flags: Qt.Window | Qt.FramelessWindowHint
    width: 1920
    height: 1080
    visibility: Window.Maximized
    minimumWidth: 1280
    minimumHeight: 800
    visible: true

    // Top title bar (matches WPF MenuWindow ColorZone header). Hidden on the
    // login page (WPF LoginWindow is frameless with no header).
    header: Rectangle {
        height: 80
        color: "#303F9F"
        visible: stack.currentItem ? (!stack.currentItem.isLoginPage && !stack.currentItem.hideGlobalHeader) : false

        // Left: device name (config) + connection status button (WPF header).
        Row {
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.verticalCenter: parent.verticalCenter
            spacing: 16
            // Hamburger button (WPF MaterialDesignHamburgerToggleButton).
            Button {
                text: "☰"
                font.pixelSize: 26
                font.bold: true
                onClicked: drawer.open()
                background: Rectangle { color: "transparent"; radius: 4 }
            }
            Label {
                text: deviceService.deviceName()
                color: "white"
                font.pixelSize: 24
                font.bold: true
                anchors.verticalCenter: parent.verticalCenter
            }
            Button {
                text: qsTr("Connection Status")
                font.bold: true
                onClicked: connStatus.text = deviceService.connectDevices()
                background: Rectangle { color: "white"; radius: 4 }
            }
        }

        // Center: current page title (WPF "主菜单" header).
        Label {
            id: titleLabel
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            text: stack.currentItem.title
            color: "white"
            font.pixelSize: 24
            font.bold: true
        }

        // Right: logged-in user (Account icon + name) + quit button (WPF header).
        Row {
            anchors.right: parent.right
            anchors.rightMargin: 20
            anchors.verticalCenter: parent.verticalCenter
            spacing: 14
            Label {
                text: "👤"
                color: "white"
                font.pixelSize: 22
                anchors.verticalCenter: parent.verticalCenter
            }
            Label {
                text: loginService.username
                color: "white"
                font.pixelSize: 18
                anchors.verticalCenter: parent.verticalCenter
            }
            Item {
                id: quitBtn
                width: 40; height: 40
                property bool hovered: false
                Rectangle {
                    anchors.fill: parent
                    radius: height / 2
                    color: quitBtn.hovered ? "#193660" : "#1E2A5A"
                    border.color: "white"; border.width: 2
                }
                Label { text: "⏻"; anchors.centerIn: parent; color: "white"; font.pixelSize: 24 }
                MouseArea {
                    anchors.fill: parent; hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onEntered: quitBtn.hovered = true
                    onExited: quitBtn.hovered = false
                    onClicked: Qt.quit()
                }
            }
        }

        // Connection status box: shows software + two COM ports (TasIO / scale).
        Rectangle {
            id: connStatusBox
            visible: connStatus.text !== ""
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 4
            width: 300
            radius: 4
            color: "white"
            border.color: "#CCCCCC"
            Label {
                id: connStatus
                anchors.fill: parent
                anchors.margins: 4
                color: "#333"
                font.pixelSize: 11
                wrapMode: Text.Wrap
                text: ""
            }
        }

    }

    StackView {
        id: stack
        anchors.fill: parent
        initialItem: "LoginPage.qml"
    }

    // Open a drawer/menu page, skipping a duplicate push when that page is
    // already on top of the stack (L20).
    function openPage(page) {
        if (stack.currentItem && stack.currentItem.pageSource === page) {
            drawer.close()
            return
        }
        stack.push(page)
        drawer.close()
    }

    // After a successful login, switch to the main menu. Delayed briefly so the
    // "Welcome, <user>" message on the login page stays visible (L11).
    Timer {
        id: loginSwitchTimer
        interval: 700
        onTriggered: stack.replace("MainMenuPage.qml")
    }

    // After a successful login, navigate to the main menu.
    Connections {
        target: loginService
        function onLoginSucceeded() {
            loginSwitchTimer.restart()
        }
    }

    // Side navigation drawer, opened by the header hamburger button
    // (WPF MaterialDesignHamburgerToggleButton). modal:true lets a click on the
    // dimmed area close the drawer (L20).
    Drawer {
        id: drawer
        width: 280
        edge: Qt.LeftEdge
        modal: true
        Column {
            anchors.fill: parent
            anchors.margins: 12
            spacing: 6
            Label {
                text: deviceService.deviceName()
                font.pixelSize: 18
                font.bold: true
                color: "#303F9F"
                anchors.horizontalCenter: parent.horizontalCenter
            }
            Rectangle { width: parent.width; height: 1; color: "#CCCCCC" }
            Repeater {
                model: [
                    { icon: "▶", text: qsTr("Start Hydrostatic Test"), page: "TestPreparationPage.qml" },
                    { icon: "≡", text: qsTr("Result Management"), page: "ResultManagementPage.qml" },
                    { icon: "☺", text: qsTr("User Management"), page: "UserManagementPage.qml" },
                    { icon: "⚙", text: qsTr("System Maintain"), page: "SystemMaintainPage.qml" }
                ]
                delegate: Button {
                    width: parent.width
                    text: modelData.icon + "  " + modelData.text
                    font.pixelSize: 16
                    onClicked: root.openPage(modelData.page)
                }
            }
            Item { width: 1; height: 16 }
            Button {
                width: parent.width
                text: "⏻  " + qsTr("Log out")
                font.pixelSize: 16
                onClicked: { stack.replace("LoginPage.qml"); drawer.close() }
            }
        }
    }
}


