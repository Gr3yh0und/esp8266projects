// Project: example for Bosch BME-280 ambient sensor delivering temperature, pressure and humidity
// Used devices: BME-280
// Michael Morscher, January 2019
// Tested on Arduino IDE 1.8.8
// Board: NodeMCU ESP8266 v3
#include <Wire.h>
#include <SPI.h>

// Additional used libraries
// Adafruit Sensor: https://github.com/adafruit/Adafruit_Sensor - tested with v1.0.2
// Adafruit BME280 Library: https://github.com/adafruit/Adafruit_BME280_Library - tested with v1.0.7
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

// Pressure/Humidity/Temperature Sensor, Type: Bosch BME-280
// Configured as software SPI using Pins D1-D4
#define BME_SCK   5    // D1 = SCL
#define BME_MISO  2    // D4 = SD0
#define BME_MOSI  4    // D2 = SDA
#define BME_CS    0    // D3 = CSB
Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK); 

// Setup Phase
void setup() {

  // Initialize Serial debug output
  Serial.begin(115200);
  Serial.print("\n\n");
  Serial.print("Setup: Starting...\n");

  // Initialize Sensors
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

  // Wait for 1 second and increase counter
  Serial.println("--- New round... ---");
  delay(1000);
}
