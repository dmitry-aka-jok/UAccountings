import QtQuick
import QtQuick.Controls

CellBase {
    objectName: "!"
    contentItem: Switch {
        enabled: !readOnly
        checked: value
        checkable: true
        onClicked: onAccepted(checked)
    }
}
