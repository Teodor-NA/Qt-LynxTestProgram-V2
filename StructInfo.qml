import QtQuick 2.12
import QtQuick.Controls 2.12
import "HelperFunctions.js" as HF


MyFrame {
    id: myFrame
    property string structName: "Not set"
    property string structId: "Not set"
    property string variableCount: "Not set"
    property var fontPixelSize: width/30
    property bool highlight: false
    property bool hovered: false

    height: column.height
    color: highlight ? Qt.darker("white", 1.15) : (hovered ? Qt.darker("white", 1.05) : "white")

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
                width: HF.evenWidthSpacing(parent)
                fontPixelSize: myFrame.fontPixelSize
            }

            DescriptionText
            {
                descripition: "Struct ID:"
                text: structId
                width: HF.evenWidthSpacing(parent)
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
                width: HF.evenWidthSpacing(parent)
                fontPixelSize: myFrame.fontPixelSize
            }

            Rectangle
            {
                width:  HF.evenWidthSpacing(parent)
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
