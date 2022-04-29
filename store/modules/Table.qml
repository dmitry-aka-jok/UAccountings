import QtQml
import QtQml.Models

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Qt.labs.qmlmodels

import UA.SQL
import UA.Datapipe

import "../delegates"


UATable {
  id: tableRoot

  property int headerHeight: 30

  property alias editor: editorLoader.sourceComponent;

  property alias model: table.model

  Component.onCompleted: {
    init()
    tableRoot.model = sqlModel
  }



  HorizontalHeaderView {
    id: tableHeader
    syncView: table
    anchors.left: table.left
    visible: tableRoot.state===""

    delegate:
        HeaderDelegate {
      id: headerDelegate
      text: fields[column]["name"]
      implicitHeight: headerHeight

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
              var newWidth = headerDelegate.width + mouseX
              if (newWidth >= 50) { // TODO Change to constant
                setColumnWidth(column, newWidth)
                table.forceLayout()
              }
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

        MouseArea {
          anchors.fill: parent
          onClicked: {
            if(sortField===column){
              if (sortOrder===2){
                sortOrder = 0
              }else{
                sortOrder += 1
              }
            }
            else {
              sortField = column
              sortOrder = 1;
            }

            applySort()

          }
        }
      }
    }
  }

  TableView {
    id: table
    anchors.fill: parent
    anchors.topMargin: tableHeader.height
    anchors.bottomMargin: searchField.height

    z:0
    reuseItems: true
    clip: true
    visible: tableRoot.state===""
    property int currentRow: 0


    columnWidthProvider:
        (column) => getColumnWidth(column)

    model: tableRoot.sqlModel

    delegate: DelegateChooser {
      role:"Type"

      DelegateChoice {
        roleValue: "Bool"
        CellBool {
          value: model["Display"]
        }
      }

      DelegateChoice {
        roleValue: "String"
        CellString {
          value: model["Display"]
        }
      }

      DelegateChoice {
        roleValue: "Numeric"
        CellNumeric {
          digits: model["Digits"]
          value: model["Display"]
        }
      }

      DelegateChoice {
        CellBase {
          value: model["Display"]
        }
      }
    }

    ScrollIndicator.vertical: ScrollIndicator {}
    ScrollIndicator.horizontal: ScrollIndicator {}
  }

  Keys.onPressed:
      (event)=> {
        if(tableRoot.state===""){
          if (event.key === Qt.Key_Up) {
            if(table.currentRow > 0)
            table.currentRow--;
            event.accepted = true;
          }
          if (event.key === Qt.Key_Down) {
            if(table.currentRow < sqlModel.rowCount()-1)
            table.currentRow++;
            event.accepted = true;
          }
          if (event.key === Qt.Key_Enter || event.key === Qt.Key_Return) {
            startEditor()
            event.accepted = true;
          }
        }else
        if(tableRoot.state==="editor"){
          if (event.key === Qt.Key_Enter || event.key === Qt.Key_Return) {
            commitEditor(editorHolder)
            tableRoot.state = ""
            event.accepted = true;
          }
          if (event.key === Qt.Key_Escape) {
            tableRoot.state = ""
            event.accepted = true;
          }
//          if (event.key === Qt.Key_Up) {
//            console.log("up")
//            editorHolder.nextItemInFocusChain(false)
//          }
//          if (event.key === Qt.Key_Down) {
//            console.log("down")
//            editorHolder.nextItemInFocusChain(true)
//          }

        }
      }

  function startEditor(){
    tableRoot.editStatement = sqlIndex(table.currentRow)
    tableRoot.state = "editor"
  }

  Row {
    id: searchRow
    anchors.left: parent.left
    anchors.bottom: parent.bottom
    width: tableRoot.width
    visible: tableRoot.state===""

    Label {
      id: searchName
      anchors.verticalCenter: parent.verticalCenter
      text: (filterField===-1)?"Всі стовпчики: ":(fields[filterField]["name"]+": ")

      MouseArea {
        anchors.fill: parent
        onClicked: {
          let newFilter = filterField;
          while (true){
            newFilter++;

            if(newFilter>=fields.length){
              newFilter=-1;
              break;
            }
            if(getColumnWidth(newFilter)!==0)
              break;
          }

          filterField = newFilter;

          applyFilter()
        }
      }
    }

    TextField {
      id: searchField
      width: parent.width - searchName.width

      focus: true
      placeholderText: "строка пошуку"
      onTextChanged: {
        sqlModel.setFilterString(searchField.text)
      }
    }
  }

  Column {
    id : editorHolder
    visible : tableRoot.state==="editor"
    anchors.fill: parent

    Loader {
      width: parent.width
      id: editorLoader
    }


    Row {
      spacing: 10


      Button {
        id: btCancel
        text: qsTr("Відміна")
        onClicked: {
          tableRoot.state = ""
        }
      }

      Button {
        id: btOk
        text: qsTr("Ok")

        onClicked: {
          commitEditor(editorHolder)
          tableRoot.state = ""
        }
      }
    }


    onVisibleChanged: {
      if(editorHolder.visible){
        prepareEditor(editorHolder)
      }

    }



  }




  states: [
    State {
      name: ""
    },
    State {
      name: "editor"
    }
  ]


}


