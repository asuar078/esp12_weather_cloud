#ifndef LEDSTRIP_H
#define LEDSTRIP_H

#include <Arduino.h>
#include <Adafruit_DotStar.h>
#include <SPI.h>

class LEDStrip {
private:
  Adafruit_DotStar strip;

public:
  LEDStrip(Adafruit_DotStar &_strip);
  void weatherUpdate(String weather);

  void clearCloud(void);
  void blueSky(void);
  void whiteClouds(void);
  void overcast(void);
  void sunSet(void);
  void rain(void);
  void cloudy(void);
  void errorWeather(void);
  void setColor(int color);

  // todo
  void thunderStorm(void);
  void disco(void);
};

#endif
