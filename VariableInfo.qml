import QtQuick 2.12
import QtQuick.Controls 2.12

MyFrame
{
    id: myFrame
    property bool checked: checkBox.checked
    property bool enableCheckbox: false
    property string variableName: "Not set"
    property var structIndex: -1
    property var variableIndex: -1
    property string variableType: "Not Set"
    property string variableValue: textInput.text
    property bool enableInput: false
    property bool checkedInput: false

    implicitHeight: 120
    implicitWidth: 400

    Row
    {
        anchors.fill: parent
        spacing: 10

        CheckBox
        {
            id: checkBox
            checked: myFrame.checkedInput
            anchors.verticalCenter: parent.verticalCenter
            visible: ((variableType !== "Not set") && (variableType !== "String")) && enableCheckbox
            onCheckedChanged:
            {
                scopeServer.changePlotItem(structIndex, variableIndex, checked)
            }
        }

        Rectangle
        {
            width: checkBox.width
            height: checkBox.height
            anchors.verticalCenter: parent.verticalCenter
            visible: !checkBox.visible
            color: "transparent"
        }

        Column
        {
            // spacing: 5
            anchors.verticalCenter: parent.verticalCenter

            DescriptionText
            {
                descripition: "Variable name:"
                text: variableName
            }

            DescriptionText
            {
                descripition: "Variable index:"
                text: variableIndex
            }

            DescriptionText
            {
                descripition: "Variable type:"
                text: variableType
            }

            Row
            {
                spacing: 10
                Label
                {
                    width: 150
                    text: "Value:"
                    font.bold: true
                    font.pixelSize: 15
                    anchors.verticalCenter: parent.verticalCenter
                }

                MyFrame
                {
                    height: 40
                    width: 150
                    radius: 10
                    border.color: enableInput ? "black" : "transparent"

                    TextInput
                    {
                        id: textInput
                        padding: 10
                        width: parent.width - 2*padding
                        anchors.verticalCenter: parent.verticalCenter
                        text: myFrame.variableValue
                        font.pixelSize: 15
                        enabled: enableInput
                        onActiveFocusChanged:
                        {
                            if(activeFocus)
                                selectAll()
                        }
                        onAccepted:
                        {
                            backEnd.sendVariable(myFrame.structIndex, myFrame.variableIndex, text)
                        }
                        // onTextChanged: variableValue = text
                    }
                }
            }
        }
    }
}
