import QtQuick
import QtQuick.Controls


ItemDelegate {
  id: delegateRoot

  Rectangle {
    anchors.top: parent.top
    color:  row === tableRoot.currentRow ? "black" : "transparent"
    height: 2
    width: parent.width
  }
  Rectangle {
    anchors.bottom: parent.bottom
    color:  row === tableRoot.currentRow ? "black" : "transparent"
    height: 2
    width: parent.width
  }
  Rectangle {
    anchors.left: parent.left
    color:  row === tableRoot.currentRow && column === 0 ? "black" : "transparent"
    height: parent.height
    width: 2
  }
  Rectangle {
    anchors.left: parent.right
    color:  row === tableRoot.currentRow && column === tableRoot.fields.length ? "black" : "transparent"
    height: parent.height
    width: 2
  }

  Image {
    anchors.fill: parent
    anchors.margins: 5
    visible:tableRoot.state==="delete"
    fillMode: Image.PreserveAspectFit
    source:"qrc:/icons/fi-bs-remove.svg"
  }

  MouseArea{
    anchors.fill: parent
    onClicked: {
      if (tableRoot.state==="delete"){
        tableRoot.remove(row)
      }
    }
  }

  highlighted: row % 2 == 0

}
