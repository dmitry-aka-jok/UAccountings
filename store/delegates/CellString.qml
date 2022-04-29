import QtQuick
import QtQuick.Controls

CellBase {
  contentItem:
  Label {
    text : value
//    wrapMode: Text.WrapAnywhere
    clip:true
    verticalAlignment: Text.AlignVCenter
  }
}
