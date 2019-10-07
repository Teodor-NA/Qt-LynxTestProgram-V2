import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import Qt.labs.platform 1.1

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
            IconButton{
                id:addSignal
                filename:"icons8-add-new-50"
                visible: topRibbon.graphButtonsVisible
                tooltip: "Add signal"
                onClicked:{
                    scopeView.maxNumberOfSignals=backend.getNumberOfSignals()
                    console.log(backend.getNumberOfSignals())
                    if(scopeView.numberOfSignals<scopeView.maxNumberOfSignals)
                    scopeView.numberOfSignals++

                    scopeView.changeSeriesType("line")


                }
            }
            IconButton{
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
            IconButton{
                property bool enabled: false
                id:linePlot
                tooltip: "Line Plot"
                visible: topRibbon.graphButtonsVisible
                filename: "icons8-plot-50"
                onClicked:{
                    if (enabled){
                        enabled = false
                        filename = "icons8-plot-50"
                        onClicked: scopeView.changeSeriesType("scatter");
                    }
                    else{
                        enabled = true
                        filename = "icons8-scatter-plot-50"
                        scopeView.changeSeriesType("line");
                    }
                }
            }

            IconButton{
                id:saveToFile
                visible: topRibbon.graphButtonsVisible
                filename:"icons8-save-50"
                tooltip: "Save to CSV"
                onClicked:fileDialogWrite.visible = true
            }
            IconButton{
                id:loadFile
                visible: topRibbon.graphButtonsVisible
                filename:"icons8-folder-50"
                tooltip: "Load File"
                onClicked:fileDialogRead.visible = true
            }
            IconButton{
                visible: topRibbon.graphButtonsVisible
                filename: "icons8-play-50"
                tooltip: "Live view"
            }

            IconButton{
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
                        backend.resumeChartviewRefresh()
                    }
                    else
                    {
                        enabled = true
                        filename = "icons8-realtime-50"
                        backend.pauseChartviewRefresh()
                    }

                }
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
                onClicked: resizeHorizontal()//graphPage.scopeView.resizeHorizontal()
            }

            IconButton
            {
                visible: topRibbon.graphButtonsVisible
                filename: "icons8-resize-vertical-50"
                tooltip: "Autoscale Y"               
                onClicked: scopeView.autoscale()
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
                id:selectionPage


            }
            GraphWindowForm
            {
                id:graphPage
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
    FileDialog {
        id: fileDialogWrite
        title: "Please choose a file"
        fileMode: FileDialog.SaveFile
        //folder:   StandardPaths.standardLocations(StandardPaths.PicturesLocation)
        nameFilters: [ "Text files (*.csv)" ]
        onAccepted: {
                backend.writeToCSV(fileDialogWrite.file)
        }
        onRejected: {
            console.log("Canceled")
        }
    }
    FileDialog {
        id: fileDialogRead
        title: "Please choose a file"
        fileMode: FileDialog.OpenFile
        //folder:   StandardPaths.standardLocations(StandardPaths.PicturesLocation)
        nameFilters: [ "Text files (*.csv)" ]
        onAccepted: {

                backend.readFromCSV(fileDialogRead.file)

        }
        onRejected: {
            console.log("Canceled")
        }
    }
}


