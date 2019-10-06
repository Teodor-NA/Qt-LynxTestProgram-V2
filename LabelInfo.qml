import QtQuick 2.12
import QtQuick.Controls 2.12

Row
{
    id: row1
    property int index: -1
    property string first: "Not set"
    property string second: "Not set"
    property bool enableInput: false
    signal accepted(int index)

    Label
    {
        width: 160
        text: qsTr(row1.first + ": ")
        font.bold: true
        font.pixelSize: 15
    }

    TextInput
    {
        width: 200
        text: row1.second
        font.pixelSize: 15
        readOnly: !enableInput
        onActiveFocusChanged:
        {
            if(activeFocus && enableInput)
            {
                selectAll()
            }
        }
        validator: DoubleValidator{locale: "en_US"}
        onAccepted:
        {
            row1.second = text
            row1.accepted(index)
        }
    }
}
