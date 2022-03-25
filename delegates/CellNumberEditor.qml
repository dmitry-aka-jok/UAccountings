import QtQuick
import QtQuick.Controls
import UA.Cell as Cell

CellTextEditor {
    IntValidator    { id: intValidator }
    DoubleValidator { id: doubleValidator }

    Component.onCompleted:  {
        if (cell.option.hasOwnProperty("min")) {
            console.log("["+cell.options.min+" .. "+cell.options.max+"]")
            /*
            if (cell.type === Cell.MetaType.Int)
                setInt(cell.option.min, cell.option.max)
            if (cell.type === Cell.MetaType.Double)
                setDouble(cell.option.min, cell.option.max)
                */
        }
    }
    function setInt(min,max) {
        intValidator.bottom = min
        intValidator.top = max
        contentItem.validator=intValidator
    }
    function setDouble(min,max) {
        doubleValidator.bottom = min
        doubleValidator.top = max
        doubleValidator.decimals=3
        contentItem.validator = doubleValidator
    }
}
