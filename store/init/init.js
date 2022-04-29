.pragma library
.import UA.SQL as SQL
.import UA.Datapipe as Core


function initUA(){
  initDatabase()
}


function initDatabase(){
    Core.Datapipe.defineTable("users", {
                    id:{type:SQL.SQLCore.PK},
                    name:{type:SQL.SQLCore.String,name:"Назва"},
                });
    Core.Datapipe.defineTable("settings", {
                    user_id:{type:SQL.SQLCore.FK,name:"Користувач",accept_null:false,reference:"users.id"},
                    section:{type:SQL.SQLCore.String,name:"Розділ"},
                    option:{type:SQL.SQLCore.String,name:"Параметр"},
                    value:{type:SQL.SQLCore.String,name:"Значення"}
                });




    Core.Datapipe.defineTable("goods", {
                    id:{type:SQL.SQLCore.PK,name:"Код"},
                    name:{type:SQL.SQLCore.String,name:"Назва"},
                    goodsgroup_id:{type:SQL.SQLCore.FK,name:"Група",accept_null:false,reference:"goodsgroups.id"},
                    is_weight:{type:SQL.SQLCore.Bool,name:"Ваговий",accept_null:false},
                    price_buy:{type:SQL.SQLCore.Numeric,digits:2,name:"Ціна закупівлі"},
                    price_sell:{type:SQL.SQLCore.Numeric,digits:2,name:"Ціна продажу"}

                });

    Core.Datapipe.defineTable("goodsgroups", {
                    id:{type:SQL.SQLCore.PK,name:"Код"},
                    name:{type:SQL.SQLCore.String,name:"Назва"}
                });
    Core.Datapipe.defineTable("contractors", {
                    id:{type:SQL.SQLCore.PK,name:"Код"},
                    name:{type:SQL.SQLCore.String,name:"Назва"},
                    shortname:{type:SQL.SQLCore.String,name:"Скорочення"},
                    inn:{type:SQL.SQLCore.String,name:"ІПН"}
                });

}

