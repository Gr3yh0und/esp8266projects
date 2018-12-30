// Project for creating a sorting hat for a harry potter themed party
// Used devices: DFplayer mini + 2x loudspeakers, 2x 12-LED WS2812B LED-Rings for "eyes"
// Specification: https://github.com/DFRobot/DFRobotDFPlayerMini/blob/master/doc/FN-M16P%2BEmbedded%2BMP3%2BAudio%2BModule%2BDatasheet.pdf
// Michael Morscher, December 2018
// Tested on Arduino IDE 1.8.8
// Board: Arduino Nano

// Additional used libraries
// DFRobotDFPlayerMini: https://github.com/DFRobot/DFRobotDFPlayerMini
// Adafruit_NeoPixel: https://github.com/adafruit/Adafruit_NeoPixel
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>
#include <Adafruit_NeoPixel.h>
#include <CapacitiveSensor.h>

// Configuration for serial connection
#define SERIAL_BAUD     115200

// Configuration for LED Rings
#define LEDS_PIN        2
#define LEDS_NUMBER     12
#define LEDS_BRIGHTNESS 1

// Configuration for capacitive sensor
#define SENSOR_PIN_1       5
#define SENSOR_PIN_2       6
#define SENSOR_SENSITIVITY 200
#define SENSOR_THRESHOLD   1000

// Configuration for DFplayer mini
#define AUDIO_PIN_TX      11
#define AUDIO_PIN_RX      10
#define AUDIO_PIN_BUSY    9
#define AUDIO_VOLUME      HAT_SOUND_VOLUME
#define AUDIO_BAUD        9600
#define AUDIO_TIMEOUT     1000
#define AUDIO_EQUALIZER   DFPLAYER_EQ_NORMAL
#define AUDIO_DATA_SOURCE DFPLAYER_DEVICE_SD

// Definition of basic colors
#define RED     strip.Color(255, 0, 0)
#define YELLOW  strip.Color(255, 255, 0)
#define BLUE    strip.Color(0, 0, 255)
#define GREEN   strip.Color(0, 255, 0)
#define WHITE   strip.Color(255, 255, 255)

// Define house variables
#define GRYFFINDOR 1
#define HUFFLEPUFF 2
#define RAVENCLAW  3
#define SLYTHERIN  4

// Define house colors
#define GRYFFINDOR_COLOR  RED
#define HUFFLEPUFF_COLOR  YELLOW
#define RAVENCLAW_COLOR   BLUE
#define SLYTHERIN_COLOR   GREEN

// Define sounds
#define GRYFFINDOR_SOUND  2
#define HUFFLEPUFF_SOUND  3
#define RAVENCLAW_SOUND   4
#define SLYTHERIN_SOUND   5

// Define house LED parameters
#define HOUSE_BLINK_NUMBER        10
#define HOUSE_BLINK_NUMBER_POST   3
#define HOUSE_BLINK_DELAY         100

// Define waiting animation parameters
#define WAITING_FREQUENCY_STEPS 10
#define WAITING_FREQUENCY_MIN   30

// Define announcement handling parameters
#define ANNOUNCEMENT_ACTIVE     1
#define ANNOUNCEMENT_FREQUENCY  30000
#define ANNOUNCEMENT_DELAY      100

// Define main functions
#define HAT_COOLDOWN_TIMER    1000
#define HAT_WAITING_COLOR     WHITE
#define HAT_SOUND_VOLUME      15

struct sound {
  uint8_t id;
  uint16_t duration;
};

sound announcementSound = {3, 3000};

// Hard coded list of waiting sounds for every house you need to take care of yourself. I've made them house-branded to make it more personal.
// The first parameter is the ID on the SD card while the second parameter is the length of the track.
uint8_t waitingSize = 4;
sound waitingGryffindor[] = {
  {2, 4000},
  {2, 4000},
  {2, 4000},
  {2, 4000},
};
sound waitingHufflepuff[] = {
  {3, 4000},
  {3, 5000},
  {3, 5000},
  {3, 5000},
};
sound waitingRavenclaw[] = {
  {4, 4000},
  {4, 5000},
  {4, 2000},
  {4, 2000},
};
sound waitingSlytherin[] = {
  {5, 4000},
  {5, 5000},
  {5, 2000},
  {5, 3000},
};

// Global variables
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LEDS_NUMBER, LEDS_PIN, NEO_GRB + NEO_KHZ800);
CapacitiveSensor sensor = CapacitiveSensor(SENSOR_PIN_1, SENSOR_PIN_2);
SoftwareSerial mySoftwareSerial(AUDIO_PIN_RX, AUDIO_PIN_TX);
DFRobotDFPlayerMini myDFPlayer;
bool cooldown = false;
bool playingSound = false;
uint8_t trackBuffer = 0;

