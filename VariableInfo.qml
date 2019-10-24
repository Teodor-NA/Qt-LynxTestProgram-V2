import QtQuick 2.12
import QtQuick.Controls 2.12
import lynxlib 1.0
import "HelperFunctions.js" as HF


MyFrame {
    id: myFrame
    property var structIndex: -1
    property var variableIndex: -1
    property bool valid: (structIndex >= 0)
    property bool enableSelection: (variableType != "String")
    property bool enableInput: false
    property string variableName: "Not set"
    property string variableType: "Not set"
    property string variableValue: valueInput.text
    property var fontPixelSize: width/30
    property bool selected: false
    property bool hovered: false

    height: column.height
    color: selected ? Qt.darker("white", 1.15) : (hovered ? Qt.darker("white", 1.05) : "white")

    LynxId
    {
        id: variableId
        structIndex: myFrame.structIndex
        variableIndex: myFrame.variableIndex
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
                border.color: myFrame.enableInput ? "black" : "transparent"

                TextInput
                {
                    id: valueInput
                    height: myFrame.fontPixelSize*2
                    width: parent.width
//                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: valueFrame.verticalCenter
                    text: variableValue
                    font.pixelSize: myFrame.fontPixelSize
                    onActiveFocusChanged: if (activeFocus) selectAll()
                    horizontalAlignment: TextInput.AlignHCenter
                    verticalAlignment: TextInput.AlignVCenter
                    enabled: (myFrame.enableInput && myFrame.valid)
                    onAccepted: backEnd.sendVariable(variableId, text)

                }
            }
        }
    }
}



//import QtQuick 2.12
//import QtQuick.Controls 2.12
//import lynxlib 1.0

//MyFrame
//{
//    id: myFrame
//    property bool checked: checkBox.checked
//    property bool enableCheckbox: false
//    property string variableName: "Not set"
//    property var structIndex: -1
//    property var variableIndex: -1
//    property string variableType: "Not Set"
//    property string variableValue: textInput.text
//    property bool enableInput: false
//    property bool checkedInput: false

//    implicitHeight: 120
//    implicitWidth: 400

//    LynxId
//    {
//        id: lynxId
//        structIndex: myFrame.structIndex
//        variableIndex: myFrame.variableIndex
//    }

//    Row
//    {
//        anchors.fill: parent
//        spacing: 10

//        CheckBox
//        {
//            id: checkBox
//            checked: myFrame.checkedInput
//            anchors.verticalCenter: parent.verticalCenter
//            visible: ((variableType !== "Not set") && (variableType !== "String")) && enableCheckbox
//            onCheckedChanged:
//            {
//                scopeServer.changePlotItem(structIndex, variableIndex, checked)
//            }
//        }

//        Rectangle
//        {
//            width: checkBox.width
//            height: checkBox.height
//            anchors.verticalCenter: parent.verticalCenter
//            visible: !checkBox.visible
//            color: "transparent"
//        }

//        Column
//        {
//            // spacing: 5
//            anchors.verticalCenter: parent.verticalCenter

//            DescriptionText
//            {
//                descripition: "Variable name:"
//                text: variableName
//            }

//            DescriptionText
//            {
//                descripition: "Variable index:"
//                text: variableIndex
//            }

//            DescriptionText
//            {
//                descripition: "Variable type:"
//                text: variableType
//            }

//            Row
//            {
//                spacing: 10
//                Label
//                {
//                    width: 150
//                    text: "Value:"
//                    font.bold: true
//                    font.pixelSize: 15
//                    anchors.verticalCenter: parent.verticalCenter
//                }

//                MyFrame
//                {
//                    height: 40
//                    width: 150
//                    radius: 10
//                    border.color: enableInput ? "black" : "transparent"

//                    TextInput
//                    {
//                        id: textInput
//                        padding: 10
//                        width: parent.width - 2*padding
//                        anchors.verticalCenter: parent.verticalCenter
//                        text: myFrame.variableValue
//                        font.pixelSize: 15
//                        enabled: enableInput
//                        onActiveFocusChanged:
//                        {
//                            if(activeFocus)
//                                selectAll()
//                        }
//                        onAccepted:
//                        {
//                            backEnd.sendVariable(myFrame.structIndex, myFrame.variableIndex, text)
//                        }
//                        // onTextChanged: variableValue = text
//                    }
//                }
//            }
//        }
//    }
//}
