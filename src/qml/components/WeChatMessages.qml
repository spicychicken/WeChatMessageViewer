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
                Layout.preferredWidth: parent.width - Theme.componentMargin*0.5
                Layout.fillHeight: true
                Layout.alignment: Qt.AlignLeft

                clip: false
                interactive: false

                model: WeChat.loadFriends()

                delegate: RowLayout {
                    width: ListView.view.width
                    height: Math.max(fontsize.contentHeight, 20)
                    spacing: Theme.componentMargin

                    Text {
                        id: legend
                        Layout.preferredWidth: 192
                        Layout.alignment: Qt.AlignBaseline

                        text: modelData
                        textFormat: Text.PlainText
                        horizontalAlignment: Text.AlignRight
                        verticalAlignment: Text.AlignBottom
                        font.pixelSize: Theme.componentFontSize
                        color: Theme.colorSubText
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

            ListView {
                Layout.preferredWidth: parent.width - Theme.componentMargin*0.5
                Layout.fillHeight: true
                Layout.alignment: Qt.AlignRight

                clip: false
                interactive: true

                ScrollBar.vertical: ScrollBar { visible: true }

                model: Qt.fontFamilies()
                delegate: Rectangle {
                    height: 28
                    width: ListView.view.width

                    color: (index % 2) ? Theme.colorForeground :Theme.colorBackground

                    Text {
                        anchors.left: parent.left
                        anchors.leftMargin: Theme.componentMargin
                        anchors.verticalCenter: parent.verticalCenter

                        text: modelData
                        font.pixelSize: Theme.componentFontSize
                        color: Theme.colorText
                    }
                }
            }
        }
    }
}
