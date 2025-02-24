import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.qmlmodels

import ThemeEngine
import WeChatEngine

Loader {
    id: wechatFriends
    anchors.fill: parent

    function loadScreen() {
        wechatFriends.active = true
        appContent.state = "Friend"
    }


    active: false
    asynchronous: false

    Component {
        id: headImage
        IconSvg {
            asynchronous: true
            anchors.fill: parent
            source: {
                if (rowData["headImg"] != "") {
                    return rowData["headImg"]
                }
                else {
                    return WeChat.defaultHeadImg
                }
            }
        }
    }

    sourceComponent: ColumnLayout {
        anchors.fill: parent

        WeChatTableView {
            id: friendsTableView

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
