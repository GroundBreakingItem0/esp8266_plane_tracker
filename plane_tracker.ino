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
const int LINE_1 = 0;
const int LINE_2 = 8;
const int LINE_3 = 16;
const int LINE_4 = 24;

WiFiClient espClient;
const char* ssid           = "xxxx";
const char* password       = "xxxx";
const char* server         = "192.168.1.111";
const short port           = 5000;


int display_brightness = 255;
int display_draw_time = 15;

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
void display_test_screen(){
  
  display.clearDisplay();
  display.setBrightness(display_brightness);
  display.setTextColor(myCYAN);
  display.setCursor(OFFSETX,LINE_1);
  display.print("EBLG>>KLAS");
  display.setTextColor(myMAGENTA);
  display.setCursor(OFFSETX,LINE_2);
  display.print("B744");

  display.setTextColor(myGREEN);
  display.setCursor(OFFSETX,LINE_3);
  display.print("10000 ft");
  display.setCursor(OFFSETX,LINE_4);
  display.setTextColor(myYELLOW);
  display.print("140 kts");

}


void display_network_settings(String wifi,String ip,String srv, int port){
  display.clearDisplay();
  display.setBrightness(display_brightness);
  display.setTextColor(myCYAN);
  display.setCursor(2,0);
  display.print("WIFI:");
  display.print(wifi);
}

void setup() {
  Serial.begin(9600);
  
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
  // Define your display layout here, e.g. 1/8 step
  //display_test_screen();
  

  // Define your scan pattern here {LINE, ZIGZAG, ZAGGIZ} (default is LINE)
  //display.setScanPattern(ZIGZAG);

  // Define multiplex implemention here {BINARY, STRAIGHT} (default is BINARY)
  //display.setMuxPattern(STRAIGHT);

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

  String url = "/";

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
    results = client.readStringUntil('\r');
  }

  Serial.println();
  Serial.println("closing connection");
  results.remove(0,1);  // Removing nlc
  return results;
}

void process_flight(String flight){
  Serial.println("Processing flight:");
  Serial.println(flight);
  char *cflight = (char*)malloc(flight.length());
  flight.toCharArray(cflight,flight.length());
  char *cfrom  = strtok(cflight,",");
  char *cto    = strtok(NULL,",");
  char *creg   = strtok(NULL,",");
  char *cflno  = strtok(NULL,",");
  char *calt   = strtok(NULL,",");
  char *cspeed = strtok(NULL,",");
  char *cac    = strtok(NULL,",");

  //void display_flight(char* sfrom,char* sto,char* sac,char* salt, char* sspeed,char* sflno, char* sreg){
  display_flight(cfrom,cto,cac,calt,cspeed,cflno,creg);
  delay(30000);
  display.clearDisplay();
  //ANR,N/A,OO-EPC,OOEPC,6025,0,DA42
  
}

void loop() {
  String flight;
  flight = get_flight_details();
  if(flight!="No flight"){
     process_flight(flight);
  }
  delay(30000);
}