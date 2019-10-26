import QtQuick 2.12
import QtQuick.Controls 2.12
import "HelperFunctions.js" as HF


MyFrame {
    id: myFrame
    property string structName: "Not set"
    property string structId: "Not set"
    property string variableCount: "Not set"
    property var dynFontPixelSize: width/30
    property var fontPixelSize: 10
    property var maxFontPixelSize: 15
    property var minFontPixelSize: 10
    property bool selected: false
    property bool hovered: false

    onDynFontPixelSizeChanged:
    {
        if (dynFontPixelSize > maxFontPixelSize)
            fontPixelSize = maxFontPixelSize
        else if (dynFontPixelSize < minFontPixelSize)
            fontPixelSize = minFontPixelSize
        else
            fontPixelSize = dynFontPixelSize
    }

    height: column.height
//    color: selected ? Qt.darker("white", 1.15) : (hovered ? Qt.darker("white", 1.05) : "white")
    color: selected ? Qt.darker("white", 1.4) : (hovered ? Qt.darker("white", 1.05) : Qt.darker("white", 1.1))

    Column
    {
        id: column
        spacing: 10
        anchors.centerIn: parent
        width: parent.width
        height: HF.childrenForcedHeight(column)
        padding: 10

        Row
        {
            id: firstRow
            height: children[0].height
            width: parent.width

            DescriptionText
            {
                descripition: "Struct name:"
                text: structName
                width: HF.starWidthSpacing(parent, 0.65)
                labelRatio: 0.4
                fontPixelSize: myFrame.fontPixelSize
            }

            DescriptionText
            {
                descripition: "Struct ID:"
                text: structId
                width: HF.starWidthSpacing(parent, 0.35)
                fontPixelSize: myFrame.fontPixelSize
            }
        }

        Row
        {
            height: children[0].height
            width: parent.width
            id: secondRow

            DescriptionText
            {
                descripition: "Variable count:"
                text: variableCount
                width: HF.starWidthSpacing(parent, 0.65)
                labelRatio: 0.4
                fontPixelSize: myFrame.fontPixelSize
            }

            Rectangle
            {
                width:  HF.starWidthSpacing(parent, 0.35)
            }

//            DescriptionText
//            {
//                descripition: "Lynx version:"
//                text: lynxVersion
//                width: HF.evenWidthSpacing(parent)
//                fontPixelSize: myFrame.fontPixelSize
//            }
        }
    }
}
