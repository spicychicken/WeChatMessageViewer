import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtCharts

import ThemeEngine

Loader {
    id: wechatInfos
    anchors.fill: parent

    function loadScreen() {
        wechatInfos.active = true
        appContent.state = "Info"
    }


    active: false
    asynchronous: true

    sourceComponent: GridLayout {
        id: grid
        anchors.fill: parent

        anchors.topMargin: Theme.componentMargin
        anchors.leftMargin: Theme.componentMargin
        anchors.rightMargin: Theme.componentMargin
        anchors.bottomMargin: Theme.componentMargin

        rows: 2
        columns: 2

        // property int wwww: Math.floor((grid.width) / (singleColumn ? 1 : 2))
        // property int hhhh: Math.floor((grid.height) / (singleColumn ? 2 : 1))

        FrameBox {
            ColumnLayout {
                anchors.fill: parent
                spacing: Theme.componentMarginS

                Column {
                    Text {
                        text: qsTr("Qt build mode")
                        textFormat: Text.PlainText
                        color: Theme.colorSubText
                        font.bold: true
                        font.pixelSize: Theme.fontSizeContentVerySmall
                        font.capitalization: Font.AllUppercase
                    }
                    Text {
                        text: "utilsApp.qtBuildMode()"
                        textFormat: Text.PlainText
                        font.pixelSize: Theme.fontSizeContentBig
                        color: Theme.colorHighContrast
                    }
                }
            }
        }

        FrameBox {
            ColumnLayout {
                anchors.fill: parent
                spacing: Theme.componentMarginS

                Column {
                    Text {
                        text: qsTr("Qt build mode")
                        textFormat: Text.PlainText
                        color: Theme.colorSubText
                        font.bold: true
                        font.pixelSize: Theme.fontSizeContentVerySmall
                        font.capitalization: Font.AllUppercase
                    }
                    Text {
                        text: "utilsApp.qtBuildMode()"
                        textFormat: Text.PlainText
                        font.pixelSize: Theme.fontSizeContentBig
                        color: Theme.colorHighContrast
                    }
                }
            }
        }

        FrameBox {
            ColumnLayout {
                anchors.fill: parent
                spacing: Theme.componentMarginS

                Column {
                    Text {
                        text: qsTr("Qt build mode")
                        textFormat: Text.PlainText
                        color: Theme.colorSubText
                        font.bold: true
                        font.pixelSize: Theme.fontSizeContentVerySmall
                        font.capitalization: Font.AllUppercase
                    }
                    Text {
                        text: "utilsApp.qtBuildMode()"
                        textFormat: Text.PlainText
                        font.pixelSize: Theme.fontSizeContentBig
                        color: Theme.colorHighContrast
                    }
                }
            }
        }

        FrameThemed {
            ColumnLayout {
                anchors.fill: parent
                spacing: Theme.componentMarginS

                Column {
                    Text {
                        text: qsTr("Qt build mode")
                        textFormat: Text.PlainText
                        color: Theme.colorSubText
                        font.bold: true
                        font.pixelSize: Theme.fontSizeContentVerySmall
                        font.capitalization: Font.AllUppercase
                    }
                    Text {
                        text: "utilsApp.qtBuildMode()"
                        textFormat: Text.PlainText
                        font.pixelSize: Theme.fontSizeContentBig
                        color: Theme.colorHighContrast
                    }
                }
            }
        }
    }
}
