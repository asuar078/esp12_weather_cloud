#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <Adafruit_DotStar.h>
#include <SPI.h>

#include "wifi_credentials.h"
#include "W_Underground.h"
#include "LEDStrip.h"

#define NUMPIXELS 60 // Number of LEDs in strip
#define DATAPIN    D4
#define CLOCKPIN   D5

bool wifi_connected = true;

Adafruit_DotStar strip = Adafruit_DotStar(
  NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BRG);

LEDStrip ledStrip(strip);

WiFiClient httpclient;
W_Underground weather(httpclient);

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

  strip.begin(); // Initialize pins for output
  strip.show();  // Turn all LEDs off ASAP
}

uint16_t delayCounter = 0;

#define LED_UPDATE_DELAY 2000

// 5 minutes between update checks. The free developer account has a limit
// on the  number of calls so don't go wild.
// #define DELAY_NORMAL    (5*60*1000)
const uint16_t DELAY_NORMAL = ((1*60*1000) / LED_UPDATE_DELAY);

// 20 minute delay between updates after an error
// #define DELAY_ERROR     (20*60*1000)
const uint16_t DELAY_ERROR = ((20*60*1000) / LED_UPDATE_DELAY);

uint16_t weatherDelay = DELAY_NORMAL;

void loop()
{

  if(WiFi.status() == WL_CONNECTED){

    Serial.print("delay counter: ");
    Serial.println(delayCounter);
    delayCounter++;

    if (delayCounter >= weatherDelay){
      if (weather.callWeatherUnderground()) {
        weather.printWeather();
        weatherDelay = DELAY_NORMAL;
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
