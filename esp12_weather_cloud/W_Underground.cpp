#include "W_Underground.h"

static char respBuf[4096];

W_Underground::W_Underground(WiFiClient &client) :  _client(client){
}

bool W_Underground::callWeatherUnderground(){

  // Open socket to WU server port 80
  Serial.print(F("Connecting to "));
  Serial.println(WUNDERGROUND);

  // Use WiFiClient class to create TCP connections
  if (!_client.connect(WUNDERGROUND, HTTP_PORT)) {
    Serial.println(F("connection failed"));
    // delay(DELAY_ERROR);
    return false;
  }

  // This will send the http request to the server
  Serial.print(WUNDERGROUND_REQ);

  _client.print(WUNDERGROUND_REQ);
  _client.flush();

  // Collect http response headers and content from Weather Underground
  // HTTP headers are discarded.
  // The content is formatted in JSON and is left in respBuf.
  int respLen = 0;
  bool skip_headers = true;

  while (_client.connected() || _client.available()) {
    if (skip_headers) {
      String aLine = _client.readStringUntil('\n');
      //Serial.println(aLine);
      // Blank line denotes end of headers
      if (aLine.length() <= 1) {
        skip_headers = false;
      }
    }
    else {
      int bytesIn;
      bytesIn = _client.read((uint8_t *)&respBuf[respLen], sizeof(respBuf) - respLen);
      // Serial.print(F("bytesIn ")); Serial.println(bytesIn);
      if (bytesIn > 0) {
        respLen += bytesIn;
        if (respLen > sizeof(respBuf)) respLen = sizeof(respBuf);
      }
      else if (bytesIn < 0) {
        Serial.print(F("read error "));
        Serial.println(bytesIn);
      }
    }
    delay(1);
  }
  _client.stop();

  if (respLen >= sizeof(respBuf)) {
    Serial.print(F("respBuf overflow "));
    Serial.println(respLen);
    // delay(DELAY_ERROR);
    return false;
  }
  // Terminate the C string
  respBuf[respLen++] = '\0';
  // Serial.print(F("respLen "));
  // Serial.println(respLen);

  // older version
  // if (showWeather(respBuf)) {
  //   delay(DELAY_NORMAL);
  // }
  // else {
  //   delay(DELAY_ERROR);
  // }

  if (extractWeather(respBuf)) {
    return true;
  }
  else {
    return false;
  }

}

bool W_Underground::extractWeather(char *json){
  StaticJsonBuffer<3*1024> jsonBuffer;

  // Skip characters until first '{' found
  // Ignore chunked length, if present
  char *jsonstart = strchr(json, '{');
  // uncomment to see entire json response
  // Serial.print(F("jsonstart ")); Serial.println(jsonstart);
  if (jsonstart == NULL) {
    debug_print("JSON data missing"); debug_print("\n");
    return false;
  }
  json = jsonstart;

  // Parse JSON
  JsonObject& root = jsonBuffer.parseObject(json);
  if (!root.success()) {
    debug_print("jsonBuffer.parseObject() failed"); debug_print("\n");
    return false;
  }

  // Extract weather info from parsed JSON
  JsonObject& current = root["current_observation"];

  const float temp_f = current["temp_f"];
  setTempF(temp_f);

  const float temp_c = current["temp_c"];
  setTempC(temp_c);

  const char *humi = current[F("relative_humidity")];
  setRH(humi);

  const char *weather = current["weather"];
  setWeather(weather);

  const char *observation_time = current["observation_time_rfc822"];
  setObTime(observation_time);

  return true;
}

void W_Underground::printWeather(){
  Serial.print(F("Temperature: ")); Serial.print(getTempF()); Serial.print(" F, ");
    Serial.print(getTempC()); Serial.print(" C, RH: "); Serial.println(getRelativeHumidity());

  Serial.print("Weather: "); Serial.println(getWeather());
  Serial.print("Time: "); Serial.println(getObTime());
  Serial.println();
}

bool W_Underground::showWeather(char *json) {
  StaticJsonBuffer<3*1024> jsonBuffer;

  // Skip characters until first '{' found
  // Ignore chunked length, if present
  char *jsonstart = strchr(json, '{');
  // Serial.print(F("jsonstart ")); Serial.println(jsonstart);
  if (jsonstart == NULL) {
    Serial.println(F("JSON data missing"));
    return false;
  }
  json = jsonstart;

  // Parse JSON
  JsonObject& root = jsonBuffer.parseObject(json);
  if (!root.success()) {
    Serial.println(F("jsonBuffer.parseObject() failed"));
    return false;
  }

  // Extract weather info from parsed JSON
  JsonObject& current = root["current_observation"];
  const float temp_f = current["temp_f"];
  debug_print(temp_f); debug_print(" F, ");
  const float temp_c = current["temp_c"];
  debug_print(temp_c); debug_print(" C, ");
  const char *humi = current[F("relative_humidity")];
  debug_print(humi);   debug_print(" RH\n");
  const char *weather = current["weather"];
  debug_print(weather); debug_print("\n");
  const char *pressure_mb = current["pressure_mb"];
  debug_print(pressure_mb); debug_print("\n");
  const char *observation_time = current["observation_time_rfc822"];
  debug_print(observation_time); debug_print("\n");

  // Extract local timezone fields
  const char *local_tz_short = current["local_tz_short"];
  debug_print(local_tz_short);debug_print("\n");
  const char *local_tz_long = current["local_tz_long"];
  debug_print(local_tz_long);debug_print("\n");
  const char *local_tz_offset = current["local_tz_offset"];
  debug_print(local_tz_offset);debug_print("\n");

  return true;
}

void W_Underground::setTempF(float tmp){
  _temp_f = tmp;
}

void W_Underground::setTempC(float tmp){
  _temp_c = tmp;
}

void W_Underground::setRH(const char *rh){
  _relative_humidity = String(rh);
}

void W_Underground::setWeather(const char *weather){
  _weather = String(weather);
}

void W_Underground::setObTime(const char *obTime){
  _ob_time = String(obTime);
}

float W_Underground::getTempF(){
  return _temp_f;
}

float W_Underground::getTempC(){
  return _temp_c;
}

String W_Underground::getRelativeHumidity(){
  return _relative_humidity;
}

String W_Underground::getWeather(){
  return _weather;
}

String W_Underground::getObTime(){
  return _ob_time;
}
