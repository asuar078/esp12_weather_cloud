#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <Adafruit_DotStar.h>
#include <SPI.h>

#include "wifi_credentials.h"
#include "W_Underground.h"

#define NUMPIXELS 60 // Number of LEDs in strip

#define DATAPIN    D4
#define CLOCKPIN   D5

bool wifi_connected = true;

Adafruit_DotStar strip = Adafruit_DotStar(
  NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BRG);


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

// Runs 10 LEDs at a time along strip, cycling through red, green and blue.
// This requires about 200 mA for all the 'on' pixels + 1 mA per 'off' pixel.
int      head  = 0, tail = -10; // Index of first 'on' and 'off' pixels
uint32_t color = 0xFF0000;      // 'On' color (starts red)

void loop()
{
  // TODO check for disconnect from AP

  strip.setPixelColor(head, color); // 'On' pixel at head
  strip.setPixelColor(tail, 0);     // 'Off' pixel at tail
  strip.show();                     // Refresh strip
  delay(20);                        // Pause 20 milliseconds (~50 FPS)

  if(++head >= NUMPIXELS) {         // Increment head index.  Off end of strip?
    head = 0;                       //  Yes, reset head index to start
    if((color >>= 8) == 0)          //  Next color (R->G->B) ... past blue now?
      color = 0xFF0000;             //   Yes, reset to red
  }
  if(++tail >= NUMPIXELS) tail = 0; // Increment, reset tail index

  // if(WiFi.status() == WL_CONNECTED){
  //
  //   if (weather.callWeatherUnderground()) {
  //     weather.printWeather();
  //     delay(DELAY_NORMAL);
  //   }
  //   else {
  //     delay(DELAY_ERROR);
  //   }
  // }
  // else{
  //   printWifiStatus();
  // }

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
