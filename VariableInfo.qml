import QtQuick 2.12
import QtQuick.Controls 2.12

MyFrame
{
    id: myFrame
    property bool checked: checkBox.checked
    property string variableName: "Not set"
    property var structIndex: -1
    property var variableIndex: -1
    property string variableType: "Not Set"
    property string variableValue: "Not set"
    property bool enableInput: false
    // onCheckedChanged: console.log("checked: " + checked)
    onVariableValueChanged: textInput.text = variableValue

    implicitHeight: 120
    implicitWidth: 400

    Row
    {
        anchors.fill: parent
        spacing: 10

        CheckBox
        {
            id: checkBox
            anchors.verticalCenter: parent.verticalCenter
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
                        text: "Not set"
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
                            // text = variableValue
                        }
                        // onTextChanged: variableValue = text
                    }
                }
            }
        }
    }
}
