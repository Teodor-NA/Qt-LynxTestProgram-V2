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
        onClearPortList: portListModel.clear()
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
                width: 300
                height: 50
                model:
                    ListModel
                    {
                        id: portListModel
                        ListElement{text: qsTr("Select port")}
                    }
                onPressedChanged:
                {
                    if(pressed)
                    {
                        backEnd.refreshPortList()
                        portComboBox.currentIndex = 0
                    }
                }
                onCurrentIndexChanged:
                {
                    backEnd.portSelected(currentIndex - 1)
                }

                ToolTip.delay: 500
                ToolTip.timeout: 3000
                ToolTip.visible: hovered
                ToolTip.text: qsTr("Select port")

            }

            IconButton
            {
                id: connectButton
                visible: topRibbon.selectButtonsVisible
                enabled: portComboBox.currentIndex > 0
                filename: "icons8-disconnected-50"
                tooltip: "Connect"
                onClicked:
                {
                    if (backEnd.uartConnected())
                    {
                        filename = "icons8-disconnected-50"
                        tooltip = "Connect"
                        portComboBox.enabled = true
                    }
                    else
                    {
                        filename = "icons8-connected-50"
                        tooltip = "Disonnect"
                        portComboBox.enabled = false
                    }

                    backEnd.connectButtonClicked();
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
                visible: topRibbon.graphButtonsVisible
                filename: "icons8-pause-50"
                tooltip: "Pause"

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

            Image {
                source: "/icons/icons8-separator-50.png"
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