// Setup Phase
void setup() {
  // Initialize serial debug connection
  Serial.begin(SERIAL_BAUD);
  Serial.println(F("Harry Potter - the sorting hat project"));

  // LED initialization
  Serial.println(F("Setup - LED"));
  strip.begin();
  strip.setBrightness(LEDS_BRIGHTNESS);
  strip.show();

  // Capacitive sensor configuration
  Serial.println(F("Setup - Sensor"));
  sensor.set_CS_AutocaL_Millis(0xFFFFFFFF);

  // Use software serial connection to communicate with dfplayer module
  Serial.println(F("Setup - DFplayer"));
  mySoftwareSerial.begin(AUDIO_BAUD);
  if (!myDFPlayer.begin(mySoftwareSerial, true, false)) {
    Serial.println(F("Error: Please check serial connection or SD card!"));
    // Added ESP8266 watch dog compability
    while (true) {
      delay(0);
    }
  }

  // Set audio player configuration
  myDFPlayer.setTimeOut(AUDIO_TIMEOUT);
  myDFPlayer.EQ(AUDIO_EQUALIZER);
  myDFPlayer.outputDevice(AUDIO_DATA_SOURCE);
  myDFPlayer.volume(AUDIO_VOLUME);
  Serial.println(F("Setup - End"));
}

void loop() {
  long sensor1 = sensor.capacitiveSensor(SENSOR_SENSITIVITY);
  static unsigned long cooldownTimer = millis();
  static unsigned long announcementTimer = millis();

  // Cooldown timer management
  if (millis() - cooldownTimer > HAT_COOLDOWN_TIMER) {
    cooldownTimer = millis();
    if (cooldown == true) {
      Serial.print("Debug - Turning of cooldown\n");
      cooldown = false;
    }
  }

  // Announcement timer management
  if (millis() - announcementTimer > ANNOUNCEMENT_FREQUENCY) {
    announcementTimer = millis();
    Serial.print("Debug - Playing announcement\n");
    playSound(announcementSound.id);
    colorRotateChanging(announcementSound.duration, ANNOUNCEMENT_DELAY);
  }

  // Check audio player module for events/errors
  if (myDFPlayer.available()) {
    printDetail(myDFPlayer.readType(), myDFPlayer.read());
  }

  // Trigger main event if not in cooldown and sensor value is higher than threshold
  if ((sensor1 >= SENSOR_THRESHOLD) && (cooldown == false)) {
    Serial.print("Debug - Sensor activated (SENS:");
    Serial.print(sensor1);
    Serial.print(")\n");

    // Show waiting animation and house afterwards
    uint8_t house = random(1, 5);
    showWaitingAnimation(HAT_WAITING_COLOR, selectWaitingSound(house));
    showHouse(house);

    // Set cooldown variable and reset announcement timer
    cooldown = true;
    announcementTimer = millis();
  }
}

// Play a certain given mp3 track on the module
void playSound(uint8_t track) {
  Serial.print("Debug - Play track with ID ");
  Serial.print(track);
  Serial.print("\n");
  myDFPlayer.play(track);
  playingSound = true;
}

// Randomly select a waiting / stalling sound and return the length of the track
uint16_t selectWaitingSound(uint8_t house) {
  sound *sounds;
  if (house == GRYFFINDOR) {
    Serial.println("Debug - House: Gryffindor");
    sounds = waitingGryffindor;
  }
  if (house == HUFFLEPUFF) {
    Serial.println("Debug - House: Hufflepuff");
    sounds = waitingHufflepuff;
  }
  if (house == RAVENCLAW) {
    Serial.println("Debug - House: Ravenclaw");
    sounds = waitingRavenclaw;
  }
  if (house == SLYTHERIN) {
    Serial.println("Debug - House: Slytherin");
    sounds = waitingSlytherin;
  }
  uint8_t track = random(0, waitingSize);
  playSound(sounds[track].id);
  return sounds[track].duration;
}

// Play a waiting animation on the LED rings
void showWaitingAnimation(uint32_t color, uint16_t duration) {
  uint8_t runs = 0;

  // Calculate how many runs are required for the waiting animation
  Serial.print("Debug - Showing waiting animation for ");
  Serial.print(duration);
  Serial.print("ms in ");
  for (uint8_t i = WAITING_FREQUENCY_MIN; duration > 0; i = i + WAITING_FREQUENCY_STEPS) {
    if ((LEDS_NUMBER * i) > duration) {
      break;
    }
    duration = duration - (LEDS_NUMBER * i);
    runs++;
  }
  Serial.print(runs);
  Serial.print(" runs\n");

  // Show the waiting animation
  uint32_t timing_total = millis();
  for (uint8_t i = WAITING_FREQUENCY_MIN + (runs * WAITING_FREQUENCY_STEPS); i >= WAITING_FREQUENCY_MIN; i = i - WAITING_FREQUENCY_STEPS) {
    uint32_t timing = millis();
    movingDot(color, i);
  }
  Serial.print("Debug - Waiting animation took ");
  Serial.print(millis() - timing_total);
  Serial.print("ms\n");

  // Turn off LEDs for a second before house is announced
  strip.clear();
  strip.show();
  delay(1000);
}

