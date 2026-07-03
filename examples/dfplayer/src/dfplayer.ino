// Project for playing mp3 files via an external audio player called dfplayer
// Used devices: DFplayer mini
// Michael Morscher, December 2018
// Tested on Arduino IDE 1.8.8
// Board: Arduino Nano

// Additional used libraries
// DFRobotDFPlayerMini: https://github.com/DFRobot/DFRobotDFPlayerMini
#include <SoftwareSerial.h>
#include "DFRobotDFPlayerMini.h"

// Configuration for player via serial connection
#define PIN_SERIAL_TX 11
#define PIN_SERIAL_RX 10
#define PIN_BUSY 9

#define AUDIO_VOLUME 30

// Global variables
SoftwareSerial mySoftwareSerial(PIN_SERIAL_RX, PIN_SERIAL_TX);
DFRobotDFPlayerMini myDFPlayer;

void printDetail(uint8_t type, int value);

// Setup Phase
void setup()
{
  // Initialize serial debug connection
  Serial.begin(115200);

  // Initialize serial connection to dfplayer
  mySoftwareSerial.begin(9600);

  // Set Busy Pin
  pinMode(PIN_BUSY, INPUT);

  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));

  // Use software serial connection to communicate with dfplayer
  if (!myDFPlayer.begin(mySoftwareSerial, true, false)) {
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1. Please check the connection!"));
    Serial.println(F("2. Please insert a SD card!"));
    while (true) {
      delay(0); // Added ESP8266 watch dog compability
    }
  }
  Serial.println(F("DFPlayer Mini online."));

  // Set timeout
  myDFPlayer.setTimeOut(500);
  myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
  myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);

  // Set playback volume (range: 0-30)
  myDFPlayer.volume(AUDIO_VOLUME);

  // Start playing first track
  myDFPlayer.play(1);
}

void loop()
{
  static unsigned long timer = millis();

  if (millis() - timer > 3000) {
    timer = millis();
    myDFPlayer.next();  //Play next mp3 every 3 second.
  }

  if (myDFPlayer.available()) {
    printDetail(myDFPlayer.readType(), myDFPlayer.read()); //Print the detail message from DFPlayer to handle different errors and states.
  }
}

// Error handling function
void printDetail(uint8_t type, int value) {
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerUSBInserted:
      Serial.println("USB Inserted!");
      break;
    case DFPlayerUSBRemoved:
      Serial.println("USB Removed!");
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }

}
