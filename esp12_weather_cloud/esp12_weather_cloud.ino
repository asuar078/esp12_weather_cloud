#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <Adafruit_DotStar.h>
#include <SPI.h>
#include <EEPROM.h>

#include "wifi_credentials.h"
#include "W_Underground.h"
#include "LEDStrip.h"

#define NUMPIXELS 60 // Number of LEDs in strip
#define DATAPIN    D4
#define CLOCKPIN   D5

#define SERVER_PORT 1133

bool wifi_connected = true;

// TCP server at port will respond to HTTP requests
ESP8266WebServer server(SERVER_PORT);

String deviceName = "weather cloud";
volatile int mode = 0;
volatile int color = 0;

Adafruit_DotStar strip = Adafruit_DotStar(
  NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BRG);

LEDStrip ledStrip(strip);

WiFiClient httpclient;
W_Underground weather(httpclient);

uint16_t delayCounter = 0;

// LED delay setting
#define LED_UPDATE_DELAY 2000

// 5 minutes between update checks. The free developer account has a limit
// on the  number of calls so don't go wild.
// #define DELAY_NORMAL    (5*60*1000)
const uint16_t DELAY_NORMAL = ((5*60*1000) / LED_UPDATE_DELAY);

// 20 minute delay between updates after an error
// #define DELAY_ERROR     (20*60*1000)
const uint16_t DELAY_ERROR = ((20*60*1000) / LED_UPDATE_DELAY);

uint16_t weatherDelay = DELAY_NORMAL;

enum Mode {Auto, Manual, Clear, BlueSky, WhiteClouds, Overcast, Sunset, Rain, Cloudy};

void setup()
{
  Serial.begin(115200);

  // We start by connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print(F("Connecting to "));
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASS);

  int wifi_connection_attemps = 0;
  while (WiFi.status() != WL_CONNECTED) {
    wifi_connection_attemps++;
    delay(500);
    Serial.print(F("."));

    if(wifi_connection_attemps > 40){
      Serial.println("ERROR: Could not connect to access point!");
      wifi_connected = false;
      break;
    }
  }
  Serial.println();

  printWifiStatus();

    // start MDNS
  if (!MDNS.begin("esp8266")) {
    Serial.println("Error setting up MDNS responder!");
    while(1) { delay(1000); }
  }
  Serial.println("mDNS responder started");

  // Add service to MDNS-SD
  MDNS.addService("http", "tcp", SERVER_PORT);

  server.on("/", handleRoot);
  server.on("/setting", handleSetting);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");

  strip.begin(); // Initialize pins for output
  strip.show();  // Turn all LEDs off ASAP

  delayCounter = weatherDelay;
}

void loop()
{
  server.handleClient();
  switch (mode) {
    case Auto:
      server.handleClient();
      auto_mode();
      break;
    case Manual:
      server.handleClient();
      ledStrip.setColor(color);
      break;
    case Clear:
      server.handleClient();
      ledStrip.clearCloud();
      break;
    case BlueSky:
      server.handleClient();
      ledStrip.blueSky();
      break;
    case WhiteClouds:
      server.handleClient();
      ledStrip.whiteClouds();
      break;
    case Overcast:
      server.handleClient();
      ledStrip.overcast();
      break;
    case Sunset:
      server.handleClient();
      ledStrip.sunSet();
      break;
    case Rain:
      server.handleClient();
      ledStrip.rain();
      break;
    case Cloudy:
      server.handleClient();
      ledStrip.cloudy();
      break;
    default:
      server.handleClient();
      ledStrip.errorWeather();
      break;
  }
    server.handleClient();
}

void auto_mode(){

  if(WiFi.status() == WL_CONNECTED){

    Serial.print("delay counter: ");
    Serial.println(delayCounter);
    delayCounter++;

    if (delayCounter >= weatherDelay){
      Serial.println("Calling weather underground");
      if (weather.callWeatherUnderground()) {
        weather.printWeather();
        weatherDelay = DELAY_NORMAL;
        delayCounter = 0;
        // delay(DELAY_NORMAL);
      }
      else {
        // delay(DELAY_ERROR);
        Serial.println("Error Weather Underground");
        weatherDelay = DELAY_ERROR;
      }
    }

    // ledStrip.rain();
    ledStrip.weatherUpdate( weather.getWeather() );
    delay(LED_UPDATE_DELAY);
  }
  else{
    printWifiStatus();
    ledStrip.errorWeather();
    delay(LED_UPDATE_DELAY);
  }
}

void printWifiStatus() {
  if(wifi_connected == false){
    Serial.println("Not connected to access point");
    return;
  }
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());


  // Current status
  Serial.print("WiFi Status: ");
  Serial.println(WiFi.status());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm\n");
}


// Handle server request
void handleSetting() {

Serial.println("Incoming message");
  if (server.hasArg("plain")== false){ //Check if body received
    server.send(200, "text/plain", "Body not received\n\n");
    return;
  }

  DynamicJsonBuffer receiveJsonBuffer(200);
  JsonObject& data = receiveJsonBuffer.parseObject(server.arg("plain"));

  if (!data.success()) {
    Serial.println("parseObject() failed");
    server.send(200, "text/plain", "json parse error\n\n");
    return;
  }

  server.send(200, "text/plain", "received\n\n");

  data.printTo(Serial);
  mode = data["mode"];
  color = data["color"];

  if(mode == 0){
    delayCounter = weatherDelay;
  }

}


void handleRoot() {

  DynamicJsonBuffer sendJsonBuffer(200);
  JsonObject& root = sendJsonBuffer.createObject();

  root["device"] = deviceName;
  root["mode"] = mode;
  root["color"] = color;

  String jsonOutput;
  root.printTo(jsonOutput);
  root.printTo(Serial);

   server.send(200, "text/json", jsonOutput);
}

void handleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}
