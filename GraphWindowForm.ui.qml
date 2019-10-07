import QtQuick 2.12
import QtQuick.Controls 2.12

Page {
    id:graphPage
    Label
    {
        anchors.centerIn: parent
        text: "Graphs will come here"
        font.pixelSize: 50
    }
    ScopeView
    {
        id: scopeView
        x: 1000
        y: 0
        z: -1
        anchors.centerIn: parent

    }
}
