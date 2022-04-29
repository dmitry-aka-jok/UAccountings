import QtQuick
import QtQuick.Controls

Switch {
  id: fieldRoot
  property alias value : fieldRoot.checked
  required property string field
  //verticalAlignment: Text.AlignVCenter

  checked: value??false
}
