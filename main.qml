import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12


Item
{
    visible: true
    anchors.fill: parent

    Column
    {
        anchors.fill: parent

        Button
        {
            id: fullScreenButton
            text: "Fullscreen"
            onClicked: { console.log("Fullscreen clicked"); backEnd.fullscreen() }
        }

        SwipeView
        {
            id: swipeWindow
            height: parent.height - tabBar.height - fullScreenButton.height
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


