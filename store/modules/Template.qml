import QtQml
import QtQml.Models

import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts

Page {
    id: pageRoot
    title: qsTr("В разработке")



}

/*
  TreeModel {
    id: treemodel
    roles: ["phrase", "phrase2"]
    TreeElement{
      property string phrase: "Hey"
      property string phrase2: "Down"
      TreeElement{
        property string phrase: "What's"
        property string phrase2: "Down"
        TreeElement{
          property string phrase: "Up?"
          property string phrase2: "Down"
        }
      }
    }
  }

  TreeView {
    anchors.fill: parent
    model: treemodel
    delegate: Item {
      id: root

      implicitWidth: padding + label.x + label.implicitWidth + padding
      implicitHeight: label.implicitHeight * 1.5

      readonly property real indent: 20
      readonly property real padding: 5

      // Assigned to by TreeView:
      required property TreeView treeView
      required property bool isTreeNode
      required property bool expanded
      required property int hasChildren
      required property int depth

      TapHandler {
        onTapped: treeView.toggleExpanded(row)
      }

      Label {
        id: indicator
        visible: root.isTreeNode && root.hasChildren
        x: padding + (root.depth * root.indent)
        text: root.expanded ? "▼ " : "▶ "
      }

      Label {
        id: label
        x: padding + (root.isTreeNode ? (root.depth + 1) * root.indent : 0)
        width: root.width - root.padding - x
        clip: true
        text: model["phrase"] + "  " + model["phrase2"]

      }
    }
  }
*/
