// Project for an ambient sensor station for the NodeMCU/ESP8266 platform
// The purpose is to collect indoor sensor information for smart home automation
// Note: Deep Sleep can be configured via Switch
// Used devices: BME-280, TSL-2561, CCS811, Switch 2-Pin
// Michael Morscher, April 2020
// Tested on Arduino IDE 1.8.12
// Board: NodeMCU ESP8266 v3
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <SPI.h>
#include <stdlib.h>
//#define ARDUINO 101

// Additional used libraries
// ArduinoOTA: https://github.com/esp8266/Arduino/tree/master/libraries/ArduinoOTA
// Syslog: https://github.com/arcao/Syslog
// arduino-mqtt: https://github.com/256dpi/arduino-mqtt
// Adafruit Sensor: https://github.com/adafruit/Adafruit_Sensor
// Adafruit BME280 Library: https://github.com/adafruit/Adafruit_BME280_Library
// Adafruit TSL2561 Library: https://github.com/adafruit/Adafruit_TSL2561.git
// Adafruit CCS811: https://github.com/adafruit/Adafruit_CCS811
#include <ArduinoOTA.h>
#include <Syslog.h>
#include <MQTT.h>
//#include "Timer.h"
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_TSL2561_U.h>
#include <Adafruit_CCS811.h>

// Configuration
#define LOCATION        "livingroom"
#define HOSTNAME        "ESP-LIVING"
#define SSID_NAME       "ssid"
#define SSID_PASSWORD   "password"
#define HOST_ADDRESS    "192.168.0.100"
#define SLEEPTIME       30e6              // in seconds

// Settings
#define VERSION 1.1
#define BAUDRATE 115200
#define OTA_PORT 8266

// Network settings
const char* ssid = SSID_NAME;
const char* password = SSID_PASSWORD;
const char* hostname = HOSTNAME;

// Syslog server connection info
#define SYSLOG_SERVER HOST_ADDRESS
#define SYSLOG_PORT 514
#define SYSLOG_APP_NAME LOCATION

// MQTT connection settings
#define BROKER_ADDRESS HOST_ADDRESS
#define BROKER_PORT 1883

// NodeMCU LED
int LED = 2;

// Light Sensor, Type: TSL-2561
// D1 = SCL
// D2 = SDA
Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);

// Gas sensor, Type: CCS811
// D1 = SCL
// D2 = SDA
Adafruit_CCS811 ccs;

// Pressure/Humidity/Temperature Sensor, Type: Bosch BME-280
// Configured as software SPI using Pins D5-D8
#define BME_SCK   14    // D5 = SCL
#define BME_MISO  15    // D8 = SD0
#define BME_MOSI  12    // D6 = SDA
#define BME_CS    13    // D7 = CSB
Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK);

// Instantiation
WiFiClient wifiClient;
MQTTClient mqttClient;
WiFiUDP udpClient;
Syslog syslog(udpClient, SYSLOG_SERVER, SYSLOG_PORT, hostname, SYSLOG_APP_NAME, LOG_KERN);

/* Unused for now
  void messageReceived(String &topic, String &payload) {
  Serial.println("MQTT message received: topic=" + topic + " (payload=" + payload + ")");
  syslog.log(LOG_DEBUG, "MQTT message received: topic=" + topic + " (payload=" + payload + ")");
  }*/

// Setup Phase
void setup() {
  // Serial configuration
  Serial.begin(BAUDRATE);
  Serial.print("Project version: ");
  Serial.println(VERSION);
  Serial.println("Setup initialised!");

  // LED initialization
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);

  // WIFI configuration
  WiFi.mode(WIFI_STA);
  WiFi.hostname(hostname);
  WiFi.begin(ssid, password);

  // Wait until Wi-Fi connection available
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
  ArduinoOTA.setPort(OTA_PORT);
  ArduinoOTA.setHostname(hostname);
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
  //mqttClient.onMessage(messageReceived);

  // MQTT connection
  while (!mqttClient.connect(hostname)) {
    delay(100);
  }
  Serial.println("Connected to broker!");

  // Initialize Sensors
  if (!bme.begin()) {
    Serial.println(("Could not find a valid BME280 sensor, check wiring!"));
    while (1);
  }
  if (!tsl.begin()) {
    Serial.print("Ooops, no TSL2561 detected ... Check your wiring or I2C ADDR!");
    while (1);
  }/*
  if (!ccs.begin()) {
    Serial.print("Ooops, no CCS811 detected ... Check your wiring or I2C ADDR!");
    while (1);
  }*/

  // Configure sensors
  tsl.enableAutoRange(true); /* Auto-gain ... switches automatically between 1x and 16x */
  tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS); /* fast but low resolution */
  /*
  while (!ccs.available());
  float temp = ccs.calculateTemperature();
  ccs.setTempOffset(temp - 25.0);*/

  syslog.log(LOG_INFO, "Startup successfull! (Version: " + String(VERSION) + ")");
  Serial.print("Setup: Successfull!\n");
}

// Measure brightness
void measureBrightness() {
  // Get a new sensor event
  sensors_event_t event;
  tsl.getEvent(&event);

  // Output light readings
  if (event.light) {
    Serial.print("Brightness = ");
    Serial.print(event.light);
    Serial.println(" lux");
    mqttClient.publish("cave/livingroom/brightness", String(event.light));
  }
}

// Measure ambient temperature
void measureTemperature() {
  Serial.print("Temperature = ");
  Serial.print(bme.readTemperature());
  Serial.println(" °C");
  mqttClient.publish("cave/livingroom/temperature", String(bme.readTemperature()));
}

// Measure ambient pressure
void measurePressure() {
  Serial.print("Pressure = ");
  Serial.print(bme.readPressure() / 100.0F);
  Serial.println(" hPa");
  mqttClient.publish("cave/livingroom/pressure", String((bme.readPressure() / 100.0F)));
}

// Measure ambient humidity
void measureHumidity() {
  Serial.print("Humidity = ");
  Serial.print(bme.readHumidity());
  Serial.println(" %");
  mqttClient.publish("cave/livingroom/humidity", String(bme.readHumidity()));
}

// Measure CO2 and gas detection
void measureGas() {
  float eCO2 = 0;
  float TVOC = 0;

  if (ccs.available()) {
    if (!ccs.readData()) {
      eCO2 = ccs.geteCO2();
      TVOC = ccs.getTVOC();
    }
  }

  Serial.print("CO2 level = ");
  Serial.print(eCO2);
  Serial.println(" ppm");
  Serial.print("TVOC level = ");
  Serial.print(TVOC);
  Serial.println(" ppb");
  if (eCO2 > 0) {
    mqttClient.publish("cave/livingroom/co2", String(eCO2));
    mqttClient.publish("cave/livingroom/tvoc", String(TVOC));
  }
}

// Endless program loop
void loop() {

  // Handle incoming OTA updates
  ArduinoOTA.handle();

  // Handle incoming MQTT mesages
  mqttClient.loop();
  if (!mqttClient.connected()) {
    setup();
  }

  // Measure sensor values
  measureBrightness();
  measureTemperature();
  measurePressure();
  measureHumidity();
  /*measureGas();*/
  Serial.print("*********************\n");

  // Required for network stack to finish sending MQTT messages
  delay(5000);

  // Set ESP to deep sleep to save energy - Make sure to connect D0 with RST on the NodeMCU!
  //ESP.deepSleep(SLEEPTIME);
}
