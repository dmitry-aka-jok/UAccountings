import QtQml
import QtQml.Models

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Qt.labs.qmlmodels

import UA.SQL
import UA.Datapipe

import "../delegates"


Item {
  id: tableRoot
  property var sqlModel;

  required property string tableName;
  required property var fields;

  property var fieldsWidths : [];
  property var fieldsTypes : [];

  property int minimalFieldWidth: 50
  property int headerHeight: 30
  property int sortField;
  property int sortOrder;
  property int filterField;

  property var beforeEdit: [];
  property string editStatement;
  property var selFields : [];
  property var selJoins  : [];


/*
  function UATable(tableName, fields)



  tableName: "goods"
  fields : [
    {field:"name", name:"Назва", onChange: "function(item){}" },
    {field:"goodsgroups_id", name:"Група", foreign_table:"goodsgroups", foreign_key:"id", foreign_name:"name"}
  ];
  actions : {onCreate: onSave: }

  onFieldNameChanged

*/


  Component.onCompleted: {

    for(var i=0; i<fields.length; i++){
      if(typeof fields[i]["foreign_table"] === 'undefined')
        selFields.push(fields[i]["field"])
      else{
        selFields.push(fields[i]["foreign_table"]+"."+fields[i]["foreign_name"])
        selJoins.push(fields[i]["foreign_table"])
      }
    }

    sqlModel = SQLCore.table(tableName).select({fields:selFields, joins:selJoins});

    let types = sqlModel.typesList();

    let sqlSettings = SQLCore.table("settings").select({fields:"value", where:{"section":tableName, "option":"columnsWidth", "user_id":Datapipe.variable("user_id",-1)}});
    let vvv = sqlSettings.value(0,0,"").split(",");

    for (let i = 0; i < fields.length; i++) {
      if (fields[i]["visible"] === false)
        fieldsWidths[i] = 0;
      else
        if(typeof vvv[i] !== 'undefined')
          fieldsWidths[i] = Math.max(vvv[i], minimalFieldWidth) ;
        else
          fieldsWidths[i] = minimalFieldWidth;

      fieldsTypes[i] = types[i];
    }
    sqlSettings = SQLCore.table("settings").select(
          {fields:"value", where:{"section":tableName, "option":"filterField", "user_id":Datapipe.variable("user_id",-1)}});
    filterField = sqlSettings.value(0,0,-1);

    sqlModel.setFilterColumn(filterField);

    sqlSettings = SQLCore.table("settings").select(
          {fields:"value", where:{"section":tableName, "option":"sortField", "user_id":Datapipe.variable("user_id",-1)}});
    sortField = sqlSettings.value(0,0,-1);

    sqlSettings = SQLCore.table("settings").select(
          {fields:"value", where:{"section":tableName, "option":"sortOrder", "user_id":Datapipe.variable("user_id",-1)}});
    sortOrder = sqlSettings.value(0,0,0);

    setSort();


    let inputFormFields = "";
    let fillFormFields = "";
    let saveFormFields = "";
    for (i = 0; i < fields.length; i++) {
      inputFormFields += `
      Label{
        id: name_${fields[i]["field"]}
        text:"${fields[i]["name"]}"
      }
      TextField{
        id: field_${fields[i]["field"]}
        Layout.fillWidth : true
      }`;

      fillFormFields += `
           field_${fields[i]["field"]}.text = editorModel.value(0,${i},-1);`
      saveFormFields += `
           if (field_${fields[i]["field"]}.text !== beforeEdit[${i}])
              saveFields.set("${fields[i]["field"]}", field_${fields[i]["field"]}.text)
      `;
    }

    let text = `
    import QtQuick
    import QtQuick.Layouts
    import QtQuick.Controls

    import UA.SQL
    import UA.Datapipe

    GridLayout{
      id: editorGrid
      width: parent.width
      columns: 2
      ${inputFormFields}

      Button {
        id: btCancel
        text: "Відміна"
        onClicked: {
          tableRoot.state = ""
        }
      }
      Button {
        id: btOk
        text: "Ok"
        onClicked: {
          let saveFields = new Map()
          ${saveFormFields}
          console.log(saveFields)
          if(saveFields.size > 0)
            console.log("yep")

          tableRoot.state = ""
        }
      }

      onVisibleChanged: {
        if(editorGrid.visible===true){
           let editorModel = SQLCore.table(tableName).select({fields:selFields, joins:selJoins, where:tableRoot.editStatement});
           beforeEdit = [];
           for(let i=0; i<fields.length; i++)
              beforeEdit[i] = editorModel.value(0,i,-1);
           ${fillFormFields}
        }
      }

    }
    `
    console.log(text)
    Qt.createQmlObject(text, editor)

  }

  function saveWidths() {
    let sss = "";
    for (let column = 0; column < fields.length; column++) {
      sss += (column==0?"":",")+fieldsWidths[column];
    }

    //console.log(sss)
    SQLCore.table("settings").insert_or_update(
          {"value":sss}, {"section":tableName, "option":"columnsWidth", "user_id":Datapipe.variable("user_id",-1)})
  }
  function saveFilter() {
    SQLCore.table("settings").insert_or_update(
          {"value":filterField}, {"section":tableName, "option":"filterField", "user_id":Datapipe.variable("user_id",-1)})
  }
  function saveSort() {
    SQLCore.table("settings").insert_or_update(
          {"value":sortField}, {"section":tableName, "option":"sortField", "user_id":Datapipe.variable("user_id",-1)})
    SQLCore.table("settings").insert_or_update(
          {"value":sortOrder}, {"section":tableName, "option":"sortOrder", "user_id":Datapipe.variable("user_id",-1)})
  }

  function setSort(){
    if (sortOrder==0)
      sqlModel.setSort(-1, Qt.AscendingOrder)
    if (sortOrder==1)
      sqlModel.setSort(sortField, Qt.AscendingOrder)
    if (sortOrder==2)
      sqlModel.setSort(sortField, Qt.DescendingOrder)
  }



  HorizontalHeaderView {
    id: tableHeader
    syncView: table
    anchors.left: table.left
    visible: tableRoot.state===""

    delegate:
        HeaderDelegate {
      id: headerDelegate
      text: fields[column]["name"]
      implicitHeight: headerHeight

      Rectangle {
        id: resizeHandle
        color: Qt.darker(parent.color, 1.05)
        height: parent.height
        width: 10
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter

        MouseArea {
          anchors.fill: parent
          drag{ target: parent; axis: Drag.XAxis }
          hoverEnabled: true
          cursorShape: Qt.SizeHorCursor
          onMouseXChanged: {
            if (drag.active) {
              //console.log(mouseX, headerDelegate.width)
              var newWidth = headerDelegate.width + mouseX
              if (newWidth >= minimalFieldWidth) {
                fieldsWidths[column] = newWidth
                table.forceLayout()
              }
            }
          }
          onReleased: {
            saveWidths();
          }
        }
      }

      Image {
        id: sortHandle

        source: (sortField!==column||sortOrder===0?"qrc:/icons/fi-bs-arrows-h-copy.svg":(sortOrder===1?"qrc:/icons/fi-bs-arrow-down.svg":"qrc:/icons/fi-bs-arrow-up.svg"))

        anchors.verticalCenter: parent.verticalCenter
        anchors.right: resizeHandle.left
        anchors.rightMargin: 1
        anchors.topMargin: 1
        anchors.bottomMargin: 1

        width: 15
        height: parent.height
        fillMode: Image.PreserveAspectFit

        MouseArea {
          anchors.fill: parent
          onClicked: {
            if(sortField===column){
              if (sortOrder===2){
                sortOrder = 0
              }else{
                sortOrder += 1
              }
            }
            else {
              sortField = column
              sortOrder = 1;
            }

            setSort()

            saveSort()
          }
        }
      }
    }
  }

  TableView {
    id: table
    anchors.fill: parent
    anchors.topMargin: tableHeader.height
    anchors.bottomMargin: searchField.height

    z:0
    reuseItems: true
    clip: true
    visible: tableRoot.state===""
    property int currentRow: 0


    columnWidthProvider:
        function (column) {
          return fieldsWidths[column];
        }


    model: sqlModel

    //        selectionModel: ItemSelectionModel {
    //           id: selecton
    //        }

    delegate:
        DelegateChooser {
      role:"Type"

      DelegateChoice {
//        roleValue: "QString"

        ItemDelegate {
          id: delegate
          //          implicitHeight: theme.basicElementSize
          text: model["Display"]
          highlighted: row % 2 == 0 //row === table.currentRow
/*
          background: Rectangle {
            color: delegate.highlighted ? theme.highlightColor : row % 2 == 0 ? theme.backgroundColor : theme.interleaveColor
          }
*/
          onClicked: {
            table.currentRow = row
          }
          onDoubleClicked: {
            tableRoot.editStatement = model["Index"]
            table.currentRow = row
            tableRoot.state = "editor"
          }
        }
      }
    }
    ScrollIndicator.vertical: ScrollIndicator {}
    ScrollIndicator.horizontal: ScrollIndicator {}
  }

  Keys.onPressed:
      (event)=> {
        if(tableRoot.state===""){
          if (event.key === Qt.Key_Up) {
            if(table.currentRow > 0)
            table.currentRow--;
            event.accepted = true;
          }
          if (event.key === Qt.Key_Down) {
            if(table.currentRow < sqlModel.rowCount()-1)
            table.currentRow++;
            event.accepted = true;
          }
        }
      }

  Row {
    id: searchRow
    anchors.left: parent.left
    anchors.bottom: parent.bottom
    width: tableRoot.width
    visible: tableRoot.state===""

    Label {
      id: searchName
      anchors.verticalCenter: parent.verticalCenter
      text: (filterField===-1)?"Всі стовпчики: ":(fields[filterField]["name"]+": ")

      MouseArea {
        anchors.fill: parent
        onClicked: {
          let newFilter = filterField;
          while (true){
            newFilter++;

            if(newFilter>=fields.length){
              newFilter=-1;
              break;
            }
            if(fieldsWidths[newFilter]!==0)
              break;
          }

          filterField = newFilter;

          sqlModel.setFilterColumn(filterField)
          saveFilter()
        }
      }
    }

    TextField {
      id: searchField
      width: parent.width - searchName.width

      focus: true
      placeholderText: "строка пошуку"
      onTextChanged: {
        sqlModel.setFilterString(searchField.text)
      }
    }
  }

  Column {
    id : editor
    visible : tableRoot.state==="editor"
    anchors.fill: parent

  }




  states: [
    State {
      name: ""
    },
    State {
      name: "editor"
    }
  ]


}


