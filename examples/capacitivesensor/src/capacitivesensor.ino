// Project for detecting if someone touches a metallic surface
// Used devices: 1M ohm resistor between both pins, metallic thread on the second pin which is used to trigger the example
// Michael Morscher, December 2018
// Tested on Arduino IDE 1.8.8
// Board: Arduino Nano

// Additional used libraries
// CapacitiveSensor: https://github.com/PaulStoffregen/CapacitiveSensor - tested with v0.5.1
#include <CapacitiveSensor.h>

// Define the two pins to use
#define PIN_1 5
#define PIN_2 6

// Define sensitivity and when an event should be triggered
#define SENSITIVITY 500
#define SENSITIVITY_THRESHOLD 1000

// 1M resistor between pins, surface for resistance added to second pin
CapacitiveSensor capSensor = CapacitiveSensor(PIN_1, PIN_2);

// Setup Phase
void setup() {

  // Serial configuration
  Serial.begin(115200);
  Serial.print("Project version: ");
  Serial.println(version);
  Serial.println("Setup initialised!");

  // Turn off autocalibrate on channel 1
  capSensor.set_CS_AutocaL_Millis(0xFFFFFFFF);
}

void loop()
{
  long sensor =  capSensor.capacitiveSensor(SENSITIVITY);

  if (sensor >= SENSITIVITY_THRESHOLD) {
    Serial.print(sensor);
    Serial.print(" - ACTIVE\n");
  }
}
