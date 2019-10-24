/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Charts module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 or (at your option) any later version
** approved by the KDE Free Qt Foundation. The licenses are as published by
** the Free Software Foundation and appearing in the file LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.12
import QtCharts 2.3
import QtQuick.Dialogs 1.2
import QtQuick.Controls 2.12
import QtQuick.Controls.Styles 1.4
//![1]
Item {
    id: chartItem
    //property var setMaxAxis: 0
    //property var setMinAxis: 0
    //property bool openGL: true
    //property bool openGLSupported: true
    property var xaxis_min: new Date()
    property var xaxis_max: new Date()
    //property var nMin : 1
    property var deltaX :10000 //10*1000ms delta time
    //property var offsetFromCurrentTime: 0
    Rectangle // Shows the checkboxes
    {
        // anchors.left: parent.left
        //anchors.verticalCenter: parent.verticalCenter
        height: parent.height
        width: parent.width*0.1

        Column
        {
            id: column
            // height: parent.height
            anchors.fill: parent
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
                // anchors.top: parentBox.bottom
                anchors.topMargin: 0
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 0
            }
            ListModel
            {
                id: listCheckModel
            }
            Component
            {
                id:myCheckDelegate
                CheckBox
                {
                    checked: true
                    text: name
                    property var varindex: varindexin
                    property string seriesColor: seriesColorIn
                    leftPadding: indicator.width
                    ButtonGroup.group: childGroup

                    indicator: Rectangle
                                {
                                    x:width
                                    anchors.verticalCenter: parent.verticalCenter
                                    implicitWidth: 30
                                    implicitHeight: 30
                                    radius: 1
                                    border.color: activeFocus ? "darkblue" : "gray"
                                    border.width: 1
                                    Rectangle {
                                        visible: checked
                                        color: seriesColor
                                        border.color: "#333"
                                        radius: 2
                                        anchors.margins: 4
                                        anchors.fill: parent
                                    }

                                }
                    onCheckedChanged:
                    {
                        console.log("index is: "+varindex)
                        if(checked && index>=0)
                        {
                            chartView.series(index).visible=true
                        }
                        else if(!checked && index>=0)
                        {
                            chartView.series(index).visible=false
                        }
                    }
                }
            }
        }//colum
//        Column
//        {
//            anchors.fill: parent


//            ListView
//            {
//                id:structViewer
//                model: signalModel
//                delegate: signalDelegate
//                width: 200
//                height: 500
//            }
//            ListModel
//            {
//                id: signalModel
//                ListElement
//                {

//                }
//            }
//            Component
//            {
//                id:signalDelegate
//                CheckBoxList
//                {

//                }

//            }
//        }//colum
    }//rectangle
    ChartView
    {
        id: chartView
        height: parent.height
        width: parent.width*0.9
        anchors.right: parent.right
        animationOptions: ChartView.SeriesAnimations //NoAnimation
        theme: ChartView.ChartThemeLight
        antialiasing: true

        legend.visible: false
        legend.font.pixelSize: 15


        FileDialog
        {
            id:screenShots
        }

        Rectangle{
            id: rubberBandRec1
            border.color: "black"
            border.width: 1
            opacity: 0.3
            visible: false
            transform: Scale { origin.x: 0; origin.y: 0; yScale: -1}
        }
        Rectangle{
            id: rubberBandRec2
            border.color: "black"
            border.width: 1
            opacity: 0.3
            visible: false
            transform: Scale { origin.x: 0; origin.y: 0; yScale: -1; xScale: -1}
        }
        Rectangle{
            id: rubberBandRec3
            border.color: "black"
            border.width: 1
            opacity: 0.3
            visible: false
            transform: Scale { origin.x: 0; origin.y: 0; xScale: -1}
        }
        Rectangle{
            id: rubberBandRec4
            border.color: "black"
            border.width: 1
            opacity: 0.3
            visible: false
        }
        MouseArea{
            property bool hasBeenPressedAndHoled: false
            id:mouseArea
            property bool pressedOnce: false
            property int lastX: 0
            property int lastY: 0
            hoverEnabled: true
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton | Qt.RightButton
            onPressed: {
                if(mouse.button === Qt.LeftButton)
                {
                    rubberBandRec1.x = mouseX; rubberBandRec1.y = mouseY; rubberBandRec1.visible = true;
                    rubberBandRec2.x = mouseX; rubberBandRec2.y = mouseY; rubberBandRec2.visible = true;
                    rubberBandRec3.x = mouseX; rubberBandRec3.y = mouseY; rubberBandRec3.visible = true;
                    rubberBandRec4.x = mouseX; rubberBandRec4.y = mouseY; rubberBandRec4.visible = true;

                    hasBeenPressedAndHoled=true
                    console.log("been pressed")
                }
                if(mouse.button === Qt.RightButton)
                {

                    if(!pressedOnce)
                    {
                        lastX = mouse.x
                        lastY = mouse.y
                        pressedOnce=true
                        console.log("been pressed")
                    }

                }

            }
            onPositionChanged: {
                if(pressedOnce)
                {
                   // console.log("pos Changed")
                    if (lastX !== mouse.x) {
                        chartView.scrollRight(lastX - mouse.x)
                        lastX = mouse.x
                        //pressedOnce=false;
                    }
                    if (lastY !== mouse.y) {
                        chartView.scrollDown(lastY - mouse.y)
                        lastY = mouse.y
                        //pressedOnce=false;
                    }
                }
            }

            onMouseXChanged: {
                rubberBandRec1.width = mouseX - rubberBandRec1.x;
                rubberBandRec2.width = rubberBandRec2.x-mouseX;
                rubberBandRec3.width = rubberBandRec3.x-mouseX;
                rubberBandRec4.width = mouseX - rubberBandRec4.x;
            }
            onMouseYChanged: {
                rubberBandRec1.height = rubberBandRec1.y - mouseY;
                rubberBandRec2.height = rubberBandRec2.y - mouseY;
                rubberBandRec3.height = mouseY - rubberBandRec3.y;
                rubberBandRec4.height = mouseY - rubberBandRec4.y;
                //            if(rubberBandRec1.height<50)
                //                rubberBandRec1.height=1;
                //            if(rubberBandRec2.height<50)
                //                rubberBandRec2.height=1;
                //            if(rubberBandRec3.height<50)
                //                rubberBandRec3.height=1;
                //            if(rubberBandRec4.height<50)
                //                rubberBandRec4.height=1;
            }
            onReleased: {
                if(mouse.button === Qt.RightButton)
                    pressedOnce=false;

                if(hasBeenPressedAndHoled && mouse.button === Qt.LeftButton)
                {


                    var x = rubberBandRec4.x-(rubberBandRec4.width<0)*Math.abs(rubberBandRec4.width);
                    var y = rubberBandRec4.y-(rubberBandRec4.height<0)*Math.abs(rubberBandRec4.height);
                    //                if(rubberBandRec4.height==1)
                    //                    rubberBandRec4.height=scopeView.plotArea.height


                    if (Math.abs(rubberBandRec4.width*rubberBandRec4.height)>100){
                        var currentMax=chartView.axisX().max
                        chartView.zoomIn(Qt.rect(x, y, Math.abs(rubberBandRec4.width),Math.abs(rubberBandRec4.height)));
                        offsetFromCurrentTime=currentMax - chartView.axisX().max
                        console.log(offsetFromCurrentTime)
                        deltaX=chartView.axisX().max - chartView.axisX().min
                    }
                    else
                        chartView.zoomIn(Qt.rect(x-100, y-100,200,200))
                    rubberBandRec1.visible = false;
                    rubberBandRec2.visible = false;
                    rubberBandRec3.visible = false;
                    rubberBandRec4.visible = false;

                }
                hasBeenPressedAndHoled=false

            }
//            onPressAndHold: {

//            }

//            onClicked: {
//                if(mouse.button === Qt.RightButton)
//                {
//                    //chartView.zoomOut();
//                    console.log(xaxis_max-xaxis_min)//ms of chart




//                }

//            }
            onDoubleClicked: {
                if(mouse.button === Qt.RightButton)
                {
                    chartView.zoomOut();
                }

            }

        }

        //PinchArea{
        //    id: pa

        //    anchors.fill: parent
        //    onPinchUpdated: {
        //        console.log ("pich clicked")
        //        chartView.zoomReset();
        //        //                    var center_x = pinch.center.x
        //        //                    var center_y = pinch.center.y
        //        //                    var width_zoom = height/pinch.scale;
        //        //                    var height_zoom = width/pinch.scale;
        //        //                    var r = Qt.rect(center_x-width_zoom/2, center_y - height_zoom/2, width_zoom, height_zoom)
        //        //                    chartView.zoomIn(r)
        //    }

        //}
        ValueAxis {
            id: axisY1
            min: -1
            max: 255

        }

        ValueAxis {
            id: axisY2
            min: -10
            max: 10
        }
        DateTimeAxis {
            id:axisX
            format: "hh:mm:ss"
            tickCount: 5
            min:xaxis_min
            max:xaxis_max
        }
    }//ChartView
    function screenShot()
    {
        var mypaths = screenShots.shortcuts.desktop.toString().replace(/^(file:\/{3})/,"")
        var filename = "/lynxLogScreenshots-"+new Date().toLocaleTimeString().replace(new RegExp(":", 'g'),".")+".png";

        chartItem.grabToImage(function(result)
        {
            if(result.saveToFile(mypaths + filename))
                console.log("Screenshot created at: " + mypaths + filename);
            else
                console.log("Screenshot failed");
        });

    }

    function resizeHorizontal()
    {
        chartView.zoomReset();
        xaxis_min=new Date(scopeServer.getFirstX());
        xaxis_max=new Date(scopeServer.getLastX())
        deltaX = scopeServer.getLastX() - scopeServer.getFirstX()
    }
    function resizeVertial()
    {

        scopeServer.calcAxisIndex(xaxis_min.getTime(),xaxis_max.getTime())
        // scopeServer.calcMinMaxY();
        var maxY = scopeServer.getFrameMaxY()//ms of chart
        var minY = scopeServer.getFrameMinY()//ms of chart

        var margin = (maxY - minY)*0.075

        axisY1.max = maxY + margin
        axisY1.min = minY - margin
        // axisY2.max = maxY*1.075
        // axisY2.min = minY*1.075
    }

    function autoscale()
    {
        chartView.zoomReset();
        resizeHorizontal()
        resizeVertial()
    }
    function changeSeriesType(type)
    {
        chartView.removeAllSeries();
        listCheckModel.clear();
        parentBox.text =qsTr("Struct Name")
        for(var n=0; n<scopeServer.getNumberOfSignals();n++)
        {
            // TODO Check struct name
        var series1
            if (type === "line")
            {
                series1 = chartView.createSeries(ChartView.SeriesTypeLine, scopeServer.getSignalText(n), axisX, axisY1);
            }
            else
            {
                series1 = chartView.createSeries(ChartView.SeriesTypeScatter, scopeServer.getSignalText(n), axisX, axisY1);
                series1.markerSize = 2;
                series1.borderColor = "transparent";
            }
            series1.useOpenGL = true
            listCheckModel.append({name:scopeServer.getSignalText(n),varindexin:n,checked:true,seriesColorIn:series1.color.toString()})
            //signalModel.get(0).listCheckModel.append({name:scopeServer.getSignalText(n),varindexin:n,checked:true,seriesColorIn:series1.color.toString()})


        }

    }
    function updateChart()
    {
        for(var n = 0; n < scopeServer.getNumberOfSignals(); n++)
        {
            scopeServer.update(chartView.series(n), n);
        }

        if(!chartView.isZoomed())
        {
            xaxis_max = new Date()
            //var today = new Date();
            xaxis_min = new Date(new Date() - deltaX)
        }
        else
        {
            xaxis_max = new Date(new Date() - offsetFromCurrentTime)

            xaxis_min = new Date(xaxis_max - deltaX)

        }
    }

}//Item

