import QtQml
import QtQml.Models

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Qt.labs.qmlmodels

import UA.SQL
import UA.Enums
import UA.Datapipe

import "../delegates"


UATable {
  id: tableRoot

  property int headerHeight: 50

//  property alias model: tableView.model
  property alias editor: editorLoader.sourceComponent;




  Component.onCompleted: {
    tableInit()
    tableView.model = tableModel()

    searchField.focus=true
  }

  RowLayout {
    id: searchRow
    anchors.top: parent.top
    width: tableRoot.width
    visible: tableRoot.state===""||tableRoot.state==="delete"

    Label {
      id: searchName
      text: (filterColumn===-1)?"Всі стовпчики: ":(fields[filterColumn]["name"]+": ") // TODO change

      MouseArea {
        anchors.fill: parent
        onClicked: {
          let newFilter = filterColumn;
          while (true){
            newFilter++;
            if(newFilter>=fields.length){
              newFilter=-1;
              break;
            }
            if(getColumnWidth(newFilter)!==0)
              break;
          }
          filterColumn = newFilter;
        }
      }
    }

    TextField {
      id: searchField
      focus: true
      placeholderText: qsTr("строка пошуку")
      Layout.fillWidth: true
      onTextChanged: {
        setFilterString(searchField.text)
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
        roleValue: SqlType.ServiceColumn
        CellService {}
      }
      DelegateChoice {
        roleValue: SqlType.AppendRow
        CellAdd {}
      }
      DelegateChoice {
        roleValue: SqlType.NaT
        CellEmpty {}
      }
      DelegateChoice {
        roleValue: SqlType.TotalsRow
        CellTotals {}
      }

      DelegateChoice {
        roleValue: SqlType.Bool
        CellBool {
          value: model["Display"]
        }
      }

      DelegateChoice {
        roleValue: SqlType.String
        CellString {
          value: model["Display"]
        }
      }

      DelegateChoice {
        roleValue: SqlType.Numeric
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
            if(tableRoot.currentRow < tableView.model.rowCount()-1)
            tableRoot.currentRow++;
            event.accepted = true;
          }
          if (event.key === Qt.Key_Enter || event.key === Qt.Key_Return) {
            if(currentRow === tableView.model.rowCount()-1 && tableView.model.appendRow)
              startEditor("add");
            else
              startEditor("edit");
            event.accepted = true;
          }
        }else
        if(tableRoot.state==="edit" || tableRoot.state==="add"){
          if (event.key === Qt.Key_Enter || event.key === Qt.Key_Return) {
            commitEditor()
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
    editorLoader.focus = true
//    /firstElementFocus = true
  }


  function commitEditor(){
//    Datapipe.setVariable("debugQueries", true)

    let result = true
    if(tableRoot.hasOwnProperty("onEditCommited"))
      result = tableRoot["onEditCommited"]()

    if (result!==false){
      if(tableRoot.state==="edit")
        commitEdit(editorHolder)
      if(tableRoot.state==="add")
        commitAdd(editorHolder)


      tableRoot.state = ""
      searchField.focus = true
    }

//    Datapipe.setVariable("debugQueries", false)

  }



  Column {
    id : editorHolder
    visible : tableRoot.state==="edit" || tableRoot.state==="add"
    anchors.fill: parent

    Loader {
      width: parent.width
      id: editorLoader
    }


    RowLayout {
      spacing: 10


      Button {
        id: btCancel
//        Layout.preferredWidth: 150
        text: qsTr("Відміна")
        onClicked: {
          tableRoot.state = ""
        }
      }

      Button {
        id: btOk
        Layout.preferredWidth: btCancel.width
        text: qsTr("Ok")

        onClicked: {
          commitEditor()
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


