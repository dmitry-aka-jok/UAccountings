.pragma library
.import UA.SQL as SQL
.import UA.Datapipe as Core
.import UA.Enums as Enums


function initUA(){
  initDatabase()
}


function initDatabase(){
    Core.Datapipe.defineTable("uac_users", {
                    id:{type:Enums.SqlType.PK},
                    name:{type:Enums.SqlType.String,name:"Назва"},
                });
    Core.Datapipe.defineTable("uac_settings", {
                    user_id:{type:Enums.SqlType.FK,name:"Користувач",accept_null:false,reference:"users.id"},
                    section:{type:Enums.SqlType.String,name:"Розділ"},
                    option:{type:Enums.SqlType.String,name:"Параметр"},
                    value:{type:Enums.SqlType.String,name:"Значення"}
                });




    Core.Datapipe.defineTable("goods", {
                    id:{type:Enums.SqlType.PK,name:"Код"},
                    name:{type:Enums.SqlType.String,name:"Назва"},
                    goodsgroup_id:{type:Enums.SqlType.FK,name:"Група",accept_null:false,reference:"goodsgroups.id"},
                    is_weight:{type:Enums.SqlType.Bool,name:"Ваговий",accept_null:false},
                    price_buy:{type:Enums.SqlType.Numeric,digits:2,name:"Ціна закупівлі"},
                    price_sell:{type:Enums.SqlType.Numeric,digits:2,name:"Ціна продажу"}

                });

    Core.Datapipe.defineTable("goodsgroups", {
                    id:{type:Enums.SqlType.PK,name:"Код"},
                    name:{type:Enums.SqlType.String,name:"Назва"}
                });
    Core.Datapipe.defineTable("contractors", {
                    id:{type:Enums.SqlType.PK,name:"Код"},
                    name:{type:Enums.SqlType.String,name:"Назва"},
                    shortname:{type:Enums.SqlType.String,name:"Скорочення"},
                    inn:{type:Enums.SqlType.String,name:"ІПН"}
                });

}

