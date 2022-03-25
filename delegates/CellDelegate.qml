import QtQuick
import Qt.labs.qmlmodels

import UA.Cell as Cell

DelegateChooser {
//    id: chooser
//    role: "cellEditor"
    DelegateChoice {
        roleValue: Cell.Property.ListEditor
        CellList {
            value: model.cellData
            type: model.cellType
            options: model.cellOptions
        }
    }
    DelegateChoice {
        roleValue: Cell.Property.ColorEditor
        CellColor {
            value: model.cellData
            type: model.cellType
            options: model.cellOptions
        }
    }
    DelegateChoice {
        roleValue: Cell.Property.TextEditor
        CellText {
            value: model.cellData
            type: model.cellType
            options: model.cellOptions
        }
    }
    DelegateChoice {
        roleValue: Cell.Property.BoolEditor
        CellBool {
            value: model.cellData
            type: model.cellType
            options: model.cellOptions
        }
    }
    DelegateChoice {
        roleValue: Cell.Property.NumberEditor
        CellNumber {
            value: model.cellData
            type: model.cellType
            options: model.cellOptions
        }
    }
}
