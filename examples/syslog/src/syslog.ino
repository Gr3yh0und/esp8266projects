// Project for sending logs to a remote syslog server
// Used devices: none
// Based on the the rsyslog library example project: https://github.com/arcao/Syslog/blob/master/examples/SimpleLoggingESP8266/SimpleLoggingESP8266.ino
// Michael Morscher, June 2018
// Tested on Arduino IDE 1.8.5
// Board: NodeMCU ESP8266 v3
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

// Additional used libraries
// Syslog: https://github.com/arcao/Syslog
#include <Syslog.h>

// Wi-Fi and OTA settings
const char* version = "1.0";
const char* ssid = "SSID";
const char* password = "password";
const char* hostname = "ESP-test";

// Syslog server connection info
#define SYSLOG_SERVER "lan-syslog-server"
#define SYSLOG_PORT 514
#define SYSLOG_APP_NAME "ESP-APP"

// A UDP instance to let us send and receive packets over UDP
WiFiUDP udpClient;

// Create a new syslog instance with LOG_KERN facility
Syslog syslog(udpClient, SYSLOG_SERVER, SYSLOG_PORT, hostname, SYSLOG_APP_NAME, LOG_KERN);

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
}

void loop() {

  syslog.log(LOG_INFO, "Begin loop");
  Serial.println("loop");

  // wait ten seconds before sending log message again
  delay(10000);
}

