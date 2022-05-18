import QtQuick
import QtQuick.Controls


TextField {
  id: fieldRoot
  property alias value : fieldRoot.text
  property alias digits: validtr.decimals
  required property string field
  selectByMouse: true

  verticalAlignment: Text.AlignVCenter
  horizontalAlignment: Text.AlignLeft

  validator: DoubleValidator {
    id: validtr
    locale:"C"
  }
//  validator: IntValidator{bottom: 11; top: 31;}
}



