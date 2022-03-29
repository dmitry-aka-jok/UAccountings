import QtQml
import QtQml.Models

import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts

import Qt.labs.qmlmodels

import UA.SQL


import "../delegates"

Page {
    id: pageRoot
    title: "Товари"
    property var sqlModel;

    property var fieldsNames : [];
    property var fieldsWidths : [];
    property var fieldsTypes : [];

    property int sortField : -1;
    property int sortFieldOrder : 0;


    Component.onCompleted: {
        fieldsNames = ["Код", "Назва", "Група"];

        sqlModel = SQL.table("goods").select({fields:["id", "name", "goodsgroups.name"], joins:"goodsgroups", order:"id"});

        let types = sqlModel.typesList();

        let sqlSettings = SQL.table("settings").select({fields:"value", where:{"section":"goods", "option":"columnsWidth", "user_id":datapipe.variable("user_id",-1)}});
        let vvv = sqlSettings.value(0,0,"").split(",");

        for (let column = 0; column < fieldsNames.length; column++) {
            if(typeof vvv[column] !== 'undefined')
                fieldsWidths[column] = vvv[column];
            else
                fieldsWidths[column] = theme.tableMinimalWidth;

            fieldsTypes[column] = types[column]
        }

    }

    function saveWidths() {
        let sss = "";
        for (let column = 0; column < fieldsNames.length; column++) {
            sss += (column==0?"":",")+fieldsWidths[column];
        }

        SQL.table("settings").insert_or_update({"value":sss}, {"section":"goods", "option":"columnsWidth", "user_id":datapipe.variable("user_id",-1)})
    }


    HorizontalHeaderView {
        id: tableHorizontalHeader
        syncView: table
        anchors.left: table.left

        delegate:
            HeaderDelegate {
            id: header
            text: fieldsNames[column]
            implicitHeight: theme.basicElementSize

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
                            var newWidth = header.width + mouseX
                            if (newWidth >= theme.tableMinimalWidth) {
                                fieldsWidths[column] = newWidth
                                table.forceLayout()
                            }
                        }
                    }
                    onReleased: {
                        //                        datapipe.setVariable("debugQueries", true)
                        saveWidths();
                        //                        datapipe.setVariable("debugQueries", false)
                    }
                }
            }

                Image {
                    id: sortHandle

                    source: (sortField!==column||sortFieldOrder===0?"qrc:/icons/fi-bs-arrows-h-copy.svg":(sortFieldOrder===1?"qrc:/icons/fi-bs-arrow-down.svg":"qrc:/icons/fi-bs-arrow-up.svg"))

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
                          if (sortFieldOrder===2){
                             sortFieldOrder = 0
                          }else{
                             sortFieldOrder += 1

                          }
                        }
                        else {
                            sortField = column
                            sortFieldOrder = 1;
                        }

                        if (sortFieldOrder==0)
                            sqlModel.setSort(-1, Qt.AscendingOrder)
                        if (sortFieldOrder==1)
                            sqlModel.setSort(sortField, Qt.AscendingOrder)
                        if (sortFieldOrder==2)
                            sqlModel.setSort(sortField, Qt.DescendingOrder)

                    }
                }
            }
        }
    }



    TableView {
        id: table
        anchors.fill: parent
        anchors.topMargin: tableHorizontalHeader.height
        z:0
        reuseItems: true
        clip: true
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
                    implicitHeight: theme.basicElementSize
                    text: model["Display"]
                    //color: index % 2 == 0 ? "white" : "#C0C0C0"
                    highlighted: row === table.currentRow
                    onClicked: {
                        table.currentRow = row
                    }
                }
            }
            DelegateChoice {
                roleValue: "int"

                ItemDelegate {
                    implicitHeight: theme.basicElementSize
                    text: model["Display"] //+ " (int)"
                    //color: index % 2 == 0 ? "white" : "#C0C0C0"
                    highlighted: row === table.currentRow
                    onClicked: {
                        table.currentRow = row
                    }
                }
            }


        }

        ScrollIndicator.vertical: ScrollIndicator { }
    }

    Keys.onPressed:
        (event)=> {
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

    TextField {
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        id: searchField
        height: 40
        width: parent.width
        focus: true
        placeholderText: "Enter search text"
        onTextChanged: {
            sqlModel.setFilterString(searchField.text)
        }
        Component.onCompleted: {
            sqlModel.setFilterColumn(1)
        }

    }
}


