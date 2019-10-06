import QtQuick 2.12
import QtQuick.Window 2.12
import backend 1.1
import QtQuick.Controls 2.12


Window
{
    visible: true
    width: 1280
    height: 720
    minimumWidth: 800
    minimumHeight: 400
    title: qsTr("Lynx Test App")
    // onWidthChanged: console.log("Width: " + width)
    // onHeightChanged: console.log("Height: " + height)

    BackEnd
    {
        id: backEnd
//        orangeLight: orangeSlider.value
//        blueLight: blueSlider.value
        onClearPortList:
        {
            portListModel.clear()
            portListModel.append({ text: qsTr("Select port") })
        }
        onAddPort: portListModel.append({ text: portName })
        onClearDevices:
        {
            deviceListModel.clear()
            deviceListModel.append({ text: qsTr("No selection") })
            deviceComboBox.currentIndex = 0
        }
        onAddDevice: deviceListModel.append({ text: device })
        onAddDeviceInfo:
        {
            deviceDescriptionLabel.second = description
            deviceIdLabel.second = id
            lynxVersionLabel.second = version
            structCountLabel.second = count
        }

        onClearStructList:
        {
            structListModel.clear()
            structListModel.append({ text: qsTr("No selection") })
            structComboBox.currentIndex = 0
        }
        onAddStruct: structListModel.append({ text: structInfo })
        onAddStructInfo:
        {
            // const QString & description, const QString & id, const QString & count
            structDescriptionLabel.second = description
            structIdLabel.second = id
            variableCountLabel.second = count
        }

        onClearVariableList: variableModel.clear()
        onAddVarable: variableModel.append( { description: description, varIndex: index, dataType: type })
        onAddVariableValue:
        {
            if ((variableIndex < 0) || (variableIndex >= variableModel.count))
            {
                console.log("Variable out of bounds. Index: " + variableIndex + ". Min index: 0. Max index: " + variableModel.count)
                return
            }

            variableModel.setProperty(variableIndex, "value", value)
        }
    }

    Row
    {
        spacing: 10
        padding: 10
        anchors.fill: parent

        Column
        {
            spacing: 10
            // padding: 20
            width: parent.width/2 - parent.spacing - parent.padding

/*
            Row
            {
                width: 200
                spacing: 10

                TextField
                {
                    width: parent.width/2
                    id: textField
                    placeholderText: qsTr("Input number")
                    validator: IntValidator { top: 2147483647; bottom: 0 }
                    onAccepted:
                    {
                        backEnd.transmitInterval = text
                        label.text = text;
                        text = qsTr("");
                    }
                }

                Label
                {
                    width: parent.width/2
                    id: label
                    text: qsTr("Not set")
                }

                Column
                {
                    spacing: 10

                    Row
                    {
                        spacing: 5

                        Slider
                        {
                            id: blueSlider

                            width: 200
                            from: 0
                            to: 255
                      }

                        Label
                        {
                            id: blueLabel
                            text: Math.floor(blueSlider.value)
                        }
                    }
                    Row
                    {
                        spacing: 5

                        Slider
                        {
                            id: orangeSlider

                            width: 200
                            from: 0
                            to: 255
                        }

                        Label
                        {
                            id: orangeLabel
                            text: Math.floor(orangeSlider.value)
                        }
                    }
                }
            }

*/

            Row
            {
                width: parent.width
                spacing: 10

                ComboBox
                {
                    width: parent.width*2/3 - parent.spacing
                    id: portComboBox
                    font.pixelSize: 15
                    model:
                        ListModel
                        {
                            id: portListModel
                            ListElement{text: qsTr("Select port")}
                        }

                    onActivated:
                    {
                        // console.log("Activated")
                        if (currentIndex > 0)
                        {
                            backEnd.portSelected(portComboBox.currentIndex - 1)
                            connectButton.enabled = true
                        }
                        else
                            connectButton.enabled = false
                    }

                    onPressedChanged:
                    {
                        if (pressed)
                        {
                            backEnd.refreshPortList()
                            if (portListModel.count < 2)
                                connectButton.enabled = false
                            // console.log("Pressed")
                        }
                    }
                }

                Button
                {
                    width: parent.width*1/3
                    id: connectButton
                    enabled: false
                    text: qsTr("Connect")
                    font.pixelSize: 15
                    onClicked:
                    {
                        backEnd.connectButtonClicked()
                        if (backEnd.uartConnected())
                        {
                            text = qsTr("Disconnect")
                            portComboBox.enabled = false
                        }
                        else
                        {
                            text = qsTr("Connect")
                            portComboBox.enabled = true
                        }
                    }
                }
            }


            Button
            {
                id: scanButton
                width: parent.width
                text: qsTr("Scan")
                font.pixelSize: 15
                onClicked: backEnd.scan()
            }

            ComboBox
            {
                id: deviceComboBox
                width: parent.width
                font.pixelSize: 15
                model:
                    ListModel
                    {
                        id: deviceListModel
                        ListElement { text: qsTr("No selection") }
                    }
                onCurrentIndexChanged: backEnd.selectDevice(currentIndex)

            }

            Frame
            {
                width: parent.width
                height: 100

                Column
                {
                    spacing: 2
                    anchors.centerIn: parent

                    LabelInfo
                    {
                        id: deviceDescriptionLabel
                        first: "Device Description"
                        second: "No selection"
                    }
                    LabelInfo
                    {
                        id: deviceIdLabel
                        first: "Device Id"
                        second: "No selection"
                    }
                    LabelInfo
                    {
                        id: lynxVersionLabel
                        first: "Lynx Version"
                        second: "No selection"
                    }
                    LabelInfo
                    {
                        id: structCountLabel
                        first: "Struct Count"
                        second: "No selection"
                    }
                }
            }

            ComboBox
            {
                id: structComboBox
                width: parent.width
                font.pixelSize: 15
                model:
                    ListModel
                    {
                        id: structListModel
                        ListElement { text: qsTr("No selection") }
                    }
                onCurrentIndexChanged: backEnd.selectStruct(currentIndex)
            }

            Button{
                id: addStructButton
                width: parent.width
                text: qsTr("Add struct")
                font.pixelSize: 15
                enabled: (structComboBox.currentIndex > 0)
                onClicked:
                {
                    backEnd.generateStruct()
                }
            }

            Button{
                id: pullStructButton
                width: parent.width
                text: qsTr("Pull struct")
                font.pixelSize: 15
                onClicked: backEnd.pullStruct()
            }

            Row{
                spacing: 10
                width: parent.width

                TextField{
                    id: timerInput
                    width: parent.width*1/3 - parent.spacing
                    placeholderText: qsTr("Input time")
                    font.pixelSize: 15
                    validator: IntValidator {bottom: 0; top: 2147483647;}
                }

                Button{
                    id: timerStartButton
                    width: parent.width*1/3 - parent.spacing
                    text: qsTr("Start periodic")
                    font.pixelSize: 15
                    enabled: timerInput.acceptableInput
                    onClicked: backEnd.startPeriodic(timerInput.text)
                }

                Button{
                    id: timerStopButton
                    width: parent.width*1/3
                    text: qsTr("Stop periodic")
                    font.pixelSize: 15
                    onClicked: backEnd.stopPeriodic()
                }
            }

            Frame
            {
                width: parent.width
                height: 70

                Column
                {
                    spacing: 2
                    anchors.centerIn: parent
                    LabelInfo
                    {
                        id: structDescriptionLabel
                        first: "Struct Description"
                        second: "No selection"
                    }
                    LabelInfo
                    {
                        id: structIdLabel
                        first: "Struct Id"
                        second: "No selection"
                    }
                    LabelInfo
                    {
                        id: variableCountLabel
                        first: "Variable Count"
                        second: "No selection"
                    }
                }
            }

            // ListView
            // {
            //     id: variableListView
            //     width: parent.width
            //     model:
            //         ListModel
            //         {
            //             id: variableListModel
            //             ListElement { text: qsTr("No selection") }
            //         }
            // }

        }


        Frame
        {
            width: parent.width/2 - parent.padding
            height: parent.height - 20

            ScrollView
            {
                width: parent.width - 20
                height: parent.height - 20
                anchors.centerIn: parent
                clip: true

                ListView
                {
                    spacing: 2
                    anchors.fill: parent
                    model: VariableModel { id: variableModel }
                    delegate: variableDelegate
                }
            }
        }
    }

    Component
    {
        id: variableDelegate

        Frame
        {
            width: parent.width
            height: 90

            Column
            {
                anchors.centerIn: parent
                spacing: 2
                LabelInfo { first: qsTr("Description"); second: description }
                LabelInfo { first: qsTr("Variable Index"); second: varIndex }
                LabelInfo { first: qsTr("Data type"); second: dataType }
                LabelInfo
                {
                    index: parseInt(varIndex)
                    first: qsTr("Value");
                    second: value
                    enableInput: true
                    onAccepted: backEnd.sendVariable(index, second) // console.log("Accepted" + index)
                }
            }
        }
    }
}


