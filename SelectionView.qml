import QtQuick 2.12
import QtQuick.Controls 2.12

Item {
    property string deviceDescription: "No selection"
    property string deviceId: "No selection"
    property string version: "No selection"
    property string structCount: "No selection"

    property string structDescription: "No selection"
    property string structId: "No selection"
    property string variableCount: "No selection"

    function clearDevices()
    {
        deviceModel.clear()
        deviceModel.append({text: "No selection"})
        deviceComboBox.currentIndex = 0
    }

    function clearDeviceInfo()
    {
        deviceDescription = "No selection"
        deviceId = "No selection"
        version = "No selection"
        structCount = "No selection"
    }

    function addDevice(deviceText)
    {
        deviceModel.append({text: deviceText})
    }

    function clearStructs()
    {
        structModel.clear()
        structModel.append({text: "No selection" })
        structComboBox.currentIndex = 0
    }

    function clearStructInfo()
    {
        structDescription = "No selection"
        structId = "No selection"
        variableCount = "No selection"
    }

    function addStruct(structText)
    {
        structModel.append({text: structText})
    }

    function addStructIndex(structIndex)
    {
        structInfo.structIndex = structIndex
    }

    function clearVariableList()
    {
        variableModel.clear()
    }

    function addVariable(variableName, variableIndex, variableType, variableValue, enableInput)
    {
        variableModel.append(
            {
                variableNameIn: variableName,
                variableIndexIn: variableIndex,
                variableTypeIn: variableType,
                variableValueIn: variableValue,
                enableInputIn: enableInput
            }
        )
    }

    function changeVariableValue(structIndex, variableIndex, value)
    {
        if (structIndex !== structInfo.structIndex)
        {
            // The struct is not currently open
            return;
        }

        if ((variableIndex < 0) || (variableIndex >= variableModel.count))
        {
            console.log("Variable index out of bounds")
            return
        }

        variableModel.setProperty(variableIndex, "variableValueIn", value)
    }

    Row
    {
        spacing: 20
        padding: 20
        anchors.fill: parent

        Column
        {
            spacing: 10
            width: 400

            ComboBox
            {
                id: deviceComboBox
                width: parent.width
                height: 50
                font.pixelSize: 15
                model:
                    ListModel
                    {
                        id: deviceModel
                        ListElement{ text: "No selection" }
                    }
                ToolTip.delay: 500
                ToolTip.timeout: 3000
                ToolTip.visible: hovered
                ToolTip.text: qsTr("Select device")
                onCurrentIndexChanged:
                {
                    if (currentIndex > 0)
                        backEnd.selectDevice(currentIndex - 1)
                    else
                    {
                        clearDeviceInfo()
                        clearStructs()
                        // structComboBox.currentIndex = 0
                    }
                }
            }

            MyFrame
            {
                width: parent.width
                height: 120

                Column
                {
                    anchors.fill: parent
                    padding: 20

                    DescriptionText
                    {
                        descripition: "Device name:"
                        text: deviceDescription
                        width: parent.width
                    }

                    DescriptionText
                    {
                        descripition: "Device id:"
                        text: deviceId
                        // height: 60
                        width: parent.width
                    }

                    DescriptionText
                    {
                        descripition: "Lynx version:"
                        text: version
                        // height: 60
                        width: parent.width
                    }

                    DescriptionText
                    {
                        descripition: "Number of structs:"
                        text: structCount
                        // height: 60
                        width: parent.width
                    }
                }
            }

            ComboBox
            {
                id: structComboBox
                width: parent.width
                height: 50
                font.pixelSize: 15
                model:
                    ListModel
                    {
                        id: structModel
                        ListElement{ text: "No selection" }
                    }
                ToolTip.delay: 500
                ToolTip.timeout: 3000
                ToolTip.visible: hovered
                ToolTip.text: qsTr("Select struct")
                onCurrentIndexChanged:
                {
                    if (currentIndex > 0)
                        backEnd.selectStruct(currentIndex - 1)
                    else
                    {
                        clearStructInfo()
                        clearVariableList()
                    }
                }
            }

            MyFrame
            {
                width: parent.width
                height: 100

                Column
                {
                    id: structInfo

                    property var structIndex: -1

                    anchors.fill: parent
                    padding: 20

                    DescriptionText
                    {
                        descripition: "Struct name:"
                        text: structDescription
                        width: parent.width
                    }

                    DescriptionText
                    {
                        descripition: "Struct id:"
                        text: structId
                        // height: 60
                        width: parent.width
                    }

                    DescriptionText
                    {
                        descripition: "Number of variables:"
                        text: variableCount
                        // height: 60
                        width: parent.width
                    }
                }
            }

            Row
            {
                spacing: 10

                Button
                {
                    text: "Add struct"
                    font.pixelSize: 15
                    enabled: (structComboBox.currentIndex > 0) && (structInfo.structIndex < 0)
                    onClicked: backEnd.generateStruct()
                }

                Button
                {
                    text: "Pull Struct"
                    font.pixelSize: 15
                    enabled: (structInfo.structIndex >= 0)
                    onClicked: backEnd.pullStruct(structInfo.structIndex)
                }
            }
        }


        ScrollView
        {
            height: parent.height - parent.padding*2
            width: 430
            clip: true

            ListView
            {
                id: variableListview
                spacing: 10
                model: variableModel
                delegate: variableDelegate
                // Component.onCompleted: variableModel.append({})

            }
        }

//        Column
//        {
//            spacing: 10

//            VariableInfo
//            {

//            }

//        }
    }

    ListModel
    {
        id: variableModel

        ListElement
        {
            checkedIn: false
            variableNameIn: "No selection"
            variableIndexIn: -1
            variableTypeIn: "No Selection"
            variableValueIn: "No selection"
            enableInputIn: false
        }

    }

    Component
    {
        id: variableDelegate

        VariableInfo
        {
            checked: checkedIn
            variableName: variableNameIn
            variableIndex: variableIndexIn
            structIndex: structInfo.structIndex
            variableType: variableTypeIn
            variableValue: variableValueIn
            enableInput: enableInputIn
        }
    }
}
