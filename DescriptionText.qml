import QtQuick 2.12
import QtQuick.Controls 2.12

Item {
    id: item
    property string descripition: "Not set"
    property string text: "Not set"
    implicitHeight: 20

    Row
    {
        anchors.verticalCenter: parent.verticalCenter
        spacing: 10
        Label
        {
            width: 150
            text: item.descripition
            font.bold: true
            font.pixelSize: 15
            anchors.verticalCenter: parent.verticalCenter
        }
        Label
        {
            width: 100
            text: item.text
            font.pixelSize: 15
            anchors.verticalCenter: parent.verticalCenter
        }
    }
}
