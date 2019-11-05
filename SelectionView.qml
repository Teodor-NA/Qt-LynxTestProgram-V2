import QtQuick 2.12
import QtQuick.Controls 2.12
import lynxlib 1.0
import "HelperFunctions.js" as HF

Item {

    Connections
    {
        target: backEnd
        onClearDevices:
        {
            deviceModel.clear()
            deviceListView.currentIndex = -1
        }
        onAddDevice: // selectionView.addDevice(description, id, version, count)
        {
            deviceModel.append
            (
                {
                    deviceNameIn: description,
                    deviceIdIn: id,
                    structCountIn: count,
                    lynxVersionIn: version,
                    portIndexIn: portIndex
                }
            )
        }
        onClearStructList: // selectionView.clearStructs()
        {
            structModel.clear()
            structListView.currentIndex = -1
        }
        onAddStruct: // selectionView.addStruct(structName, structId, variableCount)
        {
            structModel.append
            (
                {
                    portIndexIn: portIndex,
                    structNameIn: structName,
                    structIdIn: structId,
                    variableCountIn: variableCount
                }
            )
        }
        onAddStructIndex: // selectionView.addStructIndex(structIndex)
        {
            structId.structIndex = structIndex
        }
        onClearVariableList: variableModel.clear() // selectionView.clearVariableList()
        onAddVariable: // selectionView.addVariable(variableName, variableIndex, variableType, variableValue, enableInput, checked)
        {
            variableModel.append
            (
                {
                    portIndexIn: portIndex,
                    variableNameIn: variableName,
                    variableIndexIn: variableIndex,
                    variableTypeIn: variableType,
                    // variableValueIn: variableValue,
                    enableInputIn: enableInput,
                    selectedIn: checked
                }
            )
        }
        onChangeDeviceId:
        {
            // deviceModel.get(backEnd.deviceInfoIndex).deviceIdIn = qsTr(newDeviceId)
            deviceModel.setProperty(backEnd.deviceInfoIndex, "deviceIdIn", newDeviceId)
        }
    }

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
                    {
                        backEnd.selectDevice(currentItem.portIndex)
                        structListView.currentIndex = -1
                    }
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
                height: parent.height - structButtonsRow.height - periodicButtonsRow.height
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
                            backEnd.selectStruct(currentItem.portIndex)
                        else
                            variableModel.clear()
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
                height: 50
                width: parent.width
                spacing: 10

                Button
                {
                    width: HF.evenWidthSpacing(parent)
                    text: "Add struct"
                    font.pixelSize: 15
                    enabled: (structListView.currentIndex >= 0) && !structId.valid
                    onClicked: backEnd.generateStruct()
                }

                Button
                {
                    width: HF.evenWidthSpacing(parent)
                    text: "Pull struct"
                    font.pixelSize: 15
                    enabled: structId.valid
                    onClicked: lynxUart.pullStruct(structListView.currentItem.portIndex, structId)
                }
            }

            Row
            {
                id: periodicButtonsRow
                height: 50
                width: parent.width
                spacing: 10

                TextField
                {
                    id: periodicInput
                    width: HF.evenWidthSpacing(parent)
                    placeholderText: "Interval [ms]"
                    validator: IntValidator{}
                    font.pixelSize: 15
                    enabled: structId.valid
                    onAccepted: { lynxUart.startPeriodic(structListView.currentItem.portIndex, text, structId); text = "" }
                }

                Button
                {
                    width: HF.evenWidthSpacing(parent)
                    text: "Start"
                    font.pixelSize: 15
                    enabled: structId.valid
                    onClicked: { lynxUart.startPeriodic(structListView.currentItem.portIndex, periodicInput.text, structId); periodicInput.text = "" }
                }

                Button
                {
                    width: HF.evenWidthSpacing(parent)
                    text: "Stop"
                    font.pixelSize: 15
                    enabled: structId.valid
                    onClicked: lynxUart.stopPeriodic(structListView.currentItem.portIndex, structId)
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
                id: variableListView
                spacing: 10
                model: variableModel
                delegate: variableDelegate
                Component.onCompleted:
                {
                    variableModel.clear()
                    currentIndex = -1
                }

            }
        }
    }

    ListModel
    {
        id: deviceModel

        ListElement
        {
            portIndexIn: -1
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
            portIndexIn: -1
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
            portIndexIn: -1
            variableNameIn: "No selection"
            variableTypeIn: "No selection"
            variableValueIn: "No selection"
            enableInputIn: false
            selectedIn: false
        }

    }

    Component
    {
        id: deviceDelegate

        DeviceInfo
        {
            portIndex: portIndexIn
            deviceName: deviceNameIn
            deviceId: deviceIdIn
            structCount: structCountIn
            lynxVersion: lynxVersionIn
            width: deviceListview.width
            selected: (deviceListview.currentIndex == index)
            hovered: deviceMouseArea.containsMouse

            MouseArea
            {
                id: deviceMouseArea
                height: parent.height/2
                width: parent.width
                anchors.bottom: parent.bottom
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
            portIndex: portIndexIn
            structName: structNameIn
            structId: structIdIn
            variableCount: variableCountIn
            width: structListView.width
            selected: (structListView.currentIndex == index)
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
            id: varInfo
            portIndex: portIndexIn
            structIndex: structId.structIndex
            variableIndex: index
            variableName: variableNameIn
            variableType: variableTypeIn
            // variableValue: variableValueIn
            enableInput: enableInputIn
            hovered: variableMouseArea.containsMouse
            width: variableListView.width
            selected: selectedIn

            MouseArea
            {
                id: variableMouseArea
                width: parent.width
                height: parent.height/2
                onClicked:
                {
                    varInfo.selected = !varInfo.selected;
                    scopeServer.changePlotItem(varInfo.structIndex, varInfo.variableIndex, varInfo.selected)
                }
                hoverEnabled: true
                enabled: (varInfo.valid && varInfo.enableSelection)
            }
        }
    }
}
