# esp8266_plane_tracker

This esp8266 sketch is a simple sketch that displays planes flying overhead on a 64x32 RGB matrix display. 
The ESP8266 and RGB matrix are wired following the guide here: https://www.instructables.com/id/RGB-LED-Matrix-With-an-ESP8266/

The sketch is fairly simple and just polls the public flightradar 24 API for the area that you define in the code.

## Configuration

Just update the sketch with your wifi creds and replace the area variable with the area you desire to cover. 

```c++
const char* ssid           = "xxx";    // REPLACE WITH YOUR DATA
const char* password       = "xxx";    // REPLACE WITH YOUR DATA
const String area          = "<long max>,<long min>,<lat min>,<lat max>"; //REPLACE WITH YOUR DATA ex : 40.00,39.50,10.50,11.00
```

You can find your area on FlightRadar24.com by inspecting the page (right click -> inpect) and look at the network table for the queries containing the area that you're currently watching. 




