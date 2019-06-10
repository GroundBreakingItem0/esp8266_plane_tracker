# esp8266_plane_tracker

This esp8266 sketch is a simple sketch that displays planes flying overhead on a 64x32 RGB matrix display. 
The ESP8266 and RGB matrix are wired following the guide here: https://www.instructables.com/id/RGB-LED-Matrix-With-an-ESP8266/

The esp8266 relies on a raspberry pi with an RTL SDR antenna running fr24feed to parse the flight data.

The ESP8266 expects a reply with the following format : 

<FROM>,<TO>,<REG>,<FLNO>,<ALT>,<SPEED>,<ACTYPE> such as: 
ATL,FRA,LH001,20000,434,B748




