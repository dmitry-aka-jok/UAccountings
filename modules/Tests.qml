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
                    id: mouseHandle
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
        //            model: sqlModel
        //        }

        delegate:
            ItemDelegate {
            implicitHeight: theme.basicElementSize
            text: model["Field."+column]
            highlighted: row === table.currentRow
            onClicked: {
                table.currentRow = row
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
                //console.log(sqlModel.rowCount())
                if(table.currentRow < sqlModel.rowCount()-1)
                table.currentRow++;
                event.accepted = true;
            }
        }
}


