import QtQuick
import QtQuick.Controls

ItemDelegate {
  id: delegateRoot
  required property var value

  //  implicitHeight: 50

  Rectangle {
    anchors.top: parent.top
    color:  row === table.currentRow ? "black" : "transparent"
    height: 2
    width: parent.width
  }
  Rectangle {
    anchors.bottom: parent.bottom
    color:  row === table.currentRow ? "black" : "transparent"
    height: 2
    width: parent.width
  }
  Rectangle {
    anchors.left: parent.left
    color:  row === table.currentRow && column === 0 ? "black" : "transparent"
    height: parent.height
    width: 2
  }
  Rectangle {
    anchors.left: parent.right
    color:  row === table.currentRow && column === tableRoot.fields.length-1 ? "black" : "transparent"
    height: parent.height
    width: 2
  }

//  Component.onCompleted: {
//    console.log()
//  }

  text : delegateRoot.value
  highlighted: row % 2 == 0

  onClicked: {
    table.currentRow = row
    tableRoot.startEditor()
  }
}
