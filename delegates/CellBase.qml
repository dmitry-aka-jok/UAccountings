import QtQuick
import QtQuick.Controls
import "CellBase.js" as Js

//import App.Class 0.1 as Class

Control {
    id: root
    // from model
    property var value
    property int type  //: Class.MetaType.UnknownType
    property var options
    property int editorType

    // local
    property bool readOnly : false
//    property bool hasDomain : flags & Class.Property.DomainList
    property string editor
    implicitWidth: theme.tableMinimalWidth
    implicitHeight: theme.basicElementSize

    background : CellBackground { }

    MouseArea {
        id: clickarea
        anchors.fill: parent
        onClicked: if (!readOnly)
                       edit()
        visible: root.objectName !== "!"
    }
    function onAccepted(value) {
        model.cellData = value
    }
    function edit() {
        Js.createEditor(this);
    }
}
