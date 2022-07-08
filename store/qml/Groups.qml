import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import UA.SQL

import "../delegates"
import "../modules"

Table {
  tableName: "goodsgroups"
  fields : [
    {field:"name", name:"Назва"},
  ];

  editor: GridLayout {
    id: grid
    columns: 2
    columnSpacing: 10

    FieldTitle {
      text: "Назва"
    }
    FieldString {
      id:name
      field: "name"
      Layout.fillWidth : true
    }
  }


  Dialog {
      id: dialog
      modal: true
      standardButtons: Dialog.Ok
      anchors.centerIn: parent
      property alias text: label.text
      Label {
        id:label
      }
  }


  function onEditStarted(){

    return true
  }

  function onEditCommited(){
//    dialog.text = "Some fucking thing happens!"
//    dialog.open()

//    return false
  }
  function onEditRejected(){

  }
  /*
  onEditFinished:{

  }
  */
}
