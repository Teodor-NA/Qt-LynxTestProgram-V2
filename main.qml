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
        onClearDevices: selectionView.clearDevices()
        onAddDevice: selectionView.addDevice(device)
        onAddDeviceInfo:
        {
            // const QString & description, const QString & id, const QString & version, const QString & count
            selectionView.deviceDescription = description
            selectionView.deviceId = id
            selectionView.version = version
            selectionView.structCount = count
        }
    }

    Connections
    {
        target: scopeServer

        onCreateSeries:
        {

            if (linePlot.enabled)
            {
                scopeView.changeSeriesType("scatter");

            }
            else
            {
                scopeView.changeSeriesType("line");
            }
        }

        onRefreshChart:
        {
            for(var n = 0; n < scopeServer.getNumberOfSignals(); n++)
            {
                scopeServer.update(scopeView.series(n), n);
            }

            //if(!scopeView.isZoomed())
            //{
                scopeView.xaxis_max = new Date()
                var today = new Date();
                scopeView.xaxis_min = new Date(new Date() - scopeView.deltaX)
            //}
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
                        scanButton.enabled = false
                    }
                    else
                    {
                        filename = "icons8-connected-50"
                        tooltip = "Disonnect"
                        portComboBox.enabled = false
                        scanButton.enabled = true
                    }

                    backEnd.connectButtonClicked();
                }

            }

            IconButton
            {
                id: scanButton
                visible: topRibbon.selectButtonsVisible
                filename:"icons8-update-left-rotation-50"
                tooltip: "Scan for devices"
                enabled: false
                onClicked: backEnd.scan()
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
                property bool enabled: false
                visible: topRibbon.graphButtonsVisible
                filename: "icons8-play-50"
                tooltip: "Live view"
                onClicked:{
                    if (enabled){
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
                onClicked: scopeView.resizeVertial()
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
                   // anchors.centerIn: parent
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
        selectExisting: false
        title: "Please choose a file"
        folder: shortcuts.desktop
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
        selectExisting: true
        sidebarVisible :true
        modality: Qt.w
        title: "Please choose a file"
        // fileMode: FileDialog.OpenFile
         folder: shortcuts.desktop
        nameFilters: [ "Text files (*.csv)" ]
        onAccepted: {

                scopeServer.readFromCSV(fileDialogRead.file)

        }
        onRejected: {
            console.log("Canceled")
        }
    }
}
