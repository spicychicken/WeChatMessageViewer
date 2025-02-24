import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Basic
import QtQuick.Layouts
import Qt.labs.qmlmodels

Rectangle {
    id: control

    property color borderColor: Qt.rgba(228/255,228/255,228/255,1)

    ////////////////////////////////////////////////////////////////////////////////
    function createTableModelColumn(model, columnSource) {
        var columns = []
        var headerRow = {}
        if (columnSource.length !== 0) {
            var headerRow = {}
            for (var i = 0; i <= columnSource.length - 1; i++) {
                var item = columnSource[i]
                var column = Qt.createQmlObject('import Qt.labs.qmlmodels 1.0;TableModelColumn{}', model);
                column.display = item.dataIndex
                columns.push(column)
                headerRow[item.dataIndex] = item
            }
        }
        return [columns, headerRow]
    }
    property var headerSource: []
    onHeaderSourceChanged: {
        const [columns, headerRow] = createTableModelColumn(table_view_header_model, headerSource)
        if (columns.length != 0) {
            table_view_header_model.columns = columns
            table_view_header_model.rows = [headerRow]
        }
    }

    property var dataSource: []
    onDataSourceChanged: {
        const [columns, headerRow] = createTableModelColumn(table_view_data_model, headerSource)
        if (columns.length != 0) {
            table_view_data_model.clear()
            table_view_data_model.columns = columns
            table_view_data_model.rows = dataSource
        }
    }

    property var headerWidthProvider: function(column) {
        var headerModel = control.headerSource[column]
        var width = headerModel.width
        if (width) {
            return width
        }
        var minimumWidth = headerModel.minimumWidth
        if (minimumWidth){
            return minimumWidth
        }
        return 42       // default item height
    }
    property var headerHeightProvider: function(row) {
        return 36;
    }
    property var dataHeightProvider: function(row) {
        return table_view_data.height/20;
    }

    ///////////////////////// UI //////////////////////////////////////////////////////////
    // table view header model
    TableModel{
        id: table_view_header_model
    }

    // table view header
    TableView {
        id: table_view_header
        model: table_view_header_model

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top

        implicitHeight: 36

        visible: true
        clip: true

        columnWidthProvider: headerWidthProvider
        delegate: Rectangle{
            visible: true
            color: Qt.rgba(247/255,247/255,247/255,1)

            implicitHeight: 36

            Rectangle{
                border.color: control.borderColor
                width: parent.width
                height: 1
                anchors.top: parent.top
                color:"#00000000"
            }
            Rectangle{
                border.color: control.borderColor
                width: parent.width
                height: 1
                anchors.bottom: parent.bottom
                color:"#00000000"
            }
            Rectangle{
                border.color: control.borderColor
                width: 1
                height: parent.height
                anchors.left: parent.left
                visible: column !== 0
                color:"#00000000"
            }
            Rectangle{
                border.color: control.borderColor
                width: 1
                height: parent.height
                anchors.right: parent.right
                color:"#00000000"
                visible: column === table_view_data.columns - 1
            }

            Text {
                text: String(display.title)
                anchors.fill: parent
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }
    }

    TableModel{
        id: table_view_data_model
    }

    Component{
        id:table_view_data_row_text
        Text {
            text: String(display)
            elide: Text.ElideRight
            wrapMode: Text.WrapAnywhere
            anchors {
                fill: parent
                leftMargin: 11
                rightMargin: 11
                topMargin: 6
                bottomMargin: 6
            }
            verticalAlignment: Text.AlignVCenter
        }
    }

    // table view data
    TableView {
        id:table_view_data

        visible: true
        clip: true

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: table_view_header.bottom
        anchors.bottom: parent.bottom

        columnWidthProvider: headerWidthProvider
        rowHeightProvider: dataHeightProvider

        model: table_view_data_model
        delegate: Rectangle {
            visible: true

            color: {
                return (row % 2 !== 0) ? control.color : Qt.rgba(0,0,0,0.03)
            }

            Loader {
                visible: true

                property var columnData: control.headerSource[column]
                property var rowData: control.dataSource[row]

                property var display: model.display

                anchors.fill: parent
                sourceComponent: {
                    if ("component" in columnData) {
                        return columnData.component
                    }
                    return table_view_data_row_text
                }
            }
        }
    }
}
