// Project for switching several relays on the NodeMCU/ESP8266 platform
// Project toggles the state of the relays every second
// Used devices: 4 channel relay card/module, SSD1306 display
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

// Configure relay pins that are used
#define RELAY_PIN_1 D0
#define RELAY_PIN_2 D1
#define RELAY_PIN_3 D2
#define RELAY_PIN_4 D6                // D6 doesn't work on my NodeMCU - this is just a placeholder

// Global variables
const unsigned long period = 1000;    // Optional: Delay timer for every measurement
int counter = 0;                      // Uptime
bool relayState = false;              // Current relay state (true/false = on/off)

// Setup Phase
void setup()
{
  
  // Initialise Serial debug output
  Serial.begin(115200);
  Serial.print("\n\n");
  Serial.print("Setup: Starting...\n");

  // Initialise relay pins as output pins
  Serial.print("Setup: Initialising relay pins...\n");
  pinMode(RELAY_PIN_1, OUTPUT);
  pinMode(RELAY_PIN_2, OUTPUT);
  pinMode(RELAY_PIN_3, OUTPUT);
  pinMode(RELAY_PIN_4, OUTPUT);

  // Initialising pins to high which means OFF on the relay card
  // Note: With the Sainsmart relay card OFF=HIGH and ON=LOW
  Serial.print("Setup: Setting all relays to OFF...\n");
  digitalWrite(RELAY_PIN_1, HIGH);
  digitalWrite(RELAY_PIN_2, HIGH);
  digitalWrite(RELAY_PIN_3, HIGH);
  digitalWrite(RELAY_PIN_4, HIGH);

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
  display.drawString(0, 11, "Current relay state: " + String(relayState));
    
  // Print buffer on display
  display.display();
}

// Function to toggle the current relay state
void toggleRelayState()
{
  if(relayState == false){
    Serial.print(String(counter) + ": Switching relays to FALSE/OFF\n");
    digitalWrite(RELAY_PIN_1, HIGH);
    digitalWrite(RELAY_PIN_2, HIGH);
    digitalWrite(RELAY_PIN_3, HIGH);
    digitalWrite(RELAY_PIN_4, HIGH);
    relayState = true;
  }else{
    Serial.print(String(counter) + ": Switching relays to TRUE/ON\n");
    digitalWrite(RELAY_PIN_1, LOW);
    digitalWrite(RELAY_PIN_2, LOW);
    digitalWrite(RELAY_PIN_3, LOW);
    digitalWrite(RELAY_PIN_4, LOW);
    relayState = false;
  }
}

void loop ()
{ 
  toggleRelayState();
  outputToDisplay();
  counter++; 
  delay(period);
}

