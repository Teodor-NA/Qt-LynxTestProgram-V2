import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12


Item
{
    visible: true
    anchors.fill: parent

    Connections
    {
        target: backEnd
        onAddPort: portListModel.append({ text: portName })
    }

    Column
    {
        anchors.fill: parent

        Row
        {
            id: topRibbon
            property bool selectButtonsVisible: true
            property bool graphButtonsVisible: false

            spacing: 10
            padding: 10
            anchors.right: parent.right

            ComboBox
            {
                id: portComboBox
                visible: topRibbon.selectButtonsVisible
                font.pixelSize: 15
                width: 200
                height: 50
                model:
                    ListModel
                    {
                        id: portListModel
                        ListElement{text: qsTr("Select port")}
                    }

            }

            IconButton
            {
                visible: topRibbon.graphButtonsVisible
                filename: "icons8-play-50"
                tooltip: "Live view"
            }

            IconButton
            {
                property bool enabled: false
                visible: topRibbon.graphButtonsVisible
                filename: "icons8-pause-50"
                tooltip: "Pause"
                onClicked:
                {
                    if (enabled)
                    {
                        enabled = false
                        filename = "icons8-pause-50"
                    }
                    else
                    {
                        enabled = true
                        filename = "icons8-realtime-50"
                    }

                }
            }

            IconButton
            {
                visible: topRibbon.graphButtonsVisible
                filename: "icons8-expand-50"
                tooltip: "Autoscale all"
            }

            IconButton
            {
                visible: topRibbon.graphButtonsVisible
                filename: "icons8-resize-horizontal-50"
                tooltip: "Autoscale X"
            }

            IconButton
            {
                visible: topRibbon.graphButtonsVisible
                filename: "icons8-resize-vertical-50"
                tooltip: "Autoscale Y"
            }

            IconButton
            {
                filename: "icons8-full-screen-50"
                tooltip: "Toggle fullscreen/windowed"
                onClicked: backEnd.fullscreen()
            }

            IconButton
            {
                filename: "icons8-multiplication-50"
                tooltip: "Exit"
                onClicked: Qt.quit()
            }

        }

        SwipeView
        {
            id: swipeWindow
            height: parent.height - tabBar.height - topRibbon.height
            width: parent.width
            currentIndex: tabBar.currentIndex
            SelectionWindowForm
            {

            }
            GraphWindowForm
            {

            }

        }

        TabBar
        {
            id: tabBar
            width: parent.width
            currentIndex: swipeWindow.currentIndex
            onCurrentIndexChanged:
            {
                switch (currentIndex)
                {
                case 0:
                    topRibbon.selectButtonsVisible = true
                    topRibbon.graphButtonsVisible = false
                    break
                case 1:
                    topRibbon.selectButtonsVisible = false
                    topRibbon.graphButtonsVisible = true
                    break

                }
            }

            TabButton
            {
                text: "Select variables"
                font.pixelSize: 15
            }

            TabButton
            {
                text: "View graphs"
                font.pixelSize: 15
            }
        }
    }
}


