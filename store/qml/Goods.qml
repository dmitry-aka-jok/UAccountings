import "../modules"

Table {
  tableName: "goods"
  fields : [
    {field:"name", name:"Назва"},
    {field:"goodsgroups_id", name:"Група", foreign_table:"goodsgroups", foreign_key:"id", foreign_name:"name"}
  ];

  const goods = {};
  goods.proto.onNameChanged: {}
}


