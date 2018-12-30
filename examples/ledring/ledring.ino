// Project for lighting a WS2812 based LED ring
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

#define RED     strip.Color(255, 0, 0)
#define YELLOW  strip.Color(255, 255, 0)
#define BLUE    strip.Color(0, 0, 255)
#define GREEN   strip.Color(0, 255, 0)
#define WHITE   strip.Color(255, 255, 255)

Adafruit_NeoPixel strip = Adafruit_NeoPixel(LEDS_NUMBER, LEDS_PIN, NEO_GRB + NEO_KHZ800);

// Setup Phase
void setup() {

  // Serial configuration
  Serial.begin(115200);

  // Example description
  Serial.print("Example for WS2812 LED ring using Pin '");
  Serial.print(LEDS_PIN);
  Serial.print("'\n");

  // LED initialization
  strip.begin();
  strip.setBrightness(LEDS_BRIGHTNESS);
  strip.show();

  Serial.println("Setup done!");
}

void loop() {
  // Some example procedures showing how to display to the pixels:
  movingDot(WHITE, 150);
  colorBlink(RED, 2, 100);
  colorRotate(YELLOW, 2, 100);
  colorRotateChanging(100);
  colorRotateChanging2(6000, 100);
  Serial.println("Loop Done!");
}

// Show 4 base colors moving around the circle
void colorRotateChanging(uint8_t wait) {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.clear();
    strip.setPixelColor(i, RED);
    strip.setPixelColor(i + 1, YELLOW);
    strip.setPixelColor(i + 2, BLUE);
    strip.setPixelColor(i + 3, GREEN);
    strip.show();
    delay(wait);
  }
}

// Show 4 base colors moving around the circle for a given period
void colorRotateChanging2(uint16_t duration, uint8_t wait) {
  uint8_t k = 0;
  for (uint16_t i = 0; i <= (duration / wait); i++) {
    strip.clear();
    strip.setPixelColor(k % 12, RED);
    strip.setPixelColor(k % 12 + 1, YELLOW);
    strip.setPixelColor(k % 12 + 2, BLUE);
    strip.setPixelColor(k % 12 + 3, GREEN);
    if (k % 9 == 0) {
      strip.setPixelColor(0, GREEN);
    }
    if (k % 10 == 0) {
      strip.setPixelColor(0, BLUE);
      strip.setPixelColor(1, GREEN);
    }
    if (k % 11 == 0) {
      strip.setPixelColor(0, YELLOW);
      strip.setPixelColor(1, BLUE);
      strip.setPixelColor(2, GREEN);
    }
    strip.show();
    delay(wait);
    k++;
    if (k == 12) {
      k = 0;
    }
  }
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  Serial.println("Mode: ColorWipe");
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

// Only lit up one LED after each other so that it looks like its wandering around the ring (e.g. good waiting animation)
void movingDot(uint32_t c, uint8_t wait) {
  Serial.println("Mode: MovingDot");
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.clear();
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

// Let all LEDs blink in a certain frequency
void colorBlink(uint32_t c, uint8_t blinks, uint8_t wait) {
  Serial.println("Mode: ColorBlink");
  for (uint8_t k = 0; k < blinks; k++) {
    strip.clear();
    strip.show();
    delay(wait);
    for (uint16_t i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
    }
    strip.show();
    delay(wait * 4);
  }
  strip.clear();
  strip.show();
}

// Blink every second LED and change it in the next iteration (even/uneven/even/... and so on)
void colorRotate(uint32_t c, uint8_t blinks, uint8_t wait) {
  Serial.println("Mode: ColorRotate");
  for (uint8_t k = 0; k < blinks; k++) {
    strip.clear();
    strip.show();
    delay(wait);
    for (uint16_t i = 0; i < strip.numPixels(); i++) {
      if (i % 2 == k % 2) {
        strip.setPixelColor(i, c);
      }
    }
    strip.show();
    delay(wait * 2);
  }
  strip.clear();
  strip.show();
}
