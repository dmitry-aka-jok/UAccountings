import QtQuick
import QtQuick.Controls

CellBase {
  id: delegateRoot
  required property var value

  text : delegateRoot.value

  onClicked: {
    if(tableRoot.currentRow === row)
      tableRoot.startEditor("edit")
    else{
      tableRoot.currentRow = row
    }
  }
  onDoubleClicked: {
    tableRoot.currentRow = row
    tableRoot.startEditor("edit")
  }

}
