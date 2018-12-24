// Project for lighting a LED ring
// Used devices: 12-LED WS2812B LED-Ring
// Michael Morscher, December 2018
// Tested on Arduino IDE 1.8.8
// Board: Arduino Nano

// Additional used libraries
// Adafruit_NeoPixel: https://github.com/adafruit/Adafruit_NeoPixel
#include <Adafruit_NeoPixel.h>

// Define PIN for the LED Ring, amount of LEDs and the Brightness
#define LEDS_PIN 2
#define LEDS_NUMBER 12
#define LEDS_BRIGHTNESS 1

Adafruit_NeoPixel strip = Adafruit_NeoPixel(LEDS_NUMBER, LEDS_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  // LED initialization
  strip.begin();
  strip.setBrightness(LEDS_BRIGHTNESS);
  strip.show();
}

void loop() {
  // Some example procedures showing how to display to the pixels:
  movingDot(strip.Color(255, 255, 255), 150); // White
  colorBlink(strip.Color(0, 0, 255), 2, 100);
  colorRotate(strip.Color(0, 0, 255), 2, 100);
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

// Only lit up one LED after each other so that it looks like its wandering around the ring (e.g. good waiting animation)
void movingDot(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.clear();
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

// Let all LEDs blink in a certain frequency
void colorBlink(uint32_t c, uint8_t blinks, uint8_t wait) {
  for(uint8_t k=0; k<blinks; k++) {
    strip.clear();
    strip.show();
    delay(wait);
    for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
    }
    strip.show();
    delay(wait*4);
  }
  strip.clear();
  strip.show();
}

// Blink every second LED and change it in the next iteration (even/uneven/even/... and so on)
void colorRotate(uint32_t c, uint8_t blinks, uint8_t wait) {
  for (uint8_t k = 0; k < blinks; k++) {
    strip.clear();
    strip.show();
    delay(wait);
    for (uint16_t i = 0; i < strip.numPixels(); i++) {
      if(i%2 == k%2) {
        strip.setPixelColor(i, c);
      }
    }
    strip.show();
    delay(wait * 2);
  }
  strip.clear();
  strip.show();
}
