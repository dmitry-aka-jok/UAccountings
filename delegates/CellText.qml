import QtQuick
CellBase {
    contentItem: Text {
        text: value
        clip: true
        horizontalAlignment:  Text.AlignHCenter
        verticalAlignment:    Text.AlignVCenter
        elide: Text.ElideRight
    }
    editor: "CellTextEditor.qml"
}
