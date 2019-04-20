/// <reference path="googlemaps-intellisense.js" />

var points = [];
var marker = null;
var clickListener;
var poly = null;
var polygons = [];
var highlighted_marker = null;
var point_markers = [];
var newPolygon = true;
var index = -1;

createMap();


var locationMarkerList = [];
var tempPointList = [];
var polygonList = [];
var polylineList = [];

// Create the Google Map to be used.
function createMap() {

    var mapProp = {
        overviewMapControlOptions: { opened: true },
        panControl: true,
        zoomControl: true,
        mapTypeControl: true,
        scaleControl: true,
        streetViewControl: true,
        overviewMapControl: true,
        rotateControl: true,
        mapTypeId: google.maps.MapTypeId.ROADMAP,

        center: new google.maps.LatLng(35.701385, 51.380740),
        zoom: 11,       
    };
    document.map = new google.maps.Map(document.getElementById("map_canvas"), mapProp);   
}


//Convert an address to a coordinates (Geocoding)

function getcoordinatesfromaddress(address) {
    var signaltext = "";
    var addresses = address.split("_");
 

    for (var x = 0; x < addresses.length - 1; ++x) {
        var geocoder = new google.maps.Geocoder();      
        geocoder.geocode({ 'address': addresses[x] }, function (results, status) {
            if (status === 'OK') {
                signaltext = signaltext + String(results[0].geometry.location.lat()) + ";" + String(results[0].geometry.location.lng()) + ";" ;               

                if (x === addresses.length - 1) {
                    new QWebChannel(qt.webChannelTransport, function (channel) {
                        // now you retrieve your object
                        var signalLineEdit = channel.objects.signalLineEditName;
                        signalLineEdit.setText(signaltext);
                    });

                }

            } else {
                alert('Geocode was not successful for the following reason: ' + status);
            }

        });

     
    }

}


//Convert an address to a coordinates (Geocoding)
function getsinglecoordinatesfromaddress(address, name) {


        var geocoder = new google.maps.Geocoder();
        geocoder.geocode({ 'address': address }, function (results, status) {
            if (status === 'OK') {

                    new QWebChannel(qt.webChannelTransport, function (channel) {
                        // now you retrieve your object
                        var signalLineEdit = channel.objects.signalLineEditName;
                        signalLineEdit.setText(String(results[0].geometry.location.lat()) + ";" + String(results[0].geometry.location.lng()) + ";"+name);
                    });

                

            } else {
                alert('Geocode was not successful for the following reason: ' + status);
            }

        });



    }


// Displays a single baloon point on the map
function displayLocation(lat, lng, name) {

    var locationMarker = new google.maps.Marker({
        map: document.map,
        draggable: false,
        title: name,
        animation: google.maps.Animation.DROP,
        position: new google.maps.LatLng(lat, lng)
    });

    locationMarker.setMap(document.map);
     locationMarkerList.push(locationMarker);
}

// Finds the index of the marker with the title "name" in locationMarkerList
function findLocation(name) {
    var index = -1;

    for (var i = 0; i < locationMarkerList.length; ++i) {
        if (locationMarkerList[i].getTitle() == name) {
            index = i;
            break;
        }
    }
    return index;
}

// Displays a location on the map while deleting all repetitive locations
function clearAndDisplayLocation(lat, lng, name) {
    removeLocation(name)
    displayLocation(lat, lng, name);
    document.map.setCenter(new google.maps.LatLng(lat, lng), document.map.getZoom());
}

// Removes all markers of a location on the map
function removeLocation(name) {
    var index = 0;
    while (index < locationMarkerList.length) {
        if (locationMarkerList[index].getTitle() == name) {
            locationMarkerList[index].setMap(null);
            locationMarkerList.splice(index, 1);
        } else {
            index++;
        }
    }
}

// Removes all markers of all locations on the map
function removeAllLocations() {
    while (locationMarkerList.length > 0) {
        locationMarkerList[0].setMap(null);
        locationMarkerList.splice(0, 1);
    }
}


