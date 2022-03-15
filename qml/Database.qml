import QtQuick

import SQL.package

QtObject {
    property var dbGoodsGroups;
    property var dbGoods;
    property var dbContractors;
    property var dbUnits;

    Component.onCompleted: {
        dbGoods = SQL.table("goods", {
                                  id:{type:"PK"},
                                  name:{type:"String",name:"Название"},
                                  goodsgroup_id:{type:"FK",name:"Группа",accept_null:false,reference:"goodsgroups.id"}
                              });

        dbGoodsGroups = SQL.table("goodsgroups", {
                                        id:{type:"PK"},
                                        name:{type:"String",name:"Название"}
                                    });

        console.log("inited")
    }
}
