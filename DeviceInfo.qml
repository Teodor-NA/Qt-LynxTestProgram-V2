import QtQuick 2.12
import QtQuick.Controls 2.12
import "HelperFunctions.js" as HF


MyFrame {
    id: myFrame
    property string deviceName: "Not set"
    property string deviceId: "Not set"
    property string structCount: "Not set"
    property string lynxVersion: "Not set"
    property var fontPixelSize: width/30
    property bool selected: false
    property bool hovered: false

    height: column.height
    color: selected ? Qt.darker("white", 1.15) : (hovered ? Qt.darker("white", 1.05) : "white")

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
                descripition: "Device name:"
                text: deviceName
                width: HF.evenWidthSpacing(parent)
                fontPixelSize: myFrame.fontPixelSize
            }

            DescriptionText
            {
                descripition: "Device ID:"
                text: deviceId
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
                descripition: "Struct count:"
                text: structCount
                width: HF.evenWidthSpacing(parent)
                fontPixelSize: myFrame.fontPixelSize
            }

            DescriptionText
            {
                descripition: "Lynx version:"
                text: lynxVersion
                width: HF.evenWidthSpacing(parent)
                fontPixelSize: myFrame.fontPixelSize
            }
        }
    }
}
