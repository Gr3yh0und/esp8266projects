// Project for controlling and measuring everything around my garden
// Basically opening magnetic valves for watering the yard, a led light strip for BBQ nights
// and a waterflow sensor to check how much I spend on watering my garden
//
// Used devices: 4 port Relay module, Flow meter FS300A
// Based on the ArduinoOTA example project
// Michael Morscher, April 2019
// Tested on Arduino IDE 1.8.9
// Board: NodeMCU ESP8266 v3
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <stdlib.h>

// Additional used libraries
// ArduinoOTA: https://github.com/esp8266/Arduino/tree/master/libraries/ArduinoOTA
// Syslog: https://github.com/arcao/Syslog - v2.0
// arduino-mqtt: https://github.com/256dpi/arduino-mqtt - v2.4.3
// Timer: https://github.com/JChristensen/Timer - Branch v2.1
#include <ArduinoOTA.h>
#include <Syslog.h>
#include <MQTT.h>
#include "Timer.h"

// Configuration
#define LOCATION            "garden"    // Used as SYSLOG app name
#define HOSTNAME            "ESP-GARDEN"
#define SSID_NAME           "SSID"
#define SSID_PASSWORD       "PASSWORD"
#define MQTT_HOST_ADDRESS   "192.168.0.100"
#define MQTT_HOST_PORT      1883
#define SYSLOG_HOST_ADDRESS "192.168.0.100"
#define SYSLOG_HOST_PORT    514
#define OTA_PORT            8226
#define BAUDRATE            115200
#define BASE_CHANNEL        "cave/garden"

// Settings
#define VERSION       1.4
#define SECOND        1000
#define SYSLOG_LEVEL  LOG_INFO // default e.g. LOG_DEBUG or LOG_INFO

// MQTT sensor/actor settings (needs to be edited)
#define TOPIC_ACTORS_NUMBER 4
char *topics_actors[] = { "cave/garden/sprinkler",  "cave/garden/tropfschlauch", "cave/garden/lichterkette", "cave/garden/debug"};
#define TOPIC_SENSORS_NUMBER 3
char *topics_sensors[] = { "cave/garden/flow", "cave/garden/dust25", "cave/garden/dust10"};

// Set network, MQTT and syslog settings
const char* ssid = SSID_NAME;
const char* password = SSID_PASSWORD;
const char* hostname = HOSTNAME;

// Timers
Timer timer;
struct struct_timer {
  uint8_t id;
  uint8_t pin;
  uint32_t counter;
};
struct struct_timer topics_timers[TOPIC_ACTORS_NUMBER];

// Instantiation
WiFiClient wifiClient;
MQTTClient mqttClient;
WiFiUDP udpClient;
Syslog syslog(udpClient, SYSLOG_HOST_ADDRESS, SYSLOG_HOST_PORT, hostname, LOCATION, LOG_KERN);

// Configure relay pins that are used
#define RELAY_PIN_1 D0
#define RELAY_PIN_2 D1
#define RELAY_PIN_3 D3
#define SPRINKLER RELAY_PIN_1
#define TROPFSCHLAUCH RELAY_PIN_2
#define LICHTERKETTE RELAY_PIN_3

// Configure FS300A water flow sensor
#define FS300A_PIN D2

// Global variables
bool relayState[TOPIC_ACTORS_NUMBER] = { false, false, false };
int relayPin[TOPIC_ACTORS_NUMBER] = { RELAY_PIN_1, RELAY_PIN_2, RELAY_PIN_3 };
volatile int volumeCounter = 0;       // Measures flow sensor pulses
bool volumeBuffer = true;
int debugLevel = 0;

// Parameterization for relay
int outputPinForTimer = 0;

// Function which is called when an interrupt arrives
void ICACHE_RAM_ATTR flow ()
{
  volumeCounter++;
}

void activateRelay(int &pin, bool &state)
{
  digitalWrite(pin, LOW);
  state = true;
}

void deactivateRelay(int &pin, bool &state)
{
  digitalWrite(pin, HIGH);
  state = false;
}

// Always executed to update timers
void updateTimer(void *context)
{
  struct struct_timer *timers = (struct_timer *)context;
  timers->counter = int(timers->counter - 1);
  if (mqttClient.connected()) {
    mqttClient.publish(topics_actors[timers->pin], String(timers->counter));
  }
}

// Switch log level between INFO and DEBUG
void toggleDebug(String mqttChannel, int level) {
  if (level == 1) {
    debugLevel = 1;
    syslog.logMask(LOG_UPTO(LOG_DEBUG));
    syslog.log(LOG_INFO, "DEBUGGING ENABLED");
  } else {
    debugLevel = 0;
    syslog.logMask(LOG_UPTO(LOG_INFO));
    syslog.log(LOG_INFO, "DEBUGGING DISABLED");
  }
}

