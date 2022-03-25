import QtQuick
import QtQuick.Controls

CellBase {
    id: root
    objectName: "!"
    contentItem: ComboBox {
        enabled: !readOnly
        model: option.values
        editable: false
        currentIndex: model.indexOf(value)
        onActivated: root.onAccepted(model[index])
    }
}
