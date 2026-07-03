// Project for a measuring water flow on the NodeMCU/ESP8266 platform
// Used devices: FS300A water flow sensor, SSD1306 display
// Michael Morscher, May 2018
// Tested on Arduino IDE 1.8.5
// Board: NodeMCU ESP8266 v3
#include <Wire.h>

// Additional used libraries
// ESP8266-OLED-SSD1306: https://github.com/squix78/esp8266-oled-ssd1306 - tested with v4.0.0
#include <SPI.h>
#include "SSD1306.h"
#include "SSD1306Spi.h"

// Configure Display, Type: SSD1306 using SPI
// Fixed pins:
// D5 = D0 
// D7 = D1
// Variable pins:
// D3 = RES/RST/RESET
// D4 = DC
// D8 = CS
SSD1306Spi display(D3, D4, D8);

// Configure FS300A water flow sensor
#define FS300A_PIN D2

// Global variables
const unsigned long period = 1000;    // Optional: Delay timer for every measurement
int counter = 0;                      // Uptime
volatile int volumeCounter = 0;       // Measures flow sensor pulses
unsigned int volume = 0;              // Calculated litres/hour
unsigned int volumeTotal = 0;         // Total amount of liquid that ran through the sensor

// Function which is called when an interrupt arrives
void ICACHE_RAM_ATTR flow ()
{
  volumeCounter++;
}

// Setup Phase
void setup()
{
  
  // Initialise Serial debug output
  Serial.begin(115200);
  Serial.print("\n\n");
  Serial.print("Setup: Starting...\n");

  // Initialise Sensor
  // The pin is configured as PULLUP and interrupts are thrown if the edge is RISING
  Serial.print("Setup: Initialising FS300A sensor..\n");
  pinMode(FS300A_PIN, INPUT);
  digitalWrite(FS300A_PIN, HIGH);
  attachInterrupt(D2, flow, RISING);
  sei();

  // Initialising the UI will init the display too.
  Serial.print("Setup: Initialising display\n");  
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
  display.drawString(0, 11, "Current volume: " + String(volume)+ " l/h");
  display.drawString(0, 21, "Total volume: " + String(volumeTotal)+ " l");
    
  // Print buffer on display
  display.display();
}

void loop ()
{
  // pulse frequency (Hz) = 7.5Q
  // Q is the flow rate in l/min
  // (pulse frequency x 60 min) / 7.5Q = flow rate in l/hour
  volume = (volumeCounter * 60 / 7.5);
  volumeTotal = volumeTotal + (volume / 3600);
  volumeCounter = 0; 

  Serial.println("Current volume: " + String(volume) + " liters/hour");
  Serial.println("Total volume:  " + String(volumeTotal) + " liters");
  
  outputToDisplay();
  counter++;  
  delay(period);
}

