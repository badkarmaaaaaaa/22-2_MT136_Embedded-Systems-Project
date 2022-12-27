
#include <WiFi.h>       // standard library
#include <WebServer.h>  // standard library
#include "SuperMon.h"   // .h file that stores your html page code

//#define USE_INTRANET

#define LOCAL_SSID "Cedim FIM UNI"
#define LOCAL_PASS "Cedim2022_2"

#define AP_SSID "anpr22"
#define AP_PASS "1234"

const int infra=15; const int infra2=2;const int infra3=4;
bool status1 = true; bool status2=true; bool status3=true;
uint32_t SensorUpdate = 0;int FanSpeed;

// the XML array size needs to be bigger that your maximum expected size. 2048 is way too big for this example
char XML[2048];

// just some buffer holder for char operations
char buf[32];

// variable for the IP reported when you connect to your homes intranet (during debug mode)
IPAddress Actual_IP;

// definitions of your desired intranet created by the ESP32
IPAddress PageIP(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress ip;

// gotta create a server
WebServer server(80);

void setup() {

  Serial.begin(9600);
  disableCore0WDT();
  Serial.println("starting server");

  // if you have this #define USE_INTRANET,  you will connect to your home intranet, again makes debugging easier
#ifdef USE_INTRANET
  WiFi.begin(LOCAL_SSID, LOCAL_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("IP address: "); Serial.println(WiFi.localIP());
  Actual_IP = WiFi.localIP();
#endif

  // if you don't have #define USE_INTRANET, here's where you will creat and access point
  // an intranet with no internet connection. But Clients can connect to your intranet and see
  // the web page you are about to serve up
#ifndef USE_INTRANET
  WiFi.softAP(AP_SSID, AP_PASS);
  delay(100);
  WiFi.softAPConfig(PageIP, gateway, subnet);
  delay(100);
  Actual_IP = WiFi.softAPIP();
  Serial.print("IP address: "); Serial.println(Actual_IP);
#endif

  printWifiStatus();
  pinMode(infra,INPUT);pinMode(infra2,INPUT);

  // these calls will handle data coming back from your web page
  // this one is a page request, upon ESP getting / string the web page will be sent
  server.on("/", SendWebsite);

  // upon esp getting /XML string, ESP will build and send the XML, this is how we refresh
  // just parts of the web page
  server.on("/xml", SendXML);

  // upon ESP getting /UPDATE_SLIDER string, ESP will execute the UpdateSlider function
  // same notion for the following .on calls
  // add as many as you need to process incoming strings from your web page
  // as you can imagine you will need to code some javascript in your web page to send such strings
  // this process will be documented in the SuperMon.h web page code
  server.on("/UPDATE_SLIDER", UpdateSlider);
  server.on("/BUTTON_0", ProcessButton_0);
  server.on("/BUTTON_1", ProcessButton_1);

  // finally begin the server
  server.begin();

}

void loop() {
  if ((millis() - SensorUpdate) >= 50) {
    SensorUpdate = millis();
    status1=digitalRead(infra);
    status2=digitalRead(infra2);
    status3=digitalRead(infra3);
  }
  server.handleClient();
}


// function managed by an .on method to handle slider actions on the web page
// this example will get the passed string called VALUE and conver to a pwm value
// and control the fan speed
void UpdateSlider() {
  // many I hate strings, but wifi lib uses them...
  String t_state = server.arg("VALUE");
  Serial.print("UpdateSlider"); Serial.println(t_state);
  strcpy(buf, "");
  // now send it back
  server.send(200, "text/plain", buf); //Send web page
}

// now process button_0 press from the web site. Typical applications are the used on the web client can
// turn on / off a light, a fan, disable something etc
void ProcessButton_0() {
  Serial.print("Button 0 ");
  server.send(200, "text/plain", ""); 
}

// same notion for processing button_1
void ProcessButton_1() {
  Serial.println("Button 1 press");
  Serial.print("Button 1 ");
  server.send(200, "text/plain", ""); //Send web page
}

void SendWebsite() {
  Serial.println("sending web page");
  server.send(200, "text/html", PAGE_MAIN);
}

void SendXML() {
  strcpy(XML, "<?xml version = '1.0'?>\n<Data>\n");

  if(status1){
    sprintf(buf, "<B0>%s</B0>\n", "libre");
  }else{sprintf(buf, "<B0>%s</B0>\n", "ocupado");}
  strcat(XML, buf);

  if(status2){
    sprintf(buf, "<B1>%s</B1>\n", "libre");
  }else{sprintf(buf, "<B1>%s</B1>\n", "ocupado");}
  strcat(XML, buf);

  if(status3){
    sprintf(buf, "<B2>%s</B2>\n", "libre");
  }else{sprintf(buf, "<B2>%s</B2>\n", "ocupado");}
  strcat(XML, buf);

  strcat(XML, "</Data>\n");
  // wanna see what the XML code looks like?
  // actually print it to the serial monitor and use some text editor to get the size
  // then pad and adjust char XML[2048]; above
  Serial.println(XML);

  // you may have to play with this value, big pages need more porcessing time, and hence
  // a longer timeout that 200 ms
  server.send(200, "text/xml", XML);

}

// I think I got this code from the wifi example
void printWifiStatus() {

  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  // print where to go in a browser:
  Serial.print("Open http://");
  Serial.println(ip);
}

// end of code
