import QtQml
import QtQml.Models

import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts

import Qt.labs.qmlmodels

import UA.SQL
import UA.Datapipe

import "../delegates"


Item {
  id: pageRoot
  //title: "Товари"
  property var sqlModel;

  property string tableName: "goods"
  property var fields : [
    {field:"id", name:"Код", visible:false, primary:true},
    {field:"name", name:"Назва"},
    {field:"goodsgroups_id", name:"Група", foreign_table:"goodsgroups", foreign_key:"id", foreign_name:"name"}
  ];

  property var fieldsWidths : [];
  property var fieldsTypes : [];
  property int minimalFieldWidth: 50
  property int sortField;
  property int sortOrder;
  property int filterField;


  Component.onCompleted: {
    let selFields = [];
    let selJoins  = [];

    for(var i=0; i<fields.length; i++){
      if(typeof fields[i]["foreign_table"] === 'undefined')
        selFields.push(fields[i]["field"])
      else{
        selFields.push(fields[i]["foreign_table"]+"."+fields[i]["foreign_name"])
        selJoins.push(fields[i]["foreign_table"])
      }
    }

    sqlModel = SQL.table(tableName).select({fields:selFields, joins:selJoins});

    let types = sqlModel.typesList();

    let sqlSettings = SQL.table("settings").select({fields:"value", where:{"section":tableName, "option":"columnsWidth", "user_id":Datapipe.variable("user_id",-1)}});
    let vvv = sqlSettings.value(0,0,"").split(",");

    for (let i = 0; i < fields.length; i++) {
      if (fields[i]["visible"] === false)
        fieldsWidths[i] = 0;
      else
        if(typeof vvv[i] !== 'undefined')
          fieldsWidths[i] = vvv[i];
        else
          fieldsWidths[i] = theme.tableMinimalWidth;

      fieldsTypes[i] = types[i];
    }
    sqlSettings = SQL.table("settings").select(
          {fields:"value", where:{"section":tableName, "option":"filterField", "user_id":Datapipe.variable("user_id",-1)}});
    filterField = sqlSettings.value(0,0,-1);

    sqlModel.setFilterColumn(filterField);

    sqlSettings = SQL.table("settings").select(
          {fields:"value", where:{"section":tableName, "option":"sortField", "user_id":Datapipe.variable("user_id",-1)}});
    sortField = sqlSettings.value(0,0,-1);

    sqlSettings = SQL.table("settings").select(
          {fields:"value", where:{"section":tableName, "option":"sortOrder", "user_id":Datapipe.variable("user_id",-1)}});
    sortOrder = sqlSettings.value(0,0,0);

    setSort();


    let inputFormFields = "";
    for (i = 0; i < fields.length; i++) {
      inputFormFields += `
      Label{
      text:"${fields[i]["name"]}"
      }
      TextField{
      placeholderText:"field"
      Layout.fillWidth : true
      }`;
    }

    let text = `
    import QtQuick
    import QtQuick.Layouts
    import QtQuick.Controls


    GridLayout{
      columns: 2
      ${inputFormFields}

      Button {
        id: btCancel
        text: "Відміна"
        onClicked: {
          pageRoot.state = ""
        }
      }
      Button {
        id: btOk
        text: "Ok"
        onClicked: {
          pageRoot.state = ""
        }
      }
    }

    `

   // console.log(text)
    Qt.createQmlObject(text, editor)

  }

  function saveWidths() {
    let sss = "";
    for (let column = 0; column < fields.length; column++) {
      sss += (column==0?"":",")+fieldsWidths[column];
    }

    //console.log(sss)
    SQL.table("settings").insert_or_update(
          {"value":sss}, {"section":tableName, "option":"columnsWidth", "user_id":Datapipe.variable("user_id",-1)})
  }
  function saveFilter() {
    SQL.table("settings").insert_or_update(
          {"value":filterField}, {"section":tableName, "option":"filterField", "user_id":Datapipe.variable("user_id",-1)})
  }
  function saveSort() {
    SQL.table("settings").insert_or_update(
          {"value":sortField}, {"section":tableName, "option":"sortField", "user_id":Datapipe.variable("user_id",-1)})
    SQL.table("settings").insert_or_update(
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
    visible: pageRoot.state===""

    delegate:
        HeaderDelegate {
      id: headerDelegate
      text: fields[column]["name"]
      implicitHeight: 24//theme.basicElementSize

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
    visible: pageRoot.state===""
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
        roleValue: "QString"

        ItemDelegate {
          id: delegate
          //          implicitHeight: theme.basicElementSize
          text: model["Display"]
          highlighted: row % 2 == 0 //row === table.currentRow
          //          font: theme.tableFont
          /*
          background: Rectangle {
            color: delegate.highlighted ? theme.highlightColor : row % 2 == 0 ? theme.backgroundColor : theme.interleaveColor
          }
*/
          onClicked: {
            table.currentRow = row
          }
          onDoubleClicked: {
            table.currentRow = row
            pageRoot.state = "editor"
          }
        }
      }
      DelegateChoice {
        roleValue: "int"

        ItemDelegate {
          //implicitHeight: theme.basicElementSize
          text: model["Display"]
          highlighted: row % 2 == 0 //row === table.currentRow
          //font: theme.tableFont

          /*
          background: Rectangle {
            color: delegate.highlighted ? theme.highlightColor : row % 2 == 0 ? theme.backgroundColor : theme.interleaveColor
          }
          */

          onClicked: {
            table.currentRow = row
          }
          onDoubleClicked: {
            table.currentRow = row
            pageRoot.state = "editor"
          }
        }
      }
    }
    ScrollIndicator.vertical: ScrollIndicator { }
  }

  Keys.onPressed:
      (event)=> {
        if(pageRoot.state===""){
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
    width: pageRoot.width
    visible: pageRoot.state===""

    Label {
      id: searchName
      //font: theme.tableFont
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
      //font: theme.tableFont
      width: parent.width - searchName.width

      //implicitHeight: theme.basicElementSize * 1.2
      focus: true
      placeholderText: "строка пошуку"
      onTextChanged: {
        sqlModel.setFilterString(searchField.text)
      }
    }
  }

  Column {
    id : editor
    visible : pageRoot.state==="editor"
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


