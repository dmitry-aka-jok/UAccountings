import QtQuick
import QtQuick.Controls

CellBase {
  id: delegateRoot

  Image {
    anchors.fill: parent
    anchors.margins: 5
    fillMode: Image.PreserveAspectFit
    source:"qrc:/icons/fi-bs-add.svg"
  }

  MouseArea{
    anchors.fill: parent
    onClicked: {
      tableRoot.currentRow = row
      tableRoot.startEditor("add")
    }
  }
}
