var map;
var markers= [];
var QtPlaces;

var searchCircle;

function initialize(x, y)
{
    var myOptions = {
        center: new google.maps.LatLng(x, y),
        zoom: 8,
        mapTypeId: google.maps.MapTypeId.ROADMAP,
        panControl: true
    };
    map = new google.maps.Map(document.getElementById("map_canvas"), myOptions)
    //google.maps.event.addListener(map, "click", function() {} )
}
