Collection for smart home automation use cases with ESP8266
=====

The basic idea behind this collection is to enable people to quickly be able to build small smart home / IoT devices based on the NodeMCU that can be put in and around their homes.

My intention behind this project is to play around with really cheap embedded hardware and sensors while not only focussing on the embedded part but also to get an idea of the whole picture: 

Sensor -> Device -> Network Transport -> Backend Service -> Visualisation -> Control Application

By running through all these stages I want to get further insight in new technologies and learn how to create useful architectures.
(Of course I also want to get a system in the end that can intelligently control my sprinkler system in the garden.)

## Project Overview
The project goal is to get a repository of information and code about sensors, embedded devices, communication protocols and backend services as well as architecture. Therefore the project will run through different stages of development:
* Create a base platform for sensors and actors
* Check what properties should be measured or controlled
* Order the needed parts
* Create small sleek example projects as references
* Combine examples to a production ready system
* Create a database sink in the backend to store database
* Integrate a secure transport mechanism between devices and backend
* Create visualisations for the measured values
* Create a cron job like environment to schedule certain actors
* Create a mobile responsive website for controll
* Create an Android application
* Integrate VPN services to enable remote control
* Try to get the ESP8266 to run on batteries only

## Prerequisites
Certain key parts are defined at the beginning:
* Embedded systems are based on the ESP8266/ESP32
* Backend system runs on a raspberry pi 3
* The complete system is planned to be used locally in a LAN environment. However VPN access for certain services would be nice.
* The idea is to use the hottest and hippest new piece of technology around
* Learn about everything

## "Stations" finished
* [Garden control](garden)
* [Simple ambient sensor station](ambientsensorstation)
* [Brewing Station](brewingstation)

## "Stations" ToDo
* Add more sensors to garden or second station (dust, temperature, humidity, pressure, brightness, rain, wind?)
* Make use of two brightness sensors at once (e.g. one for room light, second for dishwasher)
* Add sound sensor to ambient station for to sense if air conditioning is running or not
* Make a station only showing stats in the main entrance (displays, ledrings, ...)
* Integrate DS1820 for measuring temperature (e.g. in the fridge)

## Fun projects
* [Harry Potter sorting hat](sortinghat)

## Hardware
* NodeMCU v3 (ESP8266)
* I2C and SPI sensors and actors
* Wi-Fi connection
* Raspberry Pi as backend

## Software and Protocols
* Arduino IDE 
* Arduino libraries as stated in the source code
* MQTT / Mosquitto
* Syslog
* Telegraf
* InfluxDB
* Grafana
* Raspbian

## Installation on stations
* Setup Arduino environment
* Select required hardware
* Flash NodeMCUs with sketches
* Define MQTT topics

## Installation of the backend
* Setup Raspberry Pi with Raspbian
* Setup Mosquitto
* Setup Telegraf
* Setup InfluxDB
* Setup Grafana
 
## License
GNU GPL v3
I'm happy for any pull requests or to receive critical comments! Please feel free to do that...