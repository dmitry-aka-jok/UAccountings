import QtQuick
import QtQuick.Layouts

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
      value: "Назва"
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
      value: "Скорочення"
    }
    FieldString {
      id:shortname
      field: "shortname"
      Layout.fillWidth : true
      onValueChanged:{

      }
    }

    FieldTitle {
      value: "ІПН"
    }
    FieldString {
      id:inn
      field: "inn"
      Layout.fillWidth : true
      onValueChanged:{
      }
    }
  }
/*
  onEditStarted:{

  }
  onEditFinished:{

  }
  */
}
