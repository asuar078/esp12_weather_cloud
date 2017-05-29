#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

#include "wifi_credentials.h"
#include "W_Underground.h"

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

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }
  Serial.println();

  printWifiStatus();
}

void loop()
{
  // TODO check for disconnect from AP

  if(WiFi.status() == WL_CONNECTED){

    if (weather.callWeatherUnderground()) {
      weather.printWeather();
      delay(DELAY_NORMAL);
    }
    else {
      delay(DELAY_ERROR);
    }
  }
  else{
    printWifiStatus();
  }
}

void printWifiStatus() {
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
