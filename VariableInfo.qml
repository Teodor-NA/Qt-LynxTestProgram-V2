import QtQuick 2.12
import QtQuick.Controls 2.12
import lynxlib 1.0
import "HelperFunctions.js" as HF


MyFrame {
    id: myFrame
    property var structIndex: -1
    property var variableIndex: -1
    property bool valid: ((variableId.structIndex >= 0) && (variableId.variableIndex >= 0))
    property bool enableSelection: (dataType != Lynx.E_String)
    property bool enableInput: false
    property string variableName: "Not set"
    property string variableType: "Not set"
    property var dataType: Lynx.E_NotInit
    // property string variableValue: valueInput.text
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

    onValidChanged:
    {
        if (valid)
        {
            dataType = lynx.getDataType(variableId)
            valueInput.text = lynx.getValueAsString(variableId)
        }
    }

    height: column.height
    color: selected ? Qt.darker("white", 1.4) : (hovered ? Qt.darker("white", 1.05) : Qt.darker("white", 1.1))

    Connections
    {
        target: lynx
        onNewDataReceived:
        {
            if (lynxId.structIndex === variableId.structIndex)
                if ((lynxId.variableIndex < 0) || (lynxId.variableIndex === variableId.variableIndex))
                    valueInput.text = lynx.getValueAsString(variableId)

//            if (valueInput.activeFocus && (valueInput.preeditText == valueInput.text))
//                valueInput.selectAll()
        }
    }

    LynxId
    {
        id: variableId
        structIndex: myFrame.structIndex
        variableIndex: myFrame.variableIndex
        // property bool valid: ((structIndex >= 0) && (variableIndex >= 0))
    }

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
                descripition: "Name:"
                text: variableName
                width: HF.evenWidthSpacing(parent)
                labelRatio: 0.25
                fontPixelSize: myFrame.fontPixelSize
            }

            DescriptionText
            {
                descripition: "Type:"
                text: variableType
                width: HF.evenWidthSpacing(parent)
                labelRatio: 0.25
                fontPixelSize: myFrame.fontPixelSize
            }
        }

        Row
        {
            height: valueFrame.height
            width: parent.width
            spacing: 10

            Label
            {
                id: valueLabel
                anchors.verticalCenter: parent.verticalCenter
                text: "Value:"
                font.bold: true
                font.pixelSize: myFrame.fontPixelSize
            }

            MyFrame
            {
                id: valueFrame
                width: parent.width - parent.spacing - valueLabel.width - border.width*10
                height: valueInput.height*1.1
                border.color: (myFrame.enableInput && myFrame.valid) ? "black" : "transparent"
//                visible: (myFrame.dataType != Lynx.E_Bool)

                TextInput
                {
                    id: valueInput
                    height: myFrame.fontPixelSize*2
                    width: parent.width
                    anchors.verticalCenter: valueFrame.verticalCenter
                    text: lynx.getValueAsString(variableId)
                    font.pixelSize: myFrame.fontPixelSize
                    onActiveFocusChanged: if (activeFocus) selectAll()
                    horizontalAlignment: TextInput.AlignHCenter
                    verticalAlignment: TextInput.AlignVCenter
                    enabled: (myFrame.enableInput && myFrame.valid) // && (myFrame.dataType != Lynx.E_Bool))
                    onAccepted: lynx.sendVariable(variableId, text)

                    MouseArea
                    {
                        anchors.fill: parent
                        visible: (myFrame.dataType == Lynx.E_Bool)
                        onClicked:
                        {
                            if (lynx.getValueAsBool(variableId))
                            {
                                valueInput.text = "False";
                                lynx.sendVariable(variableId, "False")
                            }
                            else
                            {
                                valueInput.text = "True";
                                lynx.sendVariable(variableId, "True")
                            }
                        }
                    }
                }
            }
        }
    }
}
