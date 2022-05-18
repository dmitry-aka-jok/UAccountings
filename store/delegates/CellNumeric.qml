import QtQuick
import QtQuick.Controls

CellData {
  id: cellRoot
  property int digits: 0
  contentItem:
  Label {
    text : value.toFixed(cellRoot.digits)
    verticalAlignment: Text.AlignVCenter
    horizontalAlignment: Text.AlignRight
  }
}
