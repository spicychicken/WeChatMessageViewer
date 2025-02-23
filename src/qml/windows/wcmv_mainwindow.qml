import QtQuick
import QtQuick.Controls
import QtQuick.Window
import QtQuick.Dialogs

import ThemeEngine
import WeChatEngine

ApplicationWindow {
    id: appWindow
    
    minimumWidth: 800
    minimumHeight: 560

    visible: true

    // flags: Qt.Window | Qt.FramelessWindowHint

    // header
    Rectangle {
        id: appHeader
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right

        height: 64
        color: Theme.colorHeader
        clip: false

        DragHandler {
            // Drag on the sidebar to drag the whole window // Qt 5.15+
            // Also, prevent clicks below this area
            onActiveChanged: if (active) appWindow.startSystemMove()
            target: null
        }

        Text { // title
            anchors.left: parent.left
            anchors.leftMargin: 30
            anchors.verticalCenter: parent.verticalCenter

            visible: true
            text: "WeChat Message Viewer"
            font.bold: true
            font.pixelSize: Theme.fontSizeHeader
            color: Theme.colorHeaderContent
        }

        Row { // buttons
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.bottom: parent.bottom

            spacing: 12
            visible: true
            rightPadding: 20

            Row {
                anchors.verticalCenter: parent.verticalCenter

                spacing: 12
                visible: true

                Button {
                    id: buttonOpen
                    text: qsTr("Open")
                    height: Theme.componentHeight
                    width: 120
                    onClicked: folderOpenDialog.open();
                }

                Loader {
                    id: loginUserListDialog
                    source: "wcmv_loginuserlist.qml"
                    active: false
                }

                FolderDialog {
                    id: folderOpenDialog
                    title: "select backup folder"
                    options: FolderDialog.ShowDirsOnly

                    onAccepted: {
                        if (WeChat.initContextFromPath(selectedFolder) == 0) {
                            loginUserListDialog.active = true
                            loginUserListDialog.item.loginUserConfirmed.connect(function(loginUser, secretKey) {
                                loginUserListDialog.destroy()
                                WeChat.performAsyncOperation(function() {
                                    WeChat.loadLoginUser(loginUser, secretKey)
                                }, function() {
                                    appSidebar.visible = true
                                    appContent.visible = true

                                    // change head
                                    if (WeChat.currentLoginUser["headImgHD"] != "") {
                                        userHeadImg.source = WeChat.currentLoginUser["headImgHD"]
                                    }
                                    else if (WeChat.currentLoginUser["headImg"] != "") {
                                        userHeadImg.source = WeChat.currentLoginUser["headImg"]
                                    }
                                    else {
                                        userHeadImg.source = WeChat.defaultHeadImg
                                    }
                                });
                            })
                        }
                    }
                }
            }
        }
    }

    // left side
    Rectangle {
        id: appSidebar
        z: 2
        visible: false

        anchors.top: appHeader.bottom
        anchors.left: parent.left
        anchors.bottom: parent.bottom

        width: Theme.isHdpi ? 72 : 80
        color: Theme.colorSidebar

        ////////////////////////////////////////////////////////////////////////////

        DragHandler {
            // Drag on the sidebar to drag the whole window // Qt 5.15+
            // Also, prevent clicks below this area
            onActiveChanged: if (active) appWindow.startSystemMove()
            target: null
        }

        Column { // top menu
            anchors.top: parent.top
            anchors.topMargin: 10
            anchors.left: parent.left
            anchors.right: parent.right

            // head image
            DesktopSidebarItem {
                id: userHeadImg
                source: WeChat.defaultHeadImg
                sourceSize: 54
                highlightMode: "content"

                // onClicked: screenDesktopComponents.loadScreen()
            }
            DesktopSidebarItem {
                source: "qrc:/assets/icons/message.svg"
                sourceSize: 40
                highlightMode: "indicator"
                text: "Messages"

                onClicked: wechatMessages.loadScreen()
            }
            DesktopSidebarItem {
                source: "qrc:/assets/icons/friend.svg"
                sourceSize: 40
                highlightMode: "indicator"
                text: "Friends"

                onClicked: wechatFriends.loadScreen()
            }
        }

        ////////////////////////////////////////////////////////////////////////////

        Column { // bottom menu
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 12

            DesktopSidebarItem {
                source: "qrc:/assets/icons/exit.svg"
                sourceSize: 40
                highlightMode: "circle"
                text: "Quit"

                onClicked: appWindow.close()
            }
        }
    }

    // content
    Rectangle {
        id: appContent
        visible: false

        anchors.top: appHeader.bottom
        anchors.left: appSidebar.right
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        color: Theme.colorBackground

        WeChatMessages {
            id: wechatMessages
        }

        WeChatFriends {
            id: wechatFriends
        }

        states: [
            State {
                name: "Message"
                PropertyChanges { target: wechatMessages; visible: true; enabled: true; focus: true; }
                PropertyChanges { target: wechatFriends; visible: false; enabled: false; }
            },
            State {
                name: "Friend"
                PropertyChanges { target: wechatMessages; visible: false; enabled: false; }
                PropertyChanges { target: wechatFriends; visible: true; enabled: true; focus: true; }
            }
        ]
    }
}
