import QtQuick
import QtQuick.Controls

Rectangle {
    id: headerRoot
    property alias text : label.text
    property int minimalWidth : 50

    signal sortClicked()

    Label {
        id: label

        anchors.fill: parent
        anchors.rightMargin: resizeHandle.width + sortHandle.width

        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: label.implicitWidth > width ? Text.AlignLeft : Text.AlignHCenter
        font.bold: true

        clip: true
    }



    Rectangle {
      id: resizeHandle
      color: Qt.darker(parent.color, 1.05)
      height: parent.height
      width: 10
      anchors.right: parent.right
      anchors.verticalCenter: parent.verticalCenter

      MouseArea {
        anchors.fill: parent
        drag{ target: parent; axis: Drag.XAxis }
        hoverEnabled: true
        cursorShape: Qt.SizeHorCursor
        onMouseXChanged: {
          if (drag.active) {
            var newWidth = headerRoot.width + mouseX
            if (newWidth >= minimalWidth)
              headerRoot.width = newWidth
          }
        }
      }
    }


    Image {
      id: sortHandle

      source: (sortField!==column||sortOrder===0?"qrc:/icons/fi-bs-arrows-h-copy.svg":(sortOrder===1?"qrc:/icons/fi-bs-arrow-down.svg":"qrc:/icons/fi-bs-arrow-up.svg"))

      anchors.verticalCenter: parent.verticalCenter
      anchors.right: resizeHandle.left
      anchors.rightMargin: 1
      anchors.topMargin: 1
      anchors.bottomMargin: 1

      width: 15
      height: parent.height
      fillMode: Image.PreserveAspectFit

    }
    MouseArea {
      anchors.fill: parent
      anchors.rightMargin: resizeHandle.width
      onClicked: {
        headerRoot.sortClicked()
      }
    }

}