// Select the chosen house
void showHouse(uint8_t house) {
  if (house == GRYFFINDOR) {
    colorWipe(GRYFFINDOR_COLOR, 20);
    playSound(GRYFFINDOR_SOUND);
    colorRotate(GRYFFINDOR_COLOR, HOUSE_BLINK_NUMBER, HOUSE_BLINK_DELAY);
    colorBlink(GRYFFINDOR_COLOR, HOUSE_BLINK_NUMBER_POST, HOUSE_BLINK_DELAY);
  }
  if (house == HUFFLEPUFF) {
    colorWipe(HUFFLEPUFF_COLOR, 20);
    playSound(HUFFLEPUFF_SOUND);
    colorRotate(HUFFLEPUFF_COLOR, HOUSE_BLINK_NUMBER, HOUSE_BLINK_DELAY);
    colorBlink(HUFFLEPUFF_COLOR, HOUSE_BLINK_NUMBER_POST, HOUSE_BLINK_DELAY);
  }
  if (house == RAVENCLAW) {
    colorWipe(RAVENCLAW_COLOR, 20);
    playSound(RAVENCLAW_SOUND);
    colorRotate(RAVENCLAW_COLOR, HOUSE_BLINK_NUMBER, HOUSE_BLINK_DELAY);
    colorBlink(RAVENCLAW_COLOR, HOUSE_BLINK_NUMBER_POST, HOUSE_BLINK_DELAY);
  }
  if (house == SLYTHERIN) {
    colorWipe(SLYTHERIN_COLOR, 20);
    playSound(SLYTHERIN_SOUND);
    colorRotate(SLYTHERIN_COLOR, HOUSE_BLINK_NUMBER, HOUSE_BLINK_DELAY);
    colorBlink(SLYTHERIN_COLOR, HOUSE_BLINK_NUMBER_POST, HOUSE_BLINK_DELAY);
  }
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t color, uint8_t wait) {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, color);
    strip.show();
    delay(wait);
  }
}

//
void movingDot(uint32_t color, uint8_t wait) {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.clear();
    strip.setPixelColor(i, color);
    strip.show();
    delay(wait);
  }
}

//
void colorBlink(uint32_t color, uint8_t blinks, uint8_t wait) {
  for (uint8_t k = 0; k < blinks; k++) {
    strip.clear();
    strip.show();
    delay(wait);
    for (uint16_t i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, color);
    }
    strip.show();
    delay(wait * 2);
  }
  strip.clear();
  strip.show();
}

//
void colorRotate(uint32_t color, uint8_t blinks, uint8_t wait) {
  for (uint8_t k = 0; k < blinks; k++) {
    strip.clear();
    strip.show();
    delay(wait);
    for (uint16_t i = 0; i < strip.numPixels(); i++) {
      if (i % 2 == k % 2) {
        strip.setPixelColor(i, color);
      }
    }
    strip.show();
    delay(wait);
  }
  strip.clear();
  strip.show();
}

//
void colorRotateChanging(uint16_t duration, uint8_t wait) {
  uint8_t k = 0;
  for (uint16_t i = 0; i <= (duration / wait); i++) {
    strip.clear();
    strip.setPixelColor(k % 12, GRYFFINDOR_COLOR);
    strip.setPixelColor(k % 12 + 1, HUFFLEPUFF_COLOR);
    strip.setPixelColor(k % 12 + 2, RAVENCLAW_COLOR);
    strip.setPixelColor(k % 12 + 3, SLYTHERIN_COLOR);
    if (k % 9 == 0) {
      strip.setPixelColor(0, SLYTHERIN_COLOR);
    }
    if (k % 10 == 0) {
      strip.setPixelColor(0, RAVENCLAW_COLOR);
      strip.setPixelColor(1, SLYTHERIN_COLOR);
    }
    if (k % 11 == 0) {
      strip.setPixelColor(0, HUFFLEPUFF_COLOR);
      strip.setPixelColor(1, RAVENCLAW_COLOR);
      strip.setPixelColor(2, SLYTHERIN_COLOR);
    }
    strip.show();
    delay(wait);
    k++;
    if (k == 12) {
      k = 0;
    }
  }
  strip.clear();
  strip.show();
}

// Display audio player module errors and events
void printDetail(uint8_t type, int value) {
  switch (type) {
    case TimeOut:
      Serial.println(F("Audio - Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Audio - Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Audio - Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Audio - Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Audio - Card Online!"));
      break;
    case DFPlayerUSBInserted:
      Serial.println("Audio - USB Inserted!");
      break;
    case DFPlayerUSBRemoved:
      Serial.println("Audio - USB Removed!");
      break;
    case DFPlayerPlayFinished:
      if(trackBuffer == value){break;}
      Serial.print(F("Audio - Number "));
      Serial.print(value);
      Serial.println(F(": Play Finished!"));
      playingSound = false;
      trackBuffer = value;
      break;
    case DFPlayerError:
      Serial.print(F("Audio - DFPlayerError:"));
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
