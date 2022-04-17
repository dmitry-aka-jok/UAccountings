.pragma library
.import UA.SQL as SQLModule


function initUA(){
  initDatabase()
}


function initDatabase(){
    SQLModule.SQLCore.defineTable("users", {
                    id:{type:SQLModule.SQLCore.PK},
                    name:{type:SQLModule.SQLCore.String,name:"Назва"},
                });
    SQLModule.SQLCore.defineTable("settings", {
//                    id:{type:SQLModule.SQLCore.PK},
                    user_id:{type:SQLModule.SQLCore.FK,name:"Користувач",accept_null:false,reference:"users.id"},
                    section:{type:SQLModule.SQLCore.String,name:"Розділ"},
                    option:{type:SQLModule.SQLCore.String,name:"Параметр"},
                    value:{type:SQLModule.SQLCore.String,name:"Значення"}
                });




    SQLModule.SQLCore.defineTable("goods", {
                    id:{type:SQLModule.SQLCore.PK,name:"Код"},
                    name:{type:SQLModule.SQLCore.String,name:"Назва"},
                    goodsgroup_id:{type:SQLModule.SQLCore.FK,name:"Група",accept_null:false,reference:"goodsgroups.id"}
                });

    SQLModule.SQLCore.defineTable("goodsgroups", {
                    id:{type:SQLModule.SQLCore.PK,name:"Код"},
                    name:{type:SQLModule.SQLCore.String,name:"Назва"}
                });
}

