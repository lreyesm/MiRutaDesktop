import QtQuick 2.12
import QtLocation 5.12
import QtPositioning 5.12

MapQuickItem
{
    id: marker
    anchorPoint.x: marker.width / 4
    anchorPoint.y: marker.height

        sourceItem:
            Image {
            id: icon
            source: w.getCurrentPriorityMarker(model.position)
            sourceSize.width: 35
            sourceSize.height: 35
            mipmap: true
            smooth: true
        }

        MouseArea{
            anchors.fill: parent
            hoverEnabled: true
            acceptedButtons: Qt.LeftButton | Qt.RightButton
            onClicked: {

                console.log(model.position.latitude, model.position.longitude);
                w.printCoordenatesMarker(model.position);
            }
            onDoubleClicked: {
                console.log('double-clicked')
            }
            onPressed: {
                console.log('pressed')

            }
            onEntered: {
                console.log('Entered')
                w.showLabelInformation(model.position);
            }
            onExited: {
                console.log('Exited')
            }
        }

//    sourceItem: Column {
//        width : 40
//        height: 60
//        Rectangle {
//            x:-3
//            y:-5
//            width : 100
//            height: 15
//            color: "white"
//            radius: 3

//            Text {
//                id: text_marker
//                anchors.fill: parent
//                text: w.getTextWhitGeoCode(model.position)
//                font.bold: true;
//                font.pointSize : 6
//                color: '#368DCE';
//                verticalAlignment : Text.AlignVCenter
//                horizontalAlignment : Text.AlignHCenter
//            }

//        }
//        Image {
//            id: icon
//            source: "qrc:///icons/marker.png"
//            sourceSize.width: 35
//            sourceSize.height: 35
//        }
////        MouseArea{
////            anchors.fill: parent
////            hoverEnabled: true
////            acceptedButtons: Qt.LeftButton | Qt.RightButton
////            onClicked: {
////                console.log(model.position.latitude, model.position.longitude);
////                w.printCoordenatesMarker(model.position);
////            }
////            onDoubleClicked: {
////                console.log('double-clicked')
////            }
////            onPressed: {
////                console.log('pressed')

////            }
////            onEntered: {
////                console.log('Entered')
////                w.showLabelInformation(model.position);
////            }
////            onExited: {
////                console.log('Exited')
////            }
////        }
//    }
}