// Call every time a MQTT message is received on the subscribed channels
void messageReceived(String &topic, String &payload) {
  Serial.println("MQTT message received: topic=" + topic + " (payload=" + payload + ")");
  syslog.log(LOG_DEBUG, "MQTT message received: topic=" + topic + " (payload=" + payload + ")");

  // Toggle relays depending on their topic names
  for (int i = 0; i < TOPIC_ACTORS_NUMBER; i++) {

    // if topic is found
    if (topic == topics_actors[i]) {
      syslog.log(LOG_DEBUG, "TIMER=" + String(topics_timers[i].id) + ", DEBUG=" + String(payload.toInt()));

      // Handle special debug channel for remote management
      if (topic == (String(BASE_CHANNEL) + "/debug")) {
        if (payload.toInt() != debugLevel) {
          toggleDebug(String(BASE_CHANNEL) + "/debug", payload.toInt());
          return;
        }
      }

      // check if a timer should get activated
      if (payload.toInt() > 0) {

        // check if a timer is already running ...
        // if a new (higher) counter should be set
        if (payload.toInt() > topics_timers[i].counter) {

          // stop existing timer
          if (topics_timers[i].counter > 0) {
            timer.stop(topics_timers[i].id);
          }

          topics_timers[i].pin = i;
          topics_timers[i].counter = payload.toInt();
          topics_timers[i].id = timer.every(SECOND, updateTimer, payload.toInt(), (void*)&topics_timers[i]);
          syslog.log(LOG_INFO, "Relay: Activating #" + String(i + 1) + " for " + String(payload) + " seconds");
          activateRelay(relayPin[i], relayState[i]);
        } else {
          // ignore
          syslog.log(LOG_DEBUG, "Relay: Ignoring message for relay #" + String(i + 1));
        }
      }

      // deactivate timer
      else {
        syslog.log(LOG_INFO, "Relay: Deactivating #" + String(i + 1));
        deactivateRelay(relayPin[i], relayState[i]);
        timer.stop(topics_timers[i].id);
        topics_timers[i].counter = 0;
      }
    }
  }

  // Create debug output for syslog and all relay states
  String states = "";
  for (int i = 0; i < TOPIC_ACTORS_NUMBER; i++) {
    states = states + " " + topics_actors[i] + "(" + relayState[i] + "),";
  }
  syslog.log(LOG_DEBUG, "New relay states are:" + states);
}

void measureFlow(void *context)
{
  // pulse frequency (Hz) = 7.5Q
  // Q is the flow rate in l/min
  // (pulse frequency x 60 min) / 7.5Q = flow rate in l/hour
  float literPerSecond = volumeCounter / 7.5 / 60;              // liter / second
  volumeCounter = 0;

  char volume[8];
  dtostrf(literPerSecond, 2, 5, volume);

  if (literPerSecond > 0) {
    Serial.println("Current volume: " + String(volume) + "l/h");
    syslog.log(LOG_DEBUG, "Durchfluss: " + String(volume));
    mqttClient.publish(topics_sensors[0], String(volume));
    volumeBuffer = true;
  } else {
    // buffer: Send one "0" message after >0 messages
    if (volumeBuffer == true) {
      Serial.println("Current volume: " + String(volume) + "l/h");
      syslog.log(LOG_DEBUG, "Durchfluss: " + String(volume));
      if (mqttClient.connected()) {
        mqttClient.publish(topics_sensors[0], String(volume));
      }
      volumeBuffer = false;
    }
  }
}

// Setup Wi-Fi network and MQTT connection
void setup_network() {
  // WIFI configuration
  WiFi.mode(WIFI_STA);
  WiFi.hostname(hostname);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Wi-Fi connection failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  Serial.println("Connected to Wi-Fi!");
  Serial.print("Local IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Hostname: ");
  Serial.println(WiFi.hostname());

  // MQTT configuration: Initialise and enable callbacks
  Serial.println("Initialising connection to MQTT broker...");
  mqttClient.begin(MQTT_HOST_ADDRESS, MQTT_HOST_PORT, wifiClient);
  mqttClient.onMessage(messageReceived);

  // MQTT connection
  while (!mqttClient.connect(hostname)) {
    delay(500);
  }
  Serial.println("Connected to broker!");

  // MQTT subscriptions for actuators
  for (int i = 0; i < TOPIC_ACTORS_NUMBER; i++) {
    mqttClient.subscribe(topics_actors[i]);
  }
}

// Setup Phase
void setup() {
  // Serial configuration
  Serial.begin(BAUDRATE);
  Serial.print("Project version: ");
  Serial.println(VERSION);
  Serial.println("Setup initialised!");

  // Initialise relay pins as output pins
  Serial.print("Setup: Initialising relay pins 1-3...\n");
  pinMode(RELAY_PIN_1, OUTPUT);
  pinMode(RELAY_PIN_2, OUTPUT);
  pinMode(RELAY_PIN_3, OUTPUT);

  // Initialising pins to high which means OFF on the relay card
  // Note: With the Sainsmart relay card OFF=HIGH and ON=LOW
  Serial.print("Setup: Setting all relays to OFF...\n");
  digitalWrite(RELAY_PIN_1, HIGH);
  digitalWrite(RELAY_PIN_2, HIGH);
  digitalWrite(RELAY_PIN_3, HIGH);

  // Initialise Sensor
  // The pin is configured as PULLUP and interrupts are thrown if the edge is RISING
  Serial.print("Setup: Initialising FS300A sensor..\n");
  pinMode(FS300A_PIN, INPUT);
  digitalWrite(FS300A_PIN, HIGH);
  attachInterrupt(FS300A_PIN, flow, RISING);
  sei();

  // Network configuration: Wi-Fi and MQTT
  setup_network();

  // Set default syslog level
  syslog.logMask(LOG_UPTO(SYSLOG_LEVEL));

  // OTA configuration
  ArduinoOTA.setPort(OTA_PORT);
  ArduinoOTA.setHostname(hostname);
  //ArduinoOTA.setPassword(adminpw);

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("OTA: Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nOTA End");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("OTA Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("OTA Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("OTA setup successfull!");

  // Timers
  int flowMeasurement = timer.every(SECOND, measureFlow, (void*)0);

  syslog.log(LOG_INFO, "System version: " + String(VERSION));
  syslog.log(LOG_INFO, "System setup successfull!");
}

void loop() {
  timer.update();
  ArduinoOTA.handle();
  mqttClient.loop();
  if (!mqttClient.connected()) {
    setup_network();
  }
}
