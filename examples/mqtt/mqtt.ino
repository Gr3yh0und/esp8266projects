// Project for sending MQTT messages to a broker
// Based on example: https://github.com/256dpi/arduino-mqtt/blob/master/examples/AdafruitHuzzahESP8266/AdafruitHuzzahESP8266.ino
// Used devices: none
// Based on the ArduinoOTA example project
// Michael Morscher, June 2018
// Tested on Arduino IDE 1.8.5
// Board: NodeMCU ESP8266 v3
#include <ESP8266WiFi.h>

// Additional used libraries
// arduino-mqtt: https://github.com/256dpi/arduino-mqtt
#include <MQTT.h>

// Wi-Fi settings
const char* version = "1.0";
const char* ssid = "SSID";
const char* password = "PASSWORD";
const char* hostname = "ESP-TEST";

// MQTT settings
#define BROKER_ADDRESS "192.168.0.1"
#define BROKER_PORT 1883
#define BROKER_TOPIC "/hello"

WiFiClient net;
MQTTClient client;

unsigned long lastMillis = 0;

void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);
}

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

  // MQTT configuration: Initialise and enable callbacks
  Serial.println("Initialising connection to MQTT broker...");
  client.begin(BROKER_ADDRESS, BROKER_PORT, net);
  client.onMessage(messageReceived);

  // MQTT connection
  while (!client.connect(hostname)) {
    delay(500);
  }
  Serial.println("Connected to broker!");

  client.subscribe(BROKER_TOPIC);
}

void loop() {
  client.loop();

  // publish a message roughly every second.
  if (millis() - lastMillis > 1000) {
    lastMillis = millis();
    Serial.println("Sending MQTT message...");
    client.publish(BROKER_TOPIC, "from ESP");
  }
}

