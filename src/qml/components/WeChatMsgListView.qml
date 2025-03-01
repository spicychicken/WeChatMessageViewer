import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Basic
import QtMultimedia

import ThemeEngine
import WeChatEngine

ListView {
    id: control

    anchors.leftMargin: 5
    anchors.rightMargin: 5
    anchors.topMargin: 5
    anchors.bottomMargin: 5

    // Layout.fillWidth: true
    Layout.fillHeight: true

    clip: true
    interactive: false
    spacing: Theme.componentMargin

    model: []

    ScrollBar.vertical: ScrollBar {
        policy: ScrollBar.AsNeeded
    }

    property var currentFriendID

    Component {
        id: textComponent

        Row {
            width: rowDelegate.contentWidth
            layoutDirection: isSender ? Qt.RightToLeft : Qt.LeftToRight

            Rectangle {
                radius: 5
                width: msgText.width
                height: msgText.height

                color: isSender ? "#95ec69" : "#ffffff"

                // little triangle point to sender
                Rectangle {
                    width: 10
                    height: 10
                    y: 12
                    anchors.horizontalCenter: isSender ? parent.right :parent.left
                    rotation: 45
                    color: isSender ? "#95ec69" : "#ffffff"
                }

                TextArea {
                    padding: 10
                    id: msgText
                    readOnly: true
                    wrapMode: TextArea.WrapAnywhere
                    text: rowData["content"]
                    textFormat: TextArea.PlainText
                    font.pixelSize: Theme.fontSizeContentBig
                    color: Theme.colorText

                    property int implicitTextWidth
                    Component.onCompleted: {
                        implicitTextWidth = msgText.implicitWidth
                        msgText.width = Math.ceil(Math.min(msgText.implicitWidth, rowDelegate.contentWidth / 4 * 3))
                    }

                    Connections {
                        target: rowDelegate
                        function onWidthChanged(value) {
                            msgText.width = Math.ceil(Math.min(msgText.implicitTextWidth, rowDelegate.contentWidth / 4 * 3))
                        }
                    }
                }
            }
        }
    }

    Component {
        id: imageComponent

        Row {
            width: rowDelegate.contentWidth
            layoutDirection: isSender ? Qt.RightToLeft : Qt.LeftToRight

            Image {
                id: msgImage

                cache: false
                readonly property int imageWidthHeight: 256

                source: WeChat.getMsgImageUrl(rowMetadataData)
                smooth: true

                Component.onCompleted: {
                    if (msgImage.sourceSize.width > imageWidthHeight || msgImage.sourceSize.height > imageWidthHeight)
                    {
                        var ratio = Math.max(msgImage.sourceSize.width / imageWidthHeight, msgImage.sourceSize.height / imageWidthHeight);
                        msgImage.sourceSize = Qt.size(msgImage.sourceSize.width / ratio, msgImage.sourceSize.height / ratio)
                    }
                    else if (msgImage.sourceSize.width < imageWidthHeight && msgImage.sourceSize.height < imageWidthHeight)
                    {
                        var ratio = Math.max(imageWidthHeight / msgImage.sourceSize.width, imageWidthHeight / msgImage.sourceSize.height)
                        msgImage.sourceSize = Qt.size(msgImage.sourceSize.width * ratio, msgImage.sourceSize.height * ratio)
                    }
                    msgImage.width = msgImage.sourceSize.width
                    msgImage.height = msgImage.sourceSize.height
                }
            }
        }
    }

    Component{
        id: videoComponent

        Row {
            width: rowDelegate.contentWidth
            layoutDirection: isSender ? Qt.RightToLeft : Qt.LeftToRight

            property bool showImage: true

            Image {
                id: msgImage

                visible: showImage
                cache: false
                readonly property int imageWidthHeight: 256

                source: WeChat.getMsgVideoThumbImageUrl(rowMetadataData)
                smooth: true

                Component.onCompleted: {
                    if (msgImage.sourceSize.width > imageWidthHeight || msgImage.sourceSize.height > imageWidthHeight)
                    {
                        var ratio = Math.max(msgImage.sourceSize.width / imageWidthHeight, msgImage.sourceSize.height / imageWidthHeight);
                        msgImage.sourceSize = Qt.size(msgImage.sourceSize.width / ratio, msgImage.sourceSize.height / ratio)
                    }
                    else if (msgImage.sourceSize.width < imageWidthHeight && msgImage.sourceSize.height < imageWidthHeight)
                    {
                        var ratio = Math.max(imageWidthHeight / msgImage.sourceSize.width, imageWidthHeight / msgImage.sourceSize.height)
                        msgImage.sourceSize = Qt.size(msgImage.sourceSize.width * ratio, msgImage.sourceSize.height * ratio)
                    }
                    msgImage.width = msgImage.sourceSize.width
                    msgImage.height = msgImage.sourceSize.height
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        msgVideo.width = msgImage.width
                        msgVideo.height = msgImage.height

                        if (WeChat.getMsgVideoUrl(rowMetadataData) !== "") {
                            showImage = false
                            msgVideo.source = "file:///" + WeChat.getMsgVideoUrl(rowMetadataData)
                            msgVideo.play()
                        }
                    }
                }
            }

            Video {
                id: msgVideo
                visible: !showImage
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        if (msgVideo.playbackState === MediaPlayer.PlayingState) {
                            msgVideo.pause()
                        }
                        else {
                            msgVideo.play()
                        }
                    }
                }

                onErrorOccurred: function(error, errorString) {
                    console.log("Video error:", errorString, msgVideo.source)
                }
            }
        }
    }

    Component {
        id: audioComponent

        Row {
            width: rowDelegate.contentWidth
            layoutDirection: isSender ? Qt.RightToLeft : Qt.LeftToRight
            Button {
                text: rowData["metadatas"]["seconds"]
                
                background: Rectangle {
                    implicitWidth: 100
                    implicitHeight: 40
                    border.color: control.down ? "#17a81a" : "#21be2b"
                    border.width: 1
                    radius: 5
                }

                onClicked: {
                    var message = {}
                    message["src"] = rowData["metadatas"]["src"]
                    message["msgSvrID"] = rowData["msgSvrID"]
                    message["dbPath"] = rowData["metadatas"]["dbPath"]
                    WeChat.playAudio(currentFriendID, message)
                }
            }
        }
    }

    Component{
        id: otherComponent

        Row {
            width: rowDelegate.contentWidth
            layoutDirection: isSender ? Qt.RightToLeft : Qt.LeftToRight
            Rectangle {
                radius: 5
                width: 250
                height: 80
                color: "#ffffff"

                // little triangle point to sender
                Rectangle {
                    width: 10
                    height: 10
                    y: 12
                    anchors.horizontalCenter: isSender ? parent.right :parent.left
                    rotation: 45
                    color: "#ffffff"
                }

                Text {
                    padding: 14
                    width: 200
                    height: 40
                    text: rowData["content"]
                    textFormat: Text.PlainText
                    font.pixelSize: Theme.fontSizeContentBig
                    color: Theme.colorText
                    wrapMode: Text.WrapAnywhere
                }

                Image {
                    anchors.verticalCenter: parent.verticalCenter
                    x: 190
                    source: "qrc:/assets/icons/unknown.png"
                    smooth: true
                }
            }
        }
    }

    delegate: Row {
        id: listViewRowDelegate

        layoutDirection: isSender ? Qt.RightToLeft : Qt.LeftToRight

        readonly property int headImgWidth: 48
        property int contentWidth: Math.max(200, listViewRowDelegate.width - headImgWidth)
        readonly property bool isSender: WeChat.isCurrentLoginUser(modelData["sender"])
        
        width: ListView.view.width
        spacing: Theme.componentMargin
        padding: 6

        onWidthChanged: {
            listViewRowDelegate.contentWidth = Math.max(200, listViewRowDelegate.width - headImgWidth)
        }
        
        IconSvg {
            asynchronous: true

            width: headImgWidth
            height: headImgWidth

            source: WeChat.getUserHeadImgUrl(modelData["sender"])
        }

        Column {
            id: msgContentColumn
            spacing: 6
            width: listViewRowDelegate.contentWidth

            Row {
                width: listViewRowDelegate.contentWidth
                layoutDirection: isSender ? Qt.RightToLeft : Qt.LeftToRight

                Text {
                    text: modelData["sender"]["displayName"] + " (" + modelData["time"] + ")"
                    textFormat: Text.PlainText
                    font.pixelSize: Theme.fontSizeContentVerySmall
                    color: Theme.colorSubText
                }
            }

            Loader {
                id: msgContentComponent

                property var rowData: modelData
                property var rowMetadataData: modelData["metadatas"]
                property var rowDelegate: listViewRowDelegate
                property var isSender: listViewRowDelegate.isSender

                sourceComponent: {
                    return createComponentByType(modelData["type"], modelData)
                }
            }
        }
    }

    function createComponentByType(type, modelData) {
        switch (type) {
            case 1:             // Text
                return textComponent
            case 3:             // Image
                return imageComponent
            case 34:            // Audio
                return audioComponent
            case 43:            // video
                return videoComponent
            default:
                return otherComponent
        }
    }
}