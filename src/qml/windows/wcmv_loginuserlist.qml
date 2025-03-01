import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts

import ThemeEngine
import WeChatEngine

Window {
    id: loginUserListWindow

    width: 500
    height: 400
    minimumWidth: width
    minimumHeight: height
    maximumWidth: width
    maximumHeight: height

    // opacity: 0.9
    title: {
        if (WeChat.openFolderBackupType == WeChatEngine.BackupType_IOS) {
            return "IOS Backup"
        }
        else if (WeChat.openFolderBackupType == WeChatEngine.BackupType_WIN) {
            return "Windows PC"
        }
        return "UNKNOWN"
    }
    visible: true
    flags: Qt.Dialog
    modality: Qt.WindowModal

    signal loginUserConfirmed(string loginUser, string secretKey)

    Rectangle {
        anchors.fill: parent
        height: 300

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 10

            SectionTitle {
                Layout.preferredWidth: parent.width
                Layout.alignment: Qt.AlignRight
                text: "Please Select Login User:"
                textSize: 16
                implicitHeight: 30
            }

            ListView {
                id: userListView
                // currentIndex: -1
                Layout.preferredWidth: parent.width
                Layout.fillHeight: true
                Layout.alignment: Qt.AlignRight

                clip: true
                interactive: true

                ScrollBar.vertical: ScrollBar {
                    policy: ScrollBar.AsNeeded
                }

                model: WeChat.listLoginUserNames()
                delegate: Rectangle {
                    property bool hovered: false
                    height: 28
                    width: ListView.view.width

                    color: {
                        if (userListView.currentIndex === index || hovered) {
                            return "lightblue"
                        }
                        else {
                            return (index % 2) ? Theme.colorForeground :Theme.colorBackground
                        }
                    }

                    Text {
                        anchors.left: parent.left
                        anchors.leftMargin: Theme.componentMargin
                        anchors.verticalCenter: parent.verticalCenter

                        text: modelData
                        font.pixelSize: Theme.componentFontSize
                        color: Theme.colorText
                    }

                    MouseArea {
                        cursorShape: Qt.PointingHandCursor
                        hoverEnabled: true
                        anchors.fill: parent
                        onClicked: {
                            userListView.currentIndex = index;
                        }

                        onEntered: {
                            hovered = true // 更新 hovered 属性来跟踪鼠标的进入和退出状态。
                        }
                        
                        onExited: {
                            hovered = false // 更新 hovered 属性来跟踪鼠标的进入和退出状态。
                        }
                    }
                }
            }

            SectionTitle {
                visible: WeChat.openFolderBackupType == WeChatEngine.BackupType_WIN
                Layout.preferredWidth: parent.width
                Layout.alignment: Qt.AlignRight
                text: "Please Input Secret Key:"
                textSize: 16
                implicitHeight: 30
            }

            TextFieldThemed {
                visible: WeChat.openFolderBackupType == WeChatEngine.BackupType_WIN
                id: wechatSecretKey
                Layout.preferredWidth: parent.width
                Layout.alignment: Qt.AlignRight
                placeholderText: qsTr("Secret Key")
            }

            CheckBox {
                visible: WeChat.openFolderBackupType == WeChatEngine.BackupType_WIN
                id: automateDetected
                onClicked: {
                    wechatSecretKey.enabled = !wechatSecretKey.enabled
                }
                text: qsTr("Automate detected Secret Key\nplease make sure WeChat App is opened and logined with user you selected !!!")
            }

            RowLayout {
                spacing: 20
                Layout.alignment: Qt.AlignCenter

                Button {
                    id: btnConfirm
                    implicitWidth: 120
                    implicitHeight: Theme.componentHeight
                    text: qsTr("Confirm")
                    enabled: {
                        if (WeChat.openFolderBackupType == WeChatEngine.BackupType_WIN && automateDetected.checkState != Qt.Checked && wechatSecretKey.text == "") {
                            return false
                        }
                        return userListView.currentIndex >= 0
                    }
                    onClicked: {
                        var secretKey = wechatSecretKey.text
                        if (automateDetected.checkState == Qt.Checked)
                        {
                            secretKey = ""
                        }
                        loginUserConfirmed(userListView.model[userListView.currentIndex], secretKey)
                    }
                }

                Button {
                    implicitWidth: 120
                    implicitHeight: Theme.componentHeight
                    text: qsTr("Cancel")
                    onClicked: loginUserListWindow.close()
                }
            }
        }
    }
}
