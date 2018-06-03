# MQTT pubsub example
This example is based on the 'AdafruitHuzzahESP8266' example provided by the MQTT library.

Check: https://github.com/256dpi/arduino-mqtt/blob/master/examples/AdafruitHuzzahESP8266/AdafruitHuzzahESP8266.ino

It basically connects to the Wi-Fi, connects to the MQTT broker, subscribes to one topic and then publishes messages to the same topic every second. As the ESP is subscribed to the topic it is also publishing as well it gets a MQTT message everytime it publishes a new one to the topic.

## Used third party libraries
* [arduino-mqtt](https://github.com/256dpi/arduino-mqtt)

## Device Configuration
In the project you first need to define/edit:
* Wi-Fi SSID
* Wi-Fi password
* Device hostname
* MQTT broker IP address
* MQTT broker port number
* MQTT broker topic

## IDE Configuration
Make sure ...
* to have the arduino-mqtt library installed in your IDE
* you've flashed the sketch onto your ESP8266
* your ESP8266 is running correctly and is logged into your Wi-Fi
* your remote MQTT broker is running and available on the same network
* everything should be fine then

## Components
* NodeMCU v3 ([Documentation](https://nodemcu.readthedocs.io/en/master/))
* Remote MQTT server (e.g. Raspberry Pi with mosquitto daemon)
 
## Logging output example
ESP8266 serial debug output:
```
> Setup initialised!
> Connected to Wi-Fi!
> Local IP address: 192.168.0.106
> Hostname: ESP-GARDEN
> Initialising connection to MQTT broker...
> Connected to broker!
> Sending MQTT message...
> incoming: /hello - from ESP
...
```

Local MQTT broker output (with subscription to topic):
```
gr3yh0und@mqttbroker:/ $ mosquitto_sub -h localhost -v -t /hello
/hello from ESP
```