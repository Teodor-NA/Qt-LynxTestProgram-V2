import QtQuick 2.12
import QtQuick.Controls 2.12

Item {
    // const QString & description, const QString & id, const QString & version, const QString & count
    property string deviceDescription: "No selection"
    property string deviceId: "No selection"
    property string version: "No selsection"
    property string deviceCount: "No selection"

    function clearDevices()
    {
        deviceModel.clear()
    }

    function addDevice(deviceText)
    {
        deviceModel.append({text: deviceText})
    }

    Column
    {
        spacing: 10
        padding: 20
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
            onCurrentIndexChanged: backEnd.selectDevice(currentIndex)

        }

        DescriptionText
        {
            descripition: "Device name:"
            text: deviceDescription
            height: 60
            width: parent.width
        }

        Row
        {
            spacing: 10
            Label
            {
                width: 150
                text: "Device id: "
                font.pixelSize: 15
            }
            Label
            {
                width: 100
                text: deviceId
                font.pixelSize: 15
            }
        }
    }
}
