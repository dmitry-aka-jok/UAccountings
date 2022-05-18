import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import UA.SQL

import "../delegates"
import "../modules"

Table {
  tableName: "contractors"

  // В теории вообще может не быть поля, которое будет в редакторе
  fields : [
    {field:"shortname", name:"Скорочення"},
    {field:"name", name:"Назва"},
    {field:"inn", name:"ІНН"}
  ];

  editor: GridLayout {
    id: grid
    columns: 2

    FieldTitle {
      text: "Назва"
    }
    FieldString {
      id:name
      field: "name"
      Layout.fillWidth : true
      onValueChanged:{
        shortname.value = name.value.replace("ФОП", "").trim()
      }
    }

    FieldTitle {
      text: "Скорочення"
    }
    FieldString {
      id:shortname
      field: "shortname"
      Layout.fillWidth : true
      onValueChanged:{

      }
    }

    FieldTitle {
      text: "ІПН"
    }
    FieldString {
      id:inn
      field: "inn"
      Layout.fillWidth : true
      onValueChanged:{
      }
    }
  }


  function onEditStarted(){

    return true
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