// Add events that will get the coordinates of the point
function getLocation(name) {
    var index = findLocation(name);
    if (index == -1) {
        clickingListener = google.maps.event.addListener(document.map, 'click', function (point) {
            displayLocation(point.latLng.lat(), point.latLng.lng(), name);

            new QWebChannel(qt.webChannelTransport, function (channel) {
                // now you retrieve your object
                var signalLineEdit = channel.objects.signalLineEditName;
                signalLineEdit.setText(String(point.latLng.lat()) + ";" + String(point.latLng.lng()) + ";" + name);
            });

            google.maps.event.removeListener(clickingListener);
        });
    } else {
        locationMarkerList[index].setDraggable(true);
        var draggingListener = google.maps.event.addListener(locationMarkerList[index], "dragend", function () {

            new QWebChannel(qt.webChannelTransport, function (channel) {
                // now you retrieve your object
                var signalLineEdit = channel.objects.signalLineEditName;
                signalLineEdit.setText(String(locationMarkerList[index].getPosition().lat()) + ";" + String(locationMarkerList[index].getPosition().lng()) + ";" + name);
            });

            locationMarkerList[index].setDraggable(false);
            google.maps.event.removeListener(draggingListener);
        });
    }
}



// Add events that will enable creating multiple locations
function createMultipleLocations(name) {
    var ii = 0;

    clickingListener = google.maps.event.addListener(document.map, 'click', function (point) {

            ii++;
            currentName = String(name + ii);
            displayLocation(point.latLng.lat(), point.latLng.lng(), currentName);

            new QWebChannel(qt.webChannelTransport, function (channel) {
                // now you retrieve your object
                var signalLineEdit = channel.objects.signalLineEditName;
                signalLineEdit.setText(String(point.latLng.lat()) + ";" + String(point.latLng.lng()) + ";" + currentName);
            });
    });


    rightClickingListener = google.maps.event.addListener(document.map, "rightclick", function () {
        new QWebChannel(qt.webChannelTransport, function (channel) {
            // now you retrieve your object
            var signalLineEdit = channel.objects.signalLineEditName;
               signalLineEdit.setText(String("TERMINATE"));
        });

       google.maps.event.removeListener(clickingListener);
       google.maps.event.removeListener(rightClickingListener);
    });
}

function addTempPoint(lat, lng) {
    tempPointList.push(new google.maps.LatLng(lat, lng));
}






// Removes a polygon from the map
function removePolygon(name) {
    var index = -1;

    for (var i = 0; i < polygonList.length; ++i) {
        if (polygonList[i].title == name) {
            index = i;
            break;
        }
    }

    if(index > -1){
        polygonList[index].setMap(null);
    polygonList.splice(index, 1);
    }
}

function drawPolygon(name, color, opacity) {

    tempPointList.push(new google.maps.LatLng(tempPointList[0].lat(), tempPointList[0].lng()));
    removePolygon(name);

    var tempPolygon = new google.maps.Polygon({
        path: tempPointList,
        strokeColor: color,
        strokeOpacity: opacity*2,
        strokeWeight: 1,
        fillColor: color,
        fillOpacity: opacity,
        title : name
    });

    tempPolygon.setMap(document.map);
    polygonList.push(tempPolygon);
    tempPointList = [];
    fitToPolygon(polygonList.length - 1);
}

// Fits the map to a specific polygon
function fitToPolygon(index) {
    var bounds = new google.maps.LatLngBounds();
    var points = polygonList[index].getPath().getArray();
    for (var n = 0; n < points.length ; n++) {
        bounds.extend(points[n]);
    }
    document.map.fitBounds(bounds);
}



// Removes a polygon from the map
function removePolyline(name) {
    var index = -1;

    for (var i = 0; i < polylineList.length; ++i) {
        if (polylineList[i].title == name) {
            index = i;
            break;
        }
    }

    if(index > -1){
    polylineList[index].setMap(null);
    polylineList.splice(index, 1);
    }
}

