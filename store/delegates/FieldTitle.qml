import QtQuick
import QtQuick.Controls

FieldBase {
  field: ""
  contentItem:
  Label {
    text : value??""
    verticalAlignment: Text.AlignVCenter
  }
}
