// Simple example for writing text to an OLED display via I2C
// Used devices: SSD1306 via I2C
// Michael Morscher, Juli 2020
// Tested on Arduino IDE 1.8.12
// Board: NodeMCU ESP8266 v3
// Based on: https://github.com/ThingPulse/esp8266-oled-ssd1306/blob/master/examples/SSD1306SimpleDemo/SSD1306SimpleDemo.ino
#include <Wire.h>

// Additional used libraries
// ESP8266-OLED-SSD1306: https://github.com/squix78/esp8266-oled-ssd1306 - tested with v4.1.0
#include "SSD1306Wire.h"        // legacy: #include "SSD1306.h"

// Configure Display, Type: SSD1306 I2C
SSD1306Wire display(0x3c, SDA, SCL);   // ADDRESS, SDA, SCL  -  SDA and SCL usually populate automatically based on your board's pins_arduino.h
SSD1306Wire display2(0x3d, SDA, SCL);   // ADDRESS, SDA, SCL  -  SDA and SCL usually populate automatically based on your board's pins_arduino.h

int counter = 0;

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println();


  // Initialising the UI will init the display too.
  display.init();
  display2.init();

  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display2.flipScreenVertically();
  display2.setFont(ArialMT_Plain_10);

}

// Print data on the display
void outputToDisplay() {
  
  // Erase old content
  display.clear();
  display2.clear();
    
  // Set layout of text, left aligned, Arial 10px
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  display2.setTextAlignment(TEXT_ALIGN_LEFT);
  display2.setFont(ArialMT_Plain_10);

  // Write output to buffer
  // Control display
  display.drawString(0, 10, "Runtime | " + String(counter) + "m, " + String(counter) + "m");
  display.drawString(0, 20, "Sensors | 70.25, 75.25");
  display.drawString(0, 30, "Average | 72.5");
  display.drawString(0, 40, "Target    |  99.0 | +19.25");
  display.drawString(0, 50, "Power    |  20% - L1");

  // System status display
  display2.drawString(0, 10, "WiFi = ON, MQTT = ON");
  display2.drawString(0, 20, "Relay = ON, Power = ON");
  display2.drawString(0, 30, "PID = ON, (42-0.2-7.2)");
  display2.drawString(0, 40, "Sensors (2)");
  display2.drawString(0, 50, "");

    
  // Print buffer on display
  display.display();
  display2.display();
}

// Endless program loop
void loop() {
  


  // Write data to display
  outputToDisplay();

  // Wait for 1 second and increase counter
  Serial.printf("--- New round... ---\n");
  delay(1000);
  counter++;  
}
