import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    signal requestPage(int page, int count)

    property string previousText: qsTr("<Previous")
    property string nextText: qsTr("Next>")
    property int pageCurrent: 0
    property int itemCount: 0
    property int pageButtonCount: 5
    property int pageCount: itemCount > 0 ? Math.ceil(itemCount / __itemPerPage) : 0
    property int __itemPerPage: 20
    property int __pageButtonHalf: Math.floor(pageButtonCount / 2) + 1
    property Component header: null
    property Component footer: null
    id: control
    implicitHeight: 40
    implicitWidth: content.width
    Row {
        id: content
        height: control.height
        spacing: 10
        padding: 10
        Loader {
            sourceComponent: header
        }
        Button {
            visible: control.pageCount > 1
            enabled: control.pageCurrent > 1
            text: control.previousText
            onClicked: function () {
                control.calcNewPage(control.pageCurrent - 1);
            }
        }
        Row {
            spacing: 5
            Button {
                property int pageNumber: 1
                visible: control.pageCount > 0
                checked: pageNumber === control.pageCurrent
                text: String(pageNumber)
                onClicked: function () {
                    control.calcNewPage(pageNumber);
                }
            }
            Text {
                visible: (control.pageCount > control.pageButtonCount &&
                    control.pageCurrent > control.__pageButtonHalf)
                text: "..."
            }
            Repeater {
                id: button_repeator
                model: (control.pageCount < 2) ? 0 : (control.pageCount >= control.pageButtonCount) ? (control.pageButtonCount - 2) : (control.pageCount - 2)
                delegate: Button {
                    property int  pageNumber: {
                        return (control.pageCurrent <= control.__pageButtonHalf)
                            ? (2 + index)
                            : (control.pageCount - control.pageCurrent <= control.pageButtonCount - control.__pageButtonHalf)
                                ? (control.pageCount - button_repeator.count + index)
                                : (control.pageCurrent + 2 + index - control.__pageButtonHalf)
                    }
                    text: String(pageNumber)
                    checked: pageNumber === control.pageCurrent
                    onClicked: function () {
                        control.calcNewPage(pageNumber);
                    }
                }
            }
            Text {
                visible: (control.pageCount > control.pageButtonCount &&
                    control.pageCount - control.pageCurrent > control.pageButtonCount - control.__pageButtonHalf)
                text: "..."
            }
            Button {
                property int pageNumber: control.pageCount
                visible: control.pageCount > 1
                checked: pageNumber === control.pageCurrent
                text: String(pageNumber)
                onClicked: function () {
                    control.calcNewPage(pageNumber);
                }
            }
        }
        Button {
            visible: control.pageCount > 1
            enabled: control.pageCurrent < control.pageCount
            text: control.nextText
            onClicked: function () {
                control.calcNewPage(control.pageCurrent + 1);
            }
        }
        Loader {
            sourceComponent: footer
        }
    }
    function calcNewPage(page) {
        if (!page)
            return
        let page_num = Number(page)
        if (page_num < 1 || page_num > control.pageCount || page_num === control.pageCurrent)
            return
        control.pageCurrent = page_num
        control.requestPage(page_num, control.__itemPerPage)
    }
}
