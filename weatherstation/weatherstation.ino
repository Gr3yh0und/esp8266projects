// Example project for a weather station with attached display
// Michael Morscher, March 2017
// Tested on Arduino IDE 1.8.1
// Board: NodeMCU ESP8266 v3
#include <Wire.h>

// Additional used libraries
// Adafruit Sensor: https://github.com/adafruit/Adafruit_Sensor
// Adafruit BMP280 Library: https://github.com/adafruit/Adafruit_BMP280_Library
// ESP8266-Arduino-SHT21: https://github.com/vincasmiliunas/ESP8266-Arduino-SHT21
// ESP8266-OLED-SSD1306: https://github.com/squix78/esp8266-oled-ssd1306
#include "SSD1306.h"
#include "SSD1306Spi.h"
#include <SHT21.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

// Configure Display, Type: SSD1306 SPI
// D3 = RES/RST/RESET
// D4 = DC
// D8 = CS
SSD1306Spi display(D3, D4, D8);

// Configure Light Sensor, Type: GL55
// A0 = DATA
int analogPin = 0;

// Configure Temperature/Humidity Sensor, Type: SHT21 / SI7021
// D2 = SDA 
// D1 = SCL
Sht21 sht21;

// Configure Pressure Sensor, Type: Bosch BMP-280
// D2 = SDA 
// D1 = SCL
Adafruit_BMP280 bmp;
#define BMP_SCK 13
#define BMP_MISO 12
#define BMP_MOSI 11 
#define BMP_CS 10

// Global variables
int counter = 0;
int brightness;
char temperature[5];
char temperature2[5];
char humidity[5];
char pressure[5];

// Setup Phase
void setup() {
  
  // Initialize Serial debug output
  Serial.begin(115200);
  Serial.print("\n\n");
  Serial.print("Setup: Starting...\n");

  // Initialize Sensors
  sht21.begin(SDA, SCL);
  Wire.begin(SDA,SCL);
  if (!bmp.begin()) {  
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    while (1);
  }

  // Initialising the UI will init the display too.
  display.init();
  display.flipScreenVertically();
  Serial.print("Setup: Successfull!\n");
}

// Print data on the display
void outputToDisplay() {
  
  // Erase old content
  display.clear();
    
  // Set layout of text, left aligned, Arial 10px
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);

  // Write output to buffer
  display.drawString(0, 0, "Uptime: " + String(counter) + "s");
  display.drawString(0, 11, "Brightness: " + String(brightness));
  display.drawString(0, 21, "Temperature: " + String(temperature) + " °C");
  display.drawString(0, 31, "Temperature2: " + String(temperature2) + " °C");
  display.drawString(0, 41, "Humidity: " + String(humidity) + " %");
  display.drawString(0, 51, "Pressure: " + String(pressure) + " hPa");
    
  // Print buffer on display
  display.display();
}

// Measure light intensity
void measureLight() {
  brightness = analogRead(analogPin);
  Serial.print("Measuring brightness: " + String(brightness) + "\n");
}

// Measure temperature and humidity
void measureTempHum() {
  float temp, hum;

  // if sensor is functioning correctly
  if (sht21.measure(temp, hum)) {
    // Convert double to string
    dtostrf(temp, 4, 1, temperature);
    dtostrf(hum, 4, 1, humidity);
    Serial.printf("Measuring temperature: ");
    Serial.printf(temperature);
    Serial.printf(" C\nMeasuring humidity: ");
    Serial.printf(humidity);
    Serial.printf(" %\n");
  } else {
    Serial.printf("Measurement failed.\n");
  }
}

// Measure current air pressure
void measurePressure() {
  // Convert double to string
  dtostrf(bmp.readPressure()/100, 4, 1, pressure); // in hPa
  dtostrf(bmp.readTemperature(), 4, 1, temperature2);
  Serial.printf("Measuring pressure: ");
  Serial.print(bmp.readPressure()/100);
  Serial.printf(" hPa\n");
  Serial.printf("Measuring temperature2: ");
  Serial.print(bmp.readTemperature());
  Serial.println(" C");
}

// Endless program loop
void loop() {
  
  // Measure sensor values
  measureLight();
  measureTempHum();
  measurePressure();

  // Write data to display
  outputToDisplay();

  // Wait for 1 second and increase counter
  Serial.printf("--- New round... ---\n");
  delay(1000);
  counter++;  
}
