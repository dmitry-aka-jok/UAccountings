import QtQuick
import QtQuick.Controls


TextField {
  id: fieldRoot
  property alias value : fieldRoot.text
  required property string field
  selectByMouse: true

  verticalAlignment: Text.AlignVCenter
  horizontalAlignment: Text.AlignLeft

}



