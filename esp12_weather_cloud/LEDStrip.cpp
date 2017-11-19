#include "LEDStrip.h"

LEDStrip::LEDStrip(Adafruit_DotStar &_strip) : strip(_strip){
}

void LEDStrip::weatherUpdate(String weather){

    if(weather.substring(0) == "Clear") { blueSky(); return; }
    if(weather.substring(0) == "Overcast") { overcast(); return;}
    if(weather.substring(0) == "Cloudy`") { cloudy(); return;}
    if(weather.substring(0) == "Clouds`") { whiteClouds(); return;}
    if(weather.substring(0) == "Rain") { rain(); return;}
    if(weather.substring(0) == "Thunderstorms") { rain(); return;}

    overcast();
}

// Clear cloud function:
void LEDStrip::clearCloud(){
  for (int i = 0; i < strip.numPixels(); i++){ //for all the LEDs
    strip.setPixelColor(i, 0, 0, 0); //turn off in cloud one
  }
  strip.show(); //show what was set in cloud one
}

// Bluesky function
// DOTSTAR_GRB
void LEDStrip::blueSky() {
  for(int i = 0; i < strip.numPixels(); i++) {    //for all of the LEDs
    strip.setPixelColor(i, 140, 70, 255);   //set LEDs a sky blue color in cloud one
  }
  strip.show();
}

// White clouds function
void LEDStrip::whiteClouds() {
  for(int i=0; i<strip.numPixels(); i++) {   //for all of the LEDs
    strip.setPixelColor(i, 200, 200, 200);  //set LEDs white-ish in cloud one
  }
  strip.show();
}

// Overcast funtion
void LEDStrip::overcast() {
  for(int i=0; i<strip.numPixels(); i++) {   //for all of the LEDs
    strip.setPixelColor(i, 80, 80, 80);  //set LEDs grey-ish in cloud one
   }
   strip.show();
}

// Sun set function:
// DOTSTAR_GRB
void LEDStrip::sunSet(){
  int i;
  for(i=0; i< 15; i++) {   //for the first 100 LEDs
    strip.setPixelColor(i, 10, 255, 10);   //set LEDs red-ish in cloud one
  }
  for(i=15; i<30; i++) {  //for LEDs 101 to 150
    strip.setPixelColor(i, 100, 235, 0);  //set LEDs orange-red in cloud one
  }
  for(i=30; i<45; i++) {  //for LEDs 151 to 250
    strip.setPixelColor(i, 100, 235, 0);  //set LEDs red-orage in cloud one
  }
  for(i=45; i<60; i++) {  //for LEDs 251 to 260
    // strip.setPixelColor(i, 0, 0, 255);  //set LEDs red-orage in cloud one
    strip.setPixelColor(i, 10, 255, 10);   //set LEDs red-ish in cloud one
  }
    strip.show(); //show all the colors that were set in cloud one
}

// DOTSTAR_GRB
void LEDStrip::rain(void){
  int ran = random(0, 2);
  int i;
  if(ran == 0){
    for(i=0; i< 10; i++) {
      strip.setPixelColor(i, 30, 30, 30);
    }
    for(i=10; i< 20; i++) {
      strip.setPixelColor(i, 10, 10, 10);
    }
    for(i=20; i<30; i++) {
      strip.setPixelColor(i, 255, 255, 255);
    }
    for(i=30; i<50; i++) {
      strip.setPixelColor(i, 80, 80, 80);
    }
    for(i=50; i<60; i++) {
      strip.setPixelColor(i, 100, 100, 100);
    }
  }
  if(ran == 1){
    for(i=0; i< 10; i++) {
      strip.setPixelColor(i, 30, 30, 30);
    }
    for(i=10; i< 20; i++) {
      strip.setPixelColor(i, 80, 80, 80);
    }
    for(i=20; i<30; i++) {
      strip.setPixelColor(i, 10, 10, 10);
    }
    for(i=30; i<50; i++) {
      strip.setPixelColor(i, 80, 80, 80);
    }
    for(i=50; i<60; i++) {
      strip.setPixelColor(i, 255, 255, 255);
    }
  }
  if(ran == 2){
    for(i=0; i< 10; i++) {
      strip.setPixelColor(i, 255, 255, 255);
    }
    for(i=10; i< 20; i++) {
      strip.setPixelColor(i, 100, 100, 100);
    }
    for(i=20; i<30; i++) {
      strip.setPixelColor(i, 255, 255, 255);
    }
    for(i=30; i<50; i++) {
      strip.setPixelColor(i, 80, 80, 80);
    }
    for(i=50; i<60; i++) {
      strip.setPixelColor(i, 10, 10, 10);
    }
  }

  strip.show();
}

void LEDStrip::thunderStorm(void){

}

void LEDStrip::cloudy(void){
  int i;
  for(i=0; i< 15; i++) {
    strip.setPixelColor(i, 200, 200, 200);  //set LEDs white-ish in cloud one
  }
  for(i=15; i<30; i++) {
    strip.setPixelColor(i, 140, 70, 255);   //set LEDs a sky blue color in cloud one
  }
  for(i=30; i<45; i++) {
    strip.setPixelColor(i, 200, 200, 200);  //set LEDs white-ish in cloud one
  }
  for(i=45; i<60; i++) {
    strip.setPixelColor(i, 140, 70, 255);   //set LEDs a sky blue color in cloud one
  }
    strip.show();
}

void LEDStrip::errorWeather(void){
  int i;
  for(i=0; i< strip.numPixels(); i++) {
    strip.setPixelColor(i, 10, 255, 10);
  }
    strip.show(); //show all the colors that were set in cloud one
}

void LEDStrip::setColor(int color){

  byte red = ((color >> 16) & 0xFF);  // Extract the RR byte
  byte green = ((color >> 8) & 0xFF);   // Extract the GG byte
  byte blue = ((color) & 0xFF);        // Extract the BB byte

  // todo: remove
  // String colors = "red " + (String)red + " green " + (String)green + " blue " + (String)blue;
  // Serial.println(colors);

  for (int i = 0; i < strip.numPixels(); i++){ //for all the LEDs
    strip.setPixelColor(i, green, red, blue);
  }

  strip.show();
}
