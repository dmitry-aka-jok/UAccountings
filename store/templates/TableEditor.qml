import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import UA.SQL
import UA.Datapipe

GridLayout{
   id: editorGrid
   width: parent.width
   columns: 2
   /*inputFormFields*/

   Button {
      id: btCancel
      text: qsTr("Відміна")
      onClicked: {
          tableRoot.state = ""
      }
   }

   Button {
      id: btOk
      text: qsTr("Ok")
      onClicked: {
        let saveFields = new Map()

        /*saveFormFields*/

        console.log(saveFields)
        if(saveFields.size > 0)
          console.log("yep")

        tableRoot.state = ""
      }
   }

   onVisibleChanged: {
        if(editorGrid.visible){
           let editorModel = SQLCore.table(tableName).select({fields:selFields, joins:selJoins, where:tableRoot.editStatement});
           beforeEdit = [];
           for(let i=0; i<fields.length; i++)
              beforeEdit[i] = editorModel.value(0,i,-1);
           /*fillFormFields*/
        }
   }
}
