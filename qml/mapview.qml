import QtQuick 2.12
import QtLocation 5.12
import QtPositioning 5.12
import QtQuick.Window 2.0

import QtQuick.Controls 2.1
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.12
import QtGraphicalEffects 1.0


//import GeneralMagic 1.0


Item {
    id: window

    Plugin{
//        id: mapbox
//        name: "mapbox"
        id: mapPlugin
        name: w.getMapPluginName()

        parameters:
//            PluginParameter { name: "here.app_id"; value: "rJr3k1ADGxLWrJgauEa0" }
//            PluginParameter { name: "here.token"; value: "nvFXd7lx_jURlmgQvhsGaw" }
//            PluginParameter { name: "here.proxy"; value: "system" }
//            PluginParameter {
//                name: "mapbox.access_token"
////                value: "pk.eyJ1IjoiZWxhdXRvbWF0aWNvbiIsImEiOiJja2F6cnFzMWswMTdvMnltc2l5Y3VscWxxIn0.rI8DreipMVQvBDgZw00esg"//mi llave
//                value: "pk.eyJ1IjoiMTMxOTk1YWRyaWFuIiwiYSI6ImNrMTJvcnQ5ajAweGgzY3FkZDZwdWsxbXUifQ.Yuj14-vJOFLd_U78K-iwOg"// llave adrian
//            }
//            PluginParameter {
//                name: "googlemaps.google_maps_key"
//                value: "AIzaSyB8Kny7pDcsLU9LbcKb9lMtEspHkWLqxKo"
//            }
        PluginParameter {
            name: w.getMapPluginParameterName();
            value: w.getMapPluginParameterValue()
        }
        PluginParameter { name: "googlemaps.map_only"; value: true }
    }

    Map {
        id: map

        anchors.fill: parent
        plugin: mapPlugin
        center: QtPositioning.coordinate(marker_model.xposition, marker_model.yposition/*59.91, 10.75*/) // Oslo
        zoomLevel: marker_model.zoomLevel
//        activeMapType: supportedMapTypes[supportedMapTypes.length -10]  //Satelital - googlemaps->3 mapbox->10
        activeMapType: supportedMapTypes[supportedMapTypes.length - marker_model.mapType]
        gesture.enabled: true
        gesture.acceptedGestures: MapGestureArea.PinchGesture | MapGestureArea.PanGesture

        onZoomLevelChanged: {
            w.hideLabelInformation();
            w.setCurrentZoomInQML(map.zoomLevel);
            console.log("ZoomLevel changed");
            console.log(map.zoomLevel);
        }

        property int lastX : -1
        property int lastY : -1
        property int pressX : -1
        property int pressY : -1
        property int jitterThreshold : 30

        Marker{
            coordinate: marker_model.current
        }

        MapItemView{
            model: marker_model

            delegate: Marker{
                id: delegate_marker
                coordinate: model.position
            }
        }

        MouseArea {
            id: mouseArea
            property variant lastCoordinate
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton | Qt.RightButton
            propagateComposedEvents: true

            onPressed : {
                map.lastX = mouse.x
                map.lastY = mouse.y
                map.pressX = mouse.x
                map.pressY = mouse.y
                lastCoordinate = map.toCoordinate(Qt.point(mouse.x, mouse.y))
                console.log("Normal pressed");
                w.hideLabelInformation();
            }

            onPositionChanged: {
                console.log("onPositionChanged");
                w.hideLabelInformation();

                if (mouse.button == Qt.LeftButton) {
                    map.lastX = mouse.x
                    map.lastY = mouse.y
                }
            }

            onDoubleClicked: {
                var mouseGeoPos = map.toCoordinate(Qt.point(mouse.x, mouse.y));
                var preZoomPoint = map.fromCoordinate(mouseGeoPos, false);
                if (mouse.button === Qt.LeftButton) {
                    w.setZoomLevel(Math.floor(map.zoomLevel + 1));
                } else if (mouse.button === Qt.RightButton) {
                    w.setZoomLevel(Math.floor(map.zoomLevel - 1));
                }
                var postZoomPoint = map.fromCoordinate(mouseGeoPos, false);
                var dx = postZoomPoint.x - preZoomPoint.x;
                var dy = postZoomPoint.y - preZoomPoint.y;

                var mapCenterPoint = Qt.point(map.width / 2.0 + dx, map.height / 2.0 + dy);
                w.setCenter(map.toCoordinate(mapCenterPoint));

                lastX = -1;
                lastY = -1;
            }

            onPressAndHold:{
                console.log((map.toCoordinate(Qt.point(mouse.x,mouse.y)).latitude),', '+ (map.toCoordinate(Qt.point(mouse.x,mouse.y)).longitude));
                w.printCoordenates((map.toCoordinate(Qt.point(mouse.x,mouse.y)).latitude)+","+ (map.toCoordinate(Qt.point(mouse.x,mouse.y)).longitude));
            }
        }

    }

}






