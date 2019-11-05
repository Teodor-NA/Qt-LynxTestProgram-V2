import QtQuick 2.12
import QtQuick.Controls 2.12
import "HelperFunctions.js" as HF


MyFrame {
    id: myFrame
    property var portIndex: -1
    property string deviceName: "Not set"
    property string deviceId: "Not set"
    property string structCount: "Not set"
    property string lynxVersion: "Not set"
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

//    Label
//    {
//        padding: 5
//        anchors.top: parent.top
//        anchors.left: parent.left
//        text: "Port index: " + portIndex
//        font.pixelSize: fontPixelSize/2
//        font.bold: true
//    }

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

            Row
            {
                width: HF.evenWidthSpacing(parent)
                height: children[1].height

                Label
                {
                    width: HF.evenWidthSpacing(parent)
                    text: "Device ID:"
                    font.pixelSize: myFrame.fontPixelSize
                    font.bold: true
                }

                TextInput
                {
                    width: HF.evenWidthSpacing(parent)
                    text: deviceId
                    font.pixelSize: myFrame.fontPixelSize
                    onAccepted:
                    {
                        console.log(typeof(portIndex))
                        console.log(typeof(deviceId))
                        console.log(typeof(text))
                        lynxUart.changeRemoteDeviceId(portIndex, qsTr(deviceId), qsTr(text))
                        focus = false
                    }
                    onActiveFocusChanged:
                    {
                        if (activeFocus)
                            selectAll()
                    }
                }
            }

//            DescriptionText
//            {
//                descripition: "Device ID:"
//                text: deviceId
//                width: HF.evenWidthSpacing(parent)
//                fontPixelSize: myFrame.fontPixelSize
//            }
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
