import QtQuick 2.12
import QtQuick.Controls 2.12

Item {
    id: item
    property string descripition: "Not set"
    property string text: "Not set"
    property var labelRatio: 0.5
    property var fontPixelSize: 15
    implicitWidth: 200
    height: fontPixelSize*1.1

    Row
    {
        anchors.fill: parent
        spacing: 10

        Label
        {
            width: (parent.width - parent.spacing)*labelRatio
            text: item.descripition
            font.bold: true
            font.pixelSize: fontPixelSize
            anchors.verticalCenter: parent.verticalCenter
        }

        Label
        {
            width: (parent.width - parent.spacing)*(1 - labelRatio)
            text: item.text
            font.pixelSize: fontPixelSize
            anchors.verticalCenter: parent.verticalCenter
        }
    }
}
