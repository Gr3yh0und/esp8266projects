// Project for OTA flashing of new firmware
// Used devices: none
// Based on the ArduinoOTA example project
// Michael Morscher, June 2018
// Tested on Arduino IDE 1.8.5
// Board: NodeMCU ESP8266 v3
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>

// Additional used libraries
// ArduinoOTA: https://github.com/esp8266/Arduino/tree/master/libraries/ArduinoOTA - tested with v1.0.0
#include <ArduinoOTA.h>

// Wi-Fi and OTA settings
const char* version = "1.0";
const char* ssid = "SSID";
const char* password = "password";
const char* hostname = "ESP-TEST";
const char* adminpw = "test123";

// Setup Phase
void setup() {

  // Serial configuration
  Serial.begin(115200);
  Serial.print("Project version: ");
  Serial.println(version);
  Serial.println("Setup initialised!");

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

  // OTA settings
  ArduinoOTA.setPort(8266);
  ArduinoOTA.setHostname(hostname);
  ArduinoOTA.setPassword(adminpw);

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
}

void loop() {
  ArduinoOTA.handle();
}
