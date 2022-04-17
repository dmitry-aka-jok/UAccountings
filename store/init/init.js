.pragma library
.import UA.SQL as SQL


function initUA(){
  initDatabase()
  console.log("Store database")
}


function initDatabase(){
    SQL.SQL.defineTable("users", {
                    id:{type:"PK"},
                    name:{type:"String",name:"Назва"},
                });
    SQL.SQL.defineTable("settings", {
                    id:{type:"PK"},
                    user_id:{type:"FK",name:"Користувач",accept_null:false,reference:"users.id"},
                    section:{type:"String",name:"Розділ"},
                    option:{type:"String",name:"Параметр"},
                    value:{type:"String",name:"Значення"}
                });




    SQL.SQL.defineTable("goods", {
                    id:{type:"PK"},
                    name:{type:"String",name:"Назва"},
                    goodsgroup_id:{type:"FK",name:"Група",accept_null:false,reference:"goodsgroups.id"}
                });

    SQL.SQL.defineTable("goodsgroups", {
                    id:{type:"PK"},
                    name:{type:"String",name:"Назва"}
                });
}

