// Project for dust particles on the NodeMCU/ESP8266 platform
// Used devices: Nova SDS011 dust sensor, SSD1306 display
// Michael Morscher, May 2018
// Tested on Arduino IDE 1.8.5
// Board: NodeMCU ESP8266 v3
#include <Wire.h>

// Additional used libraries
// ESP8266-OLED-SSD1306: https://github.com/squix78/esp8266-oled-ssd1306 - tested with v4.0.0
// SDS011: https://github.com/ricki-z/SDS011 - tested with master@09.04.2018
#include <SPI.h>
#include "SSD1306.h"
#include "SSD1306Spi.h"
#include <SDS011.h>

// Configure Display, Type: SSD1306 using SPI
// Fixed pins:
// D5 = D0 
// D7 = D1
// Variable pins:
// D3 = RES/RST/RESET
// D4 = DC
// D8 = CS
SSD1306Spi display(D3, D4, D8);

// Configure SDS011 sensor including pins D1 and D2
#define SDS_RX D1
#define SDS_TX D2
#define SDS_WARMUP_TIME 5000
#define SDS_SLEEP_TIME 10000
SDS011 sds;
float p10, p25;

// Global variables
int counter = 0;                      // Uptime

// Setup Phase
void setup()
{
  // Initialize dust sensor
  Serial.print("Setup: Initializing dust sensor\n");  
  sds.begin(SDS_RX,SDS_TX);
  
  // Initialize Serial debug output
  Serial.begin(115200);
  Serial.print("\n\n");

  // Initialising the UI will init the display too.
  Serial.print("Setup: Initializing display\n");  
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
  display.drawString(0, 11, "PM 2.5 particles: " + String(p25));
  display.drawString(0, 21, "PM 10 particles: " + String(p10));
    
  // Print buffer on display
  display.display();
}

void loop ()
{
  Serial.println("Waking up sensor...");
  sds.wakeup();
  
  Serial.println("Warming up sensor for " + String(SDS_WARMUP_TIME) + "ms...");
  delay(SDS_WARMUP_TIME);

  Serial.println("Reading values...");
  sds.read(&p25, &p10);
  Serial.println("P2.5: " + String(p25));
  Serial.println("P10:  " + String(p10));

  Serial.println("Go to sleep for " + String(SDS_SLEEP_TIME) + "ms...");
  sds.sleep();
 
  outputToDisplay();
  counter++;  
  delay(SDS_SLEEP_TIME);
}

