import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtQuick.Dialogs 1.2
// import Qt.labs.platform 1.1

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

    Connections
    {
        target: scopeServer
        onCreateSeries:
        {
            if (linePlot.enabled){
                scopeView.changeSeriesType("scatter");
            }
            else{
                scopeView.changeSeriesType("line");
            }
        }

        onRefreshChart:
        {
            // vvv Hvorfor er dette i frontend?? vvv
            for(var n = 0; n < scopeView.numberOfSignals; n++)
            {
                scopeServer.update(chartView.series(n), n);
            }
            // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

            if(!scopeView.isZoomed())
            {

                scopeView.xaxis = new Date()
                var today = new Date();
                scopeView.xaxis_min = new Date(new Date() -scopeView.deltaX)
            }


        }
        onReScale:
        {
            scopeView.resizeHorizontal()
            scopeView.autoscale()
        }
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
                id:saveToFile
                visible: topRibbon.graphButtonsVisible
                filename:"icons8-save-50"
                tooltip: "Save to CSV"
                onClicked:fileDialogWrite.visible = true
            }

            IconButton
            {
                id:loadFile
                visible: topRibbon.graphButtonsVisible
                filename:"icons8-folder-50"
                tooltip: "Load File"
                onClicked:fileDialogRead.visible = true
            }

            Image {
                source: "/icons/icons8-separator-50.png"
                visible: topRibbon.graphButtonsVisible
            }

            IconButton
            {
                id: addSignal
                filename:"icons8-add-new-50"
                visible: topRibbon.graphButtonsVisible
                tooltip: "Add signal"
                onClicked:
                {
                    scopeView.maxNumberOfSignals = scopeServer.getNumberOfSignals()

                    if(scopeView.numberOfSignals<scopeView.maxNumberOfSignals)
                        scopeView.numberOfSignals++

                    scopeView.changeSeriesType("line")
                }
            }

            IconButton
            {
                id:removeSignal
                filename:"icons8-reduce-50"
                visible: topRibbon.graphButtonsVisible
                tooltip: "Remove signal"
                onClicked:{
                    if(scopeView.numberOfSignals>1)
                        scopeView.numberOfSignals--
                    scopeView.changeSeriesType("line")
                }
            }

            IconButton
            {
                property bool enabled: false
                id: linePlot
                tooltip: "Line Plot"
                visible: topRibbon.graphButtonsVisible
                filename: "icons8-plot-50"
                onClicked:{
                    if (enabled){
                        enabled = false
                        filename = "icons8-plot-50"
                        scopeView.changeSeriesType("scatter");
                    }
                    else{
                        enabled = true
                        filename = "icons8-scatter-plot-50"
                        scopeView.changeSeriesType("line");
                    }
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

            IconButton{
                visible: topRibbon.graphButtonsVisible
                filename: "icons8-expand-50"
                tooltip: "Autoscale all"
                onClicked: {
                    scopeView.resizeHorizontal()
                    scopeView.autoscale()
                }
            }

            IconButton{
                visible: topRibbon.graphButtonsVisible
                filename: "icons8-resize-horizontal-50"
                tooltip: "Autoscale X"
                onClicked: scopeView.resizeHorizontal()//graphPage.scopeView.resizeHorizontal()
            }

            IconButton
            {
                visible: topRibbon.graphButtonsVisible
                filename: "icons8-resize-vertical-50"
                tooltip: "Autoscale Y"               
                onClicked: scopeView.autoscale()
            }

            Image {
                source: "/icons/icons8-separator-50.png"
            }

            IconButton
            {
                filename: "icons8-full-screen-50"
                tooltip: "Toggle fullscreen/windowed"
                onClicked: backEnd.fullscreenButtonClicked()
            }

            IconButton
            {
                filename: "icons8-multiplication-50"
                tooltip: "Exit"
                highlightColor: "tomato"
                visible: backEnd.fullscreen
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
                id: selectionPage


            }
            GraphWindowForm
            {
                id: graphPage

                ScopeView
                {
                    id: scopeView
                    anchors.centerIn: parent
                }
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

    FileDialog
    {
        id: fileDialogWrite
        title: "Please choose a file"
        // fileMode: FileDialog.SaveFile
        // folder:   StandardPaths.standardLocations(StandardPaths.PicturesLocation)
        nameFilters: [ "Text files (*.csv)" ]
        onAccepted: {
                scopeServer.writeToCSV(fileDialogWrite.file)
        }
        onRejected: {
            console.log("Canceled")
        }
    }

    FileDialog
    {
        id: fileDialogRead
        title: "Please choose a file"
        // fileMode: FileDialog.OpenFile
        // folder:   StandardPaths.standardLocations(StandardPaths.PicturesLocation)
        nameFilters: [ "Text files (*.csv)" ]
        onAccepted: {

                scopeServer.readFromCSV(fileDialogRead.file)

        }
        onRejected: {
            console.log("Canceled")
        }
    }
}
