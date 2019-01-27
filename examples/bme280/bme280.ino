// Project: example for Bosch BME-280 ambient sensor delivering temperature, pressure and humidity
// Used devices: BME-280
// Michael Morscher, January 2019
// Tested on Arduino IDE 1.8.8
// Board: NodeMCU ESP8266 v3
#include <Wire.h>
#include <SPI.h>

#define DELAY 1000

// Additional used libraries
// Adafruit Sensor: https://github.com/adafruit/Adafruit_Sensor - tested with v1.0.2
// Adafruit BME280 Library: https://github.com/adafruit/Adafruit_BME280_Library - tested with v1.0.7
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

// Pressure/Humidity/Temperature sensor with SPI, Type: Bosch BME-280
// Configured as software SPI using Pins D5-D8
#define BME_SCK   14    // D5 = SCL
#define BME_MISO  15    // D8 = SD0
#define BME_MOSI  12    // D6 = SDA
#define BME_CS    13    // D7 = CSB
Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK);

// Setup Phase
void setup() {

  // Initialize serial debug output
  Serial.begin(115200);
  Serial.print("\n\n");
  Serial.print("Setup: Starting...\n");

  // Initialize sensor
  if (!bme.begin()) {
    Serial.println(F("Could not find a valid BME280 sensor, check wiring!"));
    while (1);
  }
  Serial.print("Setup: Successfull!\n");
}

// Measure ambient
void measureAmbient() {
  Serial.print("Temperature = ");
  Serial.print(bme.readTemperature());
  Serial.println(" *C");

  Serial.print("Pressure = ");
  Serial.print(bme.readPressure() / 100.0F);
  Serial.println(" hPa");

  Serial.print("Humidity = ");
  Serial.print(bme.readHumidity());
  Serial.println(" %");
}

// Endless program loop
void loop() {

  // Measure sensor values
  measureAmbient();

  // Wait until next measurement
  delay(DELAY);
}
