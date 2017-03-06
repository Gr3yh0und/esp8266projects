// Libraries
#include <Wire.h>
#include "SSD1306.h"
#include "SSD1306Spi.h"
#include <SHT21.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>


// Configure Display
// D1 = RES/RST/RESET
// D2 = DC
// D8 = CS
SSD1306Spi  display(D3, D4, D8);

// Configure Light Sensor
int analogPin = 0;

// Configure Temperature/Humidity Sensor
Sht21 sht21;

// Configure Pressure
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

void setup() {
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

void outputToDisplay() {
    display.clear();
    
    // Set layout
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

void measureLight() {
  brightness = analogRead(analogPin);
  Serial.print("Measuring brightness: " + String(brightness) + "\n");
}

void measureTempHum() {
  float temp, hum;
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

void measurePressure() {
  dtostrf(bmp.readPressure()/100, 4, 1, pressure);
  dtostrf(bmp.readTemperature(), 4, 1, temperature2);
  Serial.printf("Measuring pressure: ");
  Serial.print(bmp.readPressure()/100);
  Serial.printf(" hPa\n");
  Serial.printf("Measuring temperature2: ");
  Serial.print(bmp.readTemperature());
  Serial.println(" C");
}

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
