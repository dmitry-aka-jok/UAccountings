import QtQuick
import QtQuick.Controls

CellData {
  contentItem:
  Label {
    text : value
//    wrapMode: Text.WrapAnywhere
    clip:true
    verticalAlignment: Text.AlignVCenter
  }
}