function drawPolyline(name, color, opacity) {

        removePolyline(name);

    var tempPolyline = new google.maps.Polyline({
        path: tempPointList,
        strokeColor: color,
        strokeOpacity: opacity,
        geodesic: true,
        title: name
    });
    tempPolyline.setMap(document.map);

    polylineList.push(tempPolyline);
    tempPointList = [];
    fitToPolyline(polylineList.length - 1);
}

// Fits the map to a specific polyline
function fitToPolyline(index) {

    var bounds = new google.maps.LatLngBounds();
    var points = polylineList[index].getPath().getArray();
    for (var n = 0; n < points.length ; n++) {
        bounds.extend(points[n]);
    }
    document.map.fitBounds(bounds);


}


// OVERLAYING AN IMAGE ON MAP

DebugOverlay.prototype = new google.maps.OverlayView();

function overlayImage(URL) {
  
         var swBound = document.map.getBounds().getSouthWest();
         var neBound = document.map.getBounds().getNorthEast(); 
        var bounds = new google.maps.LatLngBounds(swBound, neBound);

        console.log(document.map);


      var  overlay = new DebugOverlay(document.map.getBounds(), URL, document.map);

        var markerA = new google.maps.Marker({
            position: swBound,
            map: document.map,
            draggable: true
        });

        var markerB = new google.maps.Marker({
            position: neBound,
            map: document.map,
            draggable: true
        });

        google.maps.event.addListener(markerA, 'drag', function () {

            var newPointA = markerA.getPosition();
            var newPointB = markerB.getPosition();
            var newBounds = new google.maps.LatLngBounds(newPointA, newPointB);
            overlay.updateBounds(newBounds);
        });

        google.maps.event.addListener(markerB, 'drag', function () {

            var newPointA = markerA.getPosition();
            var newPointB = markerB.getPosition();
            var newBounds = new google.maps.LatLngBounds(newPointA, newPointB);
            overlay.updateBounds(newBounds);
        });

        google.maps.event.addListener(markerA, 'dragend', function () {

            var newPointA = markerA.getPosition();
            var newPointB = markerB.getPosition();
            console.log("point1" + newPointA);
            console.log("point2" + newPointB);
        });

        google.maps.event.addListener(markerB, 'dragend', function () {
            var newPointA = markerA.getPosition();
            var newPointB = markerB.getPosition();
            console.log("point1" + newPointA);
            console.log("point2" + newPointB);
        });

    }

    function DebugOverlay(bounds, image, map) {

        this.bounds_ = bounds;
        this.image_ = image;
        this.map_ = map;
        this.div_ = null;
        this.setMap(map);
    }

    DebugOverlay.prototype.onAdd = function () {

        var div = document.createElement('div');
        div.style.borderStyle = 'none';
        div.style.borderWidth = '0px';
        div.style.position = 'absolute';
        var img = document.createElement('img');
        img.src = this.image_;
        img.style.width = '100%';
        img.style.height = '100%';
        img.style.opacity = '0.5';
        img.style.position = 'absolute';
        div.appendChild(img);
        this.div_ = div;
        var panes = this.getPanes();
        panes.overlayLayer.appendChild(div);
    };

    DebugOverlay.prototype.draw = function () {
        var overlayProjection = this.getProjection();
        var sw = overlayProjection.fromLatLngToDivPixel(this.bounds_.getSouthWest());
        var ne = overlayProjection.fromLatLngToDivPixel(this.bounds_.getNorthEast());
        var div = this.div_;
        div.style.left = sw.x + 'px';
        div.style.top = ne.y + 'px';
        div.style.width = (ne.x - sw.x) + 'px';
        div.style.height = (sw.y - ne.y) + 'px';
    };


    DebugOverlay.prototype.updateBounds = function (bounds) {
        this.bounds_ = bounds;
        this.draw();
    };

    DebugOverlay.prototype.onRemove = function () {
        this.div_.parentNode.removeChild(this.div_);
        this.div_ = null;
    };

