import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtQuick.Dialogs 1.2
import lynxlib 1.0
// import Qt.labs.platform 1.1


Item
{
    visible: true
    anchors.fill: parent
    Connections
    {
        target: lynxUart
        onAddPort: portListModel.append({ text: port })
        onClearPortList: { portListModel.clear(); portListModel.append({ text: "Select port" })}
    }

    Connections
    {
        target: scopeServer

        onCreateSeries: linePlot.enable ?scopeView.changeSeriesType("scatter"):scopeView.changeSeriesType("line")

        onRefreshChart:scopeView.updateChart()

        onReScale: scopeView.autoscale()
    }

    Column
    {
        anchors.fill: parent

        Item
        {
            id: topRibbon
            property bool selectButtonsVisible: (tabBar.currentIndex == 0)
            property bool graphButtonsVisible: (tabBar.currentIndex == 1)

            implicitHeight: 60
            width: parent.width

            Row
            {
                id: topRibbonLeft

                spacing: 10
                padding: 10
                anchors.left: parent.left

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
                            // backEnd.refreshPortList()
                            lynxUart.scanForUart()
                            portComboBox.currentIndex = 0
                        }
                    }
                    onCurrentIndexChanged:
                    {
                        lynxUart.selectPort(currentIndex - 1)
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
                    filename: lynxUart.uartConnected ? "icons8-connected-50" : "icons8-disconnected-50"
                    tooltip: lynxUart.uartConnected ? "Disconnect" : "Connect"
                    onClicked:
                    {
                        lynxUart.connectButtonClicked();
                    }

                }

                IconButton
                {
                    id: scanButton
                    visible: topRibbon.selectButtonsVisible
                    filename:"icons8-update-left-rotation-50"
                    tooltip: "Scan for devices"
//                    enabled: lynxUart.uartConnected
                    onClicked: lynxUart.scan()
                }

                IconButton
                {
                    id: saveToFile
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

                ToolSeparator
                {
                    visible: topRibbon.graphButtonsVisible
                    anchors.verticalCenter: parent.verticalCenter
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
                            scopeView.changeSeriesType("line");
                        }
                        else{
                            enabled = true
                            filename = "icons8-scatter-plot-50"
                            scopeView.changeSeriesType("scatter");
                        }
                    }
                }

                IconButton
                {
                    id:playButton
                    property bool enabled: false
                    property bool resumeRealTimer: false
                    visible: topRibbon.graphButtonsVisible
                    filename: "icons8-play-50"
                    tooltip: "Live view"
                    onClicked:{
                        if(resumeRealTimer)
                        {
                            resumeRealTimer=false
                            scopeView.deltaX = 10000 //Reset default
                            scopeServer.resumeRealtime()
                        }

                        else if (enabled){
                            enabled = false
                            filename = "icons8-play-50"
                            scopeServer.pauseChartviewRefresh();
                        }
                        else{
                            enabled = true
                            filename = "icons8-pause-50"
                            scopeServer.resumeChartviewRefresh();
                        }
                    }
                }
                IconButton{
                    visible: topRibbon.graphButtonsVisible
                    filename: "icons8-screenshot-50"
                    tooltip: "Screenshot"
                    onClicked: {
                        scopeView.screenShot()
                    }
                }
                IconButton{
                    visible: topRibbon.graphButtonsVisible
                    filename: "icons8-expand-50"
                    tooltip: "Autoscale all"
                    onClicked: {
                        //scopeView.resizeHorizontal()
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
                    onClicked: {scopeView.resizeVertial()

                    }
                }

            }

            Row
            {
                id: topRibbonRight

                spacing: topRibbonLeft.spacing
                padding: topRibbonLeft.padding
                anchors.right: parent.right

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
        }

        SwipeView
        {
            id: swipeWindow
            height: parent.height - tabBar.height - topRibbon.height
            width: parent.width
            currentIndex: tabBar.currentIndex
            interactive: false
            SelectionWindowForm
            {
                id: selectionPage
                SelectionView
                {
                    id: selectionView
                    anchors.fill: parent
                }

            }
            GraphWindowForm
            {
                id: graphPage

                ScopeView
                {
                    id: scopeView
                    anchors.fill: parent
//                    anchors.right:  parent.right
//                    anchors.verticalCenter: parent.verticalCenter
//                    height: parent.height
//                    width: parent.width*0.8
                }
            }

        }

        TabBar
        {
            id: tabBar
            width: parent.width
            currentIndex: swipeWindow.currentIndex
//            onCurrentIndexChanged:
//            {
//                switch (currentIndex)
//                {
//                case 0:
//                    topRibbon.selectButtonsVisible = true
//                    topRibbon.graphButtonsVisible = false
//                    break
//                case 1:
//                    topRibbon.selectButtonsVisible = false
//                    topRibbon.graphButtonsVisible = true
//                    break

//                }
//            }

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

        selectExisting: false
        title: "Please choose a file"
        folder: shortcuts.desktop
        nameFilters: [ "Text files (*.csv)" ]
        onAccepted: {
            console.log("file: "+fileUrl)
                scopeServer.writeToCSV(fileDialogWrite.fileUrl)
        }
        onRejected: {
            console.log("Canceled")
        }
    }

    FileDialog
    {

        id: fileDialogRead
        selectExisting: true
        sidebarVisible :true
        //modality: Qt.w
        title: "Please choose a file"
        // fileMode: FileDialog.OpenFile
        folder: shortcuts.desktop
        nameFilters: [ "Text files (*.csv)" ]
        onAccepted:
        {
            scopeServer.readFromCSV(fileDialogRead.fileUrl)
            playButton.resumeRealTimer = true;
            playButton.filename = "icons8-realtime-50"

        }
        onRejected:
        {
            console.log("Canceled")
        }
    }
}
