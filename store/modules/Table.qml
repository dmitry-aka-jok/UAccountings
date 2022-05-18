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

  property int headerHeight: 50

  property alias model: tableView.model
  property alias editor: editorLoader.sourceComponent;


  Component.onCompleted: {
    tableInit()
    tableRoot.model = sqlModel
  }

  RowLayout {
    id: searchRow
    anchors.top: parent.top
    width: tableRoot.width
    visible: tableRoot.state===""||tableRoot.state==="delete"

    Label {
      id: searchName
      text: (filterField===-1)?"Всі стовпчики: ":(fields[filterField]["name"]+": ") // TODO change

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
        }
      }
    }

    TextField {
      id: searchField
      focus: true
      placeholderText: qsTr("строка пошуку")
      Layout.fillWidth: true
      onTextChanged: {
        sqlModel.setFilterString(searchField.text)
      }
    }
  }


  HorizontalHeaderView {
    id: tableHeader
    syncView: tableView
    anchors.left: tableView.left
    anchors.top: parent.top
    anchors.topMargin: searchRow.height
    visible: tableRoot.state==="" || tableRoot.state==="delete"

    delegate: DelegateChooser {
      DelegateChoice {
        column: 0
        Button {
          icon.source:"qrc:/icons/fi-bs-delete.svg"
          flat:true
          width:headerHeight

          onClicked: {
            if (tableRoot.state === "delete")
              tableRoot.state = ""
            else
              tableRoot.state = "delete"
          }

        }
      }

      DelegateChoice {
        HeaderDelegate {
          id: headerDelegate
          text: fieldProperty(column, "name")

          implicitHeight: headerHeight

          onWidthChanged: {
            if (getColumnWidth(column) !== headerDelegate.width){
              setColumnWidth(column, headerDelegate.width)
              tableView.forceLayout()
            }
          }

          onSortClicked:  {
            if(tableRoot.sortField===column){
              if (tableRoot.sortOrder===2){
                tableRoot.sortOrder = 0
              }else{
                tableRoot.sortOrder += 1
              }
            }
            else {
              tableRoot.sortField = column
              tableRoot.sortOrder = 1;
            }
          }
        }
      }
    }
  }

  TableView {
    id: tableView
    anchors.fill: parent
    anchors.topMargin: tableHeader.height + searchRow.height
    anchors.bottomMargin: searchField.height

    z:0
    reuseItems: true
    clip: true
    visible: tableRoot.state==="" || tableRoot.state==="delete"

    columnWidthProvider:
        (column) => getColumnWidth(column)

    delegate: DelegateChooser {
      role:"Type"

      DelegateChoice {
        roleValue: "Service"
        CellService {}
      }
      DelegateChoice {
        roleValue: "Add"
        CellAdd {}
      }
      DelegateChoice {
        roleValue: "Empty"
        CellEmpty {}
      }
      DelegateChoice {
        roleValue: "Totals"
        CellTotals {}
      }

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
        CellData {
          value: model["Display"]
        }
      }
    }

    ScrollIndicator.vertical: ScrollIndicator {}
    ScrollIndicator.horizontal: ScrollIndicator {}
  }

  Keys.onPressed:
      (event)=> {
        if(tableRoot.state==="" || tableRoot.state==="delete"){
          if (event.key === Qt.Key_Up) {
            if(tableRoot.currentRow > 0)
            tableRoot.currentRow--;
            event.accepted = true;
          }
          if (event.key === Qt.Key_Down) {
            if(tableRoot.currentRow < sqlModel.rowCount()-1)
            tableRoot.currentRow++;
            event.accepted = true;
          }
          if (event.key === Qt.Key_Enter || event.key === Qt.Key_Return) {
            startEditor()
            event.accepted = true;
          }
        }else
        if(tableRoot.state==="edit" || tableRoot.state==="add"){
          if (event.key === Qt.Key_Enter || event.key === Qt.Key_Return) {
            commitEditor(editorHolder)
            tableRoot.state = ""
            event.accepted = true;
          }
          if (event.key === Qt.Key_Escape) {
            tableRoot.state = ""
            event.accepted = true;
          }
        }
      }

  function startEditor(newState){
    if (newState === "edit"){
      prepareEdit(editorHolder, tableRoot.currentRow)
      tableRoot.state = "edit"
    }
    if (newState === "add"){
      prepareAdd(editorHolder)
      tableRoot.state = "add"
    }
  }


  Column {
    id : editorHolder
    visible : tableRoot.state==="edit" || tableRoot.state==="add"
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
          let result
          if(tableRoot.hasOwnProperty("onEditCommited"))
            result = tableRoot["onEditCommited"]()

          if (result!==false){
            if(tableRoot.state=="edit")
              commitEdit(editorHolder)
            if(tableRoot.state=="add")
              commitAdd(editorHolder)


            tableRoot.state = ""
          }
        }
      }
    }


    //    onVisibleChanged: {
    //      if(editorHolder.visible){
    //        if (tableRoot.state === "edit")
    //          prepareEdit(editorHolder)
    //        if (tableRoot.state === "add")
    //          prepareAdd(editorHolder)
    //      }
    //    }



  }




  states: [
    State {
      name: ""
    },
    State {
      name: "edit"
    },
    State {
      name: "add"
    },
    State {
      name: "delete"
    }
  ]


}


