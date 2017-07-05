#ifndef W_UNDERGROUND_H
#define W_UNDERGROUND_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

// Use your own API key by signing up for a free developer account.
// http://www.wunderground.com/weather/api/
#define WU_API_KEY "4ac8a47baa42e48f"

// US ZIP code
#define WU_LOCATION "32953"



#define WUNDERGROUND "api.wunderground.com"

#define HTTP_PORT 80

#define DEBUG 1

#ifndef DEBUG
#define debug_print(a)
#else
#define debug_print(a) Serial.print(a)
#endif

// HTTP request
const char WUNDERGROUND_REQ[] =
    "GET /api/" WU_API_KEY "/conditions/q/" WU_LOCATION ".json HTTP/1.1\r\n"
    "User-Agent: ESP8266/0.1\r\n"
    "Accept: */*\r\n"
    "Host: " WUNDERGROUND "\r\n"
    "Connection: close\r\n"
    "\r\n";

class W_Underground {
private:
  float _temp_f = 0, _temp_c = 0;
  String _weather = "", _ob_time = "", _relative_humidity = "";

  WiFiClient _client;
  bool printJsonResponse = false;
  void setTempF(float tmp);
  void setTempC(float tmp);
  void setRH(const char *rh);
  void setWeather(const char *weather);
  void setObTime(const char *obTime);
  bool showWeather(char *json);

public:
  W_Underground(WiFiClient &client);
  bool callWeatherUnderground();
  bool extractWeather(char *json);
  void printWeather();

  float getTempF();
  float getTempC();
  String getRelativeHumidity();
  String getWeather();
  String getObTime();
};

#endif
