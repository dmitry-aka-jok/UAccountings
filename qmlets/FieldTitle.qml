import QtQuick 6.0
import QtQuick.Controls.Material 6.0
import QtQuick.Layouts 6.0

Item {
    id:field
    property alias text: text.text

    implicitHeight: theme.basicElementSize * 1.5
    implicitWidth: text.implicitWidth

    Text {
        id: text
        font: theme.tableFont
//        anchors.left: parent.left
//        anchors.verticalCenter: parent.verticalCenter
        anchors.margins: theme.margins
        anchors.fill: parent
    }
}
