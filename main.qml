import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtQuick.Dialogs 1.2
// import backend 1.0
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
            selectionView.deviceDescription = description
            selectionView.deviceId = id
            selectionView.version = version
            selectionView.structCount = count
        }
        onClearStructList: selectionView.clearStructs()
        onAddStruct: selectionView.addStruct(structInfo)
        onAddStructInfo:
        {
            selectionView.structDescription = description
            selectionView.structId = id
            selectionView.variableCount = count
        }
        onAddStructIndex: selectionView.addStructIndex(structIndex)
        onClearVariableList: selectionView.clearVariableList()
        onAddVariable: selectionView.addVariable(variableName, variableIndex, variableType, variableValue, enableInput, checked)
        onChangeVariableValue: selectionView.changeVariableValue(structIndex, variableIndex, value)
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

            if(!scopeView.isZoomed())
            {
                scopeView.xaxis_max = new Date()
                //var today = new Date();
                scopeView.xaxis_min = new Date(new Date() - scopeView.deltaX)
            }
            else
            {
                scopeView.xaxis_max = new Date(new Date() - scopeView.offsetFromCurrentTime)

                scopeView.xaxis_min = new Date(scopeView.xaxis_max - scopeView.deltaX)

            }
        }
        onReScale:
        {
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
                Rectangle
                {
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    height: parent.height
                    width: parent.width*0.2

                    Column
                    {
                        ButtonGroup
                        {
                            id: childGroup
                            exclusive: false
                            checkState: parentBox.checkState
                        }
                        CheckBox
                        {
                            id: parentBox
                            text: qsTr("Parent")
                            checkState: childGroup.checkState
                        }

                        ListView
                        {
                            id:listViewer
                            model: listCheckModel
                            delegate: myCheckDelegate
                            width: 200
                            height: 150
                        }
                        ListModel
                        {
                            id: listCheckModel
//                            ListElement {
//                                name: "Item 1"
//                            }
//                                description: "Selectable item 1"
//                                selected: true
//                            }
                        }
                        Component
                        {
                            id:myCheckDelegate
                            CheckBox
                            {
                                checked: true
                                text: name
                                property var varindex: varindexin
                                leftPadding: indicator.width
                                ButtonGroup.group: childGroup
                                onCheckedChanged:
                                {
                                    console.log("index is: "+varindex)
                                    if(checked && index>=0)
                                    {
                                        scopeView.series(index).visible=true
                                    }
                                    else if(!checked && index>=0)
                                    {
                                        scopeView.series(index).visible=false
                                    }
                                }
                            }
                        }
                    }//colum
                }//rectangle
                ScopeView
                {
                    id: scopeView
                    anchors.right:  parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    height: parent.height
                    width: parent.width*0.8
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
