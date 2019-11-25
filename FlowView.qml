
import QtQuick 2.13
import QtQuick.Shapes 1.13
Item {
    height: 500
    width: 500
    x: 0
    y: 0
    Shape {
          width: 200
          height: 150
          anchors.centerIn: parent
          ShapePath {
              strokeWidth: 4
              strokeColor: "black"

              strokeStyle: ShapePath.SolidLine
              dashPattern: [ 1, 4 ]
              startX: 0; startY: 0
              PathLine { x: 200; y: 0 }
              PathLine { x: 100; y: 0 }
              PathLine { x: 100; y: 50 }
          }
      }


//    Canvas
//        {
//            id: drawingCanvas
//            width: 625
//            height: 737.5
//            anchors.rightMargin: 202
//            anchors.bottomMargin: 92
//            anchors.leftMargin: -202
//            anchors.topMargin: -92
//            anchors.fill: parent
//            onPaint:
//            {
//                var ctx = getContext("2d")
//                ctx.lineWidth = 15;
//                ctx.strokeStyle = "red"
//                ctx.beginPath()
//                ctx.moveTo(drawingCanvas.width / 2, 0)
//                ctx.lineTo((drawingCanvas.width / 2) + 10, 0)
//                ctx.closePath()
//                ctx.stroke()
//            }
//        }
}










