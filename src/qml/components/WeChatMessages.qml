import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtCharts

import ThemeEngine
import WeChatEngine

Loader {
    id: wechatMessages
    anchors.fill: parent

    function loadScreen() {
        wechatMessages.sourceComponent = component
        wechatMessages.active = true
        appContent.state = "Message"
    }

    function unloadScreen() {
        wechatMessages.active = false
        wechatMessages.sourceComponent = undefined
    }


    active: false
    asynchronous: true

    Component {
        id: component
        Grid {
            id: grid
            anchors.fill: parent

            anchors.topMargin: Theme.componentMargin
            anchors.leftMargin: Theme.componentMargin
            anchors.rightMargin: Theme.componentMargin
            anchors.bottomMargin: Theme.componentMargin

            rows: 1
            columns: 2

            property int wwww: Math.floor((grid.width) / 5)
            property int hhhh: Math.floor(grid.height)

            // friend list
            ColumnLayout {
                width: grid.wwww
                height: grid.hhhh

                SectionTitle {
                    Layout.preferredWidth: parent.width - Theme.componentMargin*0.5
                    Layout.alignment: Qt.AlignLeft
                    text: "Friends"
                }

                ListView {
                    id: userFriendList
                    currentIndex: -1
                    Layout.preferredWidth: parent.width - Theme.componentMargin*0.5
                    Layout.fillHeight: true
                    Layout.alignment: Qt.AlignLeft

                    clip: true
                    interactive: true

                    ScrollBar.vertical: ScrollBar {
                        policy: ScrollBar.AsNeeded
                    }

                    model: []

                    delegate:  Rectangle {
                        property bool hovered: false
                        height: 28
                        width: ListView.view.width

                        radius: {
                            if (hovered) {
                                return 5;
                            }
                            return 0;
                        }

                        color: {
                            if (userFriendList.currentIndex === index || hovered) {
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

                            text: modelData["displayName"]
                            font.pixelSize: Theme.componentFontSize
                            color: Theme.colorText
                        }

                        MouseArea {
                            cursorShape: Qt.PointingHandCursor
                            hoverEnabled: true
                            anchors.fill: parent
                            onClicked: {
                                messageTitle.text =  userFriendList.model[index]["displayName"]
                                gagination.pageCurrent = 1
                                var results = WeChat.listMessages(userFriendList.model[index]["userID"],
                                    gagination.pageCurrent, gagination.__itemPerPage)
                                gagination.itemCount = results["total"]

                                messagesTableView.model = results["msg"]
                                messagesTableView.currentFriendID = userFriendList.model[index]["userID"]
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
            }

            // message list
            ColumnLayout {
                width: grid.width - grid.wwww
                height: grid.hhhh

                SectionTitle {
                    id: messageTitle
                    Layout.preferredWidth: parent.width - Theme.componentMargin*0.5
                    Layout.alignment: Qt.AlignRight
                    text: "Message"
                }

                WeChatMsgListView {
                    id: messagesTableView

                    Layout.preferredWidth: parent.width - Theme.componentMargin*0.5
                    Layout.alignment: Qt.AlignRight
                }

                RowLayout {
                    implicitHeight: 36
                    Layout.preferredWidth: parent.width

                    CheckBox {
                        visible: false
                        Layout.alignment: Qt.AlignLeft
                        onClicked: {
                        }
                        text: qsTr("Filter friend with 0 message record")
                    }

                    Pagination {
                        id: gagination
                        Layout.alignment: Qt.AlignRight | Qt.ALighVCenter
                        implicitHeight: parent.implicitHeight
                        pageCurrent: 1
                        itemCount: 0
                        onRequestPage: (page, count) => {
                            var results = WeChat.listMessages(messagesTableView.currentFriendID,
                                    gagination.pageCurrent, gagination.__itemPerPage)
                            messagesTableView.model = results["msg"]
                        }
                    }
                }
            }

            Component.onCompleted: {
                WeChat.performAsyncOperation(function() {
                    var friends = WeChat.listFriends(0, 10000)
                    userFriendList.model = friends["msg"].filter(f => f["msgCount"] > 0).sort((a,b) => b["lastTime"].localeCompare(a["lastTime"]))
                    userFriendList.currentIndex = -1
                }, function() {
                })
            }
        }
    }
}
