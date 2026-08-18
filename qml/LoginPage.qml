import QtQuick
import QtQuick.Controls

/**
 * 登录页（登录窗口内容）。所有文案用 qsTr() 包裹，支持中/英切换。
 * 语言选择通过暴露给 QML 的 "lang" (LanguageHelper) 实现。
 */
Item {
    id: loginPage

    Column {
        id: form
        anchors.centerIn: parent
        spacing: 14
        width: 340

        Label {
            text: qsTr("SY1000 Hydrostatic Test System")
            font.pixelSize: 22
            font.bold: true
            horizontalAlignment: Text.AlignHCenter
            anchors.horizontalCenter: parent.horizontalCenter
        }
        Label {
            text: qsTr("Please sign in to continue")
            color: "#888"
            horizontalAlignment: Text.AlignHCenter
            anchors.horizontalCenter: parent.horizontalCenter
        }

        TextField {
            id: usernameField
            width: parent.width
            placeholderText: qsTr("Username")
        }
        TextField {
            id: passwordField
            width: parent.width
            placeholderText: qsTr("Password")
            echoMode: TextInput.Password
        }

        Row {
            spacing: 8
            width: parent.width

            ComboBox {
                id: languageBox
                width: parent.width * 0.42
                // 语言列表显示各自语言名（不随界面语言改变）
                model: ["English", "中文"]
                onActivated: (index) => lang.setLanguage(index === 0 ? "en" : "zh_CN")
            }

            Button {
                id: signInButton
                width: parent.width - languageBox.width - parent.spacing
                text: qsTr("Sign In")
                onClicked: {
                    // 占位：后续接入登录校验（用户/密码/角色）
                    console.log("sign in attempt, user =", usernameField.text,
                                ", lang =", lang.current())
                }
            }
        }

        Label {
            text: qsTr("Language")
            font.pixelSize: 11
            color: "#999"
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }
}
