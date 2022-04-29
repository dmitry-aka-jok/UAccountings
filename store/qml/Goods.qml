import QtQuick
import QtQuick.Layouts

import UA.SQL

import "../delegates"
import "../modules"

Table {
  tableName: "goods"
  fields : [
    {field:"name", name:"Назва"},
    {field:"goodsgroups_id", name:"Група", foreign_table:"goodsgroups", foreign_key:"id", foreign_name:"name"},
    {field:"is_weight", name:"Ваговий"},
    {field:"price_buy", name:"Ціна закупівлі"},
    {field:"price_sell", name:"Ціна продажу"}
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
    }

    FieldTitle {
      value: "Група"
    }
    FieldString {
      id:goodsgroups_id
      field: "goodsgroups_id"
      Layout.fillWidth : true
    }

    FieldTitle {
      value: "Ваговий"
    }
    FieldBool {
      id:is_weight
      field: "is_weight"
      Layout.fillWidth : true
    }

    FieldTitle {
      value: "Ціна закупівлі"
    }
    FieldNumeric {
      id:price_buy
      field: "price_buy"
      digits: 2
      Layout.fillWidth : true
    }

    FieldTitle {
      value: "Ціна продажу"
    }
    FieldNumeric {
      id:price_sell
      field: "price_sell"
      digits: 2
      Layout.fillWidth : true
    }

  }

}


