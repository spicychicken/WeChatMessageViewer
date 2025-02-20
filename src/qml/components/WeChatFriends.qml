import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.qmlmodels

import ThemeEngine

Loader {
    id: wechatFriends
    anchors.fill: parent

    function loadScreen() {
        wechatFriends.active = true
        appContent.state = "Friend"
    }


    active: false
    asynchronous: false

    sourceComponent: Rectangle {
        id: itemQtInfo
        // padding: 10
        // width: www

        TableView {
            anchors.fill: parent
            columnSpacing: 1
            rowSpacing: 1
            clip: true

            // model: WeChat.
            delegate: Rectangle {
                implicitWidth: 100
                implicitHeight: 50
                border.width: 1

                Text {
                    text: display
                    anchors.centerIn: parent
                }
            }
        }
    }
}
