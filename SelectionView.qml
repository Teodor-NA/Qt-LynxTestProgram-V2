import QtQuick 2.12
import QtQuick.Controls 2.12
import lynxlib 1.0
import "HelperFunctions.js" as HF

Item {

    LynxId
    {
        id: structId
        structIndex: -1
        variableIndex: -1
        property bool valid: (structIndex >= 0)
    }

    Row
    {
        spacing: 20
        padding: 20
        anchors.fill: parent

        ScrollView
        {
            height: parent.height - parent.padding*2
            width: HF.evenWidthSpacing(parent)
            clip: true

            ListView
            {
                id: deviceListview
                anchors.fill: parent
                spacing: 10
                model: deviceModel
                delegate: deviceDelegate
                onCurrentIndexChanged:
                {
                    backEnd.deviceInfoIndex = currentIndex
                    if (currentIndex >= 0)
                        backEnd.selectDevice()
                }
                Component.onCompleted:
                {
                    deviceModel.clear()
                    currentIndex = -1
                }
            }
        }

        Column
        {
            height: parent.height - parent.padding*2
            width: HF.evenWidthSpacing(parent)

            ScrollView
            {
                height: parent.height - structButtonsRow.height
                width: parent.width
                clip: true

                ListView
                {
                    id: structListView
                    anchors.fill: parent
                    spacing: 10
                    model: structModel
                    delegate: structDelegate
                    onCurrentIndexChanged:
                    {
                        backEnd.structInfoIndex = currentIndex
                        if (currentIndex >= 0)
                            backEnd.selectStruct()
                    }
                    Component.onCompleted:
                    {
                        structModel.clear()
                        currentIndex = -1
                    }
                }
            }

            Row
            {
                id: structButtonsRow
                height: 30

                Button
                {
                    id: startPeriadic
                }
            }
        }

        ScrollView
        {
            height: parent.height - parent.padding*2
            width: HF.evenWidthSpacing(parent)
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
    }

    ListModel
    {
        id: deviceModel

        ListElement
        {
            deviceNameIn: "No selection"
            deviceIdIn: "No selection"
            structCountIn: "No selection"
            lynxVersionIn: "No selection"
        }

    }

    ListModel
    {
        id: structModel

        ListElement
        {
            structNameIn: "No selection"
            structIdIn: "No selection"
            variableCountIn: "No selection"
        }

    }

    ListModel
    {
        id: variableModel

        ListElement
        {
            checkedOut: false
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
        id: deviceDelegate

        DeviceInfo
        {
            deviceName: deviceNameIn
            deviceId: deviceIdIn
            structCount: structCountIn
            lynxVersion: lynxVersionIn
            width: deviceListview.width
            highlight: (deviceListview.currentIndex == index)
            hovered: deviceMouseArea.containsMouse

            MouseArea
            {
                id: deviceMouseArea
                anchors.fill: parent
                onClicked: deviceListview.currentIndex = index
                hoverEnabled: true
            }
        }
    }

    Component
    {
        id: structDelegate

        StructInfo
        {
            structName: structNameIn
            structId: structIdIn
            variableCount: variableCountIn
            width: structListView.width
            highlight: (structListView.currentIndex == index)
            hovered: structMouseArea.containsMouse

            MouseArea
            {
                id: structMouseArea
                anchors.fill: parent
                onClicked: structListView.currentIndex = index
                hoverEnabled: true
            }
        }
    }

    Component
    {
        id: variableDelegate

        VariableInfo
        {
            variableName: variableNameIn
            variableIndex: variableIndexIn
            structIndex: structId.structIndex
            variableType: variableTypeIn
            variableValue: variableValueIn
            enableInput: enableInputIn
            checkedInput: checkedIn
            enableCheckbox: structId.valid
            onCheckedChanged: checkedOut = checkedIn = checked
        }
    }

    function clearDevices()
    {
        deviceModel.clear()
        deviceListView.currentIndex = -1
    }

    function addDevice(description, id, version, structCount)
    {
        deviceModel.append
            (
                {
                    deviceNameIn: description,
                    deviceIdIn: id,
                    structCountIn: structCount,
                    lynxVersionIn: version
                }
            )
    }

    function clearStructs()
    {
        structModel.clear()
        structListView.currentIndex = -1
    }

    function addStruct(structName, structId, variableCount)
    {
        structModel.append
        (
            {
                structNameIn: structName,
                structIdIn: structId,
                variableCountIn: variableCount
            }
        )
    }

    function addStructIndex(structIndex)
    {
        structId.structIndex = structIndex
    }

    function clearVariableList()
    {
        variableModel.clear()
    }

    function addVariable(variableName, variableIndex, variableType, variableValue, enableInput, checked)
    {
       // console.log("variableIndex: " + variableIndex + ", variableName: " + variableName + ", checked: " + checked)

        variableModel.append(
            {
                variableNameIn: variableName,
                variableIndexIn: variableIndex,
                variableTypeIn: variableType,
                variableValueIn: variableValue,
                enableInputIn: enableInput,
                checkedIn: checked
            }
        )
    }

    function changeVariableValue(lynxId, value)
    {
        if (lynxId.structIndex !== structLynxId.structIndex)
        {
            console.log("The struct index: " + lynxId.structIndex + " is not the same as the open struct index: " + structLynxId.structIndex)
            return;
        }

        if ((lynxId.variableIndex < 0) || (lynxId.variableIndex >= variableModel.count))
        {
            console.log("Variable index out of bounds")
            return
        }

        // variableModel.setProperty(variableIndex, "variableValueIn", value)
        variableModel.get(lynxId.variableIndex).variableValueIn = value
    }
}
