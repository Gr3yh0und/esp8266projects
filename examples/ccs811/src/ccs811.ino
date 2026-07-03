// Project: example for CCS811 CO2 sensor
// based on https://github.com/adafruit/Adafruit_CCS811/blob/master/examples/CCS811_test/CCS811_test.ino
// Used devices: CCS811
// Michael Morscher, January 2019
// Tested on Arduino IDE 1.8.8
// Board: NodeMCU ESP8266 v3

// Additional used libraries
// Adafruit CCS811: https://github.com/adafruit/Adafruit_CCS811 - tested with
#include <Adafruit_CCS811.h>

#define DELAY 10000

// Light Sensor, Type: CCS811
// D1 = SCL
// D2 = SDA
Adafruit_CCS811 ccs;

// Setup Phase
void setup() {

  // Initialize Serial debug output
  Serial.begin(115200);
  Serial.print("\n\n");
  Serial.print("Setup: Starting...\n");

  // Initialize sensor
  if (!ccs.begin()) {
    /* There was a problem detecting the CCS811 ... check your connections */
    Serial.print("Ooops, no CCS811 detected ... Check your wiring or I2C ADDR!");
    while (1);
  }

  // Configure sensor
  while (!ccs.available());
  float temp = ccs.calculateTemperature();
  ccs.setTempOffset(temp - 26.0);

  Serial.print("Setup: Successfull!\n");
}

// Endless program loop
void loop() {

  if (ccs.available()) {
    float temp = ccs.calculateTemperature();
    if (!ccs.readData()) {
      Serial.print("CO2: ");
      Serial.print(ccs.geteCO2());
      Serial.print("ppm, TVOC: ");
      Serial.print(ccs.getTVOC());
      Serial.print("ppb   Temp:");
      Serial.println(temp);
    }
  }

  // Wait
  delay(DELAY);
}
