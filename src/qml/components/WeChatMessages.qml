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
        wechatMessages.active = true
        appContent.state = "Message"
    }


    active: false
    asynchronous: false

    sourceComponent: Grid {
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

                clip: false
                interactive: false

                ScrollBar.vertical: ScrollBar {
                    visible: true
                    policy: ScrollBar.AsNeeded
                }

                model: WeChat.loadFriends()

                delegate:  Rectangle {
                    property bool hovered: false
                    height: 28
                    width: ListView.view.width

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
                            userFriendList.currentIndex = index;
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
                Layout.preferredWidth: parent.width - Theme.componentMargin*0.5
                Layout.alignment: Qt.AlignRight
                text: "Message"
            }

            WeChatTableView {
                id: messagesTableView

                Layout.preferredWidth: parent.width
                Layout.alignment: Qt.AlignLeft
                Layout.fillHeight: true

                property int wwww: (parent.width - 64)/5

                anchors.leftMargin: 5
                anchors.rightMargin: 5
                anchors.topMargin: 5
                anchors.bottomMargin: 5

                headerSource:[
                    {
                        title: qsTr("Head"),
                        component: headImage,
                        dataIndex: 'headImg',
                        width: 64,
                    },
                    {
                        title: qsTr("UserName"),
                        dataIndex: 'userName',
                        width: wwww,
                    },
                    {
                        title: qsTr("NickName"),
                        dataIndex: 'displayName',
                        width: wwww,
                    },
                    {
                        title: qsTr("Count"),
                        dataIndex: 'msgCount',
                        width: wwww,
                    },
                    {
                        title: qsTr("From"),
                        dataIndex: 'beginTime',
                        width: wwww,
                    },
                    {
                        title: qsTr("To"),
                        dataIndex: 'lastTime',
                        width: wwww,
                    }
                ]

                dataSource: []
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
                        var results = WeChat.listFriends((page - 1) * count, count)
                        friendsTableView.dataSource = Array.from(results["msg"])
                    }
                }
            }

            Component.onCompleted: {
                var results = WeChat.listFriends((gagination.pageCurrent - 1) * gagination.__itemPerPage, gagination.__itemPerPage)
                gagination.itemCount = results["total"]
                friendsTableView.dataSource = Array.from(results["msg"])
            }
        }
    }
}
