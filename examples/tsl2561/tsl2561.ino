// Project: example for TSL-2561 light sensor
// based on https://github.com/adafruit/Adafruit_TSL2561/blob/master/examples/sensorapi/sensorapi.ino
// Used devices: TSL-2561
// Michael Morscher, January 2019
// Tested on Arduino IDE 1.8.8
// Board: NodeMCU ESP8266 v3
#include <Wire.h>
#include <SPI.h>

// Additional used libraries
// Adafruit Sensor: https://github.com/adafruit/Adafruit_Sensor - tested with v1.0.2
// Adafruit TSL2561 Library: https://github.com/adafruit/Adafruit_TSL2561.git - tested with v1.0.2
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>

// Light Sensor, Type: TSL-2561
// D1 = SCL
// D2 = SDA
Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);

#define DELAY 1000

// Setup Phase
void setup() {

  // Initialize Serial debug output
  Serial.begin(115200);
  Serial.print("\n\n");
  Serial.print("Setup: Starting...\n");

  // Initialize sensor
  if (!tsl.begin()) {
    /* There was a problem detecting the TSL2561 ... check your connections */
    Serial.print("Ooops, no TSL2561 detected ... Check your wiring or I2C ADDR!");
    while (1);
  }

  // Configure sensor
  tsl.enableAutoRange(true); /* Auto-gain ... switches automatically between 1x and 16x */
  tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS); /* fast but low resolution */
  Serial.print("Setup: Successfull!\n");
}

// Endless program loop
void loop() {

  // Get a new sensor event
  sensors_event_t event;
  tsl.getEvent(&event);

  // Output
  if (event.light) {
    Serial.print(event.light); 
    Serial.println(" lux");
  } else {
    Serial.println("Sensor overload");
  }

  // Wait
  Serial.println("--- New round... ---");
  delay(DELAY);
}
