import QtQuick 2.12
import QtQuick.Controls 2.12

Item {
    // const QString & description, const QString & id, const QString & version, const QString & count
    property string deviceDescription: "No selection"
    property string deviceId: "No selection"
    property string version: "No selection"
    property string structCount: "No selection"

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

        Rectangle
        {
            radius: 20
            border.color: "black"
            border.width: 2
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
    }
}
