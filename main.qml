import QtQml
import QtQml.Models

import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts


import "scripts/init.js" as Init

import UA.SQL


ApplicationWindow {
    id: main
    width: 1080
    height: 768
    title: qsTr("UAccounting")
    visible: true

    Component.onCompleted: {
        Init.database()
        //console.log("Test")
    }


    QtObject {
        id: theme
        property color primaryColor: Material.primary
        property color primaryTextColor: Material.foreground
        property color accentColor: Material.accentColor

        property color backgroundColor: Material.backgroundColor
        property color highlightColor : Material.listHighlightColor


        property int basicElementSize: 30

        property int margins: 10

        property int tableMinimalWidth: 50
        property int tableDefaultWidthNumber: 50
        property int tableDefaultWidthString: 400
        property int tableDefaultWidthBool: 50

        property font headerFont: Qt.font({family: 'Encode Sans', weight: Font.Black, italic: false, pointSize: 16})
        property font tableFont: Qt.font({family: 'Encode Sans', italic: false, pointSize: 16 })
        property font textFont: Qt.font({family: 'Encode Sans', italic: false, pointSize: 16 })
        property font menuFont: Qt.font({family: 'Encode Sans', italic: false, pointSize: 16 })
    }



    ListModel {
        id: menuList
        ListElement {pageTitle: "Товары";   pageFunc: function() { mainPageTitle.text = "Товары"; stackView.clear();  stackView.push("modules/Tests.qml"); }}
        ListElement {pageTitle: "Клиенты";   pageFunc: function() { mainPageTitle.text = "Контрагенты"; stackView.clear();  stackView.push("modules/Template.qml"); }}
        ListElement {separator: true}
        ListElement {pageTitle: "Приход";  pageFunc: function() { mainPageTitle.text = "Приход"; stackView.clear();  stackView.push("modules/Template.qml"); } }
        ListElement {pageTitle: "Оплата поставщику";   pageFunc: function() { stackView.clear();  stackView.push("modules/Template.qml"); }}
        ListElement {pageTitle: "Возврат поставщику";   pageFunc: function() { stackView.clear();  stackView.push("modules/Template.qml"); }}
        ListElement {pageTitle: "Продажа";   pageFunc: function() { stackView.clear();  stackView.push("modules/Template.qml"); }}
        ListElement {pageTitle: "Возврат продажи";   pageFunc: function() { stackView.clear();  stackView.push("modules/Template.qml"); }}
        ListElement {pageTitle: "Инвентаризация";   pageFunc: function() { stackView.clear();  stackView.push("modules/Template.qml"); }}
        ListElement {separator: true}
        ListElement {pageTitle: "Остатки товаров";   pageFunc: function() { stackView.clear();  stackView.push("modules/Template.qml"); }}
        ListElement {pageTitle: "Расчеты с поставщиками";   pageFunc: function() { stackView.clear();  stackView.push("modules/Template.qml"); }}
        ListElement {pageTitle: "Расчеты с покупателями";   pageFunc: function() { stackView.clear();  stackView.push("modules/Template.qml"); }}
        ListElement {pageTitle: "Доходы";   pageFunc: function() { stackView.clear();  stackView.push("modules/Template.qml"); }}
        ListElement {separator: true}
        ListElement {pageTitle: "Управление";  pageFunc: function() { mainPageTitle.text = "Настройки"; stackView.clear();  stackView.push("modules/Template.qml"); }}
        ListElement {pageTitle: "Выход"; pageIcon: "qrc:/icons/ic_exit_to_app_48px.svg"; pageFunc: function() { Qt.quit() }}
    }

    header: ToolBar {
        contentHeight: toolButton.implicitHeight
        ToolButton {
            id: toolButton
            icon.source: "qrc:/icons/ic_menu_48px.svg"
            onClicked:
                drawer.open()
        }

        Label {
            id: mainPageTitle
            text: "Hello world!"
            anchors.centerIn: parent
            font: theme.headerFont
        }
    }



    Drawer {
        id: drawer

        implicitHeight: parent.height
        implicitWidth: Math.max (parent.width > parent.height ? 320 : 280,
                                 Math.min (parent.width, parent.height) * 0.70)

        // List model that generates the page selector
        // Options for selector items are:
        //     - spacer: acts an expanding spacer between to items
        //     - pageTitle: the text to display
        //     - separator: if the element shall be a separator item
        //     - separatorText: optional text for the separator item
        //     - pageIcon: the source of the image to display next to the title
        //
        property alias items: listView.model
        property alias index: listView.currentIndex
        onIndexChanged: {
            var isSpacer = false
            var isSeparator = false
            var item = items.get (index)

            if (typeof (item) !== "undefined") {
                if (typeof (item.spacer) !== "undefined")
                    isSpacer = item.spacer

                if (typeof (item.separator) !== "undefined")
                    isSpacer = item.separator

                if (!isSpacer && !isSeparator)
                    items.get (index).pageFunc ()
            }
        }

        ColumnLayout {
            spacing: 0

            anchors.margins: 0
            anchors.fill: parent

            Rectangle {
                z: 1
                height: 120
                id: iconRect
                Layout.fillWidth: true
                color: theme.accentColor

                Image {
                    source: "qrc:/icons/logo.png"
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                }
            }

            ListView {
                z: 0
                id: listView
                currentIndex: -1
                Layout.fillWidth: true
                Layout.fillHeight: true
                Component.onCompleted: currentIndex = 0
                model: menuList

                delegate: DrawerItem {
                    width: parent.width
                    pageSelector: listView
                    model: menuList

                    onClicked: {
                        if (listView.currentIndex !== index)
                            listView.currentIndex = index

                        drawer.close()
                    }
                }

                ScrollIndicator.vertical: ScrollIndicator { }
            }
        }
    }


    StackView {
        id: stackView
        //initialItem: menuList.get(0)["qml"]
        anchors.fill: parent

        focus: true
        Keys.onBackPressed: {
            event.accepted = true

            if (stackView.depth > 1) {
                stackView.pop()
            } else {
                drawer.open()
            }
        }
    }

}
