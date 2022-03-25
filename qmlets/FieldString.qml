import QtQuick 6.0
import QtQuick.Controls.Material 6.0
import QtQuick.Layouts 6.0

Item {
    id: field
    property alias text: textedit.text

    implicitHeight: theme.basicElementSize * 1.5

    TextEdit {
        id: textedit
        font: theme.tableFont
        //anchors.left: parent.left
        //anchors.verticalCenter: parent.verticalCenter
        anchors.margins: theme.margins
        anchors.fill: parent
        mouseSelectionMode: TextEdit.SelectCharacters
        selectByMouse: true

    }
}
