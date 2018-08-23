// Project for controlling and measuring everything around my garden
// Used devices: 
// Based on the ArduinoOTA example project
// Michael Morscher, August 2018
// Tested on Arduino IDE 1.8.5
// Board: NodeMCU ESP8266 v3
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <stdlib.h>

// Additional used libraries
// ArduinoOTA: https://github.com/esp8266/Arduino/tree/master/libraries/ArduinoOTA
// Syslog: https://github.com/arcao/Syslog
// arduino-mqtt: https://github.com/256dpi/arduino-mqtt
// Timer: https://github.com/JChristensen/Timer
#include <ArduinoOTA.h>
#include <Syslog.h>
#include <MQTT.h>
#include "Timer.h" 

#define VERSION 1.1
#define SECOND 1000

// Network settings
const char* ssid = "ssid";
const char* password = "password";
const char* hostname = "ESP-GARDEN";
#define RASPI_IP_ADDRESS "192.168.0.X"


// Syslog server connection info
#define SYSLOG_SERVER RASPI_IP_ADDRESS
#define SYSLOG_PORT 514
#define SYSLOG_APP_NAME "GARDEN"

// MQTT connection settings
#define BROKER_ADDRESS RASPI_IP_ADDRESS
#define BROKER_PORT 1883

// MQTT sensor/actor settings
#define TOPIC_ACTORS_NUMBER 3
char *topics_actors[] = { "sprinkler",  "tropfschlauch", "lichterkette"};
#define TOPIC_SENSORS_NUMBER 3
char *topics_sensors[] = { "durchfluss", "feinstaub25", "feinstaub10"};

// Timers
Timer timer; 
struct struct_timer {
  int8_t id_every;
  int8_t id_after;
  int8_t pin;
  int8_t counter;
};
struct struct_timer topics_timers[3];

// Instantiation
WiFiClient wifiClient;
MQTTClient mqttClient;
WiFiUDP udpClient;
Syslog syslog(udpClient, SYSLOG_SERVER, SYSLOG_PORT, hostname, SYSLOG_APP_NAME, LOG_KERN);

// Configure relay pins that are used
#define SPRINKLER RELAY_PIN_1
#define TROPFSCHLAUCH RELAY_PIN_2
#define LICHTERKETTE RELAY_PIN_3
#define RELAY_PIN_1 D0
#define RELAY_PIN_2 D1
#define RELAY_PIN_3 D3

// Configure FS300A water flow sensor
#define FS300A_PIN D2

// Global variables
bool relayState[TOPIC_ACTORS_NUMBER] = { false, false, false };
int relayPin[TOPIC_ACTORS_NUMBER] = { RELAY_PIN_1, RELAY_PIN_2, RELAY_PIN_3 };
volatile int volumeCounter = 0;       // Measures flow sensor pulses
bool volumeBuffer = true;

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

void updateTimer(void *context)
{
  struct struct_timer *timers = (struct_timer *)context;
  timers->counter = int(timers->counter - 1);
  mqttClient.publish(topics_actors[timers->pin], String(timers->counter));
}

void messageReceived(String &topic, String &payload) {
  Serial.println("MQTT message received: topic=" + topic + " (payload=" + payload + ")");
  syslog.log(LOG_DEBUG, "MQTT message received: topic=" + topic + " (payload=" + payload + ")");

  // Toggle relays depending on their topic names
  for (int i=0; i < TOPIC_ACTORS_NUMBER; i++){
    
    // if topic is found 
    if(topic == topics_actors[i]){

      // check if a timer should get activated
      if(payload.toInt() > 0){

        // check if a timer is already running ...
        // if a new (higher) counter should be set
        if(payload.toInt() > topics_timers[i].counter){
          timer.stop(topics_timers[i].id_after);
          timer.stop(topics_timers[i].id_every);
          topics_timers[i].pin = i;
          topics_timers[i].counter = payload.toInt();
          topics_timers[i].id_every = timer.every(SECOND, updateTimer, payload.toInt(), (void*)&topics_timers[i]);
          syslog.log(LOG_INFO, "Relay: Activating #" + String(i+1) + " for " + String(payload) + " seconds");
          activateRelay(relayPin[i], relayState[i]);
        }else{
          // ignore
          syslog.log(LOG_INFO, "Relay: Ignoring message for relay #" + String(i+1));
        }
      }

      // deactivate timer
      else{
        syslog.log(LOG_INFO, "Relay: Deactivating #" + String(i+1));
        deactivateRelay(relayPin[i], relayState[i]);
        timer.stop(topics_timers[i].id_every);
      }
    }
  }

  // Create debug output for syslog and all relay states
  String states = "";
  for (int i=0; i < TOPIC_ACTORS_NUMBER; i++){
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
  dtostrf(literPerSecond,2,5,volume);
  
  if(literPerSecond > 0){
    Serial.println("Current volume: " + String(volume) + "l/h");
    syslog.log(LOG_INFO, "Durchfluss: " + String(volume));
    mqttClient.publish("durchfluss", String(volume));
    volumeBuffer = true;
  }else{
    // buffer: Send one "0" message after >0 messages
    if(volumeBuffer == true){
      Serial.println("Current volume: " + String(volume) + "l/h");
      syslog.log(LOG_INFO, "Durchfluss: " + String(volume));
      mqttClient.publish("durchfluss", String(volume));
      volumeBuffer = false;
    }
  }
}

// Setup Phase
void setup() {
  // Serial configuration
  Serial.begin(115200);
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

  // OTA configuration
  ArduinoOTA.setPort(8266);
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

  // MQTT configuration: Initialise and enable callbacks
  Serial.println("Initialising connection to MQTT broker...");
  mqttClient.begin(BROKER_ADDRESS, BROKER_PORT, wifiClient);
  mqttClient.onMessage(messageReceived);

  // MQTT connection
  while (!mqttClient.connect(hostname)) {
    delay(500);
  }
  Serial.println("Connected to broker!");

  // MQTT subscriptions for actuators
  for (int i = 0; i < TOPIC_ACTORS_NUMBER; i++){
    mqttClient.subscribe(topics_actors[i]);
  }

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
    setup();
  }
}
