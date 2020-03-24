#include <ESP8266WiFi.h>
#include <PxMatrix.h>

#ifdef ESP8266

#include <Ticker.h>
Ticker display_ticker;
#define P_LAT 16
#define P_A 5
#define P_B 4
#define P_C 15
#define P_D 12
#define P_E 0
#define P_OE 2

#endif

PxMATRIX display(64,32,P_LAT, P_OE,P_A,P_B,P_C,P_D,P_E);

// Some standard colors
uint16_t myRED     = display.color565(255, 0, 0);
uint16_t myGREEN   = display.color565(0, 255, 0);
uint16_t myBLUE    = display.color565(0, 0, 255);
uint16_t myWHITE   = display.color565(255, 255, 255);
uint16_t myYELLOW  = display.color565(255, 255, 0);
uint16_t myCYAN    = display.color565(0, 255, 255);
uint16_t myMAGENTA = display.color565(255, 0, 255);
uint16_t myBLACK   = display.color565(0, 0, 0);

uint16_t myCOLORS[8]={myRED,myGREEN,myBLUE,myWHITE,myYELLOW,myCYAN,myMAGENTA,myBLACK};

const int OFFSETX = 0;
const int LINE_1  = 0;
const int LINE_2  = 8;
const int LINE_3  = 16;
const int LINE_4  = 24;
   
WiFiClient espClient;
const char* ssid           = "xxx";    // REPLACE WITH YOUR DATA
const char* password       = "xxx";    // REPLACE WITH YOUR DATA
const char* server         = "data-live.flightradar24.com";
const unsigned short port  = 80;
const String area          = "<long max>,<long min>,<lat min>,<lat max>"; //REPLACE WITH YOUR DATA ex : 40.00,39.50,10.50,11.00
const String path          = "/zones/fcgi/feed.js?bounds="+area+"&faa=1&satellite=1&mlat=1&flarm=1&adsb=1&gnd=1&air=1&vehicles=1&estimated=0&maxage=14400&gliders=1&stats=0&ems=1";

int display_brightness = 127;
int display_draw_time  = 15;

#ifdef ESP8266
// ISR for display refresh
void display_updater()
{
  display.display(display_draw_time);
}
#endif

void display_update_enable(bool is_enable)
{
 
#ifdef ESP8266
  if (is_enable)
    display_ticker.attach(0.002, display_updater);
  else
    display_ticker.detach();
#endif

}

void display_flight(char* sfrom,char* sto,char* sac,char* salt, char* sspeed,char* sflno, char* sreg){
    
  display.clearDisplay();
  display.setBrightness(display_brightness);
  display.setTextColor(myCYAN);
  
  display.setCursor(OFFSETX,LINE_1);
  display.print(sfrom);
  display.setTextColor(myCYAN);
  display.print(">");
  
  display.print(sto);
  display.setCursor(OFFSETX,LINE_2);
  display.setTextColor(myWHITE);
  display.print(sflno);
  if(atoi(salt)>10000) display.setTextColor(myGREEN);
  else display.setTextColor(myRED);
  display.setCursor(OFFSETX,LINE_3);
  display.print(salt);
  display.print("ft");
  display.setCursor(OFFSETX,LINE_4);
  display.setTextColor(myWHITE);
  display.print(sspeed);
  display.print("kts");
  display.setTextColor(myCYAN);
  display.print(sac);
  
}

void display_network_settings(String wifi,String ip,String srv, int port){
  display.clearDisplay();
  display.setBrightness(display_brightness);
  display.setTextColor(myRED);
  display.setCursor(2,0);
  display.print("W:");
  display.print(wifi);
}

void setup() {
  Serial.begin(9600);
  display.setMuxDelay(1,1,1,1,1);

  display.begin(16);
  display.clearDisplay();
  display_update_enable(true);

  WiFi.begin(ssid, password);
  WiFi.mode(WIFI_STA);   
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("Connecting to WiFi..");
    Serial.print(WiFi.macAddress());
  }
  display_network_settings(ssid,"x.x.x.x",server,port);
  delay(3000);
  display.clearDisplay();

}

String get_flight_details() {
  String results = "";

  Serial.print("connecting to ");
  Serial.println(server);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  if (!client.connect(server, port)) {
    Serial.println("connection failed");
    return "Connection closed";
  }

  String url = path;

  Serial.print("Requesting URL: ");
  Serial.println(url);

  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + server + "\r\n" +
               "Connection: close\r\n\r\n");
               
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return "Connection timeout";
    }
  }

  // Read all the lines of the reply from server and print them to Serial

  while (client.available()) {
    results += client.readStringUntil(']');
    break;
  }

  Serial.println();
  Serial.println("closing connection");
  results.remove(0,1);  // Removing nlc

  Serial.println("Fetched data: " +results);
  return results;
}

void process_flight(String flight){

  Serial.println("Processing flight:");
  Serial.println(flight);
  
  char *cflight = (char*)malloc(flight.length());
  
  flight.toCharArray(cflight,flight.length());
  
  char *dummy  = strtok(cflight,",");
  dummy        = strtok(NULL,",");
  dummy        = strtok(NULL,",");
  dummy        = strtok(NULL,",");
  char *calt   = strtok(NULL,",");
  char *cspeed = strtok(NULL,",");
  dummy        = strtok(NULL,",");
  dummy        = strtok(NULL,",");
  char *cac    = strtok(NULL,",");
  char *creg   = strtok(NULL,",");
  dummy        = strtok(NULL,",");
  char *cfrom  = strtok(NULL,",");
  char *cto    = strtok(NULL,",");
  char *cflno  = strtok(NULL,",");
  
  Serial.println("DUMP::::");
  Serial.println(cfrom);
  Serial.println(cto);
  Serial.println(creg);
  Serial.println(calt);
  Serial.println(cspeed);
  Serial.println(cac);
  Serial.println(cflno);
  Serial.println("ENDOFDUMP<<<");

  display_flight(cfrom,cto,cac,calt,cspeed,cflno,creg);
  delay(30000);
  display.clearDisplay();

  free(cflight);
  
}

String parse_flight(String json){
  int start_c = json.indexOf('[',0);
  String s    = json.substring(start_c); 

  s.replace("\"\"","N/A");
  s.replace("\"","");
  
  return s;
}

void loop() {
  String json;
  String flight;
  
  json   = get_flight_details();
  flight = parse_flight(json);

  if(flight!=NULL){
     process_flight(flight);
  }
  
  delay(30000);
}
