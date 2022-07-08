import QtQml
import QtQml.Models

import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts


import "init/init.js" as Init
import "modules"

import UA.SQL
import UA.Datapipe


ApplicationWindow {
  id: main
  width: 1080
  height: 768
  title: qsTr("UAccounting")
  visible: true

  Component.onCompleted: {
    Init.initUA()

    const mstr = Datapipe.jsonMenu();

    //console.log(mstr)
    const newMenu = Qt.createQmlObject(mstr, main)
    main.menuBar = newMenu
  }


  StackView {
    id: windowManager
    anchors.fill: parent

    focus: true
    Keys.onBackPressed: {
      event.accepted = true

      if (windowManager.depth > 1) {
        windowManager.pop()
      } else {
        windowManager.open()
      }
    }
  }
}
